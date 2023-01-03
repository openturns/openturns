//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a numerical math hessian implementation
 *         form a numerical math evaluation implementation by using centered
 *         finite difference formula.
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_CENTEREDFINITEDIFFERENCEHESSIAN_HXX
#define OPENTURNS_CENTEREDFINITEDIFFERENCEHESSIAN_HXX

#include "openturns/FiniteDifferenceHessian.hxx"
#include "openturns/HessianImplementation.hxx"
#include "openturns/Evaluation.hxx"
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

  /** Default constructor */
  CenteredFiniteDifferenceHessian();
  /** Parameter constructor */
  CenteredFiniteDifferenceHessian(const Point & epsilon,
                                  const Evaluation & evaluation);

  /** Second parameter constructor */
  CenteredFiniteDifferenceHessian(const Scalar epsilon,
                                  const Evaluation & evaluation);

  /** Constructor with FiniteDifferenceStep */
  CenteredFiniteDifferenceHessian(const FiniteDifferenceStep & finiteDifferenceStep,
                                  const Evaluation & evaluation);

  /* Virtual constructor */
  CenteredFiniteDifferenceHessian * clone() const override;

  /* Comparison operator */
  Bool operator ==(const CenteredFiniteDifferenceHessian & other) const;

  /* String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Here is the interface that all derived class must implement */

  /** This method computes the hessian at some point
   * @param in The point where the hessian is computed
   * @result A tensor
   */
  SymmetricTensor hessian(const Point & inP) const override;

protected:


private:

}; /* class CenteredFiniteDifferenceHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CENTEREDFINITEDIFFERENCEHESSIAN_HXX */
