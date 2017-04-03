//                                               -*- C++ -*-
/**
 *  @brief An interface for all implementation class of random vectors
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

/**
 * @class RandomVectorImplementation
 *
 * An interface for all implementation class of random vectors
 */
class OT_API RandomVectorImplementation
  : public PersistentObject
{
  CLASSNAME;

public:

  /** Some typedefs to ease reading */
  typedef Pointer<RandomVectorImplementation> Antecedent;
  typedef Antecedent                          Implementation;

  /** Default constructor */
  RandomVectorImplementation();

  /** Virtual constructor */
  virtual RandomVectorImplementation * clone() const;

  /** String converter */
  String __repr__() const;




  /* Here is the interface that all derived class must implement */

  /** Is the underlying random vector composite ? */
  virtual Bool isComposite() const;

  /** Dimension accessor */
  virtual UnsignedInteger getDimension() const;

  /** Realization accessor */
  virtual Point getRealization() const;

  /** Numerical sample accessor */
  virtual Sample getSample(const UnsignedInteger size) const;

  /** Mean accessor */
  virtual Point getMean() const;

  /** Covariance accessor */
  virtual CovarianceMatrix getCovariance() const;

  /** Get the random vector corresponding to the i-th marginal component */
  virtual Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the marginal random vector corresponding to indices components */
  virtual Implementation getMarginal(const Indices & indices) const;

  /** Fake method due to pbs with dynamic_cast and Pointer */
  virtual Antecedent getAntecedent() const;

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
  virtual NumericalScalar getThreshold() const;

  /** Description accessor */
  virtual void setDescription(const Description & description);
  virtual Description getDescription() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:


private:

  /** Description of each component */
  Description description_;

}; /* class RandomVectorImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RANDOMVECTORIMPLEMENTATION_HXX */
