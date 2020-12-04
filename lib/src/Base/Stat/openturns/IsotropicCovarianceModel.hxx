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
#ifndef OPENTURNS_IsotropicCovarianceModel_HXX
#define OPENTURNS_IsotropicCovarianceModel_HXX

#include "openturns/CovarianceModel.hxx"
#include "openturns/CovarianceModelImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IsotropicCovarianceModel
 */

class OT_API IsotropicCovarianceModel
  : public CovarianceModelImplementation
{

  CLASSNAME

public:

  /** Parameters constructor */
  IsotropicCovarianceModel(const CovarianceModel & oneDimensional,
                           const UnsignedInteger inputDimension);

  /** Virtual copy constructor */
  IsotropicCovarianceModel * clone() const override;

  /** Computation of the covariance matrix */  
  using CovarianceModelImplementation::computeAsScalar;

  #ifndef SWIG
  Scalar computeAsScalar(const Collection<Scalar>::const_iterator & s_begin,
                         const Collection<Scalar>::const_iterator & t_begin) const override;
  #endif
  Scalar computeAsScalar(const Point & tau) const override;
  Scalar computeAsScalar(const Scalar tau) const override;

  /** Gradient */
  Matrix partialGradient(const Point & s, const Point & t) const override;

  /* 1D kernel accessor */
  CovarianceModel getKernel() const;

  /** Scale accessor */
  void setScale(const Point & scale) override;

  /** Amplitude accessor */
  void setAmplitude(const Point & amplitude) override;

  /** Nugget factor accessor */
  void setNuggetFactor(const Scalar nuggetFactor) override;

  /** Is this a stationary covariance model ? */
  Bool isStationary() const override;

  /** Is it safe to compute discretize etc in parallel? */ 
  Bool isParallel() const override;

  /** Indices of the active parameters */
  void setActiveParameter(const Indices & active) override;

  /** Full Parameter accessors */
  void setFullParameter(const Point & parameter) override;
  Point getFullParameter() const override;
  Description getFullParameterDescription() const override; 

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** 1D covariance kernel accessor */
  void setKernel(const CovarianceModel & oneDimensional);

private:
  /** The 1D covariance kernel */
  CovarianceModel kernel_;

} ; /* class IsotropicCovarianceModel */

END_NAMESPACE_OPENTURNS

#endif
