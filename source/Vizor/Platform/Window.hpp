//
//  Window.hpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 19/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

#include <Vizor/Context.hpp>

namespace Vizor
{
	namespace Platform
	{
		class Window : public Context
		{
		public:
			Window(const Context & context) : Context(context) {}
			virtual ~Window();
			
			vk::SurfaceKHR surface();
			
			// #if defined(VK_USE_PLATFORM_MACOS_MVK)
			// 		extensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
			// #elif defined(VK_USE_PLATFORM_XCB_KHR)
			// 		extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
			// #elif defined(VK_USE_PLATFORM_WIN32_KHR)
			// 		extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
			// #else
			// #error "Unsupported platform!"
			// #endif
			virtual void prepare(Layers & layers, Extensions & extensions);
			
			virtual vk::Extent2D size() = 0;
			
		protected:
			virtual void setup_surface() = 0;
			
			vk::UniqueSurfaceKHR _surface;
		};
	}
}
