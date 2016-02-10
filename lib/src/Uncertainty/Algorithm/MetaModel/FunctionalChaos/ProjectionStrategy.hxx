//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for projection strategies
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
#ifndef OPENTURNS_PROJECTIONSTRATEGY_HXX
#define OPENTURNS_PROJECTIONSTRATEGY_HXX

#include "TypedInterfaceObject.hxx"
#include "ProjectionStrategyImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ProjectionStrategy
 *
 * This is the interface class for projection strategies
 */

class OT_API ProjectionStrategy
  : public TypedInterfaceObject<ProjectionStrategyImplementation>
{
  CLASSNAME;
public:

  typedef Collection<NumericalMathFunction> NumericalMathFunctionCollection;

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
  void computeCoefficients(const NumericalMathFunction & function,
                           const Basis & basis,
                           const Indices & indices,
                           const Indices & addedRanks,
                           const Indices & conservedRanks,
                           const Indices & removedRanks,
                           const UnsignedInteger marginalIndex = 0);

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

protected:

private:

} ; /* class ProjectionStrategy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROJECTIONSTRATEGY_HXX */
