//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningReliabilityFunctionImplementation implements parent class for 
 *  active learning criterion used in reliability algorithms
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

#ifndef OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTIONIMPLEMENTATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/GaussianProcessRegressionResult.hxx"
#include "openturns/GaussianProcessConditionalCovariance.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ActiveLearningReliabilityFunctionImplementation
*/

class OT_API ActiveLearningReliabilityFunctionImplementation
  : public EvaluationImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  ActiveLearningReliabilityFunctionImplementation();
  
  /** Default constructor */  
  ActiveLearningReliabilityFunctionImplementation(const Scalar reliabilityThreshold,
                                    const Scalar learningThreshold);
                                    
                                    
  /** Virtual constructor */
  ActiveLearningReliabilityFunctionImplementation * clone() const override;

  /** Input dimension accessor, defined by the Gaussian process surrogate */
  UnsignedInteger getInputDimension() const override;

  /** Output dimension accessor, always 1 (scalar criterion) */
  UnsignedInteger getOutputDimension() const override;
    
  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;
  
  /** Accessors */
  void setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult);

  GaussianProcessRegressionResult getGaussianProcessRegression() const;

  void setReliabilityThreshold(const Scalar reliabilityThreshold);

  Scalar getReliabilityThreshold() const;

  void setLearningThreshold(const Scalar learningThreshold);

  Scalar getLearningThreshold() const;
  
  /** String converter */
  String __repr__() const override;
  
  virtual Sample operator()(const Sample & inputSample, const Sample &) const; 
  
  /* return infill Sample */
  virtual Sample getInfillSample(const Sample & inputSample,
                                 const Sample & criterionValues) const ;
                                 
  virtual Bool checkConvergenceLearning(const Sample & criterionValues) const;

  /** Selection direction: true if the infill point maximizes the criterion (EFF, GMM), false if it minimizes it (U) */
  virtual Bool isMaximization() const;

  Point operator()(const Point & x) const override;
  Sample operator()(const Sample & x) const override;
  Field operator()(const Field & x) const override;

protected:

  virtual Scalar computeAsScalar(const Point & x) const;
  
  Scalar reliabilityThreshold_ = 0.0;
  Scalar learningThreshold_ = 0.0;
  GaussianProcessRegressionResult gprResult_;
  GaussianProcessConditionalCovariance gprCov_;

} ; /* class ActiveLearningReliabilityFunctionImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTIONIMPLEMENTATION_HXX */
