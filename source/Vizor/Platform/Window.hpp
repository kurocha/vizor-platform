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
			Window(const Application & application, const Context & context) : Context(context) {}
			virtual ~Window();
			
			vk::SurfaceKHR surface();
			
			virtual void prepare(Layers & layers, Extensions & extensions);
			
			virtual vk::Extent2D size() = 0;
			
		protected:
			virtual void setup_surface() = 0;
			
			vk::UniqueSurfaceKHR _surface;
		};
	}
}
