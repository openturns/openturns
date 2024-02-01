//                                               -*- C++ -*-
/**
 *  @brief The TruncatedOverMesh distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_TRUNCATEDOVERMESH_HXX
#define OPENTURNS_TRUNCATEDOVERMESH_HXX

#include "openturns/ContinuousDistribution.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/MeshDomain.hxx"
#include "openturns/IntegrationAlgorithm.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class TruncatedOverMesh
 *
 * The TruncatedOverMesh distribution.
 */
class OT_API TruncatedOverMesh
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  TruncatedOverMesh();

  /** Parameters constructor */
  TruncatedOverMesh(const Distribution & distribution, const Mesh & mesh);


  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const TruncatedOverMesh & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  TruncatedOverMesh * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get one sample of the distribution */
  Sample getSample(const UnsignedInteger size) const override;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to TruncatedOverMesh */

  /** Mesh accessor */
  void setMesh(const Mesh & mesh);
  Mesh getMesh() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:

private:
  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange() override;

  /** Build the sample of vertices for i-th simplex */
  Sample getSimplexVertices(const UnsignedInteger index) const;

  Distribution distribution_;

  /** The main parameter set of the distribution */
  Mesh mesh_;

  /** The mesh as a domain for fast inclusion testing */
  MeshDomain meshDomain_;

  /** Probabilities associated to the simplices selection */
  Point probabilities_;

  /** Upper bound of pdf for each simplex */
  Point pdfSup_;

  Scalar normalizationFactor_ = 0.0;

  /** Structures for the alias sampling method */
  Point base_;
  Indices alias_;

  /** Integration algorithm to use for CDF computation */
  IntegrationAlgorithm integrationAlgorithm_;


}; /* class TruncatedOverMesh */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRUNCATEDOVERMESH_HXX */
