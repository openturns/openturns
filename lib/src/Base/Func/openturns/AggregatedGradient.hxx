//                                               -*- C++ -*-
/**
 *  @brief The gradient part of linear combination of polynomials
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_AGGREGATEDGRADIENT_HXX
#define OPENTURNS_AGGREGATEDGRADIENT_HXX

#include "openturns/GradientImplementation.hxx"
#include "openturns/AggregatedEvaluation.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class AggregatedGradient
 *
 * The gradient part of linear combination of polynomials
 */

class OT_API AggregatedGradient
  : public GradientImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  AggregatedGradient();

  /** Parameters constructor */
  explicit AggregatedGradient(const AggregatedEvaluation & evaluation);

#ifndef SWIG
  /** Parameters constructor */
  explicit AggregatedGradient(const Pointer<AggregatedEvaluation> & p_evaluation);
#endif

  /** Virtual constructor */
  virtual AggregatedGradient * clone() const;

  /** Gradient method */
  Matrix gradient(const Point & inP) const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:

private:
  /** The associated evaluation */
  Pointer<AggregatedEvaluation> p_evaluation_;

} ; /* class AggregatedGradient */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_AGGREGATEDGRADIENT_HXX */
