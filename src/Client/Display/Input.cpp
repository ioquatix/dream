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
		
			Input::Input ()
				: m_time(systemTime())
			{
			
			}
			
			Input::Input (const Input & other)
				: m_time(other.m_time)
			{
			
			}
			
			Input::~Input()
			{
			}
			
#pragma mark -
			
			IInputHandler::~IInputHandler ()
			{
			
			}
						
			bool IInputHandler::process (const Input & input) {
				return input.act(this);
			}
			
#pragma mark -
			
			EventInput::EventInput(EventName event)
				: m_event(event)
			{
			
			}
			
			EventInput::EventInput (const EventInput & other)
				: Input(other), m_event(other.m_event)
			{
				
			}
			
			EventInput::~EventInput()
			{
			
			}
			
			bool EventInput::act(IInputHandler * handler) const
			{
				return handler->event(*this);
			}
			
			EventInput::EventName EventInput::event () const
			{
				return m_event;
			}
			
#pragma mark -
			
			ButtonInput::ButtonInput(const Key &e, const StateT &s) 
				: m_key(e), m_state(s) 
			{
			
			}
			
			ButtonInput::ButtonInput(const ButtonInput & other)
				: Input(other), m_key(other.m_key), m_state(other.m_state)
			{
			
			}
			
			bool ButtonInput::act (IInputHandler * handler) const
			{
				return handler->button(*this);
			}
			
			ButtonInput::~ButtonInput () {
			
			}
			
#pragma mark -
			
			MotionInput::MotionInput(const Key &key, const StateT &state, const Vec3 &position, const Vec3 &motion, const AlignedBox<2> & bounds) 
				: m_key(key), m_state(state), m_motion(motion), m_bounds(bounds)
			{
				// We ensure that m_position is within the coordinate system provided by bounds
				// Normally this is the origin, but a Viewport may provide a different coordinate system.
				m_position = position - (bounds.min() << 0.0);
			}
			
			MotionInput::MotionInput(const MotionInput & other)
				: Input(other), m_key(other.m_key), m_state(other.m_state), m_position(other.m_position), m_motion(other.m_motion), m_bounds(other.m_bounds)
			{
			
			}
			
			bool MotionInput::act (IInputHandler * handler) const {
				return handler->motion(*this);
			}
			
			MotionInput::~MotionInput () {
			
			}
			
			MotionInput MotionInput::inputByRefiningBounds(const AlignedBox<2> & updatedBounds) {
				return MotionInput(m_key, m_state, m_position, m_motion, updatedBounds);
			}
			
#pragma mark -
			
			ResizeInput::ResizeInput (const Vec2u & newSize)
				: m_newSize(newSize)
			{
				
			}
			
			ResizeInput::ResizeInput(const ResizeInput & other)
				: Input(other), m_newSize(other.m_newSize)
			{
			
			}
			
			bool ResizeInput::act (IInputHandler * handler) const {
				return handler->resize(*this);
			}
			
			ResizeInput::~ResizeInput () {
			
			}
			
#pragma mark -
			
			InputQueue::~InputQueue ()
			{
				std::vector<Input*> * items = m_queue.fetch();
				
				foreach(input, *items) {
					delete *input;
				}
			}
			
			bool InputQueue::resize(const ResizeInput & input)
			{
				m_queue.add(new ResizeInput(input));
				
				return true;
			}
			
			bool InputQueue::button(const ButtonInput & input)
			{
				m_queue.add(new ButtonInput(input));
				
				return true;
			}
			
			bool InputQueue::motion(const MotionInput & input)
			{
				m_queue.add(new MotionInput(input));
			
				return true;
			}
			
			bool InputQueue::event(const EventInput & input)
			{
				m_queue.add(new EventInput(input));
				
				return true;
			}
			
			void InputQueue::dequeue (IInputHandler * handler)
			{
				std::vector<Input*> * items = m_queue.fetch();
				
				foreach(input, *items) {
					handler->process(**input);
					
					delete *input;
				}
			}

		}
	}
}