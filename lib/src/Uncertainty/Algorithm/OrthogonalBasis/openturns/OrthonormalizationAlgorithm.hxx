//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ORTHONORMALIZATIONALGORITHM_HXX
#define OPENTURNS_ORTHONORMALIZATIONALGORITHM_HXX

#include "openturns/OrthonormalizationAlgorithmImplementation.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthonormalizationAlgorithm
 */

class OT_API OrthonormalizationAlgorithm
  : public TypedInterfaceObject<OrthonormalizationAlgorithmImplementation>
{
  CLASSNAME

public:
  typedef Pointer<OrthonormalizationAlgorithmImplementation> Implementation;
  typedef OrthonormalizationAlgorithmImplementation::Coefficients          Coefficients;


  /** Constructor with parameters */
  OrthonormalizationAlgorithm(const OrthonormalizationAlgorithmImplementation & implementation);

  /** Constructor with parameters */
  OrthonormalizationAlgorithm(Implementation & p_implementation);

  /** Constructor with parameters */
  explicit OrthonormalizationAlgorithm(const Distribution & measure);

  /** Default Constructor */
  OrthonormalizationAlgorithm();

  /** Calculate the coefficients of recurrence a0, a1, a2 such that
      Pn+1(x) = (a0 * x + a1) * Pn(x) + a2 * Pn-1(x) */
  Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const;

  /** Measure accessor */
  Distribution getMeasure() const;
  void setMeasure(const Distribution & measure);

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

} ; /* class OrthonormalizationAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHONORMALIZATIONALGORITHM_HXX */
