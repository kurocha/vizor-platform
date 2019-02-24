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

namespace Vizor
{
	namespace Platform
	{
		Window::~Window()
		{
		}
		
		vk::SurfaceKHR Window::surface()
		{
			if (!_surface) {
				setup_surface();
			}
			
			return _surface.get();
		}
		
		void Window::prepare(Layers & layers, Extensions & extensions) const noexcept
		{
#ifdef VK_USE_PLATFORM_MACOS_MVK
			extensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#else
#error "Unsupported Platform!"
#endif
		}
		
		void Window::setup_surface()
		{
#ifdef VK_USE_PLATFORM_MACOS_MVK
			auto surface_create_info = vk::MacOSSurfaceCreateInfoMVK()
				.setPView(_view);
			
			_surface = _instance.createMacOSSurfaceMVKUnique(surface_create_info, _allocation_callbacks);
#else
#error "Unsupported Platform!"
#endif
		}
	}
}
