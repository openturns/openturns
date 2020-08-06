//                                               -*- C++ -*-
/**
 *  @brief This is a abstract class for projection strategy implementations
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PROJECTIONSTRATEGYIMPLEMENTATION_HXX
#define OPENTURNS_PROJECTIONSTRATEGYIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Function.hxx"
#include "openturns/Indices.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Sample.hxx"
#include "openturns/DesignProxy.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ProjectionStrategyImplementation
 *
 * This is a abstract class for projection strategy implementations
 */

class OT_API ProjectionStrategyImplementation
  : public PersistentObject
{
  CLASSNAME
  // Make the FunctionalChaosAlgorithm class a friend of the ProjectionStrategyImplementation class
  // as this last class is in charge of managing data that are part of the FunctionalChaosAlgorithm
  // state
  friend class FunctionalChaosAlgorithm;

public:

  typedef Collection<Function> FunctionCollection;

  /** Default constructor */
  ProjectionStrategyImplementation();

  /** Parameter constructor */
  ProjectionStrategyImplementation(const Distribution & measure);

  /** Parameter constructor */
  ProjectionStrategyImplementation(const WeightedExperiment & weightedExperiment);

  /** Parameter constructor */
  ProjectionStrategyImplementation(const Distribution & measure,
                                   const WeightedExperiment & weightedExperiment);

  /** Parameter constructor */
  ProjectionStrategyImplementation(const Sample & inputSample,
                                   const Point & weights,
                                   const Sample & outputSample);

  /** Virtual constructor */
  ProjectionStrategyImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Measure accessor */
  virtual void setMeasure(const Distribution & measure);
  Distribution getMeasure() const;

  /** Sample accessors */
  virtual void setInputSample(const Sample & inputSample);
  virtual Sample getInputSample() const;

  virtual void setOutputSample(const Sample & outputSample);
  virtual Sample getOutputSample() const;

  /** Weights accessor */
  virtual void setWeights(const Point & weights);
  virtual Point getWeights() const;

  /** Residual accessor */
  virtual Scalar getResidual() const;

  /** Relative error accessor */
  virtual Scalar getRelativeError() const;

  /** Relative error accessor */
//   virtual void setCoefficients(const Point & alpha_k);
  virtual Point getCoefficients() const;

  /** Experiment accessors */
  virtual void setExperiment(const WeightedExperiment & weightedExperiment);
  virtual WeightedExperiment getExperiment() const;

  /** Compute the components alpha_k_p_ by projecting the model on the partial L2 basis */
  virtual void computeCoefficients(const Function & function,
                                   const FunctionCollection & basis,
                                   const Indices & indices,
                                   const Indices & addedRanks,
                                   const Indices & conservedRanks,
                                   const Indices & removedRanks,
                                   const UnsignedInteger marginalIndex = 0);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** The collection of Alpha_k coefficients */
  Point alpha_k_p_;

  /** Residual */
  Scalar residual_p_;

  /** Relative error */
  Scalar relativeError_p_;

  /** The measureing function for projection */
  Distribution measure_;

  // An  experiment that will be used to discretize the L2 integral
  WeightedExperiment weightedExperiment_;

  // The input sample which will be used for the projection
  Sample inputSample_;

  // The weights associated with the sample
  Point weights_;

  // The sample value of the model
  Sample outputSample_;

  // Proxy to speed up evaluations of the basis over the input sample
  DesignProxy proxy_;
private:

} ; /* class ProjectionStrategyImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROJECTIONSTRATEGYIMPLEMENTATION_HXX */
