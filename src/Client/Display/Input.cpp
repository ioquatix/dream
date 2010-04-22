/*
 *  Client/Display/Input.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 16/10/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "Input.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			//Input::Input (const TimeT & inputTime) : m_time(inputTime)
			//{
				
			//}
			
			IMPLEMENT_INTERFACE(InputHandler)
			
			bool IInputHandler::process (const Input & input) {
				return input.act(*this);
			}
			
			ButtonInput::ButtonInput(const Key &e, const StateT &s) 
			: m_key(e), m_state(s) {
			
			}
			
			bool ButtonInput::act(IInputHandler &h) const {
				return h.button(*this);
			}
			
			ButtonInput::~ButtonInput () {
			
			}
			
			MotionInput::MotionInput(const Key &e, const StateT &s, const Vec3 &p, const Vec3 &m) 
			: m_key(e), m_state(s), m_position(p), m_motion(m) {
			
			}
			
			bool MotionInput::act(IInputHandler &h) const {
				return h.motion(*this);
			}
			
			MotionInput::~MotionInput () {
			
			}
			
			ResizeInput::ResizeInput (const Vector<2, uint32_t> & oldSize, const Vector<2, uint32_t> & newSize)
			: m_oldSize(oldSize), m_newSize(newSize) {
				
			}
			
			bool ResizeInput::act(IInputHandler &h) const {
				return h.resize(*this);
			}
			
			ResizeInput::~ResizeInput () {
			
			}
		}
	}
}