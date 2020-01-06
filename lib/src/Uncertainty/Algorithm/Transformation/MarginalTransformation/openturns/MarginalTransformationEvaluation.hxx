//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformationEvaluation evaluation for elliptical
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MARGINALTRANSFORMATIONEVALUATION_HXX
#define OPENTURNS_MARGINALTRANSFORMATIONEVALUATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Function.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/Point.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Uniform.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MarginalTransformationEvaluation
 *
 * This class offers an interface for the Nataf function for elliptical distributions
 */
class OT_API MarginalTransformationEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  typedef Collection<Distribution>              DistributionCollection;
  typedef PersistentCollection<Distribution>    DistributionPersistentCollection;

  enum TranformationDirection { FROM, TO };

  /** Default constructor */
  MarginalTransformationEvaluation();

  /** Parameter constructor */
  explicit MarginalTransformationEvaluation(const DistributionCollection & distributionCollection,
      const UnsignedInteger direction = FROM,
      const Distribution & standardMarginal = Uniform(0.0, 1.0));

  /** Parameter constructor */
  MarginalTransformationEvaluation(const DistributionCollection & inputDistributionCollection,
                                   const DistributionCollection & outputDistributionCollection,
                                   const Bool simplify = ResourceMap::GetAsBool("MarginalTransformationEvaluation-Simplify"));

  /** Virtual constructor */
  virtual MarginalTransformationEvaluation * clone() const;

  /** Evaluation */
  using EvaluationImplementation::operator();
  Point operator () (const Point & inP) const;

  /** Gradient according to the marginal parameters */
  Matrix parameterGradient(const Point & inP) const;

  /** Parameters value accessor */
  virtual void setParameter(const Point & parameter);
  virtual Point getParameter() const;

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;
  virtual void setParameterDescription(const Description & description);

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Input distribution collection accessor */
  void setInputDistributionCollection(const DistributionCollection & inputDistributionCollection);
  DistributionCollection getInputDistributionCollection() const;

  /** Output distribution collection accessor */
  void setOutputDistributionCollection(const DistributionCollection & outputDistributionCollection);
  DistributionCollection getOutputDistributionCollection() const;

  /** Simplifications accessor */
  Collection<UnsignedInteger> getSimplifications() const;

  /** Expressions accessor */
  Collection<Function> getExpressions() const;

  /** String converter */
  String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:
  void initialize(const Bool simplify);

  // Make the gradient and the hessian friend classes of the evaluation in order to share the input and output distribution collections
  friend class MarginalTransformationGradient;
  friend class MarginalTransformationHessian;

  // marginal distributions of the input
  DistributionPersistentCollection inputDistributionCollection_;

  // Marginal distributions of the output
  DistributionPersistentCollection outputDistributionCollection_;

  // Parameter location flag
  enum ParameterSide { NONE, LEFT, RIGHT, BOTH };

  UnsignedInteger parameterSide_;

  // Flag to tell if simpler expressions are available
  mutable Indices simplifications_;

  // Collection of simpler expressions
  mutable Collection<Function> expressions_;

  // CDF tail threshold
  Scalar tailThreshold_;

}; /* MarginalTransformationEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MARGINALTRANSFORMATIONEVALUATION_HXX */
