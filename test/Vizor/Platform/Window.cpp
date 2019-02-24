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

namespace Vizor
{
	namespace Platform
	{
		using namespace Logger;
		
		class ShowWindowApplication : public Native::Application
		{
		public:
			using Native::Application::Application;
			virtual ~ShowWindowApplication() {}
			
			Vizor::Application _application;
			std::unique_ptr<Window> _window;
			std::unique_ptr<SurfaceDevice> _surface_device;
			std::unique_ptr<Swapchain> _swapchain;
			
			virtual void did_finish_launching()
			{
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
