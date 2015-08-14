//                                               -*- C++ -*-
/**
 *  @brief The test file of class LibraryLoader for standard methods
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "OT.hxx"
#include "OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;


static String GetSharedLibrarySuffix()
{
#ifdef _WIN32
  return ".dll";
#elif defined(__APPLE__)
  return ".dylib";
#else
  return ".so";
#endif
}


int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    const char * envvar = std::getenv("OPENTURNS_WRAPPER_PATH");
    const String path(envvar);

    /** Instance creation */
    LibraryLoader & loader = LibraryLoader::GetInstance();

    try
    {
      String libraryPath1 = path + Os::GetDirectorySeparator() + "testwrapper_1" + GetSharedLibrarySuffix();
      Library library1 = loader.load( libraryPath1.c_str() );
      fullprint << loader  << std::endl;
      fullprint << library1 << std::endl;

      OT::LibrarySymbol symbol1 = library1.getSymbol("fonction1");
      fullprint << "Symbol found at address " << symbol1 << std::endl;

      // reload the same library
      loader.load( libraryPath1.c_str() );
      fullprint << loader << std::endl;

      String libraryPath2 = path + Os::GetDirectorySeparator() + "testwrapper_2" + GetSharedLibrarySuffix();
      Library library2 = loader.load( libraryPath2.c_str() );
      fullprint << loader  << std::endl;
      fullprint << library2 << std::endl;

      OT::LibrarySymbol symbol2 = library2.getSymbol("fonction2");
      fullprint << "Symbol found at address " << symbol2 << std::endl;

    }
    catch (DynamicLibraryException & ex)
    {
      throw TestFailed(ex.__repr__());
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
