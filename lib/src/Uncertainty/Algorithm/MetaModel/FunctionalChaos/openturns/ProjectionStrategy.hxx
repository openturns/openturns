//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for projection strategies
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PROJECTIONSTRATEGY_HXX
#define OPENTURNS_PROJECTIONSTRATEGY_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/ProjectionStrategyImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ProjectionStrategy
 *
 * This is the interface class for projection strategies
 */

class OT_API ProjectionStrategy
  : public TypedInterfaceObject<ProjectionStrategyImplementation>
{
  CLASSNAME
public:

  typedef ProjectionStrategyImplementation::FunctionCollection FunctionCollection;

  /** Default constructor */
  ProjectionStrategy();

  /** Parameter constructor */
  ProjectionStrategy(const Distribution & measure);

  /** Constructor from implementation */
  ProjectionStrategy(const ProjectionStrategyImplementation & implementation);

  /** Measure accessor */
  void setMeasure(const Distribution & measure);
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
  virtual Point getCoefficients() const;

  /** Experiment accessors */
  virtual void setExperiment(const WeightedExperiment & weightedExperiment);
  virtual WeightedExperiment getExperiment() const;

  /** Compute the components alpha_k_p_ by projecting the model on the partial L2 basis */
  void computeCoefficients(const Function & function,
                           const FunctionCollection & basis,
                           const Indices & indices,
                           const Indices & addedRanks,
                           const Indices & conservedRanks,
                           const Indices & removedRanks,
                           const UnsignedInteger marginalIndex = 0);

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

protected:

private:

} ; /* class ProjectionStrategy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROJECTIONSTRATEGY_HXX */
