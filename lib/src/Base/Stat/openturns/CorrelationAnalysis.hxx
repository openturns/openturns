//                                               -*- C++ -*-
/**
 * @brief CorrelationAnalysis implements computation of correlation coefficients
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_CORRELATIONANALYSIS_HXX
#define OPENTURNS_CORRELATIONANALYSIS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Point.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CorrelationAnalysis
 *
 */

namespace CorrelationAnalysis
{
  /** Compute the Pearson correlation coefficient between the input sample and the output sample */
  OT_API Point PearsonCorrelation(const Sample & inputSample,
                                  const Sample & outputSample);

  /** Compute the Spearman correlation coefficient between the input sample and the output sample */
  OT_API Point SpearmanCorrelation(const Sample & inputSample,
                                   const Sample & outputSample);

  /** Compute the Standard Regression Coefficients (SRC) between the input sample and the output sample */
  OT_API Point SRC(const Sample & inputSample,
                   const Sample & outputSample,
                   const Bool normalize = false);

  /** Compute the signed Standard Regression Coefficients (SRC) between the input sample and the output sample */
  OT_API Point SignedSRC(const Sample & inputSample,
                         const Sample & outputSample);

  /** Compute the Partial Correlation Coefficients (PCC) between the input sample and the output sample */
  OT_API Point PCC(const Sample & inputSample,
                   const Sample & outputSample);

  /** Compute the Standard Rank Regression Coefficients (SRRC) between the input sample and the output sample */
  OT_API Point SRRC(const Sample & inputSample,
                    const Sample & outputSample,
                    const Bool normalize = false);

  /** Compute the Partial Rank Correlation Coefficients (PRCC) between the input sample and the output sample */
  OT_API Point PRCC(const Sample & inputSample,
                    const Sample & outputSample);

}; /* CorrelationAnalysis */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CORRELATIONANALYSIS_HXX */
