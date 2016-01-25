//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an importanceSamplingExperiment plane
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
#ifndef OPENTURNS_IMPORTANCESAMPLINGEXPERIMENT_HXX
#define OPENTURNS_IMPORTANCESAMPLINGEXPERIMENT_HXX

#include "WeightedExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ImportanceSamplingExperiment
 *
 * The class describes the probabilistic concept of importanceSamplingExperiment plan
 */
class OT_API ImportanceSamplingExperiment
  : public WeightedExperiment
{
  CLASSNAME;
public:


  /** Default constructor */
  ImportanceSamplingExperiment();

  /** Parameters constructor */
  ImportanceSamplingExperiment(const Distribution & importanceDistribution,
                               const UnsignedInteger size);

  /** Parameters constructor */
  ImportanceSamplingExperiment(const Distribution & distribution,
                               const Distribution & importanceDistribution,
                               const UnsignedInteger size);

  /** Virtual constructor */
  virtual ImportanceSamplingExperiment * clone() const;

  /** Importance distribution accessor */
  Distribution getImportanceDistribution() const;

  /** String converter */
  virtual String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /** Sample generation */
  NumericalSample generate();
  NumericalSample generate(NumericalPoint & weights);

protected:

private:
  // The importance distribution according to which the sample is built
  Distribution importanceDistribution_;

}; /* class ImportanceSamplingExperiment */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_IMPORTANCESAMPLINGEXPERIMENT_HXX */
