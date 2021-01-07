//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a numerical math gradient implementation
 *         form a numerical math evaluation implementation by using centered
 *         finite difference formula.
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
#ifndef OPENTURNS_FINITEDIFFERENCEGRADIENT_HXX
#define OPENTURNS_FINITEDIFFERENCEGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/FiniteDifferenceStep.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FiniteDifferenceGradient
 *
 * This class is for the creation of a numerical math gradient implementation
 * form a numerical math evaluation implementation by using centered
 * finite difference formula
 */
class OT_API FiniteDifferenceGradient
  : public GradientImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  FiniteDifferenceGradient();

  /** First Parameter constructor  */
  FiniteDifferenceGradient(const Point & epsilon,
                           const Evaluation & evaluation);

  /** SecondParameter constructor */
  FiniteDifferenceGradient(const Scalar epsilon,
                           const Evaluation & evaluation);

  /** Constructor with FiniteDifferenceStep */
  FiniteDifferenceGradient(const FiniteDifferenceStep & finiteDifferenceStep,
                           const Evaluation & evaluation);

  /** Comparison operator */
  virtual Bool operator ==(const FiniteDifferenceGradient & other) const;

  /** String converter */
  String  __repr__() const override;

  /** Accessor for input point dimension
   * @return The size of the point passed to the gradient method
   */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension
   * @return The size of the point returned by the function whose gradient is computed
   */
  UnsignedInteger getOutputDimension() const override;

  /** Accessor for the epsilon */
  virtual Point getEpsilon() const;

  /** Accessor for the evaluation */
  virtual Evaluation getEvaluation() const;

  /** Accessor for the finite difference step */
  virtual void setFiniteDifferenceStep(const FiniteDifferenceStep & finiteDifferenceStep);
  virtual FiniteDifferenceStep getFiniteDifferenceStep() const;

  /* Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /* Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /* Here is the interface that all derived class must implement */

  /** Virtual Constructor */
  FiniteDifferenceGradient * clone() const override;

  /** This method computes the gradient at some point
   * @param in The point where the gradient is computed
   * @result A matrix constructed with the dF_i/dx_j values (Jacobian transposed)
   */
  Matrix gradient(const Point & inP) const override;

protected:
  Evaluation evaluation_;
  FiniteDifferenceStep finiteDifferenceStep_;


}; /* class FiniteDifferenceGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FINITEDIFFERENCEGRADIENT_HXX */
