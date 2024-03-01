//                                               -*- C++ -*-
/**
 *  @brief Approximation with multiple design points based on FORM
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
#ifndef OPENTURNS_MULTIFORM_HXX
#define OPENTURNS_MULTIFORM_HXX


#include "openturns/FORM.hxx"
#include "openturns/MultiFORMResult.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
  * @class MultiFORM
  */
class OT_API MultiFORM
  : public FORM
{
  CLASSNAME
public:

  /** Default constructor */
  MultiFORM();

  /** Constructors with parameters */
  MultiFORM(const OptimizationAlgorithm & solver,
            const RandomVector & event,
            const Point & physicalStartingPoint);

  MultiFORM(const OptimizationAlgorithm & solver,
            const RandomVector & event,
            const Point & physicalStartingPoint,
            const Scalar & limitStateTolerance);

  /** Virtual constructor */
  MultiFORM * clone() const override;

  /** Result accessor */
  MultiFORMResult getResult() const;

  /** Maximum number of design points accessor */
  void setMaximumDesignPointsNumber(const UnsignedInteger numberOfDesignPoints);
  UnsignedInteger getMaximumDesignPointsNumber() const;

  /** String converter */
  String __repr__() const override;

  /** Function that computes the design point by re-using the Analytical::run() and creates a MultiFORM::Result */
  void run() override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  UnsignedInteger maximumDesignPointsNumber_;
  MultiFORMResult result_;

} ; /* class MultiFORM */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MULTIFORM_HXX */

