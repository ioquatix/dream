//
//  Client/Display/Context.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 27/03/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Context.h"
#include "../../Resources/Loader.h"

namespace Dream {
	namespace Client {
		namespace Display {
// MARK: -

			IContextDelegate::~IContextDelegate ()
			{
			}

			void IContextDelegate::render_frame_for_time (Ptr<IContext> context, TimeT time)
			{
			}

			void IContextDelegate::process_input (Ptr<IContext> context, const Input & input)
			{
			}

// MARK: -

			IContext::~IContext ()
			{
			}

			Context::Context() : _cursor_mode(CURSOR_NORMAL) {
			}

			Context::~Context ()
			{
			}

			void Context::set_delegate(Ptr<IContextDelegate> context_delegate)
			{
				_context_delegate = context_delegate;
			}

			bool Context::process(const Input & input)
			{
				if (_context_delegate) {
					_context_delegate->process_input(this, input);

					return true;
				}

				return false;
			}

			void Context::render_frame_for_time (TimeT time)
			{
				if (_context_delegate) {
					_context_delegate->render_frame_for_time(this, time);
				}
			}

			void Context::set_cursor_mode(CursorMode mode) {
				_cursor_mode = mode;
			}

			CursorMode Context::cursor_mode() const {
				return _cursor_mode;
			}
		}
	}
}
