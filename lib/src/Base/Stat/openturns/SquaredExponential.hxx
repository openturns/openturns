//                                               -*- C++ -*-
/**
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
#ifndef OPENTURNS_SQUAREDEXPONENTIAL_HXX
#define OPENTURNS_SQUAREDEXPONENTIAL_HXX

#include "openturns/CovarianceModelImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SquaredExponential
 */

class OT_API SquaredExponential
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  /** Constructor based on input dimension */
  explicit SquaredExponential(const UnsignedInteger inputDimension = 1);

  /** Parameters constructor */
  SquaredExponential(const Point & scale);

  /** Parameters constructor */
  SquaredExponential(const Point & scale,
                     const Point & amplitude);

  /** Virtual copy constructor */
  SquaredExponential * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::computeAsScalar;
  Scalar computeAsScalar(const Point & tau) const override;
#ifndef SWIG
  Scalar computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
                         const Collection<Scalar>::const_iterator & t_begin) const override;
#endif
  Scalar computeAsScalar(const Scalar tau) const override;

  /** Gradient */
  Matrix partialGradient(const Point & s,
                         const Point & t) const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

} ; /* class SquaredExponential */

END_NAMESPACE_OPENTURNS

#endif
