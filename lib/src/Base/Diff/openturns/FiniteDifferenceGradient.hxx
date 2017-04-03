//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a numerical math gradient implementation
 *         form a numerical math evaluation implementation by using centered
 *         finite difference formula.
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_FINITEDIFFERENCEGRADIENT_HXX
#define OPENTURNS_FINITEDIFFERENCEGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Pointer.hxx"
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
  CLASSNAME;
public:

  typedef Pointer<EvaluationImplementation>                          EvaluationPointer;

  /** Default constructor */
  FiniteDifferenceGradient();

  /** First Parameter constructor  */
  FiniteDifferenceGradient(const NumericalPoint & epsilon,
                           const EvaluationPointer & p_evaluation);

  /** SecondParameter constructor */
  FiniteDifferenceGradient(const NumericalScalar epsilon,
                           const EvaluationPointer & p_evaluation);

  /** Constructor with FiniteDifferenceStep */
  FiniteDifferenceGradient(const FiniteDifferenceStep & finiteDifferenceStep,
                           const EvaluationPointer & p_evaluation);

  /** Comparison operator */
  virtual Bool operator ==(const FiniteDifferenceGradient & other) const;

  /** String converter */
  virtual String  __repr__() const;

  /** Accessor for input point dimension
   * @return The size of the point passed to the gradient method
   */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension
   * @return The size of the point returned by the function whose gradient is computed
   */
  virtual UnsignedInteger getOutputDimension() const;

  /** Accessor for the epsilon */
  virtual NumericalPoint getEpsilon() const;

  /** Accessor for the evaluation */
  virtual EvaluationPointer getEvaluation() const;

  /** Accessor for the finite difference step */
  virtual void setFiniteDifferenceStep(const FiniteDifferenceStep & finiteDifferenceStep);
  virtual FiniteDifferenceStep getFiniteDifferenceStep() const;

  /* Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /* Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

  /* Here is the interface that all derived class must implement */

  /** Virtual Constructor */
  virtual FiniteDifferenceGradient * clone() const;

  /** This method computes the gradient at some point
   * @param in The point where the gradient is computed
   * @result A matrix constructed with the dF_i/dx_j values (Jacobian transposed)
   */
  virtual Matrix gradient(const NumericalPoint & inP) const;

protected:
  EvaluationPointer p_evaluation_;
  FiniteDifferenceStep finiteDifferenceStep_;


}; /* class FiniteDifferenceGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FINITEDIFFERENCEGRADIENT_HXX */
