//                                               -*- C++ -*-
/**
 *  @brief Experiment to compute Sobol' indices
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
#ifndef OPENTURNS_SOBOLINDICESEXPERIMENT_HXX
#define OPENTURNS_SOBOLINDICESEXPERIMENT_HXX

#include "openturns/WeightedExperimentImplementation.hxx"
#include "openturns/WeightedExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SobolIndicesExperiment
 *
 * @brief Experiment to compute Sobol' indices
 */
class OT_API SobolIndicesExperiment
  : public WeightedExperimentImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  SobolIndicesExperiment();

  /** Parameters constructor */
  explicit SobolIndicesExperiment(const WeightedExperiment & experiment,
                                  const Bool computeSecondOrder = false);

  /** Parameters constructor */
  SobolIndicesExperiment(const Distribution & distribution,
                         const UnsignedInteger size,
                         const Bool computeSecondOrder = false);

  /** Virtual constructor */
  SobolIndicesExperiment * clone() const override;

  /** String converter */
  String __repr__() const override;

  /* Here is the interface that all derived class must implement */

  /** Size accessor */
  void setSize(const UnsignedInteger size) override;

  /** Distribution accessor */
  void setDistribution(const Distribution & distribution) override;
  Distribution getDistribution() const override;

  /** Uniform weights ? */
  Bool hasUniformWeights() const override;

  /** Sample generation */
  Sample generateWithWeights(Point & weights) const override;

  /** WeightedExperiment accessor */
  WeightedExperiment getWeightedExperiment() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:
  WeightedExperiment experiment_;

  Bool computeSecondOrder_;
}; /* class SobolIndicesExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOBOLINDICESEXPERIMENT_HXX */
