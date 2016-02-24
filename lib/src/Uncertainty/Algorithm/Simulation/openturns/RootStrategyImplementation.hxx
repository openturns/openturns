//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_ROOTSTRATEGYIMPLEMENTATION_HXX
#define OPENTURNS_ROOTSTRATEGYIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Solver.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RootStrategyImplementation
 */

class OT_API RootStrategyImplementation
  : public PersistentObject
{

  CLASSNAME;
public:

  typedef Collection<NumericalScalar> NumericalScalarCollection;

  /** Default constructor with Brent solver */
  RootStrategyImplementation();

  /** Constructor with parameters */
  explicit RootStrategyImplementation(const Solver & solver);

  /** Constructor with parameters */
  RootStrategyImplementation(const Solver & solver,
                             const NumericalScalar maximumDistance,
                             const NumericalScalar stepSize);


  /** Virtual constructor */
  virtual RootStrategyImplementation * clone() const;

  /** Solve gives all the roots found applying the root strategy */
  virtual NumericalScalarCollection solve(const NumericalMathFunction & function,
                                          const NumericalScalar value);

  /** Solver accessor */
  void setSolver(const Solver & solver);
  Solver getSolver() const;

  /** Maximum distance accessor */
  void setMaximumDistance(const NumericalScalar maximumDistance);
  NumericalScalar getMaximumDistance() const;

  /** Step size accessor */
  void setStepSize(const NumericalScalar stepSize);
  NumericalScalar getStepSize() const;

  /** Value of the performance function at the origin accessor */
  void setOriginValue(const NumericalScalar originValue);
  NumericalScalar getOriginValue() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:
  /** Univariate root solver for the localization of zeros along a ray */
  Solver solver_;
  /** Distance beyond which the density is negligible */
  NumericalScalar maximumDistance_;
  /** Size of the intervals in which we look for zeros */
  NumericalScalar stepSize_;
  /** Flag to avoid repeated evaluations of the model at the origin */
  mutable Bool isAlreadyComputedOriginValue_;
  /** Value of the model at the origin */
  NumericalScalar originValue_;

} ; /* class RootStrategyImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ROOTSTRATEGYIMPLEMENTATION_HXX */
