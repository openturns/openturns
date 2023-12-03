//                                               -*- C++ -*-
/**
 *  @brief ExperimentIntegration class
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_INTEGRATIONEXPERIMENT_HXX
#define OPENTURNS_INTEGRATIONEXPERIMENT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/WeightedExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExperimentIntegration
 * ExperimentIntegration interface
 */

class OT_API ExperimentIntegration
  : public PersistentObject
{

  CLASSNAME
public:

  /** Default constructor */
  ExperimentIntegration();

  /** Constructor from an implementation */
  explicit ExperimentIntegration(const WeightedExperiment & weightedExperiment);

  /** Virtual constructor */
  ExperimentIntegration * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Compute the weighted integral of the function */
  Point integrate(const Function & g) const;

  /** Compute the weighted L2 norm of the function */
  Point computeL2Norm(const Function & g) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** The weighted experiment*/
  WeightedExperiment weightedExperiment_;

} ; /* class ExperimentIntegration */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INTEGRATIONEXPERIMENT_HXX */
