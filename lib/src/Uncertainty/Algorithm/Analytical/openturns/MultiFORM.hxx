//                                               -*- C++ -*-
/**
 *  @brief Approximation with multiple design points based on FORM
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

  CLASSNAME;
public:

  /** Default constructor */
  MultiFORM();

  /** Constructor with parameters */
  MultiFORM(const OptimizationAlgorithm & solver,
            const RandomVector & event,
            const Point & physicalStartingPoint);

  /** Virtual constructor */
  virtual MultiFORM * clone() const;

  /** Result accessor */
  MultiFORMResult getResult() const;

  /** Maximum number of design points accessor */
  void setMaximumNumberOfDesignPoints(const UnsignedInteger numberOfDesignPoints);
  UnsignedInteger getMaximumNumberOfDesignPoints() const;

  /** String converter */
  String __repr__() const;

  /** Function that computes the design point by re-using the Analytical::run() and creates a MultiFORM::Result */
  void run();

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  UnsignedInteger maximumNumberOfDesignPoints_;
  MultiFORMResult result_;

} ; /* class MultiFORM */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MULTIFORM_HXX */

