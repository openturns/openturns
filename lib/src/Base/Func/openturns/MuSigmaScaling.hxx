//                                               -*- C++ -*-
/**
 *  @brief Standardization (mu-sigma) scaling
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
#ifndef OPENTURNS_MUSIGMASCALING_HXX
#define OPENTURNS_MUSIGMASCALING_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MuSigmaScaling
 *
 * Standardization scaling, see issue #3104
 */
class OT_API MuSigmaScaling
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  MuSigmaScaling();

  /** Parameter constructor */
  MuSigmaScaling(const Point & mu,
                 const Point & sigma);

  /** Virtual constructor */
  MuSigmaScaling * clone() const override;

  /** Mu accessor */
  Point getMu() const;

  /** Sigma accessor */
  Point getSigma() const;

  /** Forward scaling accessor */
  Function getTransform() const;

  /** Inverse scaling accessor */
  Function getInverseTransform() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** Location parameter */
  Point mu_;

  /** Scale parameter */
  Point sigma_;

} ; /* class MuSigmaScaling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MUSIGMASCALING_HXX */
