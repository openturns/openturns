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

#include "openturns/Collection.hxx"
#include "openturns/RandomVectorImplementation.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/MultiStart.hxx"

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

  explicit RatioOfUniforms(const Distribution & distribution);

  RatioOfUniforms(const Function & logUnscaledPDF,
		  const Interval & range,
		  const Bool isScaled = false);

  /** Virtual constructor */
  RatioOfUniforms * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** LogUnscaledPDF and range accessor */
  void setLogUnscaledPDFAndRange(const Function & logUnscaledPDF,
				 const Interval & range,
				 const Bool isScaled);
  Function getLogUnscaledPDF() const;
  Interval getRange() const;

  /** Bounds accessors */
  Scalar getSupU() const;
  Point getInfV() const;
  Point getSupV() const;

  /** Optimization algorithm accessor */
  void setOptimizationAlgorithm(const OptimizationAlgorithm & optimizationAlgorithm);
  OptimizationAlgorithm getOptimizationAlgorithm() const;

  /** Candidate number accessor */
  void setCandidateNumber(const UnsignedInteger candidateNumber);
  UnsignedInteger getCandidateNumber() const;

  /** Maximum multistart accessor */
  void setMaximumMultistart(const UnsignedInteger maximumMultistart);
  UnsignedInteger getMaximumMultistart() const;

  /** R accessor */
  void setR(const Scalar r);
  Scalar getR() const;

  /** C accessor */
  Scalar getC() const;

  /** Acceptance ratio accessor */
  Scalar getAcceptanceRatio() const;

  /** Initialize the generator */
  Collection<MultiStart> initialize();

  /** Check if the sampler has been initialized */
  Bool isInitialized() const;

  /* Here is the interface that all derived class must implement */

  /** Sample generation */
  Point getRealization() const override;
  Sample getSample(const UnsignedInteger size) const override;
  Sample getSampleWithTryNumber(const UnsignedInteger size,
				UnsignedInteger & tryNumberOut) const;

protected:

private:
  Function logUnscaledPDF_;
  Interval range_;
  Bool isScaled_ = false;
  Scalar r_ = 1.0;
  Scalar supU_ = 0.0;
  Point infV_;
  Point supV_;
  Scalar acceptanceRatio_ = 0.0;
  OptimizationAlgorithm optimizationAlgorithm_;
  UnsignedInteger candidateNumber_ = 1;
  UnsignedInteger maximumMultistart_ = 1;
}; /* class RatioOfUniforms */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RATIOOFUNIFORMS_HXX */
