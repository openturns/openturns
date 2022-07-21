//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build an kronecker covariance
 *  model
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
#ifndef OPENTURNS_KRONECKERCOVARIANCEMODEL_HXX
#define OPENTURNS_KRONECKERCOVARIANCEMODEL_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/CovarianceModelImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KroneckerCovarianceModel
 */

class OT_API KroneckerCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:


  /** Constructor with input dimension */
  explicit KroneckerCovarianceModel(const UnsignedInteger inputDimension = 1);

  /** Standard constructor with scale and amplitude parameters parameters */
  KroneckerCovarianceModel(const CovarianceModel &rho,
                           const Point &amplitude);

  /** Standard constructor with scale and amplitude parameters parameters */
  KroneckerCovarianceModel(const CovarianceModel & rho,
                           const Point & amplitude,
                           const CorrelationMatrix & spatialCorrelation);

  /** Standard constructor with scale, amplitude and spatial correlation parameters */
  KroneckerCovarianceModel(const CovarianceModel &rho,
                           const CovarianceMatrix &sigma);

  /** Virtual copy constructor */
  KroneckerCovarianceModel * clone() const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::operator();
  SquareMatrix operator()(const Point &s, const Point &t) const override;
  SquareMatrix operator()(const Point &tau) const override;

  /** Computation of the covariance function */
  using CovarianceModelImplementation::computeAsScalar;
  Scalar computeAsScalar(const Point &s,
                         const Point &t) const override;
  Scalar computeAsScalar(const Point &tau) const override;
#ifndef SWIG
  Scalar computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
                         const Collection<Scalar>::const_iterator &t_begin) const override;
#endif

  /** Discretize the covariance function on a given TimeGrid */
  using CovarianceModelImplementation::discretize;
  CovarianceMatrix discretize(const Sample & vertices) const override;
  using CovarianceModelImplementation::discretizeAndFactorize;
  TriangularMatrix discretizeAndFactorize(const Sample &vertices) const override;

  /** Gradient */
  using CovarianceModelImplementation::partialGradient;
  Matrix partialGradient(const Point &s, const Point &t) const override;

  /** isParallel? */
  Bool isParallel() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Parameter accessor */
  Point getFullParameter() const override;
  void setFullParameter(const Point & parameter) override;
  Description getFullParameterDescription() const override;
  void setScale(const Point &scale) override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  friend struct KroneckerModelDiscretizePolicy;
  void setCorrelationModel(const CovarianceModel & rho);
  Scalar computeRhoOverSample(const Collection<Scalar>::const_iterator &s_begin,
                              const Collection<Scalar>::const_iterator &t_begin) const;

private :

  // kernel (or correlation) scalar model
  CovarianceModel rho_;

  // Number of extra parameter (not scale/amplitude)
  UnsignedInteger extraParameterNumber_;

} ; /* class KroneckerCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KRONECKERCOVARIANCEMODEL_HXX */
