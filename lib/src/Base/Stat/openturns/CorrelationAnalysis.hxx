//                                               -*- C++ -*-
/**
 * @brief CorrelationAnalysis implements computation of correlation coefficients
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

#ifndef OPENTURNS_CORRELATIONANALYSIS_HXX
#define OPENTURNS_CORRELATIONANALYSIS_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Point.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CorrelationAnalysis
 *
 */

class OT_API CorrelationAnalysis
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  CorrelationAnalysis();

  /** Standard constructor */
  CorrelationAnalysis(const Sample & inputSample,
                      const Sample & outputSample);

  /** Virtual constructor */
  CorrelationAnalysis * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Compute the Pearson correlation coefficient between the input sample and the output sample */
  const Point computePearsonCorrelation();

  /** Compute the Spearman correlation coefficient between the input sample and the output sample */
  const Point computeSpearmanCorrelation();

  /** Compute the Standard Regression Coefficients (SRC) between the input sample and the output sample */
  const Point computeSRC();

  /** Compute the squared Standard Regression Coefficients (SRC) between the input sample and the output sample */
  const Point computeSquaredSRC(const Bool normalize = false);

  /** Compute the Partial Correlation Coefficients (PCC) between the input sample and the output sample */
  const Point computePCC();

  /** Compute the Standard Rank Regression Coefficients (SRRC) between the input sample and the output sample */
  const Point computeSRRC();

  /** Compute the Partial Rank Correlation Coefficients (PRCC) between the input sample and the output sample */
  const Point computePRCC();

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  // computePearsonCorrelation with arguments
  const Point computePearsonCorrelation(const Sample & inputSample,
                                        const Sample & outputSample);

  // computePCC with arguments
  const Point computePCC(const Sample & inputSample,
                         const Sample & outputSample);

  // computeSRC with arguments
  const Point computeSRC(const Sample & inputSample,
                         const Sample & outputSample);
private:

  Sample inputSample_;
  Sample outputSample_;

}; /* CorrelationAnalysis */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CORRELATIONANALYSIS_HXX */
