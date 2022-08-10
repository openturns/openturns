//                                               -*- C++ -*-
/**
 *  @brief Stepwise algorithms for linear model
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
#ifndef OPENTURNS_LINEARMODELSTEPWISEALGORITHM_HXX
#define OPENTURNS_LINEARMODELSTEPWISEALGORITHM_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Basis.hxx"
#include "openturns/LinearModelResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearModelStepwiseAlgorithm
 *
 * The class building linear model using stepwise regression
 */

class OT_API LinearModelStepwiseAlgorithm :
  public PersistentObject
{
  CLASSNAME

public:

  enum Direction { BACKWARD = 1, FORWARD = 2, BOTH = 3 };

  /** Default constructor */
  LinearModelStepwiseAlgorithm();

  /** Parameters constructor */
  LinearModelStepwiseAlgorithm(const Sample &inputSample,
                               const Sample &outputSample,
                               const Basis &basis,
                               const Indices &minimalIndices,
                               const Direction direction = FORWARD,
                               const Indices &startIndices = Indices());

  /** Oldest constructor (for compatibility) */
  LinearModelStepwiseAlgorithm(const Sample &inputSample,
                               const Basis &basis,
                               const Sample &outputSample,
                               const Indices &minimalIndices,
                               const Direction direction = FORWARD,
                               const Indices &startIndices = Indices());

  /** Virtual constructor */
  LinearModelStepwiseAlgorithm * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Sample accessors */
  Sample getInputSample() const;
  Sample getOutputSample() const;

  /** Direction accessors */
  Direction getDirection() const;

  /** Penalty accessors */
  void setPenalty(const Scalar penalty);
  Scalar getPenalty() const;

  /** Maximum number of iterations accessors */
  void setMaximumIterationNumber(const UnsignedInteger maximumIteration);
  UnsignedInteger getMaximumIterationNumber() const;

  /** Perform regression */
  void run();

  /** Result accessor */
  LinearModelResult getResult();

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Compute the likelihood function */
  Scalar computeLogLikelihood();

  /** Build currentX_ and currentIndices_ from given indices */
  void buildCurrentMatrixFromIndices(const Indices & columns);

  /** Input sample */
  Sample inputSample_;

  /** Output sample */
  Sample outputSample_;

  /** The monomials collection */
  Basis basis_;

  /** The direction of the stepwise regression method */
  Direction direction_;

  /** The multiple of the degrees of freedom used for the penalty of the stepwise regression method
      - 2      Akaike   information criterion (AIC)
      - log(n) Bayesian information criterion (BIC)  */
  Scalar penalty_;

  /** The maximum number of iterations of the stepwise regression method */
  UnsignedInteger maximumIterationNumber_;

  /** The indices of minimal model */
  Indices minimalIndices_;

  /** The indices of start model */
  Indices startIndices_;

  /** The output data, stored as a matrix  */
  Matrix Y_;

  /** The matrix X_{max} containing all monomials */
  Matrix maxX_;

  /** The current matrix */
  Matrix currentX_;

  /** QR decomposition of the current matrix */
  Matrix currentQ_;
  Matrix currentInvRt_;

  /** The current residual R=Y - Q Q^T Y */
  Matrix currentResidual_;

  /** The indices of current model */
  Indices currentIndices_;

  /** Result */
  LinearModelResult result_;

  /** Whether result is cached */
  Bool hasRun_;

}; /* class LinearModelStepwiseAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARMODELSTEPWISEALGORITHM_HXX */
