/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/io/DatabasePager.h>
#include <vsg/traversals/RecordTraversal.h>
#include <vsg/ui/ApplicationEvent.h>
#include <vsg/viewer/CommandGraph.h>
#include <vsg/viewer/RenderGraph.h>
#include <vsg/vk/State.h>

using namespace vsg;

CommandGraph::CommandGraph(Device* device, int family) :
    _device(device),
    _family(family)
{
}

CommandGraph::CommandGraph(Window* window)
{
    if (window)
    {
        _device = window->device();
        _family = window->physicalDevice()->getGraphicsFamily();

        for (size_t i = 0; i < window->numFrames(); ++i)
        {
            commandBuffers.emplace_back(window->commandBuffer(i));
        }
    }
}

void CommandGraph::record(CommandBuffers& recordedCommandBuffers, ref_ptr<FrameStamp> frameStamp, ref_ptr<DatabasePager> databasePager)
{
    if (!recordTraversal)
    {
        recordTraversal = new RecordTraversal(nullptr, _maxSlot);
    }

    recordTraversal->frameStamp = frameStamp;
    recordTraversal->databasePager = databasePager;
    if (databasePager) recordTraversal->culledPagedLODs = databasePager->culledPagedLODs;

    ref_ptr<CommandBuffer> commandBuffer;
    for (auto& cb : commandBuffers)
    {
        if (cb->numDependentSubmissions() == 0)
        {
            commandBuffer = cb;
        }
    }
    if (!commandBuffer)
    {
        ref_ptr<CommandPool> cp = CommandPool::create(_device, _family);
        commandBuffer = CommandBuffer::create(_device, cp, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
        commandBuffers.push_back(commandBuffer);
    }

    commandBuffer->numDependentSubmissions().fetch_add(1);

    recordTraversal->state->_commandBuffer = commandBuffer;

    // or select index when maps to a dormant CommandBuffer
    VkCommandBuffer vk_commandBuffer = *commandBuffer;

    // need to set up the command
    // if we are nested within a CommandBuffer already then use VkCommandBufferInheritanceInfo
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(vk_commandBuffer, &beginInfo);

    accept(*recordTraversal);

    vkEndCommandBuffer(vk_commandBuffer);

    recordedCommandBuffers.push_back(recordTraversal->state->_commandBuffer);
}

ref_ptr<CommandGraph> vsg::createCommandGraphForView(Window* window, Camera* camera, Node* scenegraph)
{
    auto commandGraph = CommandGraph::create(window);

    // set up the render graph for viewport & scene
    auto renderGraph = vsg::RenderGraph::create();
    renderGraph->addChild(ref_ptr<Node>(scenegraph));

    renderGraph->camera = camera;
    renderGraph->window = window;

    renderGraph->renderArea.offset = {0, 0};
    renderGraph->renderArea.extent = window->extent2D();

    renderGraph->clearValues.resize(2);
    renderGraph->clearValues[0].color = window->clearColor();
    renderGraph->clearValues[1].depthStencil = VkClearDepthStencilValue{1.0f, 0};

    commandGraph->addChild(renderGraph);

    return commandGraph;
}
