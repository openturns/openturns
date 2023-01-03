//                                               -*- C++ -*-
/**
 *  @brief SORM implements the First Order Reliability Method
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
#ifndef OPENTURNS_SORM_HXX
#define OPENTURNS_SORM_HXX

#include "openturns/Analytical.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/SORMResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SORM
 * SORM implements the Second Order Reliability Method
 * and compute the results of such kind of analyses
 */

class OT_API SORM :
  public Analytical
{

  CLASSNAME
public:


  /** Default constructor */
  SORM();

  /** Constructor with parameters */
  SORM(const OptimizationAlgorithm & nearestPointAlgorithm,
       const RandomVector & event,
       const Point & physicalStartingPoint);


  /** Virtual constructor */
  SORM * clone() const override;

  /** Result accessor */
  SORMResult getResult() const;

  /** Result accessor */
  void setResult(const SORMResult & sormResult);

  /** String converter */
  String __repr__() const override;

  /** Function that computes the design point by re-using the Analytical::run() and creates a SORM::Result */
  void run() override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  SORMResult sormResult_;

} ; /* class SORM */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SORM_HXX */
