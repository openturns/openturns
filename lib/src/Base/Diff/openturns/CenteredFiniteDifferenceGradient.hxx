//                                               -*- C++ -*-
/**
 *  @brief Class for the creation of a numerical math gradient implementation
 *         form a numerical math evaluation implementation by using centered
 *         finite difference formula.
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CENTEREDFINITEDIFFERENCEGRADIENT_HXX
#define OPENTURNS_CENTEREDFINITEDIFFERENCEGRADIENT_HXX

#include "openturns/FiniteDifferenceGradient.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/Point.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class CenteredFiniteDifferenceGradient
 * @brief This class is for the creation of a numerical math gradient implementation
 *
 * This class is for the creation of a numerical math gradient implementation
 * form a numerical math evaluation implementation by using centered
 * finite difference formula
 */
class OT_API CenteredFiniteDifferenceGradient
  : public FiniteDifferenceGradient
{
  CLASSNAME
public:

  /** Default constructor */
  CenteredFiniteDifferenceGradient();

  /** Parameter constructor */
  CenteredFiniteDifferenceGradient(const Point & epsilon,
                                   const Evaluation & evaluation);

  /** Second parameter constructor */
  CenteredFiniteDifferenceGradient(const Scalar epsilon,
                                   const Evaluation & evaluation);

  /** Constructor from finite difference step */
  CenteredFiniteDifferenceGradient(const FiniteDifferenceStep & step,
                                   const Evaluation & evaluation);

  /* Virtual constructor */
  virtual CenteredFiniteDifferenceGradient * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;



  /* Here is the interface that all derived class must implement */

  /** This method computes the gradient at some point
   * @param in The point where the gradient is computed
   * @result A matrix constructed with the dF_i/dx_j values (Jacobian transposed)
   */
  virtual Matrix gradient(const Point & inP) const;

protected:


private:

}; /* class CenteredFiniteDifferenceGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CENTEREDFINITEDIFFERENCEGRADIENT_HXX */
