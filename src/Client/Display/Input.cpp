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
						
			bool IInputHandler::process (const Input & input) {
				return input.act(*this);
			}
			
			EventInput::EventInput(EventName event)
				: m_event(event)
			{
			
			}
			
			EventInput::~EventInput()
			{
			
			}
			
			bool EventInput::act(IInputHandler &h) const
			{
				return h.event(*this);
			}
			
			EventInput::EventName EventInput::event () const
			{
				return m_event;
			}
			
			ButtonInput::ButtonInput(const Key &e, const StateT &s) 
				: m_key(e), m_state(s) 
			{
			
			}
			
			bool ButtonInput::act(IInputHandler &h) const
			{
				return h.button(*this);
			}
			
			ButtonInput::~ButtonInput () {
			
			}
			
			MotionInput::MotionInput(const Key &key, const StateT &state, const Vec3 &position, const Vec3 &motion, const AlignedBox<2> & bounds) 
				: m_key(key), m_state(state), m_motion(motion), m_bounds(bounds)
			{
				// We ensure that m_position is within the coordinate system provided by bounds
				// Normally this is the origin, but a Viewport may provide a different coordinate system.
				m_position = position - (bounds.min() << 0.0);
			}
			
			bool MotionInput::act(IInputHandler &h) const {
				return h.motion(*this);
			}
			
			MotionInput::~MotionInput () {
			
			}
			
			MotionInput MotionInput::inputByRefiningBounds(const AlignedBox<2> & updatedBounds) {
				return MotionInput(m_key, m_state, m_position, m_motion, updatedBounds);
			}
			
			ResizeInput::ResizeInput (const Vec2u & oldSize, const Vec2u & newSize)
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