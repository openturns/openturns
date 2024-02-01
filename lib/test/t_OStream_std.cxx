//                                               -*- C++ -*-
/**
 *  @brief The test file of class OStream for pretty printing of Objects
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

class MyTest : public Object
{
public:
  virtual String __repr__() const
  {
    return String("MyTest class");
  }
  virtual String __str__(const String & )  const
  {
    return String("What a beautiful print !");
  }
};

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream prettyprint(std::cout);

  try
  {
    // Test basic functionnalities
    //checkClassWithClassName<TestObject>();

    MyTest obj;
    std::cout << obj    << "\n";
    std::cout << true   << "\n";
    std::cout << 23     << "\n";
    std::cout << 100.23 << "\n";
    std::cout << 'z'    << "\n";
    std::cout << "ch"   << "\n";
    //std::cout << &obj   << "\n";

    OStream fullprint(std::cout);
    fullprint << obj    << "\n";
    fullprint << true   << "\n";
    fullprint << 23     << "\n";
    fullprint << 100.23 << "\n";
    fullprint << 'z'    << "\n";
    fullprint << "ch"   << "\n";
    //fullprint << &obj   << "\n";


  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
