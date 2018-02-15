//                                               -*- C++ -*-
/**
 *  @brief P1 Lagrange piecewise linear function.
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_P1LAGRANGEEVALUATION_HXX
#define OPENTURNS_P1LAGRANGEEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Field.hxx"
#include "openturns/KDTree.hxx"

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

  typedef Mesh::IndicesCollection IndicesCollection;

  /** Default constructor */
  P1LagrangeEvaluation();

  /** Default constructor */
  explicit P1LagrangeEvaluation(const Field & field);

  /** Virtual constructor */
  virtual P1LagrangeEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const P1LagrangeEvaluation & other) const;

  /** String converter */
  String __repr__() const;
  String __str__( const String & offset = "" ) const;

  /** Field accessor */
  void setField(const Field & field);
  Field getField() const;

  /** Mesh accessor */
  void setMesh(const Mesh & mesh);
  Mesh getMesh() const;

  /** Vertices accessor */
  void setVertices(const Sample & vertices);
  Sample getVertices() const;

  /** Simplices accessor */
  void setSimplices(const IndicesCollection & simplices);
  IndicesCollection getSimplices() const;

  /** Values accessor */
  void setValues(const Sample & values);
  Sample getValues() const;

  /* Here is the interface that all derived class must implement */

  /** Operator () */
  virtual Point operator()(const Point & inP) const;
  virtual Sample operator()(const Sample & inS) const;

protected:
  Point evaluate(const Point & inP) const;

public:

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


protected:
  /* Mesh of the field defining the P1 Lagrange interpolation */
  Mesh mesh_;

  /* Values of the field defining the P1 Lagrange interpolation */
  Sample values_;

  /* KDTree to speed-up nearest-neighbour search */
  KDTree tree_;

  IndicesCollection verticesToSimplices_;

private:

  class P1LagrangeEvaluationComputeSamplePolicy
  {
    const Sample & input_;
    Sample & output_;
    const P1LagrangeEvaluation & lagrange_;

  public:
    P1LagrangeEvaluationComputeSamplePolicy(const Sample & input,
                                            Sample & output,
                                            const P1LagrangeEvaluation & lagrange)
      : input_(input)
      , output_(output)
      , lagrange_(lagrange)
    {
      // Nothing to do
    }

    inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
    {
      for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
        output_[i] = lagrange_.evaluate(input_[i]);
    } // operator ()
  };  // class P1LagrangeEvaluationComputeSamplePolicy

}; /* class P1LagrangeEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_P1LAGRANGEEVALUATION_HXX */
