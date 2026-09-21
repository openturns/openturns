//                                               -*- C++ -*-
/**
 *  @brief The PushForwardOverMesh distribution
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
#ifndef OPENTURNS_PUSHFORWARDOVERMESH_HXX
#define OPENTURNS_PUSHFORWARDOVERMESH_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Field.hxx"
#include "openturns/P1LagrangeEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PushForwardOverMesh
 *
 * The PushForwardOverMesh distribution is the distribution of Y = f(X)
 * where X is an absolutely continuous random vector whose support is a
 * n-dimensional domain discretized by a mesh of simplices, and f is the
 * piecewise-affine map defined by a P1 field on this reference mesh.
 */
class OT_API PushForwardOverMesh
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PushForwardOverMesh();

  /** Parameters constructor: X uniform over the mesh of the field */
  explicit PushForwardOverMesh(const Field & values);

  /** Parameters constructor */
  PushForwardOverMesh(const Distribution & antecedent,
                      const Field & values);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const PushForwardOverMesh & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  PushForwardOverMesh * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const override;
  using DistributionImplementation::setParametersCollection;
  void setParametersCollection(const PointCollection & parametersCollection) override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to PushForwardOverMesh */

  /** Antecedent accessor */
  void setAntecedent(const Distribution & antecedent);
  Distribution getAntecedent() const;

  /** P1 field accessor: the reference mesh and the values of the map */
  void setValues(const Field & values);
  Field getValues() const;

  /** Image mesh accessor */
  Mesh getMesh() const;

  /** Intrinsic dimension of the image mesh accessor */
  UnsignedInteger getIntrinsicDimension() const;

  /** Tell if the distribution is continuous */
  Bool isContinuous() const override;

  /** Tell if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

private:

  /** Set the antecedent and the P1 field with check */
  void setAntecedentAndValues(const Distribution & antecedent,
                              const Field & values);

  /** Compute the per-simplex weights sqrt(det(G_ref)) / sqrt(det(G_img))
      and store them in weights_ */
  void computeWeights();

  /** Check if the point belongs to the image of the simplex at the given
      index, and if so give its barycentric coordinates with respect to
      this image */
  Bool isOnSimplex(const Point & point,
                   const UnsignedInteger index,
                   Point & barycentricCoordinates) const;

  /** The antecedent distribution */
  Distribution antecedent_;

  /** The P1 field defining the piecewise-affine map: a n-dimensional
      reference mesh together with the image of each reference vertex in
      an ambient space of dimension p >= n */
  Field values_;

  /** The piecewise-affine map */
  P1LagrangeEvaluation evaluation_;

  /** The per-simplex weight sqrt(det(G_ref)) / sqrt(det(G_img)), the ratio
      of the reference volume to the volume induced on the image, which
      converts the density of the antecedent into the intrinsic density on
      the image mesh */
  Point weights_;

  /** Tolerance used when checking if a point belongs to the image mesh */
  Scalar epsilon_;
}; /* class PushForwardOverMesh */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PUSHFORWARDOVERMESH_HXX */