//                                               -*- C++ -*-
/**
 *  @brief The test file of class Pointer for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

class TestClass : public PersistentObject
{
public:
  explicit TestClass(const String & name) : PersistentObject()
  {
    setName(name);
    //     fullprint << "TestClass(const String & name) ("
    //                << getName() << ")" << std::endl;
  }
  explicit TestClass(const TestClass & other) : PersistentObject(other)
  {
    //     fullprint << "TestClass(const String & name) ("
    //                << getName() << ")" << std::endl;
  }
  virtual ~TestClass()
  {
    //     fullprint << "~TestClass ("
    //                << getName() << ")" << std::endl;
  }
  virtual TestClass * clone() const
  {
    return new TestClass(*this);
  }
};

void printPointer(const Pointer<TestClass> & p)
{
  OStream fullprint(std::cout);

  /** Check pointee accesssor through operator * */
  fullprint << "Object name is : " << (*p).getName() << std::endl;

  /** Check pointee accesssor through operator -> */
  fullprint << "Object name is : " << p->getName() << std::endl;

  /** Is Pointer object uniquely pointing to the pointee */
  fullprint << "Unicity : " << p.unique() << std::endl;

  /** How many Pointer objects do share the pointee */
  fullprint << "Count : " << p.use_count() << std::endl;
}

void printPointee(const Pointer<TestClass> & p)
{
  OStream fullprint(std::cout);

  /** Check operator * */
  fullprint << "p -> " << (*p) << std::endl;
}

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  String name1("One");
  String name2("Two");
  String name3("Three");
  String name4("Four");

  /** Check constructor */
  Pointer<TestClass> p1(new TestClass(name1));
  printPointee(p1);
  printPointer(p1);



  /** Check copy constructor */
  Pointer<TestClass> p2(p1);
  printPointee(p2);
  printPointer(p2);



  /** Check operator = */
  Pointer<TestClass> p3;
  p3 = p1;
  printPointee(p3);
  printPointer(p3);



  /** We try to access pointee by const methods */
  String p3OldName = p3->getName();



  /** Now we change the pointee object (copy-on-write strategy) */
  p3->setName(name2);
  printPointee(p1);
  printPointer(p1);
  printPointee(p2);
  printPointer(p2);
  printPointee(p3);
  printPointer(p3);


  /** Now we reset one Pointer object */
  p2.reset(new TestClass(name3));
  printPointee(p1);
  printPointer(p1);
  printPointee(p2);
  printPointer(p2);
  printPointee(p3);
  printPointer(p3);


  /** Now we reset one Pointer object */
  p2.reset(); // p2 should not be dereferenced until reaffected again
  printPointee(p1);
  printPointer(p1);
  //   printPointee(p2);
  //   printPointer(p2);
  printPointee(p3);
  printPointer(p3);




  /** At last, destructor */

  return ExitCode::Success;
}
