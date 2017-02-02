//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_NORMALITYTEST_HXX
#define OPENTURNS_NORMALITYTEST_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TestResult.hxx"
#include "openturns/NumericalSample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NormalityTest
 *
 */

class OT_API NormalityTest
{
public:

  /** Anderson Darling normality test for normal distributions. */
  static TestResult AndersonDarlingNormal(const NumericalSample & sample,
                                          const NumericalScalar level = 0.95);

  /** Cramer Von Mises normality test for normal distributions. */
  static TestResult CramerVonMisesNormal(const NumericalSample & sample,
                                         const NumericalScalar level = 0.95);

private:
  NormalityTest();
}; /* class NormalityTest */

END_NAMESPACE_OPENTURNS
#endif /* OPENTURNS_NORMALITYTEST_HXX */
