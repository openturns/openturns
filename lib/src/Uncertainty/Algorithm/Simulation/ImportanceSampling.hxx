//                                               -*- C++ -*-
/**
 *  @brief ImportanceSampling is an implementation of the importance sampling Montex Carlo simulation method
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
#ifndef OPENTURNS_IMPORTANCESAMPLING_HXX
#define OPENTURNS_IMPORTANCESAMPLING_HXX

#include "Simulation.hxx"
#include "Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ImportanceSampling
 */

class OT_API ImportanceSampling :
  public Simulation
{

  CLASSNAME;
public:
  /** for save/load mecanism */
  ImportanceSampling();

  /** Constructor with parameters */
  ImportanceSampling(const Event & event,
                     const Distribution & importanceDistribution);


  /** Virtual constructor */
  virtual ImportanceSampling * clone() const;

  /** Importance distribution accessor */
  Distribution getImportanceDistribution() const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** Compute the block sample */
  NumericalSample computeBlockSample();

  /** Instrumental importance distribution */
  Distribution importanceDistribution_;

} ; /* class ImportanceSampling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_IMPORTANCESAMPLING_HXX */
