//                                               -*- C++ -*-
/**
 *  @brief The result of a linear model estimation
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
#ifndef OPENTURNS_LINEARMODELRESULT_HXX
#define OPENTURNS_LINEARMODELRESULT_HXX

#include "openturns/MetaModelResult.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Basis.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Function.hxx"
#include "openturns/Normal.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearModelResult
 *
 * The result of a linear model evaluation
 */

class OT_API LinearModelResult
  : public MetaModelResult
{
  CLASSNAME

public:
  /** Default constructor */
  LinearModelResult();

  /** Parameter constructor */
  LinearModelResult(const Sample & inputSample,
                    const Basis & basis,
                    const Matrix & design,
                    const Sample & outputSample,
                    const Function & metaModel,
                    const Point & trendCoefficients,
                    const String & formula,
                    const Description & coefficientsNames,
                    const Sample & sampleResiduals,
                    const Sample & standardizedResiduals,
                    const Point & diagonalGramInverse,
                    const Point & leverages,
                    const Point & cookDistances,
                    const Scalar sigma2);

  /** Virtual constructor */
  virtual LinearModelResult * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Sample accessors */
  virtual Sample getInputSample() const;
  virtual Sample getOutputSample() const;
  virtual Sample getFittedSample() const;

  /** Condensed formula accessor */
  virtual Point getTrendCoefficients() const;

  /** Condensed formula accessor */
  virtual String getFormula() const;

  /** Coefficients names accessor */
  virtual Description getCoefficientsNames() const;

  /** Residuals accessor */
  virtual Sample getSampleResiduals() const;

  /** Noise distribution */
  virtual Normal getNoiseDistribution() const;

  /** Standardized residuals accessor */
  virtual Sample getStandardizedResiduals() const;

  /** Number of degrees of freedom */
  virtual UnsignedInteger getDegreesOfFreedom() const;

  /** Leverages accessor */
  virtual Point getLeverages() const;

  /** Diagonal Gram inverse accessor */
  virtual Point getDiagonalGramInverse() const;

  /** Cook distance accessor */
  virtual Point getCookDistances() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** input data */
  Sample inputSample_;

  /** basis */
  Basis basis_;

  /** input data */
  Matrix design_;

  /** output data */
  Sample outputSample_;

  /** Intercept and trend coefficients */
  Point beta_;

  /** The formula description */
  String condensedFormula_;

  /** Coefficients names */
  Description coefficientsNames_;

  /** Whole residuals */
  Sample sampleResiduals_;

  /** Standardized residuals */
  Sample standardizedResiduals_;

  /** Diagonal of (Xt X)^{-1} */
  Point diagonalGramInverse_;

  /** Leverages */
  Point leverages_;

  /** Cook's distances */
  Point cookDistances_;

  /** Sigma2 */
  Scalar sigma2_;

}; /* class LinearModelResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARMODELRESULT_HXX */
