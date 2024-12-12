//                                               -*- C++ -*-
/**
 *  @brief SlicedInverseRegression
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
#ifndef OPENTURNS_SLICEDINVERSEREGRESSION_HXX
#define OPENTURNS_SLICEDINVERSEREGRESSION_HXX

#include "openturns/Sample.hxx"
#include "openturns/SlicedInverseRegressionResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SlicedInverseRegression
 *
 * SIR dimension reduction
 */
class OT_API SlicedInverseRegression
  : public PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  SlicedInverseRegression();

  SlicedInverseRegression(const Sample & inputSample,
                          const Sample & outputSample,
                          const UnsignedInteger sliceNumber = 10);

  /** Virtual constructor method */
  SlicedInverseRegression * clone() const override;

  /** Slice number accessor */
  void setSliceNumber(const UnsignedInteger sliceNumber);
  UnsignedInteger getSliceNumber() const;

  /** Modes number accessor */
  void setNbModes(const UnsignedInteger nbModes);
  UnsignedInteger getNbModes() const;

  void run();

  SlicedInverseRegressionResult getResult() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Sample inputSample_;
  Sample outputSample_;

  UnsignedInteger nbModes_ = 0;
  UnsignedInteger sliceNumber_ = 10;

  SlicedInverseRegressionResult result_;

}; /* class SlicedInverseRegression */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SLICEDINVERSEREGRESSION_HXX */
