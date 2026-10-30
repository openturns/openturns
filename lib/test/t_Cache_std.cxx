//                                               -*- C++ -*-
/**
 *  @brief The test file of class Cache for standard methods
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

typedef PersistentCollection<Scalar> KeyType;
typedef PersistentCollection<Scalar> ValueType;
typedef Cache<KeyType, ValueType>              TestCache;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {

    // Create a Cache object
    TestCache myCache(3);
    myCache.setName("aCache");
    myCache.enable();

    fullprint << "myCache = " << myCache << std::endl;

    // Create a point and add it to the cache
    KeyType k(3);
    k[0] = 1.;
    k[1] = 2.;
    k[2] = 3.;
    ValueType v(2);
    v[0] = 10.;
    v[1] = 20.;
    myCache.add( k, v );

    fullprint << "myCache = " << myCache << std::endl;

    // Create a fake point
    KeyType ko(3);
    ko[0] = -1.;
    ko[1] = -2.;
    ko[2] = -3.;

    // Create a valid point
    KeyType ok(k);

    // Check which point is in the cache and retrieve corresponding values
    fullprint << "Is ko in myCache ? " << myCache.hasKey( ko ) << std::endl;
    fullprint << "Cache value for ko = " << myCache.find( ko ) << std::endl;

    fullprint << "Is ok in myCache ? " << myCache.hasKey( ok ) << std::endl;
    fullprint << "Cache value for ok = " << myCache.find( ok ) << std::endl;

    fullprint << "myCache = " << myCache << std::endl;

    // Add some more points in the cache
    KeyType p1(k);
    ValueType pv1(v);
    for (UnsignedInteger i = 0; i < p1.getSize(); ++i) p1[i] *= 2.;
    for (UnsignedInteger i = 0; i < pv1.getSize(); ++i) pv1[i] *= 2.;
    myCache.add( p1, pv1 );
    fullprint << "myCache = " << myCache << std::endl;

    KeyType p2(k);
    ValueType pv2(v);
    for (UnsignedInteger i = 0; i < p2.getSize(); ++i) p2[i] *= 3.;
    for (UnsignedInteger i = 0; i < pv2.getSize(); ++i) pv2[i] *= 3.;
    myCache.add( p2, pv2 );
    fullprint << "myCache = " << myCache << std::endl;

    KeyType p3(k);
    ValueType pv3(v);
    for (UnsignedInteger i = 0; i < p3.getSize(); ++i) p3[i] *= 4.;
    for (UnsignedInteger i = 0; i < pv3.getSize(); ++i) pv3[i] *= 4.;
    myCache.add( p3, pv3 );
    fullprint << "myCache = " << myCache << std::endl;

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
