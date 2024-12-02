//                                               -*- C++ -*-
/**
 *  @brief This class exposes the Fejer experiments
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
#ifndef OPENTURNS_FEJEREXPERIMENT_HXX
#define OPENTURNS_FEJEREXPERIMENT_HXX

#include "openturns/WeightedExperimentImplementation.hxx"
#include "openturns/FejerAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FejerExperiment
 */

class OT_API FejerExperiment
  : public WeightedExperimentImplementation
{

  CLASSNAME

public:
  /** Default constructor */
  FejerExperiment();

  FejerExperiment(const FejerAlgorithm & algorithm,
                  const Interval & bounds);

  /** Virtual copy constructor */
  FejerExperiment *clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Uniform weights ? */
  Bool hasUniformWeights() const override;

  /** Nodes and weights generation */
  Sample generateWithWeights(Point & weightsOut) const override;

  /** Nodes generation */
  Sample generate() const override;

  /** Return isRandom flag */
  Bool isRandom() const override;

  /** Size accessor */
  void setSize(const UnsignedInteger size) override;
  UnsignedInteger getSize() const override;
  
  /** Level accessor */
  void setLevel(const UnsignedInteger level) override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  FejerAlgorithm algorithm_;
  Interval bounds_;
  
} ; /* class FejerExperiment */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FEJEREXPERIMENT_HXX */
