//                                               -*- C++ -*-
/**
 *  @brief MulticollinearityAnalysis implements computation of various
 *  statistics used in a multicollinear context
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_MULTICOLLINEARITYANALYSIS_HXX
#define OPENTURNS_MULTICOLLINEARITYANALYSIS_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MulticollinearityAnalysis
 */

class OT_API MulticollinearityAnalysis
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  MulticollinearityAnalysis();

  /** Constructor with input sample */
  MulticollinearityAnalysis(const Sample & firstSample);

  /** Constructor with input and output samples */
  MulticollinearityAnalysis(const Sample & firstSample,
                            const Sample & secondSample);

  /** Virtual constructor */
  MulticollinearityAnalysis * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Compute LMG and PMVD indices */
  void computeLMGPMVD(PointWithDescription & lmgOut, PointWithDescription & pmvdOut) const;

  /** Estimate LMG and PMVD indices via a Monte Carlo method */
  void estimateLMGPMVDMonteCarlo(PointWithDescription & lmgOut, PointWithDescription & pmvdOut, const UnsignedInteger iterations) const;

  /** Compute Johnson index */
  PointWithDescription computeJohnson() const;

  /** Compute VIF metric */
  PointWithDescription computeVIF() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  Sample firstSample_;
  Sample secondSample_;

}; /* MulticollinearityAnalysis */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MULTICOLLINEARITYANALYSIS_HXX */
