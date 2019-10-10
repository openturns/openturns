//                                               -*- C++ -*-
/**
 *  @brief The class that implements all random vectors
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
#ifndef OPENTURNS_RANDOMVECTOR_HXX
#define OPENTURNS_RANDOMVECTOR_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/RandomVectorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RandomVector
 *
 * The class that implements all random vectors
 */
class OT_API RandomVector
  : public TypedInterfaceObject<RandomVectorImplementation>
{
  CLASSNAME

public:

  /** Default constructor */
  RandomVector();

  /** Default constructor */
  RandomVector(const RandomVectorImplementation & implementation);


  /** Constructor from implementation */
  RandomVector(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  RandomVector(RandomVectorImplementation * p_implementation);
#endif

  /** Constructor for distribution-based vector */
  explicit RandomVector(const Distribution & distribution);

  /** String converter */
  String __repr__() const;

  /** String converter */
  String __str__(const String & offset = "") const;

  /** Description Accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Is the underlying random vector composite ? */
  Bool isComposite() const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Realization accessor */
  Point getRealization() const;

  /** Numerical sample accessor */
  Sample getSample(const UnsignedInteger size) const;

  /** Get the random vector corresponding to the i-th marginal component */
  RandomVector getMarginal(const UnsignedInteger i) const;

  /** Get the marginal random vector corresponding to indices components */
  RandomVector getMarginal(const Indices & indices) const;

  /** Mean accessor */
  Point getMean() const;

  /** Covariance accessor */
  CovarianceMatrix getCovariance() const;

  /** This method allows to access the antecedent RandomVector in case of a composite RandomVector */
  RandomVector getAntecedent() const;

  /** This method allows to access the Function in case of a composite RandomVector */
  Function getFunction() const;

  /** This method allows to access the Distribution in case of a usual RandomVector */
  Distribution getDistribution() const;

  /** Operator accessor */
  ComparisonOperator getOperator() const;

  /** Threshold accessor */
  Scalar getThreshold() const;

  /** Domain */
  Domain getDomain() const;

  /** Parameters value accessor */
  Point getParameter() const;
  void setParameter(const Point & parameters);

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Whether it is an event */
  Bool isEvent() const;

  /** Intersection operator */
  RandomVector intersect(const RandomVector & other);
  RandomVector join(const RandomVector & other);

protected:

}; /* class RandomVector */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMVECTOR_HXX */
