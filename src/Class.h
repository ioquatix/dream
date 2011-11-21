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
	class NonCopyable
	{
	protected:
		NonCopyable () {}
		~NonCopyable () {}
	
	private: 
		NonCopyable (const NonCopyable &);
		NonCopyable & operator = (const NonCopyable &);
	};
	
	class IObject : implements SharedObject
	{
	public:
		virtual ~IObject ();
	};

	class IClassType {
		public:
			virtual ~IClassType();
			virtual bool isClassOf(const PTR(IObject) other) abstract;
	};
	
	/**
		Check whether a particular object implements a given class type.
	*/
	template <typename ClassT>
	class ClassType : implements IClassType {
		public:
			virtual ~ClassType() {}
			
			//return typeid(ClassT) == typeid(other.get());
			
			virtual bool isClassOf(const PTR(IObject) other) {
				return dynamic_cast<const ClassT*>(other.get()) != NULL;
			}
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
