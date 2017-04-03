//                                               -*- C++ -*-
/**
 *  @brief An implementation class for functionalChaos random vectors
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_FUNCTIONALCHAOSRANDOMVECTOR_HXX
#define OPENTURNS_FUNCTIONALCHAOSRANDOMVECTOR_HXX

#include "openturns/CompositeRandomVector.hxx"
#include "openturns/FunctionalChaosResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FunctionalChaosRandomVector
 *
 * An implementation class for functionalChaos random vectors
 */
class OT_API FunctionalChaosRandomVector
  : public CompositeRandomVector
{
  CLASSNAME;

public:

  /** Some typedefs to ease reading */

  /** Default constructor */
  FunctionalChaosRandomVector(const FunctionalChaosResult & functionalChaosResult);

  /** Virtual constructor */
  virtual FunctionalChaosRandomVector * clone() const;

  /** String converter */
  String __repr__() const;

  /* Here is the interface that all derived class must implement */

  /** Mean accessor */
  Point getMean() const;

  /** Covariance accessor */
  CovarianceMatrix getCovariance() const;

  /** Sobol index accessor */
  Scalar getSobolIndex(const Indices & variableIndices,
                                const UnsignedInteger marginalIndex = 0) const;
  Scalar getSobolIndex(const UnsignedInteger variableIndex,
                                const UnsignedInteger marginalIndex = 0) const;

  /** Sobol total index accessor */
  Scalar getSobolTotalIndex(const Indices & variableIndices,
                                     const UnsignedInteger marginalIndex = 0) const;
  Scalar getSobolTotalIndex(const UnsignedInteger variableIndex,
                                     const UnsignedInteger marginalIndex = 0) const;

  /** Sobol grouped index accessor */
  Scalar getSobolGroupedIndex(const Indices & variableIndices,
				       const UnsignedInteger marginalIndex = 0) const;
  Scalar getSobolGroupedIndex(const UnsignedInteger variableIndex,
				       const UnsignedInteger marginalIndex = 0) const;

  /** Functional chaos result accessor */
  FunctionalChaosResult getFunctionalChaosResult() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  friend class Factory<FunctionalChaosRandomVector>;

  /** Default constructor */
  FunctionalChaosRandomVector() : CompositeRandomVector() {};

private:
  /** Compute the covariance */
  void computeCovariance() const;

  /** Scalar value of the covariance */
  mutable CovarianceMatrix covariance_;

  /** Flag to tell if the covariance has already been computed */
  mutable Bool isAlreadyComputedCovariance_;

  /** The functional chaos result tht allows to build the random vector */
  FunctionalChaosResult functionalChaosResult_;

}; /* class FunctionalChaosRandomVector */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FUNCTIONALCHAOSRANDOMVECTOR_HXX */
