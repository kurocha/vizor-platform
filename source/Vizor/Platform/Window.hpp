//
//  Window.hpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 19/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Application.hpp"

#include <Time/Interval.hpp>
#include <Input/Handler.hpp>

namespace Vizor
{
	namespace Platform
	{
		using Scale = float;
		
		class Window : public Context, public Input::Handler
		{
		public:
			Window(const Application & application, const Context & context) : Context(context) {}
			virtual ~Window();
			
			vk::SurfaceKHR surface();
			
			// Show the window.
			virtual void show() = 0;
			
			// Hide the window.
			virtual void hide() = 0;
			
			// Indicates the window will entered the background with respect to other windows.
			virtual void will_enter_background();
			
			// Indicates the window has entered the foreground with respect to other windows.
			virtual void did_enter_foreground();
			
			virtual void prepare(Layers & layers, Extensions & extensions);
			
			// The size of the window, in pixels.
			virtual vk::Extent2D size() const = 0;
			
			// The scale a pixel w.r.t. the physical display.
			virtual Scale scale() const = 0;
			
			// Render a frame, which will be displayed at the given time.
			virtual void render(const Time::Interval & at);
			
		protected:
			virtual void setup_surface() = 0;
			
			vk::UniqueSurfaceKHR _surface;
		};
	}
}
