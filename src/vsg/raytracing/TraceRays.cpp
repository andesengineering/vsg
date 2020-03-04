/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/raytracing/TraceRays.h>
#include <vsg/vk/CommandBuffer.h>
#include <vsg/vk/Extensions.h>

using namespace vsg;

TraceRays::TraceRays()
{
}

void TraceRays::dispatch(CommandBuffer& commandBuffer) const
{
    Device* device = commandBuffer.getDevice();
    Extensions* extensions = Extensions::Get(device, true);
    auto rayTracingProperties = device->getPhysicalDevice()->getProperties<VkPhysicalDeviceRayTracingPropertiesNV, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV>();
    auto shaderGroupHandleSize = rayTracingProperties.shaderGroupHandleSize;

    using BufferSize = std::pair<VkBuffer, VkDeviceSize>;
    auto bufferAndOffset = [](auto& shaderGroup) {
        if (shaderGroup && shaderGroup->bufferData._buffer) return BufferSize(VkBuffer(*(shaderGroup->bufferData._buffer)), shaderGroup->bufferData._offset);
        return BufferSize(VK_NULL_HANDLE, 0);
    };

    auto [raygenShaderBindingTableBuffer, raygenShaderBindingOffset] = bufferAndOffset(raygen);
    auto [missShaderBindingTableBuffer, missShaderBindingOffset] = bufferAndOffset(missShader);
    auto [hitShaderBindingTableBuffer, hitShaderBindingOffset] = bufferAndOffset(hitShader);
    auto [callableShaderBindingTableBuffer, callableShaderBindingOffset] = bufferAndOffset(callableShader);

    extensions->vkCmdTraceRaysNV(
        commandBuffer,
        raygenShaderBindingTableBuffer,
        raygenShaderBindingOffset,
        missShaderBindingTableBuffer,
        missShaderBindingOffset,
        shaderGroupHandleSize,
        hitShaderBindingTableBuffer,
        hitShaderBindingOffset,
        shaderGroupHandleSize,
        callableShaderBindingTableBuffer,
        callableShaderBindingOffset,
        shaderGroupHandleSize,
        width,
        height,
        depth);
}
