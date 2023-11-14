//                                               -*- C++ -*-
/**
 *  @brief The test file of class QuadrantSampling for standard methods
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

int main(int, char *[])
{
//   Log::Show(Log::ALL);

  OStream fullprint(std::cout);

  for (UnsignedInteger d = 3; d < 4; ++ d)
  {
    RandomDirection rd(d);
    for (UnsignedInteger q = 0; q < static_cast<UnsignedInteger>(1 << d); ++ q)
    {
      QuadrantSampling qs(rd, q);
      fullprint << "quadrant=" << q << std::endl;
      for (UnsignedInteger i = 0; i < 10; ++ i)
      {
        fullprint << "  sample=" << qs.generate()[0].getCollection() << std::endl;
      }
    }

    // lets chose (-1,...,-1) for first quadrant bisector
    Point bisector(d, -1.);
    for (UnsignedInteger q = 0; q < static_cast<UnsignedInteger>(1 << d); ++ q)
    {

      QuadrantSampling qs(rd, q, bisector);
      fullprint << "quadrant=" << q << " bisector=" << bisector.getCollection() << std::endl;
      for (UnsignedInteger i = 0; i < 10; ++ i)
      {
        fullprint << "  sample=" << qs.generate()[0].getCollection() << std::endl;
      }
    }

    // let's stratify only the 2 last dims
    UnsignedInteger sDim = 2;
    Indices strataIndices(sDim);
    strataIndices.fill();

    for (UnsignedInteger q = 0; q < static_cast<UnsignedInteger>(1 << sDim); ++ q)
    {
      QuadrantSampling qs(rd, q);
      qs.setStrataIndices(strataIndices);
      fullprint << "quadrant=" << q << " strataIndices=" << strataIndices << std::endl;
      for (UnsignedInteger i = 0; i < 10; ++ i)
      {
        fullprint << "  sample=" << qs.generate()[0].getCollection() << std::endl;
      }
    }

  }
  return 0;
}

