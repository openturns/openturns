//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math function implementations
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
#ifndef OPENTURNS_FUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_FUNCTIONIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Field.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Evaluation.hxx"
#include "openturns/Gradient.hxx"
#include "openturns/Hessian.hxx"
#include "openturns/Description.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

// Forward declaration
class Function;

/**
 * @class FunctionImplementation
 *
 * The class that simulates a numerical math function,
 * its gradient and its hessian. This class is just an interface
 * to actual implementation objects that can be hot-replaced
 * during computation. Each implementation object refers to
 * the evaluation, the gradient or the hessian.
 */
class OT_API FunctionImplementation
  : public PersistentObject
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */
  typedef Pointer<FunctionImplementation>           Implementation;

public:

  /** Default constructor */
  FunctionImplementation();

  /** Constructor from implementations */
  FunctionImplementation(const Evaluation & funcImpl,
                         const Gradient & gradImpl,
                         const Hessian  & hessImpl);

  /** Single function implementation constructor */
  FunctionImplementation(const Evaluation & evaluation);

  /** Multiplication of two 1D output functions with the same input dimension */
  virtual FunctionImplementation operator * (const FunctionImplementation & right) const;

  /** Multiplication of two 1D output functions with the same input dimension */
  virtual FunctionImplementation operator * (const Implementation & p_right) const;

  /** Virtual constructor */
  FunctionImplementation * clone() const override;

  /** Comparison operator */
protected:
  using PersistentObject::operator ==;
public:
  Bool operator ==(const FunctionImplementation & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;
  String _repr_html_() const override;

  /** Function implementation accessors */
  virtual void setEvaluation(const Evaluation & evaluation);
  Evaluation getEvaluation() const;

  /** Gradient implementation accessors */
  void setGradient(const Gradient & gradient);
  Gradient getGradient() const;

  /** Hessian implementation accessors */
  void setHessian(const Hessian & hessian);
  Hessian getHessian() const;

  /** Flag for default gradient accessors */
  Bool getUseDefaultGradientImplementation() const;
  void setUseDefaultGradientImplementation(const Bool gradientFlag);

  /** Flag for default hessian accessors */
  Bool getUseDefaultHessianImplementation() const;
  void setUseDefaultHessianImplementation(const Bool hessianFlag);

  /** Operator () */
  virtual Point operator() (const Point & inP) const;

  virtual Sample operator() (const Sample & inS) const;

  virtual Field operator() (const Field & inField) const;

  /** Method gradient() returns the Jacobian transposed matrix of the function at point */
  virtual Matrix gradient(const Point & inP) const;

  /** Method hessian() returns the symmetric tensor of the function at point */
  virtual SymmetricTensor hessian(const Point & inP) const;

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const Point & inP) const;

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameter);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;
  virtual void setParameterDescription(const Description & description);

  /** Accessor for parameter dimension */
  virtual UnsignedInteger getParameterDimension() const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Description Accessor, i.e. the names of the input and output parameters */
  virtual void setDescription(const Description & description);
  virtual Description getDescription() const;

  /** Input description Accessor, i.e. the names of the input parameters */
  void setInputDescription(const Description & inputDescription);
  virtual Description getInputDescription() const;

  /** Output description Accessor, i.e. the names of the Output parameters */
  void setOutputDescription(const Description & outputDescription);
  virtual Description getOutputDescription() const;

  /** Get the i-th marginal function */
  virtual Function getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  virtual Function getMarginal(const Indices & indices) const;

  /** Number of calls to the evaluation */
  virtual UnsignedInteger getCallsNumber() const;
  virtual UnsignedInteger getEvaluationCallsNumber() const;

  /** Number of calls to the gradient */
  virtual UnsignedInteger getGradientCallsNumber() const;

  /** Number of calls to the hessian */
  virtual UnsignedInteger getHessianCallsNumber() const;

  /** Linearity accessors */
  virtual Bool isLinear() const;
  virtual Bool isLinearlyDependent(const UnsignedInteger index) const;

  /** Is it safe to call in parallel? */
  virtual Bool isParallel() const;

  /** Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
  virtual Graph draw(const UnsignedInteger inputMarginal,
                     const UnsignedInteger outputMarginal,
                     const Point & centralPoint,
                     const Scalar xMin,
                     const Scalar xMax,
                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber"),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the given 1D marginal output as a function of the given 2D marginal input around the given central point */
  virtual Graph draw(const UnsignedInteger firstInputMarginal,
                     const UnsignedInteger secondInputMarginal,
                     const UnsignedInteger outputMarginal,
                     const Point & centralPoint,
                     const Point & xMin,
                     const Point & xMax,
                     const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber")),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the output of the function with respect to its input when the input and output dimensions are 1 */
  virtual Graph draw(const Scalar xMin,
                     const Scalar xMax,
                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber"),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
  virtual Graph draw(const Point & xMin,
                     const Point & xMax,
                     const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("Evaluation-DefaultPointNumber")),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** A pointer on the actual numerical math function implementation */
  Evaluation evaluation_;

  /** A pointer on the actual numerical math gradient implementation */
  Gradient gradient_;

  /** A pointer on the actual numerical math hessian implementation */
  Hessian hessian_;

protected:

  /** Flag to tell if the current gradient is a default implementation */
  mutable Bool useDefaultGradientImplementation_;

  /** Flag to tell if the curren hessian is a default implementation */
  mutable Bool useDefaultHessianImplementation_;

}; /* class FunctionImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FUNCTIONIMPLEMENTATION_HXX */
