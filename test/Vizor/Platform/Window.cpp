//
//  Window.cpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 24/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Vizor/Application.hpp>
#include <Vizor/Platform/Window.hpp>

namespace Vizor
{
	namespace Platform
	{
		class ShowWindowApplication : public Native::Application
		{
		public:
			using Native::Application::Application;
			virtual ~ShowWindowApplication() {}
			
			Vizor::Application _application;
			std::unique_ptr<Window> _window;
			
			virtual void did_finish_launching()
			{
				_window = std::make_unique<Window>(_application.context(), *this);
				
				//_window->set_cursor(Display::Cursor::HIDDEN);
				_window->set_title("Hello World");
				
				_window->show();
			}
		};
		
		UnitTest::Suite WindowTestSuite {
			"Vizor::Platform::Window",
			
			{"it should have some real tests",
				[](UnitTest::Examiner & examiner) {
					ShowWindowApplication application;
					
					application.run();
				}
			},
		};
	}
}
