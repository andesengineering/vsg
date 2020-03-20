#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/core/Object.h>
#include <vsg/core/type_name.h>

#include <functional>
#include <map>

namespace vsg
{

    class VSG_DECLSPEC ObjectFactory : public vsg::Object
    {
    public:
        ObjectFactory();

        virtual vsg::ref_ptr<vsg::Object> create(const std::string& className);

        using CreateFunction = std::function<vsg::ref_ptr<vsg::Object>()>;
        using CreateMap = std::map<std::string, CreateFunction>;

        CreateMap& getCreateMap() { return _createMap; }
        const CreateMap& getCreateMap() const { return _createMap; }

        /// return the ObjectFactory singleton instance
        static ref_ptr<ObjectFactory>& instance();

    protected:
        CreateMap _createMap;
    };

    // Helper tempalte class for registering the ability to create a Object of specified T on deamnd.
    template<class T>
    struct RegisterWithObjectFactoryProxy
    {
        RegisterWithObjectFactoryProxy()
        {
            ObjectFactory::instance()->getCreateMap()[type_name<T>()] = []() { return T::create(); };
        }
    };

// helper define for define the type_name() for a type, note must be placed in public scope.
#define EVSG_type_name(T) \
    template<>            \
    constexpr const char* vsg::type_name<T>() noexcept { return #T; }

} // namespace vsg
