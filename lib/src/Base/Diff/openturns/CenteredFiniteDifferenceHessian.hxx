//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a numerical math hessian implementation
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

#ifndef OPENTURNS_CENTEREDFINITEDIFFERENCEHESSIAN_HXX
#define OPENTURNS_CENTEREDFINITEDIFFERENCEHESSIAN_HXX

#include "openturns/FiniteDifferenceHessian.hxx"
#include "openturns/HessianImplementation.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Point.hxx"
#include "openturns/SymmetricTensor.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class CenteredFiniteDifferenceHessian
 * @brief This class is for the creation of a numerical math hessian implementation
 *
 * This class is for the creation of a numerical math hessian implementation
 * form a numerical math evaluation implementation by using centered
 * finite difference formula
 */
class OT_API CenteredFiniteDifferenceHessian
  : public FiniteDifferenceHessian
{
  CLASSNAME
public:

  typedef Pointer<EvaluationImplementation>               EvaluationPointer;

  /** Default constructor */
  CenteredFiniteDifferenceHessian();
  /** Parameter constructor */
  CenteredFiniteDifferenceHessian(const Point & epsilon,
                                  const EvaluationPointer & p_evaluation);

  /** Second parameter constructor */
  CenteredFiniteDifferenceHessian(const Scalar epsilon,
                                  const EvaluationPointer & p_evaluation);

  /** Constructor with FiniteDifferenceStep */
  CenteredFiniteDifferenceHessian(const FiniteDifferenceStep & finiteDifferenceStep,
                                  const EvaluationPointer & p_evaluation);

  /* Virtual constructor */
  virtual CenteredFiniteDifferenceHessian * clone() const;

  /* Comparison operator */
  Bool operator ==(const CenteredFiniteDifferenceHessian & other) const;

  /* String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;



  /* Here is the interface that all derived class must implement */

  /** This method computes the hessian at some point
   * @param in The point where the hessian is computed
   * @result A tensor
   */
  virtual SymmetricTensor hessian(const Point & inP) const;

protected:


private:

}; /* class CenteredFiniteDifferenceHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CENTEREDFINITEDIFFERENCEHESSIAN_HXX */
