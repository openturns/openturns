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
  CorrelationAnalysis(const Sample & sampleX,
                      const Sample & sampleY);

  /** Virtual constructor */
  CorrelationAnalysis * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Compute the Pearson correlation coefficient between the input sample and the output sample */
  Point computePearsonCorrelation() const;

  /** @deprecated method : use computePearsonCorrelation */
  /** Compute the Pearson correlation coefficient between the input sample and the output sample */
  static Point PearsonCorrelation(const Sample & inputSample,
                                  const Sample & outputSample);

  /** Compute the Spearman correlation coefficient between the input sample and the output sample */
  Point computeSpearmanCorrelation() const;

  /** @deprecated method : use computeSpearmanCorrelation */
  /** Compute the Spearman correlation coefficient between the input sample and the output sample */
  static Point SpearmanCorrelation(const Sample & inputSample,
                                   const Sample & outputSample);

  /** Compute the Kendall Tau coefficient between the input sample and the output sample */
  Point computeKendallTau() const;

  /** Compute the Standard Regression Coefficients (SRC) between the input sample and the output sample */
  Point computeSRC() const;

  /** Compute the squared Standard Regression Coefficients (SRC) between the input sample and the output sample */
  Point computeSquaredSRC(const Bool normalize = false) const;

  /** Compute the Partial Correlation Coefficients (PCC) between the input sample and the output sample */
  Point computePCC() const;

  /** @deprecated method : use computePCC */
  /** Compute the Partial Correlation Coefficients (PCC) between the input sample and the output sample */
  static Point PCC(const Sample & inputSample,
                   const Sample & outputSample);

  /** Compute the Standard Rank Regression Coefficients (SRRC) between the input sample and the output sample */
  Point computeSRRC() const;

  /** Compute the Partial Rank Correlation Coefficients (PRCC) between the input sample and the output sample */
  Point computePRCC() const;

  /** @deprecated method : use computePRCC */
  /** Compute the Partial Rank Correlation Coefficients (PRCC) between the input sample and the output sample */
  static Point PRCC(const Sample & inputSample,
                    const Sample & outputSample);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  // computePearsonCorrelation with arguments
  Point computePearsonCorrelation(const Sample & sampleX,
                                  const Sample & sampleY) const;

  // computePCC with arguments
  Point computePCC(const Sample & sampleX,
                   const Sample & sampleY) const;

  // computeSRC with arguments
  Point computeSRC(const Sample & sampleX,
                   const Sample & sampleY) const;
private:

  Sample sampleX_;
  Sample sampleY_;

}; /* CorrelationAnalysis */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CORRELATIONANALYSIS_HXX */
