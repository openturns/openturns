//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningReliabilityFunction implements parent class for 
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

#ifndef OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTION_HXX
#define OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/GaussianProcessRegressionResult.hxx"
#include "openturns/GaussianProcessConditionalCovariance.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ActiveLearningReliabilityFunction
*/

class OT_API ActiveLearningReliabilityFunction
  : public EvaluationImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  ActiveLearningReliabilityFunction();
  
  /** Default constructor */  
  ActiveLearningReliabilityFunction(const Scalar reliabilityThreshold,
                                    const Scalar learningThreshold);
                                    
                                    
  /*ActiveLearningReliabilityFunction(const Scalar reliabilityThreshold,
                                    const GaussianProcessRegressionResult & gprResult,
                                    const Scalar learningThreshold,
                                    const Distribution & inputDistribution,
                                    const Scalar outputQuantileLevel);*/
                                                                                                                   
  /** Virtual constructor */
  ActiveLearningReliabilityFunction * clone() const override;
    
  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;
  
  /** Accessors */
  void setGaussianProcessRegression(const GaussianProcessRegressionResult & gprResult);
  
  void setReliabilityThreshold(const Scalar reliabilityThreshold);
  
  void setLearningThreshold(const Scalar learningThreshold);
  
  /** String converter */
  String __repr__() const override;
  
  virtual Sample operator()(const Sample & inputSample, const Sample & inputDoE) const; 
  
  /* return infill Sample */
  virtual Sample getInfillSample(const Sample & inputSample,
                                 const Sample & criterionValues) const ;
                                 
  virtual Bool checkConvergenceLearning(const Sample & criterionValues) const;
  
protected:

  virtual Scalar computeAsScalar(const Point & x) const;

  Point operator()(const Point & x) const override;


   
  Scalar reliabilityThreshold_;
  Scalar learningThreshold_;
  GaussianProcessRegressionResult gprResult_;
  GaussianProcessConditionalCovariance gprCov_;

} ; /* class ActiveLearningReliabilityFunction */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ACTIVELEARNINGRELIABILITYFUNCTION_HXX */
