/*
 *  Client/Display/Input.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 16/10/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_INPUT_H
#define _DREAM_CLIENT_DISPLAY_INPUT_H

#include "../../Core/Timer.h"
#include "../../Numerics/Vector.h"

#include <map>

namespace Dream {
	namespace Client {
		namespace Display {
			using namespace Dream::Core;
			using namespace Dream::Numerics;
			
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
			
			/* Simple encapsulation of a device and its button */
			class Key {
			protected:
				DeviceT m_device;
				ButtonT m_button;
				
			public:
				const DeviceT& device () const { return m_device; }
				const ButtonT& button () const { return m_button; }
				
				bool operator< (const Key& other) const {
					return m_device < other.m_device && m_button < other.m_button;
				}
				
				bool operator<= (const Key& other) const {
					return m_device <= other.m_device && m_button <= other.m_button;
				}
				
				bool operator> (const Key& other) const {
					return m_device > other.m_device && m_button > other.m_button;
				}
				
				bool operator>= (const Key& other) const {
					return m_device >= other.m_device && m_button >= other.m_button;
				}
				
				bool operator== (const Key& other) const {
					return m_device == other.m_device && m_button == other.m_button;
				}
				
				bool operator!= (const Key& other) const {
					return ! ((*this) == other);
				}
				
				Key (const DeviceT &device, const ButtonT &button) : m_device(device), m_button(button) {
					/* Simple Constructor */
				}
			};
			
			class ResizeInput;
			class ButtonInput;
			class MotionInput;
			class IInputHandler;
			
			class Input
			{
			protected:
				//TimeT m_time;
				
			public:
				//Input (const TimeT & inputTime);
				
				//TimeT time () const { return m_time; }
				virtual bool act(IInputHandler &h) const abstract;
			};
			
			class IInputHandler : IMPLEMENTS(Object)
			{
				EXPOSE_INTERFACE(InputHandler)
				
				class Class : IMPLEMENTS(Object::Class)
				{
				};
				
				virtual bool resize(const ResizeInput &) { return true; }
				virtual bool button(const ButtonInput &) { return false; }
				virtual bool motion(const MotionInput &) { return false; }
			
				virtual bool process (const Input & input);
			};
			
			class ButtonInput : public Input {
			private:
				Key m_key;
				StateT m_state;
				
			public:
				ButtonInput(const Key &e, const StateT &s);
				virtual ~ButtonInput ();
				
				virtual bool act(IInputHandler &h) const;
				
				const Key & key () const { return m_key; }
				const StateT & state () const { return m_state; }
				
				bool buttonPressed (const char b) const {
					return m_state == Pressed && m_key.button() == b;
				}
				
				bool buttonReleased (const char b) const {
					return m_state == Released && m_key.button() == b;
				}
			};
			
			class MotionInput : public Input {
			private:
				Key m_key;
				StateT m_state;
				Vec3 m_position, m_motion;
				
			public:
				MotionInput(const Key &e, const StateT &s, const Vec3 &p, const Vec3 &m);
				virtual ~MotionInput ();
				
				virtual bool act(IInputHandler &h) const;
				
				const Vec3 & currentPosition () const { return m_position; }
				const Vec3 previousPosition () const { return m_position - m_motion; }
				const Vec3 motion () const { return m_motion; }
				
				const Key & key () const { return m_key; }
				const StateT & state () const { return m_state; }
				
				bool buttonPressed(MouseButton b) const {
					return m_state == Pressed && m_key.button() == b;
				}
				
				bool buttonPressedOrDragged(MouseButton b) const {
					return (m_state == Pressed || m_state == Dragged) && m_key.button() == b;
				}
				
				bool buttonReleased(MouseButton b) const {
					return m_state == Released && m_key.button() == b;
				}
			};
			
			class ResizeInput : public Input {
			private:
				Vector<2, uint32_t> m_oldSize;
				Vector<2, uint32_t> m_newSize;
				
			public:
				ResizeInput(const Vector<2, uint32_t> & oldSize, const Vector<2, uint32_t> & newSize);
				virtual ~ResizeInput ();
				
				virtual bool act(IInputHandler &h) const;
				
				Vector<2, uint32_t> oldSize () const { return m_oldSize; }
				Vector<2, uint32_t> newSize () const { return m_newSize; }
			};
			
			template <typename action_t>
			class InputMapper {
			protected:
				typedef std::map<Key, action_t> ActionsMap;
				ActionsMap m_actions;
				
			public:
				void bind (const Key &e, action_t a) {
					m_actions[e] = a;
				}
				
				void bind (DeviceT d, ButtonT b, action_t a) {
					Key e(d,b);
					m_actions[e] = a;
				}
				
				bool lookup (const Key &e, const action_t &a) const {
					typename ActionsMap::const_iterator i = m_actions.find(e);
					if (i != m_actions.end()) {
						a = *i;
						return true;
					} else {
						return false;
					}
				}
			};
		}
	}
}

#endif