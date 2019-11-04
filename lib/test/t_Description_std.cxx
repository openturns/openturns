//                                               -*- C++ -*-
/**
 *  @brief The test file of class Description for standard methods
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

  String val1, val2;

  /* Default constructor */
  Description desc1;
  const Description & ref_desc1(desc1);

  /* Check method add() */
  desc1.add("X");
  desc1.add("Y");

  UnsignedInteger size = desc1.getSize();
  fullprint << "size of desc1 = " << size << std::endl;

  val1 = ref_desc1[0];
  val2 = ref_desc1[1];
  fullprint << "desc1[0] = " << val1 << std::endl
            << "desc1[1] = " << val2 << std::endl;


  /* Constructor with size */
  Description desc2(2);
  const Description & ref_desc2(desc2);

  /* Check operator[] methods */
  desc2[0] = "a";
  desc2[1] = "b";

  val1 = ref_desc2[0];
  val2 = ref_desc2[1];
  fullprint << "desc2[0] = " << val1 << std::endl
            << "desc2[1] = " << val2 << std::endl;


  /* Copy constructor */
  Description desc3(desc1);
  const Description & ref_desc3(desc3);

  val1 = ref_desc3[0];
  val2 = ref_desc3[1];
  fullprint << "desc3[0] = " << val1 << std::endl
            << "desc3[1] = " << val2 << std::endl;


  /* Assignment operator */
  Description desc4;
  const Description & ref_desc4(desc4);
  desc4 = ref_desc2;

  val1 = ref_desc4[0];
  val2 = ref_desc4[1];
  fullprint << "desc4[0] = " << val1 << std::endl
            << "desc4[1] = " << val2 << std::endl;

  /* Stream operator */
  fullprint << "desc1 = " << ref_desc1 << std::endl;

  /* sort method and copy on write mecanism */
  Description desc5(3);
  desc5[0] = "c";
  desc5[1] = "b";
  desc5[2] = "a";
  Description desc5copy(desc5);

  std::sort(desc5copy.begin(), desc5copy.end());
  fullprint << "desc5copy = " << desc5copy << std::endl;
  fullprint << "desc5 = " << desc5 << std::endl;

  return ExitCode::Success;
}
