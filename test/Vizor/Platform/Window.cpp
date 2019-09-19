//
//  Window.cpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 24/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Vizor/Application.hpp>
#include <Vizor/Platform/Window.hpp>

#include <Vizor/Platform/SurfaceDevice.hpp>
#include <Vizor/Platform/Swapchain.hpp>

#include <Logger/Console.hpp>

#include <Resources/FileLoader.hpp>
#include <Resources/RelativeLoader.hpp>
#include <Resources/LoadError.hpp>

#include <URI/File.hpp>

#include <thread>

namespace Vizor
{
	namespace Platform
	{
		using namespace Logger;
		using namespace Resources;
		using namespace Memory;
		
		class ShowWindowApplication : public Native::Application
		{
		public:
			using Native::Application::Application;
			virtual ~ShowWindowApplication() {}
			
			Vizor::Application _application;
			std::unique_ptr<Window> _window;
			std::unique_ptr<SurfaceDevice> _surface_device;
			std::unique_ptr<Swapchain> _swapchain;
			
			Owned<Loader<Data>> _loader;
			
			vk::UniqueShaderModule create_shader(const Buffers::Buffer & buffer) {
				auto info = vk::ShaderModuleCreateInfo()
					.setPCode((uint32_t*)buffer.begin())
					.setCodeSize(buffer.size());

				return _surface_device->device().createShaderModuleUnique(info, _application.allocation_callbacks());
			}
			
			vk::UniqueShaderModule load_shader(const URI::Generic & uri) {
				Console::debug("Loading shader", uri);
				
				auto data = _loader->load(uri);
				
				if (!data) {
					throw LoadError(uri, "Couldn't load required shader!");
				}
				
				return create_shader(*data);
			}
			
			vk::UniqueRenderPass _render_pass;
			
			void create_render_pass() {
				auto color_attachment = vk::AttachmentDescription()
					.setFormat(_swapchain->surface_format().format)
					.setSamples(vk::SampleCountFlagBits::e1)
					.setLoadOp(vk::AttachmentLoadOp::eClear)
					.setStoreOp(vk::AttachmentStoreOp::eStore)
					.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
					.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
					.setInitialLayout(vk::ImageLayout::eUndefined)
					.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
				
				auto attachment_reference = vk::AttachmentReference()
					.setAttachment(0)
					.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
				
				auto subpass = vk::SubpassDescription()
					.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
					.setColorAttachmentCount(1)
					.setPColorAttachments(&attachment_reference);
				
				auto dependency = vk::SubpassDependency()
					.setSrcSubpass(VK_SUBPASS_EXTERNAL)
					.setDstSubpass(0)
					.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
					.setSrcAccessMask(vk::AccessFlagBits(0))
					.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
					.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

				auto info = vk::RenderPassCreateInfo()
					.setAttachmentCount(1)
					.setPAttachments(&color_attachment)
					.setSubpassCount(1)
					.setPSubpasses(&subpass)
					.setDependencyCount(1)
					.setPDependencies(&dependency);
				
				_render_pass = _surface_device->device().createRenderPassUnique(info, _application.allocation_callbacks());
			}
			
			vk::UniquePipelineLayout _pipeline_layout;
			vk::UniquePipeline _pipeline;
			
			void create_graphics_pipeline() {
				auto vertex_shader = load_shader("Vizor/Platform/triangle.vert.spv");
				auto fragment_shader = load_shader("Vizor/Platform/triangle.frag.spv");
				
				auto vertex_shader_stage_create_info = vk::PipelineShaderStageCreateInfo()
					.setStage(vk::ShaderStageFlagBits::eVertex)
					.setModule(vertex_shader.get())
					.setPName("main");
				
				auto fragment_shader_stage_create_info = vk::PipelineShaderStageCreateInfo()
					.setStage(vk::ShaderStageFlagBits::eFragment)
					.setModule(fragment_shader.get())
					.setPName("main");
				
				vk::PipelineShaderStageCreateInfo shader_stages[] = {
					vertex_shader_stage_create_info,
					fragment_shader_stage_create_info
				};
				
				auto vertex_input_create_info = vk::PipelineVertexInputStateCreateInfo()
					.setVertexBindingDescriptionCount(0)
					.setVertexAttributeDescriptionCount(0);
				
				auto input_assembly_create_info = vk::PipelineInputAssemblyStateCreateInfo()
					.setTopology(vk::PrimitiveTopology::eTriangleList)
					.setPrimitiveRestartEnable(false);
				
				auto extent = _swapchain->extent();
				auto viewport = vk::Viewport(0.0, 0.0, extent.width, extent.height, 0.0, 1.0);
				
				auto scissor = vk::Rect2D({0, 0}, extent);
				
				auto viewport_state_create_info = vk::PipelineViewportStateCreateInfo()
					.setViewportCount(1)
					.setPViewports(&viewport)
					.setScissorCount(1)
					.setPScissors(&scissor);
				
				auto rasterization_state_create_info = vk::PipelineRasterizationStateCreateInfo()
					.setDepthClampEnable(false)
					.setRasterizerDiscardEnable(false)
					.setPolygonMode(vk::PolygonMode::eFill)
					.setLineWidth(1.0)
					.setCullMode(vk::CullModeFlagBits::eNone)
					.setFrontFace(vk::FrontFace::eClockwise)
				;

				auto multisample_state_create_info = vk::PipelineMultisampleStateCreateInfo()
					.setRasterizationSamples(vk::SampleCountFlagBits::e1)
					.setSampleShadingEnable(false);

				typedef vk::ColorComponentFlagBits C;
				auto color_blend_attachment_state = vk::PipelineColorBlendAttachmentState()
					.setColorWriteMask(C::eA | C::eR | C::eG | C::eB)
					// .setColorBlendOp(vk::BlendOp::eAdd)
					// .setSrcColorBlendFactor(vk::BlendFactor::eOne)
					// .setDstColorBlendFactor(vk::BlendFactor::eZero)
					// .setAlphaBlendOp(vk::BlendOp::eAdd)
					// .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
					// .setDstAlphaBlendFactor(vk::BlendFactor::eDstAlpha)
					.setBlendEnable(false);
				
				auto color_blend_state_create_info = vk::PipelineColorBlendStateCreateInfo()
					.setBlendConstants({{1.0, 1.0, 1.0, 1.0}})
					.setAttachmentCount(1).setPAttachments(&color_blend_attachment_state);
				
				auto layout_create_info = vk::PipelineLayoutCreateInfo();
				
				_pipeline_layout = _surface_device->device().createPipelineLayoutUnique(layout_create_info, _application.allocation_callbacks());
				
				auto graphics_pipeline_create_info = vk::GraphicsPipelineCreateInfo()
					.setStageCount(2).setPStages(shader_stages)
					.setPVertexInputState(&vertex_input_create_info)
					.setPInputAssemblyState(&input_assembly_create_info)
					.setPViewportState(&viewport_state_create_info)
					.setPMultisampleState(&multisample_state_create_info)
					.setPColorBlendState(&color_blend_state_create_info)
					.setPRasterizationState(&rasterization_state_create_info)
					.setLayout(_pipeline_layout.get())
					.setRenderPass(_render_pass.get());

				_pipeline = _surface_device->device().createGraphicsPipelineUnique(vk::PipelineCache(), graphics_pipeline_create_info, _application.allocation_callbacks());
			}
			
			std::vector<vk::UniqueFramebuffer> _framebuffers;
			
			void create_framebuffers()
			{
				const auto & extent = _swapchain->extent();
				const auto & buffers = _swapchain->buffers();
				_framebuffers.reserve(buffers.size());
				
				for (std::size_t i = 0; i < buffers.size(); i++) {
						vk::ImageView attachments[] = {
								buffers[i].image_view.get()
						};
						
						auto framebuffer_create_info = vk::FramebufferCreateInfo()
							.setRenderPass(_render_pass.get())
							.setAttachmentCount(1).setPAttachments(attachments)
							.setWidth(extent.width)
							.setHeight(extent.height)
							.setLayers(1)
						;
						
						_framebuffers.push_back(
							_surface_device->device().createFramebufferUnique(framebuffer_create_info, _application.allocation_callbacks())
						);
				}
			}
			
			vk::UniqueCommandPool _command_pool;
			
			void create_command_pool()
			{
				_command_pool = _surface_device->device().createCommandPoolUnique(
					vk::CommandPoolCreateInfo()
						.setQueueFamilyIndex(_surface_device->graphics_queue_family_index())
						.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer),
					_application.allocation_callbacks());
			}
			
			std::vector<vk::UniqueCommandBuffer> _command_buffers;
			
			void create_command_buffers()
			{
				const auto & buffers = _swapchain->buffers();
				
				auto allocate_info = vk::CommandBufferAllocateInfo()
					.setCommandPool(_command_pool.get())
					.setLevel(vk::CommandBufferLevel::ePrimary)
					.setCommandBufferCount(buffers.size())
				;
				
				_command_buffers = _surface_device->device().allocateCommandBuffersUnique(allocate_info);
			}
			
			void prepare_command_buffers()
			{
				auto clear_value = vk::ClearValue()
					.setColor(std::array<float, 4>{0.0, 0.0, 0.2, 1.0})
				;
				
				for (size_t i = 0; i < _command_buffers.size(); i++)
				{
					auto & commands = _command_buffers[i];
					
					commands->begin(vk::CommandBufferBeginInfo());
					
					commands->beginRenderPass(
						vk::RenderPassBeginInfo()
							.setRenderPass(_render_pass.get())
							.setFramebuffer(_framebuffers[i].get())
							.setRenderArea(vk::Rect2D({0, 0}, _swapchain->extent()))
							.setClearValueCount(1).setPClearValues(&clear_value),
						vk::SubpassContents::eInline
					);
					
					commands->bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline.get());
					
					commands->draw(3, 1, 0, 0);
					
					commands->endRenderPass();
					
					commands->end();
				}
			}
			
			static const int MAX_FRAMES_IN_FLIGHT = 2;
			
			std::vector<vk::UniqueSemaphore> _image_available;
			std::vector<vk::UniqueSemaphore> _render_finished;
			std::vector<vk::UniqueFence> _fences;
			size_t _current_frame = 0;
			
			void prepare_synchronisation()
			{
				auto semaphore_create_info = vk::SemaphoreCreateInfo();
				
				auto fence_create_info = vk::FenceCreateInfo()
					.setFlags(vk::FenceCreateFlagBits::eSignaled);
				
				for (std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
				{
					_image_available.push_back(
						_surface_device->device().createSemaphoreUnique(semaphore_create_info, _application.allocation_callbacks())
					);
					
					_render_finished.push_back(
						_surface_device->device().createSemaphoreUnique(semaphore_create_info, _application.allocation_callbacks())
					);
					
					_fences.push_back(
						_surface_device->device().createFenceUnique(fence_create_info, _application.allocation_callbacks())
					);
				}
			}
			
			void draw_frame()
			{
				auto device = _surface_device->device();
				auto fence = _fences[_current_frame].get();
				
				device.waitForFences(1, &fence, true, UINT64_MAX);
				
				uint32_t image_index;
				
				{
					auto result = device.acquireNextImageKHR(_swapchain->swapchain(), UINT64_MAX, _image_available[_current_frame].get(), nullptr, &image_index);
				
					// if (result == VK_ERROR_OUT_OF_DATE_KHR) {
					// 		// recreateSwapChain();
					// 		return;
					// } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
					// 		throw std::runtime_error("failed to acquire swap chain image!");
					// }
				}
				
				auto submit_info = vk::SubmitInfo()
					.setCommandBufferCount(1)
					.setPCommandBuffers(&_command_buffers[image_index].get());
				
				vk::Semaphore wait_semaphores[] = {_image_available[_current_frame].get()};
				vk::PipelineStageFlags wait_stages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
				submit_info
					.setWaitSemaphoreCount(1)
					.setPWaitSemaphores(wait_semaphores)
					.setPWaitDstStageMask(wait_stages);
				
				vk::Semaphore signal_semaphores[] = {_render_finished[_current_frame].get()};
				submit_info
					.setSignalSemaphoreCount(1)
					.setPSignalSemaphores(signal_semaphores);
				
				device.resetFences(1, &fence);
				
				_surface_device->graphics_queue().submit(1, &submit_info, fence);
				
				auto present_info = vk::PresentInfoKHR()
					.setWaitSemaphoreCount(1)
					.setPWaitSemaphores(signal_semaphores);
				
				vk::SwapchainKHR swapchains[] = {_swapchain->swapchain()};
				present_info
					.setSwapchainCount(1)
					.setPSwapchains(swapchains)
					.setPImageIndices(&image_index);
				
				{
					auto result = _surface_device->present_queue().presentKHR(present_info);
					
					// if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
					// 		framebufferResized = false;
					// 		recreateSwapChain();
					// } else if (result != VK_SUCCESS) {
					// 		throw std::runtime_error("failed to present swap chain image!");
					// }
				}
				
				_current_frame = (_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
				
				Logger::Console::info("draw_frame:", _current_frame);
			}
			
			std::thread _renderer;
			
			virtual void did_finish_launching()
			{
				URI::File fixture_path(getenv("SHADERS_FIXTURES"), true);
				
				auto file_loader = owned<FileLoader>();
				_loader = owned<RelativeLoader<Data>>(fixture_path, file_loader);
				
				Console::warn("Preparing window...");
				_window = std::make_unique<Window>(_application.context(), *this);
				_window->show();
				
				Console::warn("Preparing surface...");
				_surface_device = std::make_unique<SurfaceDevice>(_application.context(), *_window);
				
				Swapchain::QueueFamilyIndices queue_family_indices = {
					_surface_device->graphics_queue_family_index(),
					_surface_device->present_queue_family_index(),
				};
				
				auto size = _window->layout().bounds.size();
				vk::Extent2D extent(size[0], size[1]);
				
				try {
					Console::warn("Preparing context...");
					auto context = _surface_device->context();
					Console::warn("Preparing swapchain...");
					_swapchain = std::make_unique<Swapchain>(context, queue_family_indices, extent);
				} catch (std::runtime_error & error) {
					Console::error(error.what());
				} catch (...) {
					Console::error("Failed!");
				}
				
				//_window->set_cursor(Display::Cursor::HIDDEN);
				_window->set_title("Hello World");
				
				Console::warn("Instantiating swapchain...");
				_swapchain->swapchain();
				
				create_render_pass();
				create_graphics_pipeline();
				create_framebuffers();
				create_command_pool();
				create_command_buffers();
				prepare_command_buffers();
				prepare_synchronisation();
				draw_frame();
				
				_renderer = std::thread([&]{
					while (true)
						draw_frame();
				});
			}
		};
		
		UnitTest::Suite WindowTestSuite {
			"Vizor::Platform::Window",
			
			{"it should have some real tests",
				[](UnitTest::Examiner & examiner) {
					ShowWindowApplication application;
					
					application.run();
				}
			},
		};
	}
}
