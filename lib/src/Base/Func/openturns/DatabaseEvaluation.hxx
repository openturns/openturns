//                                               -*- C++ -*-
/**
 *  @brief An implementation from samples
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DATABASEEVALUATION_HXX
#define OPENTURNS_DATABASEEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/NearestNeighbourAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class DatabaseEvaluation
 * Provided two samples X (input) and Y (output), returns Yi when evaluated on Xi.
 */
class OT_API DatabaseEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
  friend class FunctionImplementation;

public:

  /** Default constructor */
  DatabaseEvaluation();

  /** Default constructor */
  DatabaseEvaluation(const Sample & inputSample,
                     const Sample & outputSample);

  /** Virtual constructor */
  DatabaseEvaluation * clone() const override;

  /** Comparison operator */
  Bool operator ==(const DatabaseEvaluation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "" ) const override;

  /** Input sample accessor */
  void setInputSample(const Sample & inputSample);
  Sample getInputSample() const;

  /** Output sample accessor */
  void setOutputSample(const Sample & outputSample);
  Sample getOutputSample() const;

  /** Nearest neighbour algorithm accessor */
  NearestNeighbourAlgorithm getNearestNeighbourAlgorithm() const;
  void setNearestNeighbourAlgorithm(const NearestNeighbourAlgorithm & tree);

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  Point operator()(const Point & inP) const override;
  Sample operator()(const Sample & inS) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:
  /** Set cached sample */
  void setSample(const Sample & inputSample,
                 const Sample & outputSample);

  Sample inputSample_;
  Sample outputSample_;
  NearestNeighbourAlgorithm nearestNeighbour_;

private:


}; /* class DatabaseEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DATABASEEVALUATION_HXX */
