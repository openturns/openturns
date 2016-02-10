//                                               -*- C++ -*-
/**
 * @brief CorrelationAnalysis implements computation of correlation coefficients
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_CORRELATIONANALYSIS_HXX
#define OPENTURNS_CORRELATIONANALYSIS_HXX

#include "OTprivate.hxx"
#include "NumericalSample.hxx"
#include "NumericalPoint.hxx"
#include "NumericalMathFunction.hxx"
#include "SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CorrelationAnalysis
 *
 */

class OT_API CorrelationAnalysis
{
public:

  /** Compute the Pearson correlation coefficient between the component number index of the input sample and the 1D outputSample */
  static NumericalScalar PearsonCorrelation(const NumericalSample & inputSample,
      const NumericalSample & outputSample,
      const UnsignedInteger index = 0);

  /** Compute the Spearman correlation coefficient between the component number index of the input sample and the 1D outputSample */
  static NumericalScalar SpearmanCorrelation(const NumericalSample & inputSample,
      const NumericalSample & outputSample,
      const UnsignedInteger index = 0);

  /** Compute the Standard Regression Coefficients (SRC) between the input sample and the output sample */
  static NumericalPoint SRC(const NumericalSample & inputSample,
                            const NumericalSample & outputSample);

  /** Compute the Partial Correlation Coefficients (PCC) between the input sample and the output sample */
  static NumericalPoint PCC(const NumericalSample & inputSample,
                            const NumericalSample & outputSample);

  /** Compute the Standard Rank Regression Coefficients (SRRC) between the input sample and the output sample */
  static NumericalPoint SRRC(const NumericalSample & inputSample,
                             const NumericalSample & outputSample);

  /** Compute the Partial Rank Correlation Coefficients (PRCC) between the input sample and the output sample */
  static NumericalPoint PRCC(const NumericalSample & inputSample,
                             const NumericalSample & outputSample);

private:
  /** Default constructor */
  CorrelationAnalysis();

}; /* class CorrelationAnalysis */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CORRELATIONANALYSIS_HXX */
