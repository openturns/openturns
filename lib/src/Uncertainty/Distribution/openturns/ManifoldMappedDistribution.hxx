//                                               -*- C++ -*-
/**
 *  @brief The push-forward of a Euclidean distribution onto a Riemannian manifold
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
#ifndef OPENTURNS_MANIFOLDMAPPEDDISTRIBUTION_HXX
#define OPENTURNS_MANIFOLDMAPPEDDISTRIBUTION_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Manifold.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ManifoldMappedDistribution
 *
 * The distribution on a Riemannian manifold obtained by pushing forward
 * a background Euclidean distribution of dimension equal to the intrinsic
 * dimension of the manifold through the exponential map at a chart center:
 * a background realization x is mapped to exp(center, sum_k x_k B_k) where
 * the B_k form the tangent basis of the manifold at the chart center.
 *
 * The density with respect to the Riemannian volume follows from the
 * change of variables formula through the logarithmic map, corrected by
 * the volume density of the exponential map.
 */
class OT_API ManifoldMappedDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ManifoldMappedDistribution();

  /** Parameters constructor */
  ManifoldMappedDistribution(const Distribution & background,
                             const Manifold & manifold,
                             const Point & chartCenter);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const ManifoldMappedDistribution & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  ManifoldMappedDistribution * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the log-PDF of the distribution */
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using DistributionImplementation::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to ManifoldMappedDistribution */

  /** Background distribution accessor */
  void setBackground(const Distribution & background);
  Distribution getBackground() const;

  /** Manifold accessor */
  void setManifold(const Manifold & manifold);
  Manifold getManifold() const;

  /** Chart center accessor */
  void setChartCenter(const Point & chartCenter);
  Point getChartCenter() const;

  /** Tangent basis at the chart center accessor */
  Sample getTangentBasis() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const override;

  /** Tell if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Set the background, manifold and chart center with check */
  void setBackgroundManifoldAndChartCenter(const Distribution & background,
      const Manifold & manifold,
      const Point & chartCenter);

  /** Compute the numerical range of the distribution */
  void computeRange() override;

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

private:

  /** Background Euclidean distribution */
  Distribution background_;

  /** Manifold supporting the distribution */
  Manifold manifold_;

  /** Chart center on the manifold */
  Point chartCenter_;

  /** Orthonormal tangent basis at the chart center */
  Sample tangentBasis_;

}; /* class ManifoldMappedDistribution */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MANIFOLDMAPPEDDISTRIBUTION_HXX */
