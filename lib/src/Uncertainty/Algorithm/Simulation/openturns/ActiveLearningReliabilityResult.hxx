//                                               -*- C++ -*-
/**
 *  @brief class for ActiveLearningReliabilityResult
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
#ifndef OPENTURNS_ACTIVELEARNINGRELIABILITYRESULT_HXX
#define OPENTURNS_ACTIVELEARNINGRELIABILITYRESULT_HXX


#include "openturns/GaussianProcessRegressionResult.hxx"
#include "openturns/Collection.hxx"
//#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ActiveLearningReliabilityResult
 *
 * ActiveLearningReliabilityResult is a class to store all the results provided by ActiveLearningReliabilityAlgorithm
 */
class OT_API ActiveLearningReliabilityResult
  : public PersistentObject
{

  CLASSNAME
public:

  typedef Collection<Scalar> ScalarCollection;
  
  /** Default constructor */
  ActiveLearningReliabilityResult();

  /** Standard constructor */
  
  ActiveLearningReliabilityResult(const Scalar probabilityEstimate,
                                  const Scalar reliabilityIndex,
                                  const GaussianProcessRegressionResult &gprResult,
                                  const ScalarCollection probabilityHistory,
                                  const ScalarCollection reliabilityIndexHistory, 
                                  const UnsignedInteger functionCallNumber, 
                                  const Interval probabilityCI, 
                                  const Interval reliabilityIndexCI);
  /** Virtual constructor */
  ActiveLearningReliabilityResult * clone() const override;

  /** Method save() stores the object through the StorageManager */
  //void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  //void load(Advocate & adv) override;
 
  // Probability Estimate
  Scalar getProbabilityEstimate() const;

  void setProbabilityEstimate(const Scalar probabilityEstimate);

  // Reliability Index
  Scalar getReliabilityIndex() const;

  void setReliabilityIndex(const Scalar reliabilityIndex);

  // GPR Result
  GaussianProcessRegressionResult getGprResult() const;

  void setGprResult(const GaussianProcessRegressionResult & gprResult);

  // Probability History
  ScalarCollection getProbabilityHistory() const;

  void setProbabilityHistory(const ScalarCollection & probabilityHistory);

  // Reliability Index History
  ScalarCollection getReliabilityIndexHistory() const;

  void setReliabilityIndexHistory(const ScalarCollection & reliabilityIndexHistory);

  // Function call number
  UnsignedInteger getFunctionCallNumber() const;

  void setFunctionCallNumber(const UnsignedInteger functionCallNumber);

  // Probability confidence interval
  Interval getProbabilityConfidenceInterval() const;

  void setProbabilityConfidenceInterval(const Interval probabilityCI);

  // Reliability index confidence interval
  Interval getReliabilityIndexConfidenceInterval() const ;

  void setReliabilityIndexConfidenceInterval(const Interval reliabilityIndexCI);
  
protected:

  // Gaussian Process Regressor
  GaussianProcessRegressionResult gprResult_;

  // Probability of failure and reliability index
  Scalar probabilityEstimate_;
  Scalar reliabilityIndex_;

  // Probability and reliability index histories
  Collection<Scalar> probabilityHistory_;
  Collection<Scalar> reliabilityIndexHistory_;

  // Simulation budget
  UnsignedInteger functionCallNumber_;

  // Probability confidence interval
  Interval probabilityCI_;

  // Reliability index confidence interval
  Interval reliabilityIndexCI_;

}; /* class ActiveLearningReliabilityResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ACTIVELEARNINGRELIABILITYRESULT_HXX */
