//
//  Events/Input.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 16/10/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#include "Input.h"

namespace Dream
{
	namespace Events
	{
		Input::Input () : _time(system_time())
		{
		}

		Input::Input (const Input & other) : _time(other._time)
		{
		}

		Input::~Input()
		{
		}

// MARK: -

		IInputHandler::~IInputHandler ()
		{
		}

		bool IInputHandler::process (const Input & input) {
			return input.act(this);
		}

// MARK: -

		EventInput::EventInput(EventName event) : _event(event)
		{
		}

		EventInput::EventInput (const EventInput & other) : Input(other), _event(other._event)
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
			return _event;
		}

// MARK: -

		ButtonInput::ButtonInput(const Key &e, const StateT &s) : _key(e), _state(s)
		{
		}

		ButtonInput::ButtonInput(const ButtonInput & other) : Input(other), _key(other._key), _state(other._state)
		{
		}

		bool ButtonInput::act (IInputHandler * handler) const
		{
			return handler->button(*this);
		}

		ButtonInput::~ButtonInput () {
		}

// MARK: -

		MotionInput::MotionInput(const Key &key, const StateT &state, const Vec3 &position, const Vec3 &motion, const AlignedBox2 & bounds) : _key(key), _state(state), _motion(motion), _bounds(bounds)
		{
			// We ensure that _position is within the coordinate system provided by bounds
			// Normally this is the origin, but a Viewport may provide a different coordinate system.
			_position = position - (bounds.min() << 0.0);
		}

		MotionInput::MotionInput(const MotionInput & other) : Input(other), _key(other._key), _state(other._state), _position(other._position), _motion(other._motion), _bounds(other._bounds)
		{
		}

		bool MotionInput::act (IInputHandler * handler) const {
			return handler->motion(*this);
		}

		MotionInput::~MotionInput () {
		}

		MotionInput MotionInput::input_by_refining_bounds(const AlignedBox2 & updated_bounds) {
			return MotionInput(_key, _state, _position, _motion, updated_bounds);
		}

// MARK: -

		ResizeInput::ResizeInput (const Vec2u & new_size) : _new_size(new_size)
		{
		}

		ResizeInput::ResizeInput(const ResizeInput & other) : Input(other), _new_size(other._new_size)
		{
		}

		bool ResizeInput::act (IInputHandler * handler) const {
			return handler->resize(*this);
		}

		ResizeInput::~ResizeInput () {
		}

// MARK: -

		InputQueue::~InputQueue ()
		{
			std::vector<Input*> * items = _queue.fetch();

			for (auto input : *items) {
				delete input;
			}
		}

		bool InputQueue::resize(const ResizeInput & input)
		{
			_queue.add(new ResizeInput(input));

			return true;
		}

		bool InputQueue::button(const ButtonInput & input)
		{
			_queue.add(new ButtonInput(input));

			return true;
		}

		bool InputQueue::motion(const MotionInput & input)
		{
			_queue.add(new MotionInput(input));

			return true;
		}

		bool InputQueue::event(const EventInput & input)
		{
			_queue.add(new EventInput(input));

			return true;
		}

		void InputQueue::dequeue (IInputHandler * handler)
		{
			std::vector<Input*> * items = _queue.fetch();

			for (auto input : *items) {
				handler->process(*input);

				delete input;
			}
		}
	}
}
