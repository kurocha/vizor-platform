//
//  Window.cpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 24/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Vizor/Application.hpp>
#include <Vizor/ForwardRenderer.hpp>
#include <Vizor/Platform/Window.hpp>

#include <Vizor/Platform/SurfaceDevice.hpp>
#include <Vizor/Platform/SwapchainContext.hpp>

#include <Logger/Console.hpp>

#include <Resources/FileLoader.hpp>
#include <Resources/RelativeLoader.hpp>
#include <Resources/LoadError.hpp>

#include <URI/File.hpp>

#include <Numerics/Vector.hpp>
#include <Numerics/Matrix.hpp>
#include <Numerics/Transforms.hpp>
#include <Numerics/Radians.hpp>
#include <Geometry/Box.hpp>

#include <thread>

namespace Vizor
{
	namespace Platform
	{
		using namespace Logger;
		using namespace Resources;
		using namespace Memory;
		using namespace Numerics;
		
		struct Camera {
			Numerics::Mat44 projection = Numerics::IDENTITY;
			Numerics::Mat44 model = Numerics::IDENTITY;
			Numerics::Mat44 view = Numerics::IDENTITY;
		};
		
		class ShowWindowApplication : public Native::Application
		{
		public:
			using Native::Application::Application;
			virtual ~ShowWindowApplication() {}
			
			Vizor::Application _application;
			std::unique_ptr<Window> _window;
			std::unique_ptr<SurfaceDevice> _surface_device;
			std::unique_ptr<SwapchainContext> _swapchain_context;
			
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
			
			std::unique_ptr<ForwardRenderer> _forward_renderer;
			
			void create_render_pass() {
				_forward_renderer = std::make_unique<ForwardRenderer>(_surface_device->context(), _swapchain_context->surface_format().format);
			}
			
			Camera _camera;
			DeviceBuffer _uniform_buffer;
			
			void setup_uniform_buffer()
			{
				_uniform_buffer = _surface_device->context().make_host_buffer_with(_camera, vk::BufferUsageFlagBits::eUniformBuffer);

				// Store information in the uniform's descriptor
				_uniform_buffer.descriptor = vk::DescriptorBufferInfo(*_uniform_buffer.buffer, 0, sizeof(_camera));
			}

			void update_uniform_buffer()
			{
				_surface_device->context().write_memory(*_uniform_buffer.memory, (Byte *)&_camera, sizeof(_camera));
			}
			
			vk::UniquePipelineCache _pipeline_cache;
			
			vk::UniqueDescriptorPool _descriptor_pool;
			vk::UniqueDescriptorSetLayout _descriptor_set_layout;
			vk::UniquePipelineLayout _pipeline_layout;
			vk::UniquePipeline _pipeline;
			
			vk::UniqueShaderModule _vertex_shader, _fragment_shader;
			vk::DescriptorSet _descriptor_set;
			
			inline vk::WriteDescriptorSet descriptor_set_bind(const vk::DescriptorSet & desc_set, vk::DescriptorImageInfo const &image_info, vk::DescriptorType type, int dest_binding) {
				return vk::WriteDescriptorSet()
					.setDstSet(desc_set)
					.setPImageInfo(&image_info)
					.setDstBinding(dest_binding)
					.setDescriptorCount(1)
					.setDescriptorType(type);
			}
			
			inline vk::WriteDescriptorSet descriptor_set_bind(const vk::DescriptorSet & desc_set, vk::DescriptorBufferInfo const &buffer_info, vk::DescriptorType type, int dest_binding) {
				return vk::WriteDescriptorSet()
					.setDstSet(desc_set)
					.setPBufferInfo(&buffer_info)
					.setDstBinding(dest_binding)
					.setDescriptorCount(1)
					.setDescriptorType(type);
			}
			
			void create_graphics_pipeline() {
				auto context = _surface_device->context();
				
				if (!_pipeline_cache) {
					auto pipeline_cache_create_info = vk::PipelineCacheCreateInfo();
					
					_pipeline_cache = _surface_device->device().createPipelineCacheUnique(pipeline_cache_create_info, _application.allocation_callbacks());
				}
				
				if (!_vertex_shader) {
					_vertex_shader = load_shader("Vizor/Platform/triangle.vert.spv");
				}
				
				if (!_fragment_shader) {
					_fragment_shader = load_shader("Vizor/Platform/triangle.frag.spv");
				}
				
				if (!_descriptor_pool) {
					_descriptor_pool = context.create_descriptor_pool({
						vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1),
					}, 1);
				}
				
				_descriptor_set_layout = context.create_descriptor_layout({
					vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr),
				});
				
				_descriptor_set = context.allocate_descriptor_sets(*_descriptor_pool, {*_descriptor_set_layout}).at(0);
				
				auto buffer_info = vk::DescriptorBufferInfo(*_uniform_buffer.buffer, 0, _uniform_buffer.info.size);
				
				context.device().updateDescriptorSets({
					descriptor_set_bind(_descriptor_set, buffer_info, vk::DescriptorType::eUniformBuffer, 0),
				}, {});
				
				auto vertex_shader_stage_create_info = vk::PipelineShaderStageCreateInfo()
					.setStage(vk::ShaderStageFlagBits::eVertex)
					.setModule(_vertex_shader.get())
					.setPName("main");
				
				auto fragment_shader_stage_create_info = vk::PipelineShaderStageCreateInfo()
					.setStage(vk::ShaderStageFlagBits::eFragment)
					.setModule(_fragment_shader.get())
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
				
				auto extent = _swapchain_context->extent();
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
				
				auto depth_stencil_state_create_info = vk::PipelineDepthStencilStateCreateInfo()
					.setDepthTestEnable(true)
					.setDepthWriteEnable(true)
					.setDepthCompareOp(vk::CompareOp::eLessOrEqual);

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
				
				std::array set_layouts = {
					_descriptor_set_layout.get()
				};
				
				auto layout_create_info = vk::PipelineLayoutCreateInfo()
					.setSetLayoutCount(set_layouts.size())
					.setPSetLayouts(set_layouts.data());
				
				_pipeline_layout = _surface_device->device().createPipelineLayoutUnique(layout_create_info, _application.allocation_callbacks());
				
				auto graphics_pipeline_create_info = vk::GraphicsPipelineCreateInfo()
					.setStageCount(2).setPStages(shader_stages)
					.setPVertexInputState(&vertex_input_create_info)
					.setPInputAssemblyState(&input_assembly_create_info)
					.setPViewportState(&viewport_state_create_info)
					.setPMultisampleState(&multisample_state_create_info)
					.setPColorBlendState(&color_blend_state_create_info)
					.setPRasterizationState(&rasterization_state_create_info)
					.setPDepthStencilState(&depth_stencil_state_create_info)
					.setLayout(_pipeline_layout.get())
					.setRenderPass(_forward_renderer->render_pass());

				_pipeline = _surface_device->device().createGraphicsPipelineUnique(_pipeline_cache.get(), graphics_pipeline_create_info, _application.allocation_callbacks());
			}
			
			std::vector<vk::UniqueFramebuffer> _framebuffers;
			DeviceImageView _depth_buffer;
			
			void create_framebuffers()
			{
				const auto & extent = _swapchain_context->extent();
				const auto & buffers = _swapchain_context->buffers();
				
				_depth_buffer = _forward_renderer->make_depth_buffer({extent.width, extent.height, 1});
				
				_framebuffers.clear();
				_framebuffers.reserve(buffers.size());
				
				for (std::size_t i = 0; i < buffers.size(); i++) {
						std::array attachments = {
							buffers[i].image_view.get(),
							_depth_buffer.view.get(),
						};
						
						auto framebuffer_create_info = vk::FramebufferCreateInfo()
							.setRenderPass(_forward_renderer->render_pass())
							.setAttachmentCount(attachments.size()).setPAttachments(attachments.data())
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
				const auto & buffers = _swapchain_context->buffers();
				
				auto allocate_info = vk::CommandBufferAllocateInfo()
					.setCommandPool(_command_pool.get())
					.setLevel(vk::CommandBufferLevel::ePrimary)
					.setCommandBufferCount(buffers.size())
				;
				
				_command_buffers = _surface_device->device().allocateCommandBuffersUnique(allocate_info);
			}
			
			void prepare_command_buffers()
			{
				std::array clear_values = {
					vk::ClearValue().setColor(std::array{0.0f, 0.0f, 0.0f, 0.0f}),
					vk::ClearValue().setDepthStencil({1.0f, 0}),
				};
				
				for (size_t i = 0; i < _command_buffers.size(); i++)
				{
					auto & commands = _command_buffers[i];
					
					commands->begin(vk::CommandBufferBeginInfo());
					
					commands->beginRenderPass(
						vk::RenderPassBeginInfo()
							.setRenderPass(_forward_renderer->render_pass())
							.setFramebuffer(_framebuffers[i].get())
							.setRenderArea(vk::Rect2D({0, 0}, _swapchain_context->extent()))
							.setClearValueCount(clear_values.size()).setPClearValues(clear_values.data()),
						vk::SubpassContents::eInline
					);
					
					commands->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *_pipeline_layout, 0, {_descriptor_set}, nullptr);
					
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
				
				_image_available.clear();
				_render_finished.clear();
				_fences.clear();
				
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
			
			void recreate_swapchain()
			{
				auto size = _window->layout().bounds.size();
				vk::Extent2D extent(size[0], size[1]);
				
				_current_frame = 0;
				_swapchain_context->resize(extent);
				
				create_render_pass();
				create_graphics_pipeline();
				create_framebuffers();
				create_command_buffers();
				prepare_command_buffers();
				prepare_synchronisation();
			}
			
			void draw_frame()
			{
				// Logger::Console::info("draw_frame:", _current_frame);
				
				auto device = _surface_device->device();
				auto fence = _fences[_current_frame].get();
				
				device.waitForFences(1, &fence, true, UINT64_MAX);
				
				uint32_t image_index;
				
				device.acquireNextImageKHR(_swapchain_context->swapchain(), UINT64_MAX, _image_available[_current_frame].get(), nullptr, &image_index);
				
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
				
				vk::SwapchainKHR swapchains[] = {_swapchain_context->swapchain()};
				present_info
					.setSwapchainCount(1)
					.setPSwapchains(swapchains)
					.setPImageIndices(&image_index);
				
				_surface_device->present_queue().presentKHR(present_info);
				
				_current_frame = (_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
			}
			
			Time::Timer _timer;
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
				
				SwapchainContext::QueueFamilyIndices queue_family_indices = {
					_surface_device->graphics_queue_family_index(),
					_surface_device->present_queue_family_index(),
				};
				
				auto size = _window->layout().bounds.size();
				vk::Extent2D extent(size[0], size[1]);
				
				try {
					Console::warn("Preparing context...");
					auto context = _surface_device->context();
					Console::warn("Preparing swapchain...");
					_swapchain_context = std::make_unique<SwapchainContext>(context, queue_family_indices, extent);
				} catch (std::runtime_error & error) {
					Console::error(error.what());
				} catch (...) {
					Console::error("Failed!");
				}
				
				//_window->set_cursor(Display::Cursor::HIDDEN);
				_window->set_title("Hello World");
				
				Console::warn("Instantiating swapchain...");
				_swapchain_context->swapchain();
				
				create_render_pass();
				setup_uniform_buffer();
				create_graphics_pipeline();
				create_framebuffers();
				create_command_pool();
				create_command_buffers();
				prepare_command_buffers();
				prepare_synchronisation();
				
				_renderer = std::thread([&]{
					while (true) {
						try {
							_camera.model = Numerics::Transforms::rotate(Numerics::radians((double)_timer.time()), Vec3{0, 0, 1});
							update_uniform_buffer();
							
							draw_frame();
						} catch (vk::OutOfDateKHRError) {
							Console::warn("Recreate swapchain...");
							_surface_device->device().waitIdle();
							recreate_swapchain();
						}
					}
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
