//
//  Application.cpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 23/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#include "Application.hpp"

namespace Vizor
{
	namespace Platform
	{
		Application::Application()
		{
		}
		
		Application::~Application()
		{
		}
		
		void Application::did_finish_launching()
		{
		}
		
		virtual bool Application::will_terminate()
		{
			return true;
		}
		
		// Indicates the application has entered the background with respect to other running applications.
		void Application::will_enter_background()
		{
		}
		
		// Indicates the application has entered the foreground with respect to other running applications.
		void Application::did_enter_foreground()
		{
		}
	}
}
