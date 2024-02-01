//                                               -*- C++ -*-
/**
 *  @brief An interface for all implementation class of random vectors
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
#ifndef OPENTURNS_RANDOMVECTORIMPLEMENTATION_HXX
#define OPENTURNS_RANDOMVECTORIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Function.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/ComparisonOperator.hxx"
#include "openturns/FunctionalChaosResult.hxx"
#include "openturns/Domain.hxx"
#include "openturns/Process.hxx"

BEGIN_NAMESPACE_OPENTURNS

// Forward class
class RandomVector;

/**
 * @class RandomVectorImplementation
 *
 * An interface for all implementation class of random vectors
 */
class OT_API RandomVectorImplementation
  : public PersistentObject
{
  CLASSNAME

public:

  /** Default constructor */
  RandomVectorImplementation();

  /** Virtual constructor */
  RandomVectorImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;




  /* Here is the interface that all derived class must implement */

  /** Is the underlying random vector composite ? */
  virtual Bool isComposite() const;

  /** Dimension accessor */
  virtual UnsignedInteger getDimension() const;

  /** Realization accessor */
  virtual Point getRealization() const;

  /** Fixed value accessor */
  virtual Point getFrozenRealization(const Point & fixedPoint) const;

  /** Numerical sample accessor */
  virtual Sample getSample(const UnsignedInteger size) const;

  /** Fixed sample accessor */
  virtual Sample getFrozenSample(const Sample & fixedSample) const;

  /** Mean accessor */
  virtual Point getMean() const;

  /** Covariance accessor */
  virtual CovarianceMatrix getCovariance() const;

  /** Get the random vector corresponding to the i-th marginal component */
  virtual RandomVector getMarginal(const UnsignedInteger i) const;

  /** Get the marginal random vector corresponding to indices components */
  virtual RandomVector getMarginal(const Indices & indices) const;

  /** Fake method due to pbs with dynamic_cast and Pointer */
  virtual RandomVector getAntecedent() const;

  /** Fake method due to pbs with dynamic_cast and Pointer */
  virtual Function getFunction() const;

  /** Fake method due to pbs with dynamic_cast and Pointer */
  virtual Distribution getDistribution() const;

  /** Fake method due to pbs with dynamic_cast and Pointer */
  virtual ComparisonOperator getOperator() const;

  /** Fake method due to pbs with dynamic_cast and Pointer */
  virtual Domain getDomain() const;

  /** Fake method due to pbs with dynamic_cast and Pointer */
  virtual Process getProcess() const;

  /** Fake method due to pbs with dynamic_cast and Pointer */
  virtual Scalar getThreshold() const;

  /** Description accessor */
  virtual void setDescription(const Description & description);
  virtual Description getDescription() const;

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameters);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;

  /** Whether it is an event */
  virtual Bool isEvent() const;

  /** Composed event accessor */
  virtual RandomVector asComposedEvent() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  /** Description of each component */
  Description description_;

}; /* class RandomVectorImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMVECTORIMPLEMENTATION_HXX */
