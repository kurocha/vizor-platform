//
//  Window.cpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 19/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#include "Window.hpp"

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
		
		// Indicates the window will entered the background with respect to other windows.
		void Window::will_enter_background()
		{
		}
		
		// Indicates the window has entered the foreground with respect to other windows.
		void Window::did_enter_foreground()
		{
		}
		
		void Window::render(const Time::Interval & at)
		{
		}
	}
}
