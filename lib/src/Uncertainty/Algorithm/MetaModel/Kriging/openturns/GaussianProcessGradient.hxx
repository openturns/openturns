//                                               -*- C++ -*-
/**
 *  @brief The class building gaussian process regression gradient
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_GAUSSIANPROCESSGRADIENT_HXX
#define OPENTURNS_GAUSSIANPROCESSGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Basis.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
  * @class GaussianProcessGradient
  *
  * This class permits prediction on a gaussian process
  */
class OT_API GaussianProcessGradient
  : public GradientImplementation
{
  CLASSNAME

public:

  /** Default constructor */
  GaussianProcessGradient();

  /** Constructor with parameters */
  GaussianProcessGradient(const Basis & basis,
                          const Sample & inputSample,
                          const CovarianceModel & correlationModel,
                          const Point & beta,
                          const Sample & gamma);

  /** Virtual constructor */
  GaussianProcessGradient * clone() const override;

  /** Comparison operator */
  using GradientImplementation::operator ==;
  Bool operator ==(const GaussianProcessGradient & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  //   using GradientImplementation::gradient;
  Matrix gradient(const Point & inP) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  // Basis
  Basis basis_;

  /// Training (input) sample
  Sample inputSample_;

  /// Correlation model
  CovarianceModel covarianceModel_;

  /// Regression weights
  Point beta_;
  Sample gamma_;

};

END_NAMESPACE_OPENTURNS

#endif
