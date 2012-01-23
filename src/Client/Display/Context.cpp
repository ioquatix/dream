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
			
#pragma mark -

			IContextDelegate::~IContextDelegate ()
			{
			}
			
			void IContextDelegate::renderFrameForTime (PTR(IContext) context, TimeT time)
			{
			}
			
			void IContextDelegate::processInput (PTR(IContext) context, const Input & input)
			{
			}
			
#pragma mark -
			
			IContext::~IContext ()
			{
			}
			
			Context::~Context ()
			{
			}

			void Context::setDelegate(PTR(IContextDelegate) contextDelegate)
			{
				m_contextDelegate = contextDelegate;
			}
			
			bool Context::process(const Input & input)
			{
				if (m_contextDelegate) {
					m_contextDelegate->processInput(this, input);
				
					return true;
				}
				
				return false;
			}
			
			void Context::renderFrameForTime (TimeT time)
			{
				if (m_contextDelegate) {
					m_contextDelegate->renderFrameForTime(this, time);
				}
			}
			
		}
	}
}
