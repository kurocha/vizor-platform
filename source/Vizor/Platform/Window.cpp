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
	}
}
