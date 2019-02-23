//
//  Key.hpp
//  This file is part of the "Vizor Platform" project and released under the .
//
//  Created by Samuel Williams on 16/10/2006.
//  Copyright, 2006, by Samuel Williams. All rights reserved.
//

#pragma once

namespace Vizor
{
	namespace Platform
	{
		namespace Input
		{
			enum Device : std::uint64_t
			{
				NullDevice = 0,
				DefaultKeyboard = 1,
				DefaultMouse = 2,
				DefaultTouchPad = 3,
				AnyDevice = 0xFFFF
			};
			
			enum State : std::uint64_t
			{
				Released = 0,
				Pressed = 1,
				Dragged = 2,
				
				// Indicates that the mouse motion was cancelled, and any related action should be abandoned.
				Cancelled = 8,
			};
			
			using Button = std::uint64_t;
			
			// If a mouse button is released after being pressed or dragged, it will
			// create a motion key with State::Released and MouseButton::Mouse*Button
			// If a mouse is just dragged around with no button, it will generate similar
			// keys but with Mouse
			enum MouseButton : std::uint8_t
			{
				NullButton = 0,
				MouseLeftButton = 1,
				MouseMiddleButton = 2,
				MouseRightButton = 3,
				
				// Trackpad / Scroll Wheel input
				MouseScroll = 0xF1,
				
				// Window Events
				MouseEntered = 0xF2,
				MouseExited = 0xF3
			};
			
			// List of key codes for Mac OS X:
			// http://forums.macrumors.com/showthread.php?t=780577
			enum KeyboardButton : std::uint8_t
			{
			};
			
			/* Simple encapsulation of a device and its button */
			class Key
			{
			public:
				Key(const Device & device, const Button & button) : _device(device), _button(button) {}
				
				const Device & device() const noexcept {return _device;}
				const Button & button() const noexcept {return _button;}
				
				bool operator<(const Key & other) const noexcept {
					return _device < other._device && _button < other._button;
				}
				
				bool operator<=(const Key & other) const noexcept {
					return _device <= other._device && _button <= other._button;
				}
				
				bool operator>(const Key & other) const noexcept {
					return _device > other._device && _button > other._button;
				}
				
				bool operator>=(const Key & other) const noexcept {
					return _device >= other._device && _button >= other._button;
				}
				
				bool operator==(const Key & other) const noexcept {
					return _device == other._device && _button == other._button;
				}
				
				bool operator!=(const Key & other) const noexcept {
					return !((*this) == other);
				}
				
			protected:
				Device _device;
				Button _button;
			};
		}
	}
}
