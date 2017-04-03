//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for all numerical math function implementations
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_FUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_FUNCTIONIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Field.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/GradientImplementation.hxx"
#include "openturns/HessianImplementation.hxx"
#include "openturns/Description.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

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
  CLASSNAME;
public:

  /* Some typedefs for easy reading */
  typedef Pointer<FunctionImplementation>           Implementation;
  typedef EvaluationImplementation::Implementation  EvaluationPointer;
  typedef GradientImplementation::Implementation    GradientPointer;
  typedef HessianImplementation::Implementation     HessianPointer;

public:

  /** Static methods for documentation of analytical functions
   * @deprecated */
  static Description GetValidConstants();
  static Description GetValidFunctions();
  static Description GetValidOperators();


public:

  /** Default constructor */
  FunctionImplementation();

  /** Analytical formula constructor */
  FunctionImplementation(const Description & inputVariablesNames,
                                      const Description & outputVariablesNames,
                                      const Description & formulas);

  /** Database constructor */
  FunctionImplementation(const Sample & inputSample,
                                      const Sample & outputSample);

  /** Constructor from implementations */
  FunctionImplementation(const EvaluationPointer & funcImpl,
                                      const GradientPointer & gradImpl,
                                      const HessianPointer  & hessImpl);

  /** Single function implementation constructor */
  FunctionImplementation(const EvaluationPointer & evaluationImplementation);

  /** Multiplication of two 1D output functions with the same input dimension */
  virtual FunctionImplementation operator * (const FunctionImplementation & right) const;

  /** Multiplication of two 1D output functions with the same input dimension */
  virtual FunctionImplementation operator * (const Implementation & p_right) const;

  /** Virtual constructor */
  virtual FunctionImplementation * clone() const;

  /** Comparison operator */
  Bool operator ==(const FunctionImplementation & other) const;

  /** String converter */
  virtual String __repr__() const;
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
  virtual Sample getInputPointHistory() const;
  virtual Sample getInputParameterHistory() const;

  /** Function implementation accessors */
  void setEvaluation(const EvaluationPointer & evaluation);
  const EvaluationPointer & getEvaluation() const;

  /** Gradient implementation accessors */
  void setGradient(const GradientPointer & gradient);
  const GradientPointer & getGradient() const;

  /** Hessian implementation accessors */
  void setHessian(const HessianPointer & hessian);
  const HessianPointer & getHessian() const;

  /** Flag for default gradient accessors */
  Bool getUseDefaultGradientImplementation() const;
  void setUseDefaultGradientImplementation(const Bool gradientFlag);

  /** Flag for default hessian accessors */
  Bool getUseDefaultHessianImplementation() const;
  void setUseDefaultHessianImplementation(const Bool hessianFlag);

  /** Operator () */
  virtual Point operator() (const Point & inP) const;

  virtual Point operator()(const Point & inP,
                                    const Point & parameter);
  virtual Sample operator() (const Point & point,
                                      const Sample & parameters);

  virtual Sample operator() (const Sample & inS) const;

  virtual Field operator() (const Field & inField) const;


  /** Method gradient() returns the Jacobian transposed matrix of the function at point */
  virtual Matrix gradient(const Point & inP) const;
  virtual Matrix gradient(const Point & inP,
                          const Point & parameter);

  /** Method hessian() returns the symmetric tensor of the function at point */
  virtual SymmetricTensor hessian(const Point & inP) const;
  virtual SymmetricTensor hessian(const Point & inP,
                                  const Point & parameter);

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const Point & inP) const;
  virtual Matrix parameterGradient(const Point & inP,
                                   const Point & parameter);

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
                     const Point & centralPoint,
                     const NumericalScalar xMin,
                     const NumericalScalar xMax,
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
  virtual Graph draw(const NumericalScalar xMin,
                     const NumericalScalar xMax,
                     const UnsignedInteger pointNumber = ResourceMap::GetAsUnsignedInteger("NumericalMathEvaluation-DefaultPointNumber"),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Draw the output of the function with respect to its input when the input dimension is 2 and the output dimension is 1 */
  virtual Graph draw(const Point & xMin,
                     const Point & xMax,
                     const Indices & pointNumber = Indices(2, ResourceMap::GetAsUnsignedInteger("NumericalMathEvaluation-DefaultPointNumber")),
                     const GraphImplementation::LogScale scale = GraphImplementation::NONE) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  /** A pointer on the actual numerical math function implementation */
  EvaluationPointer p_evaluationImplementation_;

  /** A pointer on the actual numerical math gradient implementation */
  GradientPointer p_gradientImplementation_;

  /** A pointer on the actual numerical math hessian implementation */
  HessianPointer p_hessianImplementation_;

protected:

  /** Flag to tell if the current gradient is a default implementation */
  mutable Bool useDefaultGradientImplementation_;

  /** Flag to tell if the curren hessian is a default implementation */
  mutable Bool useDefaultHessianImplementation_;

}; /* class FunctionImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FUNCTIONIMPLEMENTATION_HXX */
