#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/core/Data.h>

#include <vsg/maths/mat4.h>
#include <vsg/maths/vec2.h>
#include <vsg/maths/vec3.h>
#include <vsg/maths/vec4.h>

#include <vsg/io/Input.h>
#include <vsg/io/Output.h>

#define VSG_array2D(N, T) \
    using N = Array2D<T>; \
    template<>            \
    constexpr const char* type_name<N>() noexcept { return "vsg::" #N; }

namespace vsg
{
    template<typename T>
    class Array2D : public Data
    {
    public:
        using value_type = T;
        using iterator = value_type*;
        using const_iterator = const value_type*;

        Array2D() :
            _width(0),
            _height(0),
            _data(nullptr) {}
        Array2D(std::uint32_t width, std::uint32_t height, value_type* data) :
            _width(width),
            _height(height),
            _data(data) {}
        Array2D(std::uint32_t width, std::uint32_t height, value_type* data, Layout layout) :
            Data(layout),
            _width(width),
            _height(height),
            _data(data) {}
        Array2D(std::uint32_t width, std::uint32_t height) :
            _width(width),
            _height(height),
            _data(new value_type[static_cast<std::size_t>(width) * height]) {}

        template<typename... Args>
        static ref_ptr<Array2D> create(Args... args)
        {
            return ref_ptr<Array2D>(new Array2D(args...));
        }

        std::size_t sizeofObject() const noexcept override { return sizeof(Array2D); }

        // implementation provided by Visitor.h
        void accept(Visitor& visitor) override;
        void accept(ConstVisitor& visitor) const override;

        const char* className() const noexcept override { return type_name<Array2D>(); }

        void read(Input& input) override
        {
            std::size_t original_size = size();

            Data::read(input);
            std::uint32_t width = input.readValue<std::uint32_t>("Width");
            std::uint32_t height = input.readValue<std::uint32_t>("Height");
            std::size_t new_size = computeValueCountIncludingMipmaps(width, height, 1, _layout.maxNumMipmaps);
            if (input.matchPropertyName("Data"))
            {
                if (_data) // if data already may be able to reuse it
                {
                    if (original_size != new_size) // if existing data is a different size delete old, and create new
                    {
                        delete[] _data;
                        _data = new value_type[new_size];
                    }
                }
                else // allocate space for data
                {
                    _data = new value_type[new_size];
                }

                _width = width;
                _height = height;

                input.read(new_size, _data);
            }
        }

        void write(Output& output) const override
        {
            Data::write(output);
            output.writeValue<std::uint32_t>("Width", _width);
            output.writeValue<std::uint32_t>("Height", _height);

            output.writePropertyName("Data");
            output.write(valueCount(), _data);
            output.writeEndOfLine();
        }

        std::size_t size() const { return (_layout.maxNumMipmaps <= 1) ? static_cast<std::size_t>(_width) * _height : computeValueCountIncludingMipmaps(_width, _height, 1, _layout.maxNumMipmaps); }

        bool empty() const { return _width == 0 && _height == 0; }

        void clear()
        {
            _width = 0;
            _height = 0;
            if (_data) { delete[] _data; }
            _data = nullptr;
        }

        void assign(std::uint32_t width, std::uint32_t height, value_type* data, Layout layout = Layout())
        {
            if (_data) delete[] _data;

            _layout = layout;
            _width = width;
            _height = height;
            _data = data;
        }

        // release the data so that ownership can be passed on, the local data pointer and size is set to 0 and destruction of Array will no result in the data being deleted.
        void* dataRelease() override
        {
            void* tmp = _data;
            _data = nullptr;
            _width = 0;
            _height = 0;
            return tmp;
        }

        std::size_t valueSize() const override { return sizeof(value_type); }
        std::size_t valueCount() const override { return size(); }

        std::size_t dataSize() const override { return size() * sizeof(value_type); }

        void* dataPointer() override { return _data; }
        const void* dataPointer() const override { return _data; }

        void* dataPointer(std::size_t i) override { return _data + i; }
        const void* dataPointer(std::size_t i) const override { return _data + i; }

        std::uint32_t dimensions() const override { return 2; }

        std::uint32_t width() const override { return _width; }
        std::uint32_t height() const override { return _height; }
        std::uint32_t depth() const override { return 1; }

        value_type* data() { return _data; }
        const value_type* data() const { return _data; }

        std::size_t index(std::uint32_t i, std::uint32_t j) const noexcept { return static_cast<std::size_t>(j) * _width + i; }

        value_type& operator[](std::size_t i) { return _data[i]; }
        const value_type& operator[](std::size_t i) const { return _data[i]; }

        value_type& at(std::size_t i) { return _data[i]; }
        const value_type& at(std::size_t i) const { return _data[i]; }

        value_type& operator()(std::uint32_t i, std::uint32_t j) { return _data[index(i, j)]; }
        const value_type& operator()(std::uint32_t i, std::uint32_t j) const { return _data[index(i, j)]; }

        value_type& at(std::uint32_t i, std::uint32_t j) { return _data[index(i, j)]; }
        const value_type& at(std::uint32_t i, std::uint32_t j) const { return _data[index(i, j)]; }

        void set(std::size_t i, const value_type& v) { _data[i] = v; }
        void set(std::uint32_t i, std::uint32_t j, const value_type& v) { _data[index(i, j)] = v; }

        iterator begin() { return _data; }
        const_iterator begin() const { return _data; }

        iterator end() { return _data + size(); }
        const_iterator end() const { return _data + size(); }

    protected:
        virtual ~Array2D()
        {
            if (_data) delete[] _data;
        }

    private:
        std::uint32_t _width;
        std::uint32_t _height;
        value_type* _data;
    };

    VSG_array2D(ubyteArray2D, std::uint8_t);
    VSG_array2D(ushortArray2D, std::uint16_t);
    VSG_array2D(uintArray2D, std::uint32_t);
    VSG_array2D(floatArray2D, float);
    VSG_array2D(doubleArray2D, double);

    VSG_array2D(vec2Array2D, vec2);
    VSG_array2D(vec3Array2D, vec3);
    VSG_array2D(vec4Array2D, vec4);

    VSG_array2D(dvec2Array2D, dvec2);
    VSG_array2D(dvec3Array2D, dvec3);
    VSG_array2D(dvec4Array2D, dvec4);

    VSG_array2D(ubvec2Array2D, ubvec2);
    VSG_array2D(ubvec3Array2D, ubvec3);
    VSG_array2D(ubvec4Array2D, ubvec4);

    VSG_array2D(usvec2Array2D, usvec2);
    VSG_array2D(usvec3Array2D, usvec3);
    VSG_array2D(usvec4Array2D, usvec4);

    VSG_array2D(uivec2Array2D, uivec2);
    VSG_array2D(uivec3Array2D, uivec3);
    VSG_array2D(uivec4Array2D, uivec4);

    VSG_array2D(block64Array2D, block64);
    VSG_array2D(block128Array2D, block128);

} // namespace vsg
