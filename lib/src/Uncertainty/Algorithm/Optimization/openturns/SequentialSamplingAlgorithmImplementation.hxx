//                                               -*- C++ -*-
/**
 *  @brief Regression from a data sample upon a particular basis
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SEQUENTIALSAMPLINGALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_SEQUENTIALSAMPLINGALGORITHMIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/IndicesCollection.hxx"
#include "openturns/Sample.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SequentialSamplingAlgorithmImplementation
 *
 * Regression from a data sample upon a particular basis
 */
class OT_API SequentialSamplingAlgorithmImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  typedef Collection<Function> FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  /** Default constructor */
  SequentialSamplingAlgorithmImplementation();

  /** Default constructor */
  SequentialSamplingAlgorithmImplementation(const Sample & x,
      const Sample & y);

  /** Virtual constructor */
  SequentialSamplingAlgorithmImplementation * clone() const override;

  /** Sample accessors */
  Sample getInputSample() const;
  Sample getOutputSample() const;

  /** Generation indices accessor */
  Indices getGenerationIndices() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Generate a new sample */
  virtual Sample generate(const UnsignedInteger size = 1) const;

  /** Update the state */
  virtual void update(const Sample & x, const Sample & y);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Input sample */
  Sample x_;

  /** Output sample */
  Sample y_;

  /** Indices of last element of each generation */
  Indices generationIndices_;

private:

}; /* class SequentialSamplingAlgorithmImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SEQUENTIALSAMPLINGALGORITHMIMPLEMENTATION_HXX */
