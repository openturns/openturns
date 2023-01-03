//                                               -*- C++ -*-
/**
 *  @brief P1 Lagrange piecewise linear function.
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_P1LAGRANGEEVALUATION_HXX
#define OPENTURNS_P1LAGRANGEEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Field.hxx"
#include "openturns/NearestNeighbourAlgorithm.hxx"
#include "openturns/EnclosingSimplexAlgorithm.hxx"
#include "openturns/ProcessSample.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class P1LagrangeEvaluation
 * Provided a field, compute the P1 piecewise-linear Lagrange interpolation.
 */
class OT_API P1LagrangeEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
  friend class FunctionImplementation;
  friend class P1LagrangeEvaluationComputeSamplePolicy;

public:

  /** Default constructor */
  P1LagrangeEvaluation();

  /** Parameters constructor */
  explicit P1LagrangeEvaluation(const Field & field);

  /** Parameters constructor */
  explicit P1LagrangeEvaluation(const ProcessSample & sample);

  /** Virtual constructor */
  P1LagrangeEvaluation * clone() const override;

  /** Comparison operator */
  Bool operator ==(const P1LagrangeEvaluation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "" ) const override;

  /** Field accessor */
  void setField(const Field & field);
  Field getField() const;

  /** Mesh accessor */
  void setMesh(const Mesh & mesh);
  Mesh getMesh() const;

  /** Values accessor */
  void setValues(const Sample & values);
  Sample getValues() const;

  /** Nearest neighbour algorithm accessor */
  void setNearestNeighbourAlgorithm(const NearestNeighbourAlgorithm & nearestNeighbour);
  NearestNeighbourAlgorithm getNearestNeighbourAlgorithm() const;

  /** Enclosing simplex algorithm accessor */
  void setEnclosingSimplexAlgorithm(const EnclosingSimplexAlgorithm & enclosingSimplex);
  EnclosingSimplexAlgorithm getEnclosingSimplexAlgorithm() const;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  using EvaluationImplementation::operator ();
  Point operator()(const Point & inP) const override;
  Sample operator()(const Sample & inS) const override;

protected:
  Point evaluate(const Point & inP) const;

public:

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:
  /* Mesh of the field defining the P1 Lagrange interpolation */
  Mesh mesh_;

  /* Values of the field defining the P1 Lagrange interpolation */
  Sample values_;

  /* NearestNeighbourAlgorithm to speed-up nearest-neighbour search */
  NearestNeighbourAlgorithm nearestNeighbour_;

  /* EnclosingSimplexAlgorithm to speed-up point location */
  EnclosingSimplexAlgorithm enclosingSimplex_;

private:

}; /* class P1LagrangeEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_P1LAGRANGEEVALUATION_HXX */
