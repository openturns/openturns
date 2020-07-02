//                                               -*- C++ -*-
/**
 *  @brief The header file that declares exit codes for tests
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_OTTESTCODE_HXX
#define OPENTURNS_OTTESTCODE_HXX

#include "openturns/OTconfig.hxx"

#include <exception>
#include <string>         // for std::string
#include <sstream>        // for std::ostringstream
#include <cstdlib>        // for exit codes
#include <cstring>        // for strcmp

#include "openturns/RandomGenerator.hxx"
#include "openturns/OStream.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/PlatformInfo.hxx"

#define TESTPREAMBLE { OT::PlatformInfo::SetTwoDigitExponent(); }

BEGIN_NAMESPACE_OPENTURNS

/**
 * @brief      The namespace for test specific definitions
 */

namespace Test
{

/**
 * @typedef int ExitCodeValue
 * ExitCodeValue is the type of the exit code to the operating
 * system. The actual type is determined at configuration time.
 */
typedef int ExitCodeValue;


/**
 * @class ExitCode
 * Class ExitCode is a compound that declares exit codes to the
 * operating system when doing the test suite of the project.
 * Each code has a special meaning to the operating system, and
 * precisely to the compilation tools (automake et al.) that use
 * them to know if the test succeeded or failed.
 */

class ExitCode
{

public:
  /** You should return Success when it's OK */
  static const ExitCodeValue Success = EXIT_SUCCESS;

  /** You should return Error when something went wrong */
  static const ExitCodeValue Error = EXIT_FAILURE;

  /** You should return ExpectedToFail when it failed as it should */
  static const ExitCodeValue ExpectedToFail = 77;

}

; /* class ExitCode */





/**
 * Function parseOptions analyse what the user put
 * on the command line of the executable.
 */
inline void parseOptions(int argc, char *argv[])
{
  for(int i = 1; i < argc; ++ i)
  {
    if (!strcmp(argv[i], "--version"))
    {
      std::cout << argv[0] << " version " << PACKAGE_VERSION
                << " (copyright 2005-2010 " << PACKAGE_NAME << ")"
                << std::endl;
      exit(ExitCode::Success);
    }
  } /* end for */

#if defined(_MSC_VER) && (_MSC_VER < 1900)
  // Windows displays scientific notation with 3 digits in the exponent.
  // We force 2 digits to avoid test failures.
  _set_output_format(_TWO_DIGIT_EXPONENT);
#endif
}



/**
 * Random generator initialization
 */
inline void setRandomGenerator()
{
  RandomGenerator::SetSeed(0);
}





/**
 * @class TestFailed
 *
 * Class TestFailed is an exception derived class that should be
 * used to throw exception in unitary test code.
 */
class TestFailed : public std::exception
{
public:
  TestFailed(const std::string & message) : message_(message) {}
  ~TestFailed() throw() {}
  const char * what() const throw()
  {
    return message_.c_str();
  }

private:

  const std::string message_;
};

inline std::ostream & operator <<(std::ostream & os, const TestFailed & obj)
{
  return os << "*** EXCEPTION ***" << std::endl
         << "TestFailed : " << obj.what() << std::endl
         << "*****************";
}


/**
 * @fn streamObject(const T & anObject)
 *
 * Try to stream an object onto an ostream object (std::cout).
 * This method tests the operator << of the object.
 */
template <class T>
void streamObject(const T & anObject)
{
  std::cout << "streamObject(const T & anObject)"
            << std::endl;
  OT::OStream fullprint(std::cout);
  fullprint << anObject << std::endl;
}


/**
 * @fn show_className()
 *
 * Try to get the name of the class. Check the GetClassName static
 * method.
 */
template <class T>
void showClassName()
{
  std::cout << "Testing class "
            << T::GetClassName()
            << std::endl;
}


/**
 * @fn checkConstructorAndDestructor()
 *
 * Try to instanciate an object and delete it. This method tests the
 * default constructor and the destructor ot the class.
 */
template <class T>
void checkConstructorAndDestructor()
{
  std::cout << "checkConstructorAndDestructor()"
            << std::endl;

  // Call the default constructor
  T anObject;
}


/**
 * @fn checkCopyConstructor()
 *
 * Try to instanciate an object and copy-construct it.
 */
template <class T>
void checkCopyConstructor()
{
  std::cout << "checkCopyConstructor()"
            << std::endl;

  // Call the default constructor
  T anObject;

  // Call the copy-contructor
  T anCopiedObject( anObject );
}


/**
 * @fn areSameObjects(const T & firstObject, const T & secondObject)
 *
 * Try to compare two objects supposed to be identical.
 * This method tests the operator == of the object.
 */
template <class T>
OT::Bool areSameObjects(const T & firstObject,
                        const T & secondObject)
{
  std::cout << "areSameObjects(const T & firstObject, const T & secondObject)"
            << std::endl;

  return (firstObject == secondObject);
}


/**
 * @fn areDifferentObjects(const T & firstObject, const T & secondObject)
 *
 * Try to compare two objects supposed to be different.
 * This method tests the operator != of the object.
 */
template <class T>
OT::Bool areDifferentObjects(const T & firstObject,
                             const T & secondObject)
{
  std::cout << "areDifferentObjects(const T & firstObject, const T & secondObject)"
            << std::endl;

  return (firstObject != secondObject);
}


/**
 * @fn checkClassWithoutClassName()
 *
 * Try to check the standard functionnalities of the class
 */
template <class T>
void checkClassWithoutClassName()
{
  checkConstructorAndDestructor<T>();
  checkCopyConstructor<T>();

  T object1;
  streamObject<T>(object1);

  T object2(object1);
  streamObject<T>(object2);

  if (! areSameObjects<T>(object1, object2))
  {
    throw TestFailed("areSameObjects<T>(object1, object2)");
  }

  if (areDifferentObjects<T>(object1, object2))
  {
    throw TestFailed("areDifferentObjects<T>(object1, object2)");
  }
}


/**
 * @fn checkClassWithClassName()
 *
 * Try to check some basic functionnalities of the class
 */
template <class T>
void checkClassWithClassName()
{
  showClassName<T>();
  checkClassWithoutClassName<T>();
}


inline void assert_almost_equal(Scalar a, Scalar b, Scalar rtol = 1.0e-5, Scalar atol = 1.0e-8)
{
  if (std::abs(a - b) > atol + rtol * std::abs(b) )
  {
    throw TestFailed(OSS() << "Value " << a << " is not close enough to " << b);
  }
}


inline void assert_almost_equal(const Point & a, const Point & b, Scalar rtol = 1.0e-5, Scalar atol = 1.0e-8)
{
  if (a.getDimension() != b.getDimension())
    throw InvalidArgumentException(HERE) << "A and B must have the same dimension";
  const UnsignedInteger dimension = a.getDimension();
  for (UnsignedInteger j = 0; j < dimension; ++ j )
  {
    assert_almost_equal(a[j], b[j], rtol, atol);
  }
}


inline void assert_almost_equal(const Sample & a, const Sample & b, Scalar rtol = 1.0e-5, Scalar atol = 1.0e-8)
{
  if (a.getSize() != b.getSize())
    throw InvalidArgumentException(HERE) << "A and B must have the same size";
  if (a.getDimension() != b.getDimension())
    throw InvalidArgumentException(HERE) << "A and B must have the same dimension";
  const UnsignedInteger size = a.getSize();
  const UnsignedInteger dimension = a.getDimension();
  for ( UnsignedInteger i = 0; i < size; ++ i )
  {
    for (UnsignedInteger j = 0; j < dimension; ++ j )
    {
      assert_almost_equal(a(i, j), b(i, j), rtol, atol);
    }
  }
}

inline void assert_almost_equal(const Matrix &a, const Matrix &b, Scalar rtol = 1.0e-5, Scalar atol = 1.0e-8)
{
  if (a.getNbRows() != b.getNbRows())
    throw InvalidArgumentException(HERE) << "A and B must have the same row number";
  if (a.getNbColumns() != b.getNbColumns())
    throw InvalidArgumentException(HERE) << "A and B must have the same column number";
  const UnsignedInteger rows = a.getNbRows();
  const UnsignedInteger columns = a.getNbColumns();

  for (UnsignedInteger j = 0; j < columns; ++j)
  {
    for (UnsignedInteger i = 0; i < rows; ++i)
    {
      assert_almost_equal(a(i, j), b(i, j), rtol, atol);
    }
  }
}

} /* namespace Test */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OTTESTCODE_HXX */
