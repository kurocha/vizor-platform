//
//  Application.hpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 23/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

#include <Vizor/Context.hpp>

namespace Vizor
{
	namespace Platform
	{
		// Represents a running application within the context of a display manager.
		class Application
		{
		public:
			Application();
			virtual ~Application();
			
			// Start the application running in whatever context that may be. Once the application is running, should invoke `did_finish_launching()`. Will not return until `stop()` is invoked. This function should receive events from the window server and act appropriately. It should also trigger any rendering callbacks on windows that have been created.
			virtual void run() = 0;
			
			// Cause `run()` to return.
			virtual void stop() = 0;
			
			// Invoked after the application state is initialized by `run()`. It is an appropriate place to create and show windows.
			virtual void did_finish_launching();
			
			// Indicates that the appliction should exit.
			// @return whether the application can exit or not.
			virtual bool will_terminate();
			
			// Indicates the application has entered the background with respect to other running applications.
			virtual void will_enter_background();
			
			// Indicates the application has entered the foreground with respect to other running applications.
			virtual void did_enter_foreground();
		};
	}
}
