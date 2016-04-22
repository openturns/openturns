//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math function implementations
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
#ifndef OPENTURNS_NUMERICALMATHFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_NUMERICALMATHFUNCTIONIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/NumericalPointWithDescription.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/Field.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/NumericalMathGradientImplementation.hxx"
#include "openturns/NumericalMathHessianImplementation.hxx"
#include "openturns/Description.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NumericalMathFunctionImplementation
 *
 * The class that simulates a numerical math function,
 * its gradient and its hessian. This class is just an interface
 * to actual implementation objects that can be hot-replaced
 * during computation. Each implementation object refers to
 * the evaluation, the gradient or the hessian.
 */
class OT_API NumericalMathFunctionImplementation
  : public PersistentObject
{
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<NumericalMathFunctionImplementation>           Implementation;
  typedef NumericalMathEvaluationImplementation::Implementation  EvaluationImplementation;
  typedef NumericalMathGradientImplementation::Implementation    GradientImplementation;
  typedef NumericalMathHessianImplementation::Implementation     HessianImplementation;

private:

  /** List of muParser valid constants */
  static Description ValidConstants_;

  /** List of muParser valid functions */
  static Description ValidFunctions_;

  /** List of muParser valid operators */
  static Description ValidOperators_;

  /** Flag to tell if the documentation has been initialized */
  static Bool IsDocumentationInitialized_;

  /** Method that initialize the fields related to the documentation of the analytical functions */
  static void InitializeDocumentation();

public:

  /** Static methods for documentation of analytical fnctions */
  static Description GetValidConstants();
  static Description GetValidFunctions();
  static Description GetValidOperators();


public:

  /** Default constructor */
  NumericalMathFunctionImplementation();

  /** Analytical formula constructor */
  NumericalMathFunctionImplementation(const Description & inputVariablesNames,
                                      const Description & outputVariablesNames,
                                      const Description & formulas);

  /** Database constructor */
  NumericalMathFunctionImplementation(const NumericalSample & inputSample,
                                      const NumericalSample & outputSample);

  /** Constructor from implementations */
  NumericalMathFunctionImplementation(const EvaluationImplementation & funcImpl,
                                      const GradientImplementation & gradImpl,
                                      const HessianImplementation  & hessImpl);

  /** Single function implementation constructor */
  NumericalMathFunctionImplementation(const EvaluationImplementation & evaluationImplementation);

  /** Multiplication of two 1D output functions with the same input dimension */
  virtual NumericalMathFunctionImplementation operator * (const NumericalMathFunctionImplementation & right) const;

  /** Multiplication of two 1D output functions with the same input dimension */
  virtual NumericalMathFunctionImplementation operator * (const Implementation & p_right) const;

  /** Virtual constructor */
  virtual NumericalMathFunctionImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const NumericalMathFunctionImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
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
  void setGradient(const GradientImplementation & gradient);
  const GradientImplementation & getGradient() const;

  /** Hessian implementation accessors */
  void setHessian(const HessianImplementation & hessian);
  const HessianImplementation & getHessian() const;

  /** Flag for default gradient accessors */
  Bool getUseDefaultGradientImplementation() const;
  void setUseDefaultGradientImplementation(const Bool gradientFlag);

  /** Flag for default hessian accessors */
  Bool getUseDefaultHessianImplementation() const;
  void setUseDefaultHessianImplementation(const Bool hessianFlag);

  /** Operator () */
  virtual NumericalPoint operator() (const NumericalPoint & inP) const;

  virtual NumericalPoint operator()(const NumericalPoint & inP,
                                    const NumericalPoint & parameter);
  virtual NumericalSample operator() (const NumericalPoint & point,
                                      const NumericalSample & parameters);

  virtual NumericalSample operator() (const NumericalSample & inS) const;

  virtual Field operator() (const Field & inField) const;


  /** Method gradient() returns the Jacobian transposed matrix of the function at point */
  virtual Matrix gradient(const NumericalPoint & inP) const;
  virtual Matrix gradient(const NumericalPoint & inP,
                          const NumericalPoint & parameter);

  /** Method hessian() returns the symmetric tensor of the function at point */
  virtual SymmetricTensor hessian(const NumericalPoint & inP) const;
  virtual SymmetricTensor hessian(const NumericalPoint & inP,
                                  const NumericalPoint & parameter);

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const NumericalPoint & inP) const;
  virtual Matrix parameterGradient(const NumericalPoint & inP,
                                   const NumericalPoint & parameter);

  /** Parameters value accessor */
  virtual NumericalPoint getParameter() const;
  virtual void setParameter(const NumericalPoint & parameter);

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
  virtual Description getInputDescription() const;

  /** Output description Accessor, i.e. the names of the Output parameters */
  virtual Description getOutputDescription() const;

  /** Get the i-th marginal function */
  virtual Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the function corresponding to indices components */
  virtual Implementation getMarginal(const Indices & indices) const;

  /** Number of calls to the evaluation */
  virtual UnsignedInteger getEvaluationCallsNumber() const;

  /** Number of calls to the gradient */
  virtual UnsignedInteger getGradientCallsNumber() const;

  /** Number of calls to the hessian */
  virtual UnsignedInteger getHessianCallsNumber() const;

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

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  /** A pointer on the actual numerical math function implementation */
  EvaluationImplementation p_evaluationImplementation_;

  /** A pointer on the actual numerical math gradient implementation */
  GradientImplementation p_gradientImplementation_;

  /** A pointer on the actual numerical math hessian implementation */
  HessianImplementation p_hessianImplementation_;

protected:

  /** Flag to tell if the current gradient is a default implementation */
  mutable Bool useDefaultGradientImplementation_;

  /** Flag to tell if the curren hessian is a default implementation */
  mutable Bool useDefaultHessianImplementation_;

}; /* class NumericalMathFunctionImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NUMERICALMATHFUNCTIONIMPLEMENTATION_HXX */
