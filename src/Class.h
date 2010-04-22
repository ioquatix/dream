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

#include <typeinfo>
#include <boost/intrusive_ptr.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>
#include <boost/mpl/if.hpp>

/// Shorthand for pointer types.
#define REF(cls) Dream::intrusive_ptr<cls> 
#define PTR(cls) REF(cls) &

/// Use in a class to provide support for abstract instantiation.
/// @sa IClass
#define EXPOSE_CLASS(type) \
public: \
class Class; \
typedef type this_t; \
static Class klass; \
\
virtual Class * objectClass() const; \
\
inline static Class * staticClass () \
{ \
	return & klass; \
} \
\
static Dream::ClassType<type> * staticType (); \
virtual Dream::IClassType * objectType () const; \
static bool isClassOf(const Dream::IObject * object);

/// Use in a classes Class to provide dynamic type information.
/// @sa IClassType
/// @sa IClass
#define EXPOSE_CLASSTYPE \
public: \
virtual IClassType * classType ();

/// Use in the implementation file for a class that uses EXPOSE_CLASS.
/// @sa EXPOSE_CLASS
#define IMPLEMENT_CLASS(type) \
type::Class type::klass; \
\
type::Class * type::objectClass() const \
{ \
	return &klass; \
} \
\
IClassType * type::objectType() const \
{ \
	return staticType(); \
} \
\
bool type::isClassOf(const IObject * object) \
{ \
	return dynamic_cast<const type *>(object) != NULL; \
} \
\
Dream::ClassType<type> * type::staticType () \
{ \
	static ClassType<type> s_classType; \
	return & s_classType; \
} \
\
IClassType * type::Class::classType () \
{ \
	return staticType();\
}

/// Use as a short-cut for implementing an interface.
#define IMPLEMENTS(type) virtual public I##type
/// Use as a short-cut for implementing an interface in another namespace.
#define IMPLEMENTS_NS(ns, type) virtual public ns::I##type

/// Use in an interface class to provide support for abstract instantiation.
#define EXPOSE_INTERFACE(type) \
public: \
virtual ~I##type (); \
class Class; \
typedef I##type this_t; \
static ClassType<I##type> * staticType (); \
\
static bool isClassOf(const IObject * object) \
{ \
	return dynamic_cast<const I##type *>(object) != NULL; \
} \
\
virtual Class * objectClass() const abstract;

#define IMPLEMENT_INTERFACE(type) \
I##type::~I##type () {}\
\
ClassType<I##type> * I##type::staticType () \
{ \
	static ClassType<I##type> s_classType; \
	return & s_classType; \
}

namespace Dream
{
	using boost::intrusive_ptr;
	typedef int RefCountT;

	/** A helper function for returning intrusive_ptr values.

	 @code
	 REF(MyClass) = ptr(new MyClass);
	 @endcode
	 */
	template <typename object_t>
	inline intrusive_ptr<object_t> ptr (object_t * o)
	{
		return intrusive_ptr<object_t>(o);
	}

	using boost::dynamic_pointer_cast;

	class IObject;
	class IClass;
	class Object;

	/** Abstract base class for type information.

	 @sa TypedValue
	 */
	class IType
	{
	public:
		/// The std::type_info of the class, as returned by typeid()
		virtual const std::type_info & typeinfo () const abstract;

		/// The name of the class, as returned by typeid().name()
		const char * name () const;

		/// The size of the class, as returned by sizeof(MyClass)
		virtual IndexT size () const abstract;

		/// Whether the typeinfo is equal to another
		bool operator== (const std::type_info & otherTypeinfo) const;
	};

	/** Abstract base class for class type information.

	 Encapsulates access to std::type_info and the Class objects. The specific instance of this object is provided by ClassType in the EXPOSE_CLASSTYPE
	 macro.

	 @code
	 class MyClass : public Object {
		EXPOSE_CLASS(MyClass)

		class Class : public Object::Class {
			EXPOSE_CLASSTYPE
		};
	 };

	 void main ()
	 {
		IClassType * classType = MyClass::staticType();
		ensure(classType->typeinfo() == typeid(MyClass));
	 }
	 @endcode

	 @sa ClassType
	 */
	class IClassType : public IType
	{
	public:
		/// Checks if a specific instance is of the class represented by this ClassType.
		virtual bool isClassOf (const IObject * other) abstract;

		/// A helper function identical to isClassOf
		bool isClassOf (const REF(IObject) & other);

		/// The IClass that can be used to instantiate objects of this type.
		/// Will return NULL if classtype is abstract!
		virtual IClass * objectClass () const abstract;

		/// Returns true if the class that this classtype represents is abstract, i.e. an interface.
		/// If this is the case, objectClass() will return NULL.
		virtual bool isInterface () const abstract;
	};

	/**
	 Abstract base class for object instantiation.

	 Encapsulates access to the specific ClassType instance, and derived classes will contain constructors.

	 @code
	 // MyClass.h
	 class MyClass : public Object {
		unsigned m_i;

		MyClass (unsigned i);

		EXPOSE_CLASS(MyClass)

		class Class : public Object::Class {
			EXPOSE_CLASSTYPE

			virtual REF(MyClass) initWithCount (unsigned);
		};

		virtual ~MyClass ();
	 };

	 // MyClass.cpp
	 IMPLEMENT_CLASS(MyClass)

	 MyClass::MyClass (unsigned i) : m_i(i)
	 {
	 }

	 MyClass::~MyClass ()
	 {
	 }

	 REF(MyClass) MyClass::Class::initWithCount (unsigned i)
	 {
		return ptr(new MyClass(i));
	 }

	 void main ()
	 {
		REF(MyClass) a, b, c;
		// Three different ways of instantiating the same object
		a = MyClass::klass.initWithCount(10);

		b = MyClass::staticClass()->initWithCount(10);

		MyClass::Class * myClass = MyClass::staticClass();
		c = myClass->initWithCount(10);
	 }
	 @endcode
	 */
	class IClass
	{
	public:
		virtual IClassType * classType () abstract;
	};

	/** Implements the specific ClassType variable for all classes.

	 This is typically accessed through IObject::staticType().
	 */
	template <typename class_t>
	class ClassType : IMPLEMENTS (ClassType)
	{
	private:
		struct IsInterface
		{
			static IClass * objectClass ()
			{ return NULL; }
		};

		struct IsObject
		{
			static IClass * objectClass ()
			{ return class_t::staticClass (); }
		};

		typedef boost::is_base_and_derived<Object, class_t> is_object_t;

	public:
		/// Specific class-type
		// static ClassType s_classType;

		virtual bool isClassOf (const IObject * other)
		{
			return dynamic_cast<const class_t *> (other) != NULL;
		}

		virtual const std::type_info & typeinfo () const
		{
			return typeid(class_t);
		}

		virtual const char * name () const
		{
			return typeid(class_t).name();
		}

		virtual IndexT size () const
		{
			return sizeof(class_t);
		}

		virtual IClass * objectClass () const
		{
			using namespace boost;
			using namespace boost::mpl;
			return if_<is_object_t, IsObject, IsInterface>::type::objectClass();
		}

		virtual bool isInterface () const
		{
			return !is_object_t::value;
		}
	};

	/// This generates all the static IClassType's required.
	//template <typename class_t>
	//ClassType<class_t> ClassType<class_t>::s_classType;

	class IObject;
	/// boost::intrusive_ptr reference counting. Increases reference count by one.
	void intrusive_ptr_add_ref (const IObject * obj);
	/// boost::intrusive_ptr reference counting. Decreases reference count by one and will delete object when count
	/// reaches 0.
	void intrusive_ptr_release (const IObject * obj);

	/** Root object for class hierarchy.

	 This class must have no data members, and no virtual functions that are not abstract.
	 */
	class IObject
	{
	protected:
		friend void intrusive_ptr_add_ref (const IObject *);
		friend void intrusive_ptr_release (const IObject *);

		/// Increase the reference count by one.
		virtual void retain () const abstract;
		/// Decrease the reference count by one.
		virtual bool release () const abstract;
	public:
		/// Abstract class type.
		typedef IClass Class;

		virtual ~IObject ();

		/// The current reference count for this object.
		virtual RefCountT referenceCount () const abstract;

		/// The Class * that represents this object.
		virtual Class * objectClass () const abstract;
		/// The ClassType * that represents this object.
		virtual IClassType * objectType () const abstract;

		/// Returns the name of the class, as given by typeid().name()
		const char * className () const;
		
		/// Returns true if this object matches the interface/class type specified.
		virtual bool isKindOf (IClassType * cls) const;
		
		/// Helper for dealing with klass values
		bool isKindOf (IClass & klass) const
		{
			return isKindOf(klass.classType());
		}
		
		/// Helper function for casting objects
		///
		/// @code
		/// REF(MyDeriviedType) derived = object->cast<MyDerivedType>();
		/// @endcode
		template <typename return_t>
		intrusive_ptr<return_t> cast ()
		{
			return dynamic_pointer_cast<return_t> (ptr (this));
		}
	};

	/** The top level concrete object class.

	 Provides a basic implementation of reference counting functionality and
	 */
	class Object : IMPLEMENTS (Object)
	{
	protected:
		/// Default constructor for Object. Sets the reference count to 0.
		Object ();
		/// Default copy constructor for Object. Sets the reference count of this object to 0.
		Object (const Object & other);
		/// Copy operator for Object. Sets the reference count of this object to 0.
		void operator= (const Object & other);

		EXPOSE_CLASS (Object)

		class Class : IMPLEMENTS (Object::Class)
		{
			EXPOSE_CLASSTYPE
		};

	private:
		/// The actual reference count variable.
		volatile mutable RefCountT m_referenceCount;
	
	protected:
		/// Increments m_referenceCount.
		virtual void retain () const;
		/// Decrements m_referenceCount.
		/// To implement your own memory management, override this function and handle the case where the reference
		/// count hits zero. Never return true.
		/// @returns True if this object is no longer needed and needs to be deleted. (i.e. reference count has hit 0)
		/// @returns False if this object is still useful. (i.e. reference count is greater than zero)
		virtual bool release () const;

	public:
		virtual ~Object ();

		/// Return the reference count of the object.
		RefCountT referenceCount () const
		{
			return m_referenceCount;
		}
	};
}

#endif
