//
//  Events/Input.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 16/10/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CLIENT_DISPLAY_INPUT_H
#define _DREAM_CLIENT_DISPLAY_INPUT_H

#include "Thread.h"
#include "../Core/Timer.h"
#include "../Numerics/Vector.h"
#include "../Geometry/AlignedBox.h"

#include <map>

namespace Dream
{
	namespace Events
	{
	
		using namespace Dream::Core;
		using namespace Dream::Numerics;
		using namespace Dream::Geometry;
		
		typedef unsigned DeviceT;
		typedef unsigned ButtonT;
		typedef unsigned StateT;
		
		enum Device {
			NullDevice = 0,
			DefaultKeyboard = 1,
			DefaultMouse = 2,
			DefaultTouchPad = 3,
			AnyDevice = 0xFFFF
		};
		
		enum State {
			Released = 0,
			Pressed = 1,
			Dragged = 2,
		};
		
		// If a mouse button is released after being pressed or dragged, it will
		// create a motion key with State::Released and MouseButton::Mouse*Button
		// If a mouse is just dragged around with no button, it will generate similar
		// keys but with Mouse
		
		enum MouseButton {
			NullButton = 0,
			MouseLeftButton = 1,
			MouseMiddleButton = 2,
			MouseRightButton = 3,
			
			/// Trackpad / Scroll Wheel input
			MouseScroll = 0xF1,
			
			/// Window Events
			MouseEntered = 0xF2,
			MouseExited = 0xF3
		};
		
		// List of key codes for Mac OS X:
		// http://forums.macrumors.com/showthread.php?t=780577
		enum KeyboardButton {
			
		};
		
		/* Simple encapsulation of a device and its button */
		class Key {
		protected:
			DeviceT _device;
			ButtonT _button;
			
		public:
			const DeviceT& device () const { return _device; }
			const ButtonT& button () const { return _button; }
			
			bool operator< (const Key& other) const {
				return _device < other._device && _button < other._button;
			}
			
			bool operator<= (const Key& other) const {
				return _device <= other._device && _button <= other._button;
			}
			
			bool operator> (const Key& other) const {
				return _device > other._device && _button > other._button;
			}
			
			bool operator>= (const Key& other) const {
				return _device >= other._device && _button >= other._button;
			}
			
			bool operator== (const Key& other) const {
				return _device == other._device && _button == other._button;
			}
			
			bool operator!= (const Key& other) const {
				return ! ((*this) == other);
			}
			
			Key (const DeviceT &device, const ButtonT &button) : _device(device), _button(button) {
				/* Simple Constructor */
			}
		};
		
		class ResizeInput;
		class ButtonInput;
		class MotionInput;
		class EventInput;
		class IInputHandler;
		
		class Input
		{
		protected:
			TimeT _time;
			
		public:
			Input ();
			Input (const Input & other);
			
			virtual ~Input();
			
			TimeT time () const { return _time; }
			virtual bool act (IInputHandler * handler) const abstract;
		};
		
		class IInputHandler
		{
		protected:
			friend class ResizeInput;
			virtual bool resize(const ResizeInput &) { return false; }
			
			friend class ButtonInput;
			virtual bool button(const ButtonInput &) { return false; }
			
			friend class MotionInput;
			virtual bool motion(const MotionInput &) { return false; }
			
			friend class EventInput;
			virtual bool event(const EventInput &) { return false; }
			
		public:
			virtual ~IInputHandler ();
			virtual bool process (const Input & input);
		};
		
		class EventInput : public Input {
		public:
			enum EventName {
				EXIT = 1,
				PAUSE = 2,
				RESUME = 3
			};
			
			EventInput(EventName event);
			EventInput(const EventInput & other);
			
			virtual ~EventInput ();
			
			EventName event() const;
		
		protected:
			EventName _event;
			
			virtual bool act (IInputHandler * handler) const;
		};
		
		class ButtonInput : public Input {
		private:
			Key _key;
			StateT _state;
			
		public:
			ButtonInput(const Key &e, const StateT &s);
			ButtonInput(const ButtonInput & other);
			
			virtual ~ButtonInput ();
			
			virtual bool act (IInputHandler * handler) const;
			
			const Key & key () const { return _key; }
			const StateT & state () const { return _state; }
			
			bool button_pressed (const char b) const {
				return _state == Pressed && _key.button() == b;
			}
			
			bool button_released (const char b) const {
				return _state == Released && _key.button() == b;
			}
		};
		
		class MotionInput : public Input {
		private:
			Key _key;
			StateT _state;
			Vec3 _position, _motion;
			
			// The bounds of the view/screen which handled the motion input.
			AlignedBox<2> _bounds;
			
		public:
			MotionInput(const Key &key, const StateT &state, const Vec3 &position, const Vec3 &motion, const AlignedBox<2> & bounds);
			MotionInput(const MotionInput & other);
			
			virtual ~MotionInput ();
			
			virtual bool act (IInputHandler * handler) const;
			
			const Vec3 & current_position () const { return _position; }
			const Vec3 previous_position () const { return _position - _motion; }
			const Vec3 motion () const { return _motion; }
			
			const AlignedBox<2> & bounds() const { return _bounds; }
			
			// Updated bounds must be within the coordinate system provided by the current input event.
			MotionInput input_by_refining_bounds(const AlignedBox<2> & updated_bounds);
			
			const Key & key () const { return _key; }
			const StateT & state () const { return _state; }
			
			bool button_pressed(MouseButton b) const {
				return _state == Pressed && _key.button() == b;
			}
			
			bool button_pressed_or_dragged(MouseButton b) const {
				return (_state == Pressed || _state == Dragged) && _key.button() == b;
			}
			
			bool button_released(MouseButton b) const {
				return _state == Released && _key.button() == b;
			}
		};
		
		class ResizeInput : public Input {
		private:
			Vec2u _new_size;
			
		public:
			ResizeInput(const Vec2u & new_size);
			ResizeInput(const ResizeInput & other);
			
			virtual ~ResizeInput ();
			
			virtual bool act (IInputHandler * handler) const;
			
			Vec2u new_size () const { return _new_size; }
		};
		
		/// The input queue can send user input across threads.
		class InputQueue : implements IInputHandler {
			protected:
				Events::Queue<Input*> _queue;
				
				virtual bool resize(const ResizeInput &);
				virtual bool button(const ButtonInput &);
				virtual bool motion(const MotionInput &);
				virtual bool event(const EventInput &);
				
			public:
				virtual ~InputQueue ();
									
				void dequeue (IInputHandler * handler);
		};
		
		template <typename ActionT>
		class InputMapper {
		protected:
			typedef std::map<Key, ActionT> ActionsMap;
			ActionsMap _actions;
			
		public:
			void bind (const Key &e, ActionT a) {
				_actions[e] = a;
			}
			
			void bind (DeviceT d, ButtonT b, ActionT a) {
				Key e(d,b);
				_actions[e] = a;
			}
			
			bool lookup (const Key &e, const ActionT &a) const {
				typename ActionsMap::const_iterator i = _actions.find(e);
				if (i != _actions.end()) {
					a = *i;
					return true;
				} else {
					return false;
				}
			}
		};
	}
}

#endif
