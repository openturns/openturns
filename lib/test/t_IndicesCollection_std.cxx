//                                               -*- C++ -*-
/**
 *  @brief The test file of class Collection for standard methods
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

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  /* Default constructor */
  IndicesCollection flat1;
  fullprint << "flat1=" << flat1 << std::endl;

  /* Constructor with Collection<Indices> */
  Collection<Indices> coll2;
  Indices ind1(1);
  Indices ind4(4);
  ind4.fill(8);
  Indices ind3(3);
  ind3.fill(3);
  coll2.add(ind1);
  coll2.add(ind4);
  coll2.add(ind3);
  IndicesCollection flat2(coll2);

  fullprint << "flat2=" << flat2 << std::endl;

  /* Constructor with Collection<Indices>, strided */
  Collection<Indices> coll3;
  coll3.add(ind3);
  coll3.add(ind3);
  coll3.add(ind3);
  IndicesCollection flat3(coll3);

  fullprint << "flat3=" << flat3 << std::endl;

  /* Constructor with dimensions and values */
  Indices full(16);
  full.fill();
  IndicesCollection flat4(8, 2, full);

  fullprint << "flat4=" << flat4 << std::endl;

  /* Copy constructor */
  IndicesCollection flat5(flat2);

  fullprint << "flat5=" << flat5 << std::endl;

  /* Iterator */
  Indices x4;
  for (IndicesCollection::const_iterator cit = flat4.cbegin_at(3), guard = flat4.cend_at(3); cit != guard; ++cit)
  {
    x4.add(*cit);
  }
  fullprint << "flat4[3]=" << x4 << std::endl;

  Indices x5;
  for (IndicesCollection::const_iterator cit = flat5.cbegin_at(1), guard = flat5.cend_at(1); cit != guard; ++cit)
  {
    x5.add(*cit);
  }
  fullprint << "flat5[1]=" << x5 << std::endl;
  return ExitCode::Success;
}
