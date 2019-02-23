//
//  MotionEvent.hpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 23/2/2019.
//  Copyright, 2019, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Event.hpp"
#include "Key.hpp"

#include <Numerics/Vector.hpp>
#include <Geometry/Box.hpp>

namespace Vizor
{
	namespace Platform
	{
		namespace Input
		{
			using Position = Numerics::Vector<2, float>;
			using Delta = Numerics::Vector<2, float>;
			using Bounds = Geometry::Box<2, float>;
			
			class MotionEvent : public Event
			{
			public:
				MotionEvent(const Time::Interval & time, const Key & key, const State & state, const Position & position, const Delta & delta, const Bounds & bounds) : Event(time), _key(key), _state(state), _position(position), _delta(delta), _bounds(bounds) {}
				
				const Key & key () const noexcept {return _key;}
				const State & state () const noexcept {return _state;}
				
				const Position & current_position() const noexcept {return _position;}
				Position previous_position() const noexcept {return _position - _delta;}
				const Delta & delta() const noexcept {return _delta;}
				
				const Bounds & bounds() const noexcept {return _bounds;}
				
				// Updated bounds must be within the coordinate system provided by the current input event.
				MotionEvent with(const Bounds & updated_bounds) const noexcept {
					return MotionEvent(_time, _key, _state, _position, _delta, updated_bounds);
				}
				
				bool button_pressed(MouseButton button) const noexcept {
					return _state == Pressed && _key.button() == button;
				}

				bool button_pressed_or_dragged(MouseButton button) const noexcept {
					return (_state == Pressed || _state == Dragged) && _key.button() == button;
				}

				bool button_released(MouseButton button) const noexcept {
					return _state == Released && _key.button() == button;
				}
				
				virtual bool apply(Handler & handler) const;
				
			protected:
				Key _key;
				State _state;
				Position _position;
				Delta _delta;
				
				// The bounds of the view/screen which handled the motion input.
				Bounds _bounds;
			};
		}
	}
}
