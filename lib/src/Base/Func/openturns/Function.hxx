//                                               -*- C++ -*-
/**
 *  @brief The class that implements numerical math functions
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
  typedef FunctionImplementation::EvaluationPointer      EvaluationPointer;
  typedef FunctionImplementation::GradientPointer        GradientPointer;
  typedef FunctionImplementation::HessianPointer         HessianPointer;

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

  /** @deprecated */
  Function(const Description & inputVariablesNames,
           const Description & outputVariablesNames,
           const Description & formulas);

#ifndef SWIG
  /** Constructor from evaluation */
  explicit Function(const EvaluationPointer & evaluationImplementation);

  /** Constructor from implementations */
  Function(const EvaluationPointer & evaluationImplementation,
           const GradientPointer & gradientImplenmentation,
           const HessianPointer  & hessianImplementation);
#endif

  /** @deprecated */
  Function(const Field & field);

  /** Comparison operator */
  Bool operator ==(const Function & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;



  /** @brief Enable the internal cache
   *
   * The cache stores previously computed output values, so calling the cache before processing the %Function
   * can save much time and avoid useless computations. However, calling the cache can eat time if the computation is
   * very short. So cache is disabled by default, except when the underlying implementation uses a wrapper.
   *
   * The reason is that building and linking to a wrapper is an extra burden that is valuable only if the computation
   * code is long enough to justify it. Calling the cache in this case will save time for sure.
   */
  void enableCache() const;

  /** @brief Disable the internal cache
   * @see enableCache()
   */
  void disableCache() const;

  /** @brief Test the internal cache activity
   * @see enableCache()
   */
  Bool isCacheEnabled() const;

  /** @brief Returns the number of successful hits in the cache
   */
  UnsignedInteger getCacheHits() const;

  /** @brief Add some content to the cache
   */
  void addCacheContent(const Sample & inSample, const Sample & outSample);

  /** @brief Returns the cache input
   */
  Sample getCacheInput() const;

  /** @brief Returns the cache output
   */
  Sample getCacheOutput() const;

  /** @brief Empty the cache
   */
  void clearCache() const;

  /** Enable or disable the input/output history
   * The input and output strategies store input and output values of the function,
   * in order to allow to retrieve these values e.g. after the execution of an algorithm
   * for which the user has no access to the generated inputs and the corresponding output.
   */
  void enableHistory() const;

  /** @brief Disable the history mechanism
   * @see enableHistory()
   */
  void disableHistory() const;

  /** @brief Test the history mechanism activity
   * @see enableHistory()
   */
  Bool isHistoryEnabled() const;

  /** @brief Clear the history mechanism
   * @see enableHistory()
   */
  void clearHistory() const;

  /** @brief Retrieve the history of the input values
   * @see enableHistory()
   */
  HistoryStrategy getHistoryInput() const;

  /** @brief Retrieve the history of the output values
   * @see enableHistory()
   */
  HistoryStrategy getHistoryOutput() const;

  /** Input point / parameter history accessor */
  Sample getInputPointHistory() const;
  Sample getInputParameterHistory() const;

  /** Function implementation accessors */
  void setEvaluation(const EvaluationPointer & evaluation);
  const EvaluationPointer & getEvaluation() const;

  /** Gradient implementation accessors */
  void setGradient(const GradientImplementation & gradient);
#ifndef SWIG
  void setGradient(const GradientPointer & gradient);
#endif
  const GradientPointer & getGradient() const;

  /** Hessian implementation accessors */
  void setHessian(const HessianImplementation & hessian);
#ifndef SWIG
  void setHessian(const HessianPointer & hessian);
#endif
  const HessianPointer & getHessian() const;

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
  Point operator() (const Point & inP,
                    const Point & parameter);
  Sample operator() (const Point & point,
                     const Sample & parameters);

  Sample operator() (const Sample & inS) const;

  Field operator() (const Field & inTS) const;


  /** Method gradient() returns the Jacobian transposed matrix of the function at point */
  Matrix gradient(const Point & inP) const;
  Matrix gradient(const Point & inP,
                  const Point & parameters);

  /** Method hessian() returns the symmetric tensor of the function at point */
  SymmetricTensor hessian(const Point & inP) const;
  SymmetricTensor hessian(const Point & inP,
                          const Point & parameters);

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const Point & inP) const;
  virtual Matrix parameterGradient(const Point & inP,
                                   const Point & parameters);

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
  Description getInputDescription() const;

  /** Output description Accessor, i.e. the names of the Output parameters */
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
