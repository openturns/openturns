//                                               -*- C++ -*-
/**
 *  @brief This is a abstract class for projection strategy implementations
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_PROJECTIONSTRATEGYIMPLEMENTATION_HXX
#define OPENTURNS_PROJECTIONSTRATEGYIMPLEMENTATION_HXX

#include "PersistentObject.hxx"
#include "NumericalPoint.hxx"
#include "Distribution.hxx"
#include "Collection.hxx"
#include "NumericalMathFunction.hxx"
#include "Indices.hxx"
#include "WeightedExperiment.hxx"
#include "Pointer.hxx"
#include "NumericalSample.hxx"
#include "Basis.hxx"
#include "DesignProxy.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ProjectionStrategyImplementation
 *
 * This is a abstract class for projection strategy implementations
 */

class OT_API ProjectionStrategyImplementation
  : public PersistentObject
{
  CLASSNAME;
  // Make the FunctionalChaosAlgorithm class a friend of the ProjectionStrategyImplementation class
  // as this last class is in charge of managing data that are part of the FunctionalChaosAlgorithm
  // state
  friend class FunctionalChaosAlgorithm;

public:

  typedef Collection<NumericalMathFunction> NumericalMathFunctionCollection;
  typedef Pointer<WeightedExperiment>       WeightedExperimentImplementation;

  // friend class Factory<ProjectionStrategyImplementation>;

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
  ProjectionStrategyImplementation(const NumericalSample & inputSample,
                                   const NumericalPoint & weights,
                                   const NumericalSample & outputSample);

  /** Virtual constructor */
  virtual ProjectionStrategyImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Measure accessor */
  virtual void setMeasure(const Distribution & measure);
  Distribution getMeasure() const;

  /** Sample accessors */
  virtual NumericalSample getInputSample() const;
  virtual NumericalSample getOutputSample() const;

  /** Weights accessor */
  virtual NumericalPoint getWeights() const;

  /** Residual accessor */
  virtual NumericalScalar getResidual() const;

  /** Relative error accessor */
  virtual NumericalScalar getRelativeError() const;

  /** Relative error accessor */
  virtual NumericalPoint getCoefficients() const;

  /** Experiment accessors */
  virtual void setExperiment(const WeightedExperiment & weightedExperiment);
  virtual const WeightedExperiment & getExperiment() const;

  /** Compute the components alpha_k_p_ by projecting the model on the partial L2 basis */
  virtual void computeCoefficients(const NumericalMathFunction & function,
                                   const Basis & basis,
                                   const Indices & indices,
                                   const Indices & addedRanks,
                                   const Indices & conservedRanks,
                                   const Indices & removedRanks,
                                   const UnsignedInteger marginalIndex = 0);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

  /** The collection of Alpha_k coefficients */
  NumericalPoint alpha_k_p_;

  /** Residual */
  NumericalScalar residual_p_;

  /** Relative error */
  NumericalScalar relativeError_p_;

  /** The measureing function for projection */
  Distribution measure_;

  // An  experiment that will be used to discretize the L2 integral
  WeightedExperimentImplementation p_weightedExperiment_;

  // The input sample which will be used for the projection
  NumericalSample inputSample_;

  // The weights associated with the sample
  NumericalPoint weights_;

  // The sample value of the model
  NumericalSample outputSample_;

  DesignProxy proxy_;
private:

} ; /* class ProjectionStrategyImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROJECTIONSTRATEGYIMPLEMENTATION_HXX */
