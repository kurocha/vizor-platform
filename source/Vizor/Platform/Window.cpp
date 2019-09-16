//
//  Window.cpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 24/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#include "Window.hpp"

#ifdef VK_USE_PLATFORM_MACOS_MVK
#include <vulkan/vulkan_macos.h>
#endif

#include <Logger/Console.hpp>
#include <Streams/Container.hpp>
#include <Streams/Safe.hpp>

namespace Vizor
{
	namespace Platform
	{
		using namespace Logger;
		
		Window::~Window()
		{
		}
		
		vk::SurfaceKHR Window::surface()
		{
			if (!_surface) {
				Console::info("setup_surface()");
				setup_surface();
				Console::info("setup_surface() ->", _surface.get());
			}
			
			return _surface.get();
		}
		
		void Window::prepare(Layers & layers, Extensions & extensions) const noexcept
		{
		}
		
		void Window::setup_surface()
		{
#if defined(VK_USE_PLATFORM_MACOS_MVK)
			auto surface_create_info = vk::MacOSSurfaceCreateInfoMVK()
				.setPView(_view);
			
			_surface = _instance.createMacOSSurfaceMVKUnique(surface_create_info, _allocation_callbacks);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
			auto surface_create_info = vk::XcbSurfaceCreateInfoKHR()
				.setConnection(this->connection())
				.setWindow(this->handle());
			
			_surface = _instance.createXcbSurfaceKHRUnique(surface_create_info, _allocation_callbacks);
#else
#error "Unsupported Platform!"
#endif
		}
	}
}
