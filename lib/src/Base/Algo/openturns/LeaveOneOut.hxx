//                                               -*- C++ -*-
/**
 *  @brief Implicit leave-one-out cross validation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LEAVEONEOUT_HXX
#define OPENTURNS_LEAVEONEOUT_HXX

#include "openturns/FittingAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class LeaveOneOut
 *
 * Implicit leave-one-out cross validation
 */
class OT_API LeaveOneOut
  : public FittingAlgorithmImplementation
{
  CLASSNAME
public:
  typedef Collection<Function> FunctionCollection;

  /** Default constructor */
  LeaveOneOut();

  /** Virtual constructor */
  LeaveOneOut * clone() const override;

  /** String converter */
  String __repr__() const override;

  using FittingAlgorithmImplementation::run;
  /** Perform cross-validation */
  Scalar run(const Sample & x,
             const Sample & y,
             const Point & weight,
             const FunctionCollection & psi,
             const Indices & indices) const override;

  Scalar run(const Sample & y,
             const Point & weight,
             const Indices & indices,
             const DesignProxy & proxy) const override;

  Scalar run(LeastSquaresMethod & method,
             const Sample & y) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

}; /* class LeaveOneOut */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEAVEONEOUT_HXX */
