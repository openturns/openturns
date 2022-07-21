//                                               -*- C++ -*-
/**
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
#ifndef OPENTURNS_GENERALIZEDEXPONENTIAL_HXX
#define OPENTURNS_GENERALIZEDEXPONENTIAL_HXX

#include "openturns/CovarianceModelImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GeneralizedExponential
 */

class OT_API GeneralizedExponential
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  /** Default constructor */
  explicit GeneralizedExponential(const UnsignedInteger inputDimension = 1);

  /** Parameters constructor */
  GeneralizedExponential(const Point & scale,
                         const Scalar p);

  GeneralizedExponential(const Point & scale,
                         const Point & amplitude,
                         const Scalar p);

  /** Virtual copy constructor */
  GeneralizedExponential * clone() const override;

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

  /** P accessor */
  Scalar getP() const;
  void setP(const Scalar p);

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Parameter accessor */
  void setFullParameter(const Point & parameter) override;
  Point getFullParameter() const override;
  Description getFullParameterDescription() const override;

private:

  /** Exponent of the model */
  Scalar p_;

} ; /* class GeneralizedExponential */

END_NAMESPACE_OPENTURNS

#endif
