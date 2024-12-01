//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of a RatioOfUniforms generator
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
#ifndef OPENTURNS_RATIOOFUNIFORMS_HXX
#define OPENTURNS_RATIOOFUNIFORMS_HXX

#include "openturns/RandomVectorImplementation.hxx"
#include "openturns/OptimizationAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class RatioOfUniforms
 *
 * The class describes the probabilistic concept of ratio of uniforms generator
 */
class OT_API RatioOfUniforms
  : public RandomVectorImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  RatioOfUniforms();

  /** Parameters constructor */
  RatioOfUniforms(const Distribution & distribution);

  /** Virtual constructor */
  RatioOfUniforms * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution);

  /** Optimization algorithm accessor */
  void setOptimizationAlgorithm(const OptimizationAlgorithm & optimizationAlgorithm);
  OptimizationAlgorithm getOptimizationAlgorithm() const;

  /** Candidate number accessor */
  void setCandidateNumber(const UnsignedInteger candidateNumber);
  UnsignedInteger getCandidateNumber() const;

  /** Acceptance ratio accessor */
  Scalar getAcceptanceRatio() const;

  /** R accessor */
  void setR(const Scalar r);
  Scalar getR() const;

  /** Initialize the generator */
  void initialize();

  /** Check if the sampler has been initialized */
  Bool isInitialized() const;

  /* Here is the interface that all derived class must implement */

  /** Sample generation */
  Point getRealization() const override;
  Sample getSample(const UnsignedInteger size) const override;

protected:

private:
  Distribution distribution_;
  Scalar r_ = 1.0;
  Scalar supU_ = 0.0;
  Point infV_;
  Point supV_;
  OptimizationAlgorithm optimizationAlgorithm_;
  UnsignedInteger candidateNumber_ = 1;
}; /* class RatioOfUniforms */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RATIOOFUNIFORMS_HXX */
