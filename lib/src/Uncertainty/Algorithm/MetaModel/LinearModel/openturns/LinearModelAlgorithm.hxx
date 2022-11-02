//                                               -*- C++ -*-
/**
 *  @brief LinearModelAlgorithm implements the linear model
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LINEARMODELALGORITHM_HXX
#define OPENTURNS_LINEARMODELALGORITHM_HXX

#include "openturns/MetaModelAlgorithm.hxx"
#include "openturns/Sample.hxx"
#include "openturns/LinearModelResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LinearModelAlgorithm
 *
 * LinearModelAlgorithm implements the notion of linear model
 */

class OT_API LinearModelAlgorithm :
  public MetaModelAlgorithm
{
  CLASSNAME

public:

  /** Default constructor is private */
  LinearModelAlgorithm();

  /** Parameters constructor */
  LinearModelAlgorithm(const Sample & inputSample,
                       const Sample & outputSample);

  /** Parameters constructor */
  LinearModelAlgorithm(const Sample &inputSample,
                       const Sample &outputSample,
                       const Basis &basis);

  /** Oldest constructor (for compatibility) */
  LinearModelAlgorithm(const Sample &inputSample,
                       const Basis &basis,
                       const Sample &outputSample);

  /** Virtual constructor */
  LinearModelAlgorithm *clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Basis accessor */
  Basis getBasis() const;

  /** Perform regression */
  void run() override;

  /** result accessor */
  LinearModelResult getResult();

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


private:
  /** The basis */
  Basis basis_;

  /** Result */
  LinearModelResult result_;

  /** Bool to tell if optimization has run */
  Bool hasRun_ = false;

}; /* class LinearModelAlgorithm */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARMODELALGORITHM_HXX */
