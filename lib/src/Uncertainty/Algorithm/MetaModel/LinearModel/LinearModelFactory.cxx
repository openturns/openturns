//                                               -*- C++ -*-
/**
 *  @brief Factory for linear model
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/LinearModelFactory.hxx"
#include "openturns/LinearModel.hxx"
#include "openturns/LinearModelAlgorithm.hxx"
#include "openturns/LinearModelAnalysis.hxx"


BEGIN_NAMESPACE_OPENTURNS

/* Constructor */
LinearModelFactory::LinearModelFactory()
{
  LOGWARN(OSS() << "LinearModelFactory is deprecated, use LinearModelAlgorithm");
}

/* LinearModel creation */
LinearModel LinearModelFactory::build(const Sample & samplePred,
                                      const Sample & sampleLab,
                                      const Scalar levelValue) const
{
  LinearModelAlgorithm algo(samplePred, sampleLab);
  const LinearModelResult result(algo.getResult());
  const LinearModelAnalysis analysis(result);
  const Interval confidenceIntervals(analysis.getCoefficientsConfidenceInterval(levelValue));
  const Point regression(result.getTrendCoefficients());
  const LinearModel::ScalarCollection pValues(analysis.getCoefficientsPValues());
  return LinearModel(regression, confidenceIntervals, pValues);
}

END_NAMESPACE_OPENTURNS

