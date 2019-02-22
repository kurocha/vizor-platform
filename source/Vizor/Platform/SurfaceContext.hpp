//
//  SurfaceContext.hpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 16/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

#include <Vizor/GraphicsContext.hpp>
#include "Window.hpp"

namespace Vizor
{
	namespace Platform
	{
		class SurfaceContext : public GraphicsContext
		{
		public:
			SurfaceContext(const GraphicsContext & graphics_context, vk::Queue present_queue, vk::SurfaceKHR surface) : GraphicsContext(graphics_context), _present_queue(present_queue), _surface(surface) {}
			virtual ~SurfaceContext();
			
			vk::Queue present_queue() {return _present_queue;}
			vk::SurfaceKHR surface() {return _surface;}
			
		protected:
			vk::Queue _present_queue = nullptr;
			vk::SurfaceKHR _surface = nullptr;
		};
	}
}
