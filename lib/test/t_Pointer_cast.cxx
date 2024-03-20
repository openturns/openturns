//                                               -*- C++ -*-
/**
 *  @brief The test file of class Pointer for cast
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

class BaseClass : public PersistentObject
{
public:
  BaseClass(const String & name) : PersistentObject()
  {
    OStream fullprint(std::cout);

    setName(name);
    fullprint << "BaseClass(const String & name) ("
              << getName() << ")" << std::endl;
  }
  BaseClass(const BaseClass & other) : PersistentObject(other)
  {
    OStream fullprint(std::cout);

    fullprint << "BaseClass(const BaseClass & other) ("
              << getName() << ")" << std::endl;
  }
  virtual ~BaseClass()
  {
    OStream fullprint(std::cout);

    fullprint << "~BaseClass ("
              << getName() << ")" << std::endl;
  }
  virtual void doit() const
  {
    OStream fullprint(std::cout);

    fullprint << "BaseClass::doit ("
              << getName() << ")" << std::endl;
  }
  virtual BaseClass * clone() const
  {
    return new BaseClass(*this);
  }
};




class DerivedClass : public BaseClass
{
public:
  explicit DerivedClass(const String & name) : BaseClass(name)
  {
    OStream fullprint(std::cout);

    fullprint << "DerivedClass(const String & name) ("
              << getName() << ")" << std::endl;
  }
  explicit DerivedClass(const DerivedClass & other) : BaseClass(other)
  {
    OStream fullprint(std::cout);

    fullprint << "DerivedClass(const String & name) ("
              << getName() << ")" << std::endl;
  }
  virtual ~DerivedClass()
  {
    OStream fullprint(std::cout);

    fullprint << "~DerivedClass ("
              << getName() << ")" << std::endl;
  }
  virtual void doit() const
  {
    OStream fullprint(std::cout);

    fullprint << "DerivedClass::doit ("
              << getName() << ")" << std::endl;
  }
  virtual DerivedClass * clone() const
  {
    return new DerivedClass(*this);
  }
};



class NotDerivedClass : public PersistentObject
{
public:
  explicit NotDerivedClass(const String & name) : PersistentObject()
  {
    setName(name);
  }
  explicit NotDerivedClass(const NotDerivedClass & other) : PersistentObject(other)
  {
  }
  virtual ~NotDerivedClass()
  {
  }
  virtual void doit() const
  {
    OStream fullprint(std::cout);

    fullprint << "NotDerivedClass::doit ("
              << getName() << ")" << std::endl;
  }
  virtual NotDerivedClass * clone() const
  {
    return new NotDerivedClass(*this);
  }
};



void functionThatCastPointer(const Pointer<BaseClass> & p_base)
{
  OStream fullprint(std::cout);

  fullprint << "void functionThatCastPointer(const Pointer<BaseClass> & p_base)" << std::endl;
  p_base->doit();
}

Pointer<BaseClass> functionThatReturnPointer(const Pointer<DerivedClass> & p_derived)
{
  OStream fullprint(std::cout);

  fullprint << "Pointer<BaseClass> functionThatReturnPointer(const Pointer<DerivedClass> & p_derived)" << std::endl;
  p_derived->doit();
  return p_derived;
}


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  {
    /** We create a pointer onto a BaseClass object */
    Pointer<BaseClass> p_base = new BaseClass("B1");
    p_base->doit();

    /** We create a pointer onto a DerivedClass object */
    Pointer<DerivedClass> p_derived = new DerivedClass("D1");
    p_derived->doit();

    /** We create a pointer onto a BaseClass object but we put a DerivedClass object into it */
    p_base = new DerivedClass("D2");
    p_base->doit();


    /** We try to pass a Derived Pointer to a function that except a Base Pointer */
    functionThatCastPointer(p_derived);

    /** We try to get a Base Pointer from a function that except a Derived Pointer */
    p_base = functionThatReturnPointer(p_derived);
    p_base->doit();
  }



  /** Now we try to pass a NotDerived Pointer  to a function that except a Base Pointer */
  /** THIS SHOULD NOT WORK */
  Pointer<NotDerivedClass> p_notderived = new NotDerivedClass("ND1");
  // functionThatCastPointer(p_notderived); // ERROR : cannot convert `NotDerivedClass*' to `BaseClass*'


  /** We cast a Derived Pointer to a Base Pointer to a Derived Pointer again */
  {
    /** We create a pointer onto a DerivedClass object */
    Pointer<DerivedClass> p_derived = new DerivedClass("D3");
    p_derived->doit();

    /** Then the pointer to a Base class */
    Pointer<BaseClass> p_base(p_derived);
    p_base->doit();

    /** Then again a pointer to a derived class */
    Pointer<DerivedClass> p_rederived;
    p_rederived.assign(p_base);
    p_rederived->doit();
  }


  /** At last, destructor */

  return ExitCode::Success;
}
