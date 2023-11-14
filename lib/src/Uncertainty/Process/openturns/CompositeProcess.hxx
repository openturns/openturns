//                                               -*- C++ -*-
/**
 *  @brief A class which implements the CompositeProcess process
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
#ifndef OPENTURNS_COMPOSITEPROCESS_HXX
#define OPENTURNS_COMPOSITEPROCESS_HXX

#include "openturns/ProcessImplementation.hxx"
#include "openturns/Process.hxx"
#include "openturns/FieldFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CompositeProcess
 *
 * The implementation of the CompositeProcess class
 */
class OT_API CompositeProcess
  : public ProcessImplementation
{
  CLASSNAME

public:


  /** Default constructor */
  CompositeProcess();

  /** Standard constructor */
  CompositeProcess(const FieldFunction & function,
                   const Process & antecedent);

  /** Virtual constructor */
  CompositeProcess * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** String converter  - pretty print */
  String __str__(const String & offset = "") const override;

  /** Is the underlying a composite process ? */
  Bool isComposite() const override;

  /** Realization accessor */
  Field getRealization() const override;

  /** Continuation of the last realization on a given number of steps */
  using ProcessImplementation::getFuture;
  TimeSeries getFuture(const UnsignedInteger stepNumber) const override;

  /** Antecedent accessor */
  Process getAntecedent() const;

  /** Function accessor */
  FieldFunction getFunction() const;

  /** Get the marginal random vector corresponding to indices components */
  Process getMarginal(const Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** The function used for realization  */
  FieldFunction function_;

  /** The antecedent of the process through the function */
  Process antecedent_;

}; /* class CompositeProcess */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSITEPROCESS_HXX */
