//                                               -*- C++ -*-
/**
 *  @brief The UniformOverMesh distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_UNIFORMOVERMESH_HXX
#define OPENTURNS_UNIFORMOVERMESH_HXX

#include "openturns/ContinuousDistribution.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/MeshDomain.hxx"
#include "openturns/IntegrationAlgorithm.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/Dirichlet.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class UniformOverMesh
 *
 * The UniformOverMesh distribution.
 */
class OT_API UniformOverMesh
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  UniformOverMesh();

  /** Parameters constructor */
  UniformOverMesh(const Mesh & mesh);


  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const UniformOverMesh & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  UniformOverMesh * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point) */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /* Interface specific to UniformOverMesh */

  /** Mesh accessor */
  void setMesh(const Mesh & mesh);
  Mesh getMesh() const;

  /** Integration algorithm accessor */
  void setIntegrationAlgorithm(const IntegrationAlgorithm & integrationAlgorithm);
  IntegrationAlgorithm getIntegrationAlgorithm() const;

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

  /** The main parameter set of the distribution */
  Mesh mesh_;

  /** The mesh as a domain for fast inclusion testing */
  MeshDomain meshDomain_;

  /** Volumes of the simplices */
  Point simplicesVolumes_;

  /** Volume of the mesh */
  Scalar meshVolume_;
 
  /** Auxiliary discrete distribution to select a simplex */
  UserDefined simplexSelection_;

  /** Integration algorithm to use for CDF computation */
  IntegrationAlgorithm integrationAlgorithm_;
  
}; /* class UniformOverMesh */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_UNIFORMOVERMESH_HXX */
