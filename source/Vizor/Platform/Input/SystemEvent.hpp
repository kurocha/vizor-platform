//
//  SystemEvent.hpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 23/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Event.hpp"

namespace Vizor
{
	namespace Platform
	{
		namespace Input
		{
			class SystemEvent : public Event
			{
			public:
				enum Flags : std::uint8_t
				{
					EXIT = 1 << 0,
					PAUSE = 1 << 1,
					RESUME = 1 << 2,
				};
				
				SystemEvent(const Time::Interval & time, Flags flags) : Event(time), _flags(flags) {}
				
				Flags flags() const noexcept {return _flags;}
				
				virtual bool apply(Handler & handler) const;
				
			protected:
				Flags _flags;
				
			};
		}
	}
}
