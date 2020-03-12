#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/nodes/StateGroup.h>
#include <vsg/vk/Buffer.h>
#include <vsg/vk/Descriptor.h>
#include <vsg/vk/vk_buffer.h>

namespace vsg
{

    /** Compute the VkIndexType from Data source's value size.*/
    inline VkIndexType computeIndexType(const Data* indices)
    {
        if (indices)
        {
            switch (indices->valueSize())
            {
            case (1): return VK_INDEX_TYPE_UINT8_EXT;
            case (2): return VK_INDEX_TYPE_UINT16;
            case (4): return VK_INDEX_TYPE_UINT32;
            default: break;
            }
        }
        // nothing valid assigned
        return VK_INDEX_TYPE_MAX_ENUM;
    }

    class VSG_DECLSPEC BindIndexBuffer : public Inherit<Command, BindIndexBuffer>
    {
    public:
        BindIndexBuffer() {}

        BindIndexBuffer(Data* indices);

        BindIndexBuffer(const BufferData& bufferData);

        BindIndexBuffer(Buffer* buffer, VkDeviceSize offset, VkIndexType indexType);

        void setIndices(ref_ptr<Data> indices) { _indices = indices; }
        Data* getIndices() { return _indices; }
        const Data* getIndices() const { return _indices; }

        void add(ref_ptr<Buffer> buffer, VkDeviceSize offset);

        void read(Input& input) override;
        void write(Output& output) const override;

        void compile(Context& context) override;

        void dispatch(CommandBuffer& commandBuffer) const override;

    protected:
        virtual ~BindIndexBuffer();

        ref_ptr<Data> _indices;

        struct VulkanData
        {
            BufferData bufferData;
            VkIndexType indexType;
        };

        vk_buffer<VulkanData> _vulkanData;
    };
    VSG_type_name(vsg::BindIndexBuffer);

} // namespace vsg
