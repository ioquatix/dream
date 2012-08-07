//
//  Class.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 10/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CLASS_H
#define _DREAM_CLASS_H

#include "Framework.h"
#include "Reference.h"

#include <map>

namespace Dream
{
	class NonCopyable {
	protected:
		NonCopyable () {}
		~NonCopyable () {}

	private:
		NonCopyable (const NonCopyable &);
		NonCopyable & operator = (const NonCopyable &);
	};

	/// This function returns the offset of a member of a class.
	///		member_offset(VertexData::color)
	template <class T, typename U>
	inline constexpr std::ptrdiff_t member_offset(U T::* member)
	{
		return reinterpret_cast<std::ptrdiff_t>(&(reinterpret_cast<T const volatile*>(NULL)->*member));
	}

	class IObject : implements SharedObject {
	public:
		virtual ~IObject ();
	};

	class IClassType {
	public:
		virtual ~IClassType();
		virtual bool is_class_of(const Ptr<IObject> other) abstract;
	};

	/**
	    Check whether a particular object implements a given class type.
	*/
	template <typename ClassT>
	class ClassType : implements IClassType {
	public:
		virtual ~ClassType() {}

		//return typeid(ClassT) == typeid(other.get());

		virtual bool is_class_of(const Ptr<IObject> other) {
			return dynamic_cast<const ClassT*>(other.get()) != NULL;
		}
	};

	class IFinalizer;

	/** The top level concrete object class.

	 Provides a basic implementation of Ref counting functionality and
	 */
	class Object : implements IObject {
	private:
		struct FinalizerReference {
			FinalizerReference * next;
			IFinalizer * finalizer;
		};

		FinalizerReference * _finalizers;

	public:
		Object();
		virtual ~Object();

		void insert_finalizer(IFinalizer * finalizer);
		bool erase_finalizer(IFinalizer * finalizer);
	};

	/** Provides weak reference semantic between objects.

	 */
	class IFinalizer {
	public:
		virtual ~IFinalizer() {
		}

		// Called when the object is going to be deleted.
		virtual void finalize(Object * object) = 0;
	};

	/** Implements a simple weak-key value storage container which is typically useful for decoupled rendering and logic systems.

	 */
	template <typename ValueT>
	class ObjectCache : public Object, virtual protected IFinalizer {
	public:
		typedef std::map<Object *, Ref<ValueT>> CacheMapT;

	protected:
		CacheMapT _cache;

		virtual void finalize(Object * object) {
			_cache.erase(object);
		}

	public:
		virtual ~ObjectCache() {
			// We need to remove ourselves from the list of finalizers for each
			for (auto node : _cache) {
				node.first->erase_finalizer(this);
			}
		}

		void set(Ptr<Object> object, Ptr<ValueT> cache) {
			_cache[object.get()] = cache;
			object->insert_finalizer(this);
		}

		Ref<ValueT> lookup(Ptr<Object> object) {
			auto i = _cache.find(object.get());

			if (i != _cache.end()) {
				return i->second;
			}

			return nullptr;
		}

		const CacheMapT & objects() {
			return _cache;
		}
	};
}

#endif
