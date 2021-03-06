#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/state/Descriptor.h>
#include <vsg/vk/ImageData.h>

namespace vsg
{

    struct SamplerImage
    {
        ref_ptr<Sampler> sampler;
        ref_ptr<Data> data;
    };
    using SamplerImages = std::vector<SamplerImage>;

    class VSG_DECLSPEC DescriptorImage : public Inherit<Descriptor, DescriptorImage>
    {
    public:
        DescriptorImage();

        DescriptorImage(ref_ptr<Sampler> sampler, ref_ptr<Data> image, uint32_t dstBinding = 0, uint32_t dstArrayElement = 0, VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

        template<class T>
        DescriptorImage(ref_ptr<Sampler> sampler, ref_ptr<T> image, uint32_t dstBinding = 0, uint32_t dstArrayElement = 0, VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) :
            DescriptorImage(sampler, ref_ptr<Data>(image), dstBinding, dstArrayElement, descriptorType) {}

        DescriptorImage(const SamplerImage& samplerImage, uint32_t dstBinding = 0, uint32_t dstArrayElement = 0, VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

        DescriptorImage(const SamplerImages& samplerImages, uint32_t dstBinding = 0, uint32_t dstArrayElement = 0, VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

        SamplerImages& getSamplerImages() { return _samplerImages; }
        const SamplerImages& getSamplerImages() const { return _samplerImages; }

        // get the Vulkan related objects, populated by compile traversal
        ImageDataList& getImageList(uint32_t deviceID) { return _vulkanData[deviceID].imageDataList; }

        void read(Input& input) override;
        void write(Output& output) const override;

        void compile(Context& context) override;

        void assignTo(Context& context, VkWriteDescriptorSet& wds) const override;

        uint32_t getNumDescriptors() const override;

    protected:
        SamplerImages _samplerImages;

        struct VulkanData
        {
            ImageDataList imageDataList;
        };

        vk_buffer<VulkanData> _vulkanData;
    };
    VSG_type_name(vsg::DescriptorImage);

} // namespace vsg
