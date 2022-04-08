//                                               -*- C++ -*-
/**
 *  @brief StatTest implements statistical tests
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
#ifndef OPENTURNS_VISUALTEST_HXX
#define OPENTURNS_VISUALTEST_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/LinearModelResult.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Graph.hxx"
#include "openturns/GridLayout.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Test
 *
 */

namespace VisualTest
{

  /** Draw the QQplot of two Samples when its dimension is 1 */
  OT_API Graph DrawQQplot(const Sample & sample1,
                          const Sample & sample2,
                          const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Distribution-DefaultPointNumber"));


  /** Draw the QQplot of one Sample and one Distribution when its dimension is 1 */
  OT_API Graph DrawQQplot(const Sample & sample,
                          const Distribution & dist);

  /** Draw the CDFplot of two Samples when its dimension is 1 */
  OT_API Graph DrawCDFplot(const Sample & sample1,
                           const Sample & sample2);


  /** Draw the CDFplot of one Sample and one Distribution when its dimension is 1 */
  OT_API Graph DrawCDFplot(const Sample & sample,
                           const Distribution & dist);

  /** Draw the Henry curve for one Sample when its dimension is 1 */
  OT_API Graph DrawHenryLine(const Sample & sample);

  /** Draw the Henry line for a sample and a given normal distribution when its dimension is 1 */
  OT_API Graph DrawHenryLine(const Sample & sample,
                             const Distribution & normal);

  /** Draw 2-d projections of a multivariate sample */
  OT_API GridLayout DrawPairs(const Sample & sample);

  /** Draw 2-d projections of a multivariate sample, plus marginals of a distribution */
  OT_API GridLayout DrawPairsMarginals(const Sample & sample, const Distribution & distribution);

  /** Draw the visual test for a 1D LinearModel */
  OT_API Graph DrawLinearModel(const Sample & sample1,
                               const Sample & sample2,
                               const LinearModelResult & linearModelResult);

  /** Draw the visual test for a 1D LinearModel using the training Samples **/
  OT_API Graph DrawLinearModel(const LinearModelResult & linearModelResult);

  /** Draw the visual test for a 1D LinearModel's residuals */
  OT_API Graph DrawLinearModelResidual(const Sample & sample1,
                                       const Sample & sample2,
                                       const LinearModelResult & linearModelResult);

  /** Draw the visual test for a 1D LinearModel's residuals using the training Samples */
  OT_API Graph DrawLinearModelResidual(const LinearModelResult & linearModelResult);

  /** Draw the parallel coordinates visual test */
  OT_API Graph DrawParallelCoordinates(const Sample & inputSample,
                                       const Sample & outputSample,
                                       const Scalar minValue,
                                       const Scalar maxValue,
                                       const String & color,
                                       const Bool quantileScale = true);

  /** Draw the Kendall plot to assess a copula for a bidimensional sample */
  OT_API Graph DrawKendallPlot(const Sample & data,
                               const Distribution & copula);

  /** Draw the Kendall plot to assess if two bidimensional samples share the same copula */
  OT_API Graph DrawKendallPlot(const Sample & firstSample,
                               const Sample & secondSample);

} /* VisualTest */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_VISUALTEST_HXX */
