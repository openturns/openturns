//                                               -*- C++ -*-
/**
 *  @brief The class that implements numerical math functions
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_FUNCTION_HXX
#define OPENTURNS_FUNCTION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/FunctionImplementation.hxx"
#include "openturns/ProductFunction.hxx"
#include "openturns/ComparisonOperator.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Function
 * @brief Simulates a numerical math function
 *
 * The class that simulates a numerical math function,
 * its gradient and its hessian. This class is just an interface
 * to actual implementation objects that can be hot-replaced
 * during computation. Each implementation object refers to
 * the function, the gradient or the hessian.
 * @see FunctionImplementation
 */
class OT_API Function
  : public TypedInterfaceObject<FunctionImplementation>
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */
  typedef Collection<Function>                                  FunctionCollection;

  /** Default constructor */
  Function();

  /** Constructor from FunctionImplementation */
  Function(const FunctionImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation */
  Function(const Implementation & p_implementation);

  /** Constructor from implementation pointer */
  Function(FunctionImplementation * p_implementation);
#endif

  /** Constructor from evaluation implementation */
  Function(const EvaluationImplementation & evaluation);

#ifndef SWIG
  /** Constructor from evaluation */
  explicit Function(const Evaluation & evaluation);

  /** Constructor from implementations */
  Function(const Evaluation & evaluation,
           const Gradient & gradient,
           const Hessian  & hessian);
#endif

  /** Comparison operator */
  Bool operator ==(const Function & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

  /** Function implementation accessors */
  void setEvaluation(const Evaluation & evaluation);
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


  /** Multiplication of two 1D output functions with the same input dimension */
  virtual ProductFunction operator * (const Function & right) const;

  /** Addition of two functions with the same input and output dimensions */
  virtual Function operator + (const Function & right) const;

  /** Soustraction of two functions with the same input and output dimensions */
  virtual Function operator - (const Function & right) const;

  /** Operator () */
  Point operator() (const Point & inP) const;

  Sample operator() (const Sample & inS) const;

  Field operator() (const Field & inTS) const;

  /** Method gradient() returns the Jacobian transposed matrix of the function at point */
  Matrix gradient(const Point & inP) const;


  /** Method hessian() returns the symmetric tensor of the function at point */
  SymmetricTensor hessian(const Point & inP) const;

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const Point & inP) const;

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameter);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;
  virtual void setParameterDescription(const Description & description);

  /** Accessor for parameter dimension */
  UnsignedInteger getParameterDimension() const;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const;

  /** Description Accessor, i.e. the names of the input and output parameters */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** Input description Accessor, i.e. the names of the input parameters */
  void setInputDescription(const Description & inputDescription);
  Description getInputDescription() const;

  /** Output description Accessor, i.e. the names of the Output parameters */
  void setOutputDescription(const Description & inputDescription);
  Description getOutputDescription() const;

  /** Get the i-th marginal function */
  Function getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  Function getMarginal(const Indices & indices) const;

  /** Number of calls to the evaluation */
  UnsignedInteger getCallsNumber() const;
  UnsignedInteger getEvaluationCallsNumber() const;

  /** Number of calls to the gradient */
  UnsignedInteger getGradientCallsNumber() const;

  /** Number of calls to the hessian */
  UnsignedInteger getHessianCallsNumber() const;

  /** Linearity accessors */
  Bool isLinear() const;
  Bool isLinearlyDependent(const UnsignedInteger index) const;

  /** Draw the given 1D marginal output as a function of the given 1D marginal input around the given central point */
  virtual Graph draw(const UnsignedInteger inputMarginal,
                     const UnsignedInteger outputMarginal,
                     const Point & centralPoint,
                     const Scalar xMin,
                     const Scalar xMax,
                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("NumericalMathEvaluation-DefaultPointNumber"),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the given 1D marginal output as a function of the given 2D marginal input around the given central point */
  virtual Graph draw(const UnsignedInteger firstInputMarginal,
                     const UnsignedInteger secondInputMarginal,
                     const UnsignedInteger outputMarginal,
                     const Point & centralPoint,
                     const Point & xMin,
                     const Point & xMax,
                     const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("NumericalMathEvaluation-DefaultPointNumber")),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the output of the function with respect to its input when the input and output dimensions are 1 */
  virtual Graph draw(const Scalar xMin,
                     const Scalar xMax,
                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("NumericalMathEvaluation-DefaultPointNumber"),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
  virtual Graph draw(const Point & xMin,
                     const Point & xMax,
                     const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("NumericalMathEvaluation-DefaultPointNumber")),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

}; /* class Function */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FUNCTION_HXX */
