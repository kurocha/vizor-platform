//
//  Handler.hpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 23/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

namespace Vizor
{
	namespace Platform
	{
		namespace Input
		{
			class Event;
			class ResizeEvent;
			class ButtonEvent;
			class MotionEvent;
			class SystemEvent;
			
			class Handler
			{
			public:
				Handler() {}
				virtual ~Handler();
				
				virtual bool process(const Event &);
				
				virtual bool process(const ResizeEvent &) {return false;}
				virtual bool process(const ButtonEvent &) {return false;}
				virtual bool process(const MotionEvent &) {return false;}
				virtual bool process(const SystemEvent &) {return false;}
			};
		}
	}
}
