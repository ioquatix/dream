/*
 *  Class.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 10/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Class.h"

// Dynamic Lookup (Deprecated at present)
// #include <map>
// #include <boost/pool/detail/singleton.hpp>

#include "Core/Strings.h"
#include "Core/Timer.h"

// For testing
#include "Core/CodeTest.h"

#include <libkern/OSAtomic.h>

namespace Dream
{
#pragma mark -
#pragma mark Dynamic Lookup (Deprecated at present)

	/*
	 using boost::details::pool::singleton_default;

	 typedef std::map<const String, IClass*> ClassMapT;
	 typedef singleton_default<ClassMapT> ClassMap;

	 IClass* IClass::classWithName (const char * name)
	 {
	 ClassMapT::iterator cls = ClassMap::instance().find(name);

	 if (cls == ClassMap::instance().end())
	 return NULL;
	 else
	 return cls->second;
	 }
	 */

#pragma mark -
#pragma mark class IType

	/// The name of the class, as returned by typeid().name()
	const char * IType::name () const
	{
		return typeinfo().name ();
	}

	/// Whether the typeinfo is equal to another
	bool IType::operator== (const std::type_info & otherTypeinfo) const
	{
		return this->typeinfo() == otherTypeinfo;
	}

#pragma mark -
#pragma mark class IClassType

	bool IClassType::isClassOf (const REF(IObject) & other)
	{
		return isClassOf(other.get());
	}

#pragma mark -
#pragma mark class IObject

	IObject::~IObject ()
	{
	}

	const char * IObject::className () const
	{
		return objectType()->name();
	}

	bool IObject::isKindOf (IClassType * cls) const
	{
		return cls->isClassOf(this);
	}

#pragma mark -
#pragma mark class Object

	IMPLEMENT_CLASS (Object)

	Object::~Object ()
	{
	}
	
#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
	class ITestInterface : IMPLEMENTS (Object)
	{
		EXPOSE_INTERFACE (TestInterface)

		class Class : IMPLEMENTS (Object::Class)
		{
		public:
			virtual REF(ITestInterface) init () abstract;
		};

	public:
		//virtual unsigned value () const abstract;
	};

	IMPLEMENT_INTERFACE(TestInterface)
	
	class ObjectImplementation : public Object, IMPLEMENTS (TestInterface)
	{
		EXPOSE_CLASS (ObjectImplementation)

		ObjectImplementation ()
		{
		}

		class Class : public Object::Class, IMPLEMENTS (TestInterface::Class)
		{
			EXPOSE_CLASSTYPE

			virtual REF(ITestInterface) init ()
			{
				return new ObjectImplementation;
			}
		};

	public:
		virtual ~ObjectImplementation ()
		{
		}
	};

	IMPLEMENT_CLASS (ObjectImplementation)

	class OtherImplementation : public Object, IMPLEMENTS (TestInterface)
	{
		EXPOSE_CLASS (OtherImplementation)

		OtherImplementation ()
		{
		}

		class Class : public Object::Class, IMPLEMENTS (TestInterface::Class)
		{
			EXPOSE_CLASSTYPE

			virtual REF(ITestInterface) init ()
			{
				return new OtherImplementation;
			}
		};

	public:
		virtual ~OtherImplementation ()
		{
		}
	};

	IMPLEMENT_CLASS (OtherImplementation)

	class DerivedImplementation : public ObjectImplementation
		{
			DerivedImplementation ()
			{
			}

			EXPOSE_CLASS (DerivedImplementation)

			class Class : public ObjectImplementation::Class
			{
				EXPOSE_CLASSTYPE

				virtual REF(ITestInterface) init ()
				{
					return new DerivedImplementation;
				}
			};

		public:
			virtual ~DerivedImplementation ()
			{
			}
		};

	IMPLEMENT_CLASS (DerivedImplementation)

	UNIT_TEST(Class)
	{
		testing("Static Type");

		check(ITestInterface::staticType()->objectClass() == (IClass*)NULL) << "Interface has no object class";
		check(ITestInterface::staticType()->isInterface()) << "Interface is marked correctly";

		check(ObjectImplementation::staticType()->objectClass() == ObjectImplementation::staticClass()) << "Object has object class";
		check(!DerivedImplementation::staticType()->isInterface()) << "Interface is marked correctly";

		check(sizeof(ObjectImplementation) == ObjectImplementation::staticType()->size()) << "Object size is reported correctly";
		check(typeid(ObjectImplementation) == ObjectImplementation::staticType()->typeinfo()) << "Typeinfo is reported correctly";

		testing("Construction");

		REF(ITestInterface) o1 = ObjectImplementation::staticClass()->init();
		REF(ITestInterface) o2 = OtherImplementation::staticClass()->init();
		REF(ITestInterface) o3 = DerivedImplementation::staticClass()->init();

		check(o1->referenceCount() == 1) << "Reference count is correct";
		check(o2->referenceCount() == 1) << "Reference count is correct";
		check(o3->referenceCount() == 1) << "Reference count is correct";

		check(o1->objectClass() == ObjectImplementation::staticClass()) << "Dynamic class is the same as static class";
		check(o2->objectClass() == OtherImplementation::staticClass()) << "Dynamic class is the same as static class";
		check(o3->objectClass() == DerivedImplementation::staticClass()) << "Dynamic class is the same as static class";

		testing("Instantiation");

		ITestInterface::Class * cls1, * cls2, * cls3;
		cls1 = o1->objectClass();
		cls2 = o2->objectClass();
		cls3 = o3->objectClass();

		REF(ITestInterface) o4 = cls1->init();
		REF(ITestInterface) o5 = cls2->init();
		REF(ITestInterface) o6 = cls3->init();

		check(o1->objectType() == o4->objectType()) << "Dynamic instantiation returned correct type";
		check(o2->objectType() == o5->objectType()) << "Dynamic instantiation returned correct type";
		check(o3->objectType() == o6->objectType()) << "Dynamic instantiation returned correct type";

		testing("Inheritance");

		// o3 is a kind of o1
		check(o1->objectType()->isClassOf(o3)) << "Derived is class of parent";
		check(o3->isKindOf(o1->objectType())) << "Derived is class of parent shorthand";
		check(!o1->isKindOf(o3->objectType())) << "Parent is not class of child";
	}
	
	class IPerformanceTest
	{
	public:			
		virtual void callByVal (REF(ITestInterface) testInterface) abstract;
		virtual void callByRef (PTR(ITestInterface) testInterface) abstract;
		virtual void callByPtr (ITestInterface * testInterface) abstract;
		virtual void call () abstract;
	};
	
	class PerformanceTest : public IPerformanceTest
	{
	public:			
		unsigned i;
		
		virtual void callByVal (REF(ITestInterface) testInterface) { i += 1; }
		virtual void callByRef (PTR(ITestInterface) testInterface) { i += 1; }
		virtual void callByPtr (ITestInterface * testInterface) { i += 1; }
		virtual void call () { i += 1; }
	};
	
	UNIT_TEST(IntrusivePtrPerformance)
	{
		using namespace Core;
		
		IPerformanceTest * interface = new PerformanceTest;
		REF(ITestInterface) o1 = ObjectImplementation::staticClass()->init();
		Stopwatch w;
		
		const unsigned COUNT = 10000000;
		
		w.reset();
		
		w.start();
		for (unsigned i = COUNT; i > 0; i -= 1)
			interface->call();
		w.pause();
		
		std::cerr << "Baseline: " << w.time() << " " << (w.time() / COUNT) << "cps" << std::endl;
		TimeT baselineDuration = w.time();
					
		w.reset();
		interface->callByPtr(o1.get());
		
		w.start();
		for (unsigned i = COUNT; i > 0; i -= 1)
			interface->callByPtr(o1.get());
		w.pause();
		
		std::cerr << "   ByPtr: " << w.time() << " " << (w.time() / baselineDuration) * 100.0 << "% of baseline" << std::endl;			
		
		w.reset();
		interface->callByVal(o1);
		
		w.start();
		for (unsigned i = COUNT; i > 0; i -= 1)
			interface->callByVal(o1);
		w.pause();
		
		std::cerr << "   ByVal: " << w.time() << " " << (w.time() / baselineDuration) * 100.0 << "% of baseline" << std::endl;
					
		w.reset();
		interface->callByRef(o1);
		
		w.start();
		for (unsigned i = COUNT; i > 0; i -= 1)
			interface->callByRef(o1);
		w.pause();
		
		std::cerr << "   ByRef: " << w.time() << " " << (w.time() / baselineDuration) * 100.0 << "% of baseline" << std::endl;			
	}
#endif
}
