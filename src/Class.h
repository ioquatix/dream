/*
 *  Class.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 10/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLASS_H
#define _DREAM_CLASS_H

#include "Framework.h"
#include "Reference.h"

namespace Dream
{
	class IObject : public SharedObject
	{
	public:
		virtual ~IObject ();
	};

	/** The top level concrete object class.

	 Provides a basic implementation of reference counting functionality and
	 */
	class Object : implements IObject
	{
	public:
		virtual ~Object ();
	};
}

#endif
