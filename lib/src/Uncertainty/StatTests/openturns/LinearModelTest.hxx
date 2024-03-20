//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
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
#ifndef OPENTURNS_LINEARMODELTEST_HXX
#define OPENTURNS_LINEARMODELTEST_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TestResult.hxx"
#include "openturns/Sample.hxx"
#include "openturns/LinearModelResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearModelTest
 *
 */

namespace LinearModelTest
{
typedef Collection<TestResult>      TestResultCollection;

OT_API TestResult LinearModelFisher(const Sample & firstSample,
                                    const Sample & secondSample,
                                    const LinearModelResult & linearModelResult,
                                    const Scalar level = ResourceMap::GetAsScalar( "LinearModelTest-DefaultLevel" ));

/**  */
OT_API TestResult LinearModelFisher(const Sample & firstSample,
                                    const Sample & secondSample,
                                    const Scalar level = ResourceMap::GetAsScalar( "LinearModelTest-DefaultLevel" ));

/**  */
OT_API TestResult LinearModelResidualMean(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModelResult & linearModelResult,
    const Scalar level = ResourceMap::GetAsScalar( "LinearModelTest-DefaultLevel" ));

/**  */
OT_API TestResult LinearModelResidualMean(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level = ResourceMap::GetAsScalar( "LinearModelTest-DefaultLevel" ));

/**  */
OT_API TestResult LinearModelHarrisonMcCabe(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModelResult & linearModelResult,
    const Scalar level = ResourceMap::GetAsScalar( "LinearModelTest-DefaultLevel" ),
    const Scalar breakPoint = ResourceMap::GetAsScalar( "LinearModelTest-DefaultHarrisonMcCabeBreakpoint" ),
    const Scalar simulationSize = ResourceMap::GetAsUnsignedInteger( "LinearModelTest-DefaultHarrisonMcCabeSimulationSize" ));

/**  */
OT_API TestResult LinearModelHarrisonMcCabe(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level = ResourceMap::GetAsScalar( "LinearModelTest-DefaultLevel" ),
    const Scalar breakPoint = ResourceMap::GetAsScalar( "LinearModelTest-DefaultHarrisonMcCabeBreakpoint" ),
    const Scalar simulationSize = ResourceMap::GetAsUnsignedInteger( "LinearModelTest-DefaultHarrisonMcCabeSimulationSize" ));

/**  */
OT_API TestResult LinearModelBreuschPagan(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModelResult & linearModelResult,
    const Scalar level = ResourceMap::GetAsScalar( "LinearModelTest-DefaultLevel" ));

/**  */
OT_API TestResult LinearModelBreuschPagan(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level = ResourceMap::GetAsScalar( "LinearModelTest-DefaultLevel" ));

/**  */
OT_API TestResult LinearModelDurbinWatson(const Sample & firstSample,
    const Sample & secondSample,
    const LinearModelResult & linearModelResult,
    const String hypothesis = ResourceMap::GetAsString( "LinearModelTest-DefaultDurbinWatsonHypothesis" ),
    const Scalar level = ResourceMap::GetAsScalar( "LinearModelTest-DefaultLevel" ));

/**  */
OT_API TestResult LinearModelDurbinWatson(const Sample & firstSample,
    const Sample & secondSample,
    const String hypothesis = ResourceMap::GetAsString( "LinearModelTest-DefaultDurbinWatsonHypothesis" ),
    const Scalar level = ResourceMap::GetAsScalar( "LinearModelTest-DefaultLevel" ));

/** Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), PartialRegression performs the Regression test simultaneously on all firstSample[i] and secondSample, for i in the selection. The Regression test tests ifthe regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The t-test is used. */
OT_API TestResultCollection PartialRegression(const Sample & firstSample,
    const Sample & secondSample,
    const Indices & selection,
    const Scalar level = 0.05);

/** Regression test between 2 samples : firstSample of dimension n and secondSample of dimension 1. If firstSample[i] is the numerical sample extracted from firstSample (ith coordinate of each point of the numerical sample), FullRegression performs the Regression test simultaneously on all firstSample[i] and secondSample. The Regression test tests ifthe regression model between two scalar numerical samples is significant. It is based on the deviation analysis of the regression. The t-test is used. */
OT_API TestResultCollection FullRegression(const Sample & firstSample,
    const Sample & secondSample,
    const Scalar level = 0.05);

}; /* LinearModelTest */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARMODELTEST_HXX */
