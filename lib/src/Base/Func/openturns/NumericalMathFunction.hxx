//                                               -*- C++ -*-
/**
 *  @brief The class that implements numerical math functions
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_NUMERICALMATHFUNCTION_HXX
#define OPENTURNS_NUMERICALMATHFUNCTION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/NumericalMathFunctionImplementation.hxx"
#include "openturns/ProductNumericalMathFunction.hxx"
#include "openturns/ComparisonOperator.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NumericalMathFunction
 * @brief Simulates a numerical math function
 *
 * The class that simulates a numerical math function,
 * its gradient and its hessian. This class is just an interface
 * to actual implementation objects that can be hot-replaced
 * during computation. Each implementation object refers to
 * the function, the gradient or the hessian.
 * @see NumericalMathFunctionImplementation
 */
class OT_API NumericalMathFunction
  : public TypedInterfaceObject<NumericalMathFunctionImplementation>
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Collection<NumericalMathFunction>                                  NumericalMathFunctionCollection;
  typedef NumericalMathFunctionImplementation::EvaluationImplementation      EvaluationImplementation;
  typedef NumericalMathFunctionImplementation::GradientImplementation        GradientImplementation;
  typedef NumericalMathFunctionImplementation::HessianImplementation         HessianImplementation;

  /** Default constructor */
  NumericalMathFunction();

  /** Constructor from NumericalMathFunctionImplementation */
  NumericalMathFunction(const NumericalMathFunctionImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation */
  NumericalMathFunction(const Implementation & p_implementation);

  /** Constructor from implementation pointer */
  NumericalMathFunction(NumericalMathFunctionImplementation * p_implementation);
#endif

  /** Constructor from evaluation implementation */
  NumericalMathFunction(const NumericalMathEvaluationImplementation & evaluation);

  /** @brief Composition constructor
   *
   * Builds a new %NumericalMathFunction from two others as if they were mathematicaly composed,
   *
   * Example: h = f o g
   * - f is the left %NumericalMathFunction
   * - g is the right %NumericalMathFunction
   * - h is the composed %NumericalMathFunction
   * .
   * The condition for successful composition is that the dimension of the output of g is the dimension
   * of the input of f. The composed %NumericalMathFunction has the input dimension of g and the output dimension
   * of f.
   * @param left The left %NumericalMathFunction (aka f)
   * @param right The right %NumericalMathFunction (aka g)
   */
  NumericalMathFunction(const NumericalMathFunction & left,
                        const NumericalMathFunction & right);

  /** @brief Analytical formula constructor
   *
   * Builds a new %NumericalMathFunction by analytical expression parsing. The expression involving the input
   * variables (stored in \e inputVariablesNames) to produce the output variables (stored in \e outputVariablesNames)
   * are described in \e formulas.
   *
   * The input dimension of the new %NumericalMathFunction is the size of \e inputVariablesNames and
   * the output dimension of the new %NumericalMathFunction is the size of \e outputVariablesName.
   * @param inputVariablesNames The ordered collection of input variables names
   * @param outputVariablesNames The ordered collection of output variables names
   * @param formulas The ordered collection of analytical expressions to compute the output variables
   */
  NumericalMathFunction(const Description & inputVariablesNames,
                        const Description & outputVariablesNames,
                        const Description & formulas);

  /** Same as the previous one, but with default names for the output variables */
  NumericalMathFunction(const Description & inputVariablesNames,
                        const Description & formulas);

  /** Indicator function constructor */
  NumericalMathFunction(const NumericalMathFunction & function,
                        const ComparisonOperator & comparisonOperator,
                        const NumericalScalar threshold);

  /** Aggregated function constructor: the output is the aggregation of the several functions */
  NumericalMathFunction(const NumericalMathFunctionCollection & functionCollection);

  /** Linear combination function constructor */
  NumericalMathFunction(const NumericalMathFunctionCollection & functionCollection,
                        const NumericalPoint & coefficients);

  /** Dual linear combination function constructor */
  NumericalMathFunction(const NumericalMathFunctionCollection & functionCollection,
                        const NumericalSample & coefficients);

  /** Simplified analytical formula constructor */
  NumericalMathFunction(const String & inputVariableName,
                        const String & formula,
                        const String & outputVariableName = "outputVariable");

#ifndef SWIG
  /** Constructor from implementations */
  NumericalMathFunction(const EvaluationImplementation & evaluationImplementation,
                        const GradientImplementation & gradientImplenmentation,
                        const HessianImplementation  & hessianImplementation);
#endif

  /** Constructor from samples */
  NumericalMathFunction(const NumericalSample & inputSample,
                        const NumericalSample & outputSample);

  /** Constructor from field using P1 Lagrange interpolation */
  NumericalMathFunction(const Field & field);

  /** Constructor by splitting the input of a function between variables and parameters */
  NumericalMathFunction(const NumericalMathFunction & function,
                        const Indices & set,
                        const NumericalPoint & referencePoint,
                        const Bool parametersSet = true);

  /** Comparison operator */
  Bool operator ==(const NumericalMathFunction & other) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;



  /** @brief Enable the internal cache
   *
   * The cache stores previously computed output values, so calling the cache before processing the %NumericalMathFunction
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
  void addCacheContent(const NumericalSample & inSample, const NumericalSample & outSample);

  /** @brief Returns the cache input
   */
  NumericalSample getCacheInput() const;

  /** @brief Returns the cache output
   */
  NumericalSample getCacheOutput() const;

  /** @brief Empty the cache
   */
  void clearCache() const;

  /** Enable or disable the input/output history
   * The input and output strategies store input and output values of the function,
   * in order to allow to retrieve these values e.g. after the execution of an allgorithm
   * for which the user has no access to the generated inputs and the corresponding outut.
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

  /** Function implementation accessors */
  void setEvaluation(const EvaluationImplementation & evaluation);
  const EvaluationImplementation & getEvaluation() const;

  /** Gradient implementation accessors */
  void setGradient(const NumericalMathGradientImplementation & gradient);
#ifndef SWIG
  void setGradient(const GradientImplementation & gradient);
#endif
  const GradientImplementation & getGradient() const;

  /** Hessian implementation accessors */
  void setHessian(const NumericalMathHessianImplementation & hessian);
#ifndef SWIG
  void setHessian(const HessianImplementation & hessian);
#endif
  const HessianImplementation & getHessian() const;

  /** Flag for default gradient accessors */
  Bool getUseDefaultGradientImplementation() const;
  void setUseDefaultGradientImplementation(const Bool gradientFlag);

  /** Flag for default hessian accessors */
  Bool getUseDefaultHessianImplementation() const;
  void setUseDefaultHessianImplementation(const Bool hessianFlag);


  /** Multiplication of two 1D output functions with the same input dimension */
  virtual ProductNumericalMathFunction operator * (const NumericalMathFunction & right) const;

  /** Addition of two functions with the same input and output dimensions */
  virtual NumericalMathFunction operator + (const NumericalMathFunction & right) const;

  /** Soustraction of two functions with the same input and output dimensions */
  virtual NumericalMathFunction operator - (const NumericalMathFunction & right) const;

  /** Operator () */
  NumericalPoint operator() (const NumericalPoint & inP) const;
  NumericalPoint operator() (const NumericalPoint & inP,
                             const NumericalPoint & parameter);
  NumericalSample operator() (const NumericalPoint & point,
                              const NumericalSample & parameters);

  NumericalSample operator() (const NumericalSample & inS) const;

  Field operator() (const Field & inTS) const;


  /** Method gradient() returns the Jacobian transposed matrix of the function at point */
  Matrix gradient(const NumericalPoint & inP) const;
  Matrix gradient(const NumericalPoint & inP,
                  const NumericalPoint & parameters);

  /** Method hessian() returns the symmetric tensor of the function at point */
  SymmetricTensor hessian(const NumericalPoint & inP) const;
  SymmetricTensor hessian(const NumericalPoint & inP,
                          const NumericalPoint & parameters);

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const NumericalPoint & inP) const;
  virtual Matrix parameterGradient(const NumericalPoint & inP,
                                    const NumericalPoint & parameters);

  /** Parameters value accessor */
  virtual NumericalPoint getParameter() const;
  virtual void setParameter(const NumericalPoint & parameter);

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
  NumericalMathFunction getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  NumericalMathFunction getMarginal(const Indices & indices) const;

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
                     const NumericalPoint & centralPoint,
                     const NumericalScalar xMin,
                     const NumericalScalar xMax,
                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("NumericalMathEvaluationImplementation-DefaultPointNumber"),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the given 1D marginal output as a function of the given 2D marginal input around the given central point */
  virtual Graph draw(const UnsignedInteger firstInputMarginal,
                     const UnsignedInteger secondInputMarginal,
                     const UnsignedInteger outputMarginal,
                     const NumericalPoint & centralPoint,
                     const NumericalPoint & xMin,
                     const NumericalPoint & xMax,
                     const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("NumericalMathEvaluationImplementation-DefaultPointNumber")),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the output of the function with respect to its input when the input and output dimensions are 1 */
  virtual Graph draw(const NumericalScalar xMin,
                     const NumericalScalar xMax,
                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("NumericalMathEvaluationImplementation-DefaultPointNumber"),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
  virtual Graph draw(const NumericalPoint & xMin,
                     const NumericalPoint & xMax,
                     const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("NumericalMathEvaluationImplementation-DefaultPointNumber")),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Static methods for documentation of analytical fnctions */
  static Description GetValidConstants();
  static Description GetValidFunctions();
  static Description GetValidOperators();

}; /* class NumericalMathFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NUMERICALMATHFUNCTION_HXX */
