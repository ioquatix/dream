//
//  Reference.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 3/10/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_REFERENCE_H
#define _DREAM_REFERENCE_H

#include "Assertion.h"

#include <cstddef>
#include <algorithm>
#include <stdint.h>
#include <iostream>

namespace Dream {

	/* Why use these?
	
		Reference counting is expensive and mostly needless.
		
		The only place you need to increment a reference count is when 
		you need to save an object outside of the lexical scope. In this
		case use Reference<>.
		
		When you have a function that can accept either
			- A raw pointer
			- A Reference<>
		use Pointer<>
		
		When you use Pointer<>, you won't increment reference count. Another
		option is to use const Reference<> &, however you can't convert a
		raw pointer to this type.
	*/

#ifdef DREAM_REFERENCE_NO_USING
	// If we don't have support for alias templates, we can use the preprocessor to make the classes look identical
#define Pointer Ptr
#define Reference Ref
#endif
	
	void debug_allocations ();
	
	class SharedObject {
		public:
			typedef int32_t NumberT;
		protected:
			/// The number of references to this instance.
			mutable volatile NumberT _count;
		
		public:
			/// Default constructor. Sets the reference count to 0.
			SharedObject ();

			/// Default copy constructor. Sets the reference count of this object to 0.
			SharedObject (const SharedObject & other);
			
			/// Copy operator. Doesn't modify reference count.
			SharedObject & operator= (const SharedObject & other);
			
			virtual ~SharedObject ();
			
			void retain () const;
			
			/// Decrement the reference count - delete the object if zero.
			/// @returns true if the object was deleted.
			bool release () const;
			
			// delete this object
			void deallocate () const;
			
			NumberT reference_count () const;
	};
	
	template <typename ObjectT>
	class Pointer {
		protected:
			ObjectT* _object;
			
		public:
			Pointer () : _object(NULL) {
			}
			
			Pointer (ObjectT * object) : _object(object) {
			}
			
			template <typename OtherObjectT>
			Pointer (OtherObjectT* object) : _object(dynamic_cast<ObjectT*>(object)) {
			}
			
			template <typename OtherObjectT>
			Pointer (Pointer<OtherObjectT> other) : _object(dynamic_cast<ObjectT*>(other.get())) {
			}
			
			ObjectT* operator-> () const {
				ensure(_object != NULL);
				return _object;
			}
						
			ObjectT& operator* () const {
				ensure(_object != NULL);
				return *_object;
			}
			
			bool operator== (const Pointer & other) const
			{
				return _object == other._object;
			}
			
			bool operator!= (const Pointer & other) const
			{
				return _object != other._object;
			}
			
			bool operator< (const Pointer & other) const
			{
				return _object < other._object;
			}
			
			bool operator> (const Pointer & other) const
			{
				return _object > other._object;
			}
			
			bool operator<= (const Pointer & other) const
			{
				return _object <= other._object;
			}
			
			bool operator>= (const Pointer & other) const
			{
				return _object >= other._object;
			}
			
			ObjectT* get () const
			{
				return _object;
			}
			
			typedef ObjectT* Pointer::* safe_bool;
			
			operator safe_bool() const
			{
				return _object ? &Pointer::_object : 0;
			}
			
			template <typename OtherObjectT>
			Pointer<OtherObjectT> as () const {
				return Pointer<OtherObjectT>(dynamic_cast<OtherObjectT*>(_object));
			}
	};
	
	template <typename ObjectT>
	using Ptr = Pointer<ObjectT>;
	
	void mark_static_allocation (void*);
	
	template <typename ObjectT>
	class Static : public Pointer<ObjectT> {
		public:
			Static () : Pointer<ObjectT>(new ObjectT) {
				this->_object->retain();
				mark_static_allocation(this->_object);
			}
			
			Static (ObjectT* object) : Pointer<ObjectT>(object) {
				this->_object->retain();
				mark_static_allocation(this->_object);
			}
			
			~Static () {
				this->_object->release();
			}
	};
	
	template <typename ObjectT>
	class Reference : public Pointer<ObjectT> {
		private:
			void construct () {
				if (this->_object)
					this->_object->retain();
			}
			
			template <typename OtherObjectT>
			static ObjectT* extract (OtherObjectT* object) {
				return object;
			}
			
			template <typename OtherObjectT>
			static ObjectT* extract (Pointer<OtherObjectT>& object) {
				return object.get();
			}
			
		public:
			void clear () {
				if (this->_object) {
					this->_object->release();
					this->_object = NULL;
				}
			}
		
			Reference& set (ObjectT* object) {
				clear();
				
				if (object) {
					object->retain();
					this->_object = object;
				}
				
				return *this;
			}
			
			Reference () {
			}
			
			Reference (ObjectT* object) : Pointer<ObjectT>(object) {
				construct();
			}
			
			Reference (const Reference& other) : Pointer<ObjectT>(other.get()) {
				construct();
			}

			template <typename OtherObjectT>			
			Reference (Pointer<OtherObjectT> other) : Pointer<ObjectT>(other.get()) {
				construct();
			}
			
			Reference& operator= (const Reference& other) {
				return set(other.get());
			}
			
			template <typename OtherObjectT>
			Reference& operator= (Pointer<OtherObjectT>& other) {
				return set(other.get());
			}
			
			Reference& operator= (ObjectT* object) {
				return set(object);
			}
			
			template <typename OtherObjectT>
			Reference& operator= (OtherObjectT* object) {
				return set(dynamic_cast<ObjectT*>(object));
			}
			
			~Reference () {
				clear();
			}
	};
	
	template <typename ValueT>
	Reference<ValueT> ref (ValueT* value) {
		return Reference<ValueT>(value);
	}
	
	template <typename ObjectT>
	using Ref = Reference<ObjectT>;
	
	template <typename ValueT>
	class Shared
	{
		protected:
			Ref<SharedObject> _controller;
			ValueT * _value;
		
		public:
			Ref<SharedObject> controller () const
			{
				return _controller;
			}
			
			Shared ()
				: _controller(NULL), _value(NULL)
			{
			
			}
			
			~Shared ()
			{
				// If this is the last Shared<...> we delete the value.
				if (_value && _controller->reference_count() == 1) {
					delete _value;
				}
			}
			
			Shared (ValueT * value)
				: _controller(new SharedObject), _value(value)
			{
				
			}
			
			Shared (const Shared & other)
				: _controller(other._controller), _value(other._value)
			{
				
			}
			
			template <typename OtherValueT>
			Shared (OtherValueT * object)
				: _controller(new SharedObject), _value(dynamic_cast<ValueT*>(object))
			{
			
			}
			
			template <typename OtherValueT>
			Shared (Shared<OtherValueT> other)
			{
				_value = dynamic_cast<ValueT*>(other.get());
				
				if (_value)
					_controller = other.controller();
				else
					_controller = NULL;
			}
			
			ValueT * get () const
			{
				return _value;
			}
			
			/// Copy operator. Doesn't modify reference count.
			Shared & operator= (const Shared & other)
			{
				_controller = other._controller;
				_value = other._value;
				
				return *this;
			}
			
			/// Copy operator. Doesn't modify reference count.
			template <typename OtherValueT>
			Shared & operator= (const Shared<OtherValueT> & other)
			{
				_value = dynamic_cast<ValueT*>(other.get());
				
				if (_value)
					_controller = other.controller();
				
				return *this;
			}
			
			template <typename OtherValueT>
			Shared & operator= (OtherValueT * other)
			{
				_value = dynamic_cast<ValueT*>(other);
				
				if (_value)
					_controller = new SharedObject;
				
				return *this;
			}
			
			ValueT* operator-> () const {
				ensure(_value != NULL);
				return _value;
			}
						
			ValueT& operator* () const {
				ensure(_value != NULL);
				return *_value;
			}
			
			typedef ValueT* Shared::* safe_bool;
			
			operator safe_bool() const
			{
				return _value ? &Shared::_value : 0;
			}
	};
}

#endif
