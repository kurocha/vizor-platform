//
//  ResizeEvent.hpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 23/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Event.hpp"

#include <Numerics/Vector.hpp>

namespace Vizor
{
	namespace Platform
	{
		namespace Input
		{
			using Size = Numerics::Vector<2, std::size_t>;
			
			class ResizeEvent : public Event
			{
			public:
				ResizeEvent(const Time::Interval & time, Size size) : Event(time), _size(size) {}
				
				Size size() const noexcept {return _size;}
				
				virtual bool apply(Handler & handler) const;
				
			protected:
				Size _size;
			};
		}
	}
}
