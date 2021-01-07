//                                               -*- C++ -*-
/**
 *  @brief Approximation algorithm for system events based on FORM
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SYSTEMFORM_HXX
#define OPENTURNS_SYSTEMFORM_HXX

#include "openturns/Analytical.hxx"
#include "openturns/MultiFORMResult.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
  * @class SystemFORM
  *
  */
class OT_API SystemFORM : public Analytical
{
  CLASSNAME;
public:

  /** Default constructor */
  SystemFORM();

  /** Constructor with parameters */
  SystemFORM(const OptimizationAlgorithm & nearestPointAlgorithm,
             const RandomVector & event,
             const Point & physicalStartingPoint);

  /** Virtual constructor */
  SystemFORM * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Event accessor */
  void setEvent(const RandomVector & systemEvent) override;

  /** Function that computes the design point by re-using the Analytical::run() and creates a SystemFORM::Result */
  void run() override;

  /** Result accessor */
  MultiFORMResult getResult() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  MultiFORMResult multiFORMResult_;

} ; /* class SystemFORM */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SYSTEMFORM_HXX */



