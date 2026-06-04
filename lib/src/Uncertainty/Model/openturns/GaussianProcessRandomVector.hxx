//                                               -*- C++ -*-
/**
 *  @brief An implementation class for kriging random vectors
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
#ifndef OPENTURNS_GAUSSIANPROCESSRANDOMVECTOR_HXX
#define OPENTURNS_GAUSSIANPROCESSRANDOMVECTOR_HXX

#include "openturns/UsualRandomVector.hxx"
#include "openturns/GaussianProcessRegressionResult.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class GaussianProcessRandomVector
 *
 * An implementation class for gaussian process random vectors
 */
class OT_API GaussianProcessRandomVector
  : public UsualRandomVector
{
  CLASSNAME

public:
  /** Default constructor */
  GaussianProcessRandomVector();

  /** Parameter constructors */
  GaussianProcessRandomVector(const GaussianProcessRegressionResult & gprResult,
                              const Point & point);

  GaussianProcessRandomVector(const GaussianProcessRegressionResult & gprResult,
                              const Sample & sample);

  /** Virtual constructor */
  GaussianProcessRandomVector * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Get a realization of the process */
  using UsualRandomVector::getRealization;
  Point getRealization() const override;

  /** Get realizations of the process */
  using UsualRandomVector::getSample;
  Sample getSample(const UnsignedInteger size) const override;

  /** Kriging result accessor */
  GaussianProcessRegressionResult getGaussianProcessRegressionResult() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** The Gaussian Process Regression result that allows one to build the random vector */
  GaussianProcessRegressionResult gprResult_;

  /** The points on which we seek the realizations */
  Sample sample_;

}; /* class GaussianProcess */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GAUSSIANPROCESSRANDOMVECTOR_HXX */
