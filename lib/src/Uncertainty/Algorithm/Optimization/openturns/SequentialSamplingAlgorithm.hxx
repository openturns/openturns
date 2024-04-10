//                                               -*- C++ -*-
/**
 *  @brief Regression from a data sample upon a particular basis
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SEQUENTIALSAMPLINGALGORITHM_HXX
#define OPENTURNS_SEQUENTIALSAMPLINGALGORITHM_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/SequentialSamplingAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SequentialSamplingAlgorithm
 *
 * Regression from a data sample upon a particular basis
 */
class OT_API SequentialSamplingAlgorithm
  : public TypedInterfaceObject<SequentialSamplingAlgorithmImplementation>
{
  CLASSNAME

public:

  typedef TypedInterfaceObject<SequentialSamplingAlgorithmImplementation>::Implementation Implementation;

  /** Default constructor */
  SequentialSamplingAlgorithm();

  /** Constructor from implementation */
  SequentialSamplingAlgorithm(const SequentialSamplingAlgorithmImplementation & implementation);

  /** Constructor from implementation */
  SequentialSamplingAlgorithm(const Implementation & p_implementation);

  /** Sample accessors */
  Sample getInputSample() const;
  Sample getOutputSample() const;

  /** Generation indices accessor */
  Indices getGenerationIndices() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Generate a new sample */
  Sample generate(const UnsignedInteger size = 1) const;

  /** Update the state */
  void update(const Sample & x, const Sample & y);

}; /* class SequentialSamplingAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SEQUENTIALSAMPLINGALGORITHM_HXX */
