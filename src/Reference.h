/*
 *  Reference.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 3/10/09.
 *  Copyright 2009 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_REFERENCE_H
#define _DREAM_REFERENCE_H

#include <cstddef>
#include <algorithm>
#include <stdint.h>
#include <iostream>

#define PTR(t) Dream::Pointer<t> 
#define REF(t) Dream::Reference<t> 

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
	
	void debugAllocations ();
	
	class SharedObject {
		public:
			typedef int32_t NumberT;
		protected:
			/// The number of references to this instance.
			mutable volatile NumberT m_count;
		
		public:
			/// Default constructor. Sets the reference count to 0.
			SharedObject ();

			/// Default copy constructor. Sets the reference count of this object to 0.
			SharedObject (const SharedObject & other);
			
			/// Copy operator. Doesn't modify reference count.
			void operator= (const SharedObject & other);
			
			virtual ~SharedObject ();
			
			void retain () const;
			void release () const;
			
			// delete this object
			void deallocate () const;
			
			NumberT referenceCount () const;
	};
	
	template <typename ObjectT>
	class Pointer {
		protected:
			ObjectT* m_object;
			
		public:
			Pointer () : m_object(NULL) {
			}
			
			template <typename OtherObjectT>
			Pointer (OtherObjectT* object) : m_object(dynamic_cast<ObjectT*>(object)) {
			}
			
			template <typename OtherObjectT>
			Pointer (Pointer<OtherObjectT> other) : m_object(other.get()) {
			}
			
			ObjectT* operator-> () const {
				ensure(m_object != NULL);
				return m_object;
			}
						
			ObjectT& operator* () const {
				ensure(m_object != NULL);
				return *m_object;
			}
			
			bool operator== (const Pointer & other) const
			{
				return m_object == other.m_object;
			}
			
			bool operator!= (const Pointer & other) const
			{
				return m_object != other.m_object;
			}
			
			bool operator< (const Pointer & other) const
			{
				return m_object < other.m_object;
			}
			
			bool operator> (const Pointer & other) const
			{
				return m_object > other.m_object;
			}
			
			bool operator<= (const Pointer & other) const
			{
				return m_object <= other.m_object;
			}
			
			bool operator>= (const Pointer & other) const
			{
				return m_object >= other.m_object;
			}
			
			ObjectT* get () const
			{
				return m_object;
			}
			
			typedef ObjectT* Pointer::* safe_bool;
			
			operator safe_bool() const
			{
				return m_object ? &Pointer::m_object : 0;
			}
			
			template <typename OtherObjectT>
			Pointer<OtherObjectT> dynamicCast () const {
				return Pointer<OtherObjectT>(dynamic_cast<OtherObjectT*>(m_object));
			}
	};
	
	void markStaticAllocation (void*);
	
	template <typename ObjectT>
	class Static : public Pointer<ObjectT> {
		public:
			Static () : Pointer<ObjectT>(new ObjectT) {
				this->m_object->retain();
				markStaticAllocation(this->m_object);
			}
			
			Static (ObjectT* object) : Pointer<ObjectT>(object) {
				this->m_object->retain();
				markStaticAllocation(this->m_object);
			}
			
			~Static () {
				this->m_object->release();
			}
	};
	
	template <typename ObjectT>
	class Reference : public Pointer<ObjectT> {
		private:
			void construct () {
				if (this->m_object)
					this->m_object->retain();
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
				if (this->m_object) {
					this->m_object->release();
					this->m_object = NULL;
				}
			}
		
			Reference& set (ObjectT* object) {
				clear();
				
				if (object) {
					object->retain();
					this->m_object = object;
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
}

#endif
