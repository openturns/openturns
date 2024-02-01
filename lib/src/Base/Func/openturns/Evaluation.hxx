//                                               -*- C++ -*-
/**
 *  @brief The class that implements numerical math functions
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_EVALUATION_HXX
#define OPENTURNS_EVALUATION_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Evaluation
 * @brief Simulates a numerical math function
 *
 * The class that simulates a numerical math function.
 * This class is just an interface to actual implementation
 * objects that can be hot-replaced during computation.
 * @see EvaluationImplementation
 */
class OT_API Evaluation
  : public TypedInterfaceObject<EvaluationImplementation>
{
  CLASSNAME
public:

  /* Some typedefs for easy reading */
  typedef Pointer<EvaluationImplementation>   Implementation;

  /** Default constructor */
  Evaluation();

  /** Constructor from EvaluationImplementation */
  Evaluation(const EvaluationImplementation & implementation);

#ifndef SWIG
  /** Constructor from implementation */
  Evaluation(const Implementation & p_implementation);

  /** Constructor from implementation pointer */
  Evaluation(EvaluationImplementation * p_implementation);
#endif

  /** Comparison operator */
  using TypedInterfaceObject<EvaluationImplementation>::operator ==;
  Bool operator ==(const Evaluation & other) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** String converter */
  String _repr_html_() const override;

  /** Description Accessor, i.e. the names of the input and output parameters */
  virtual void setDescription(const Description & description);
  Description getDescription() const;

  /** Input description Accessor, i.e. the names of the input parameters */
  Description getInputDescription() const;
  void setInputDescription(const Description & inputDescription);

  /** Output description Accessor, i.e. the names of the Output parameters */
  Description getOutputDescription() const;
  void setOutputDescription(const Description & outputDescription);

  /** Operator () */
  Point operator() (const Point & inP) const;

  Sample operator() (const Sample & inS) const;

  Field operator() (const Field & inTS) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Accessor for parameters dimension */
  virtual UnsignedInteger getParameterDimension() const;

  /** Get the i-th marginal evaluation */
  virtual Evaluation getMarginal(const UnsignedInteger i) const;

  /** Get the evaluation corresponding to indices components */
  virtual Evaluation getMarginal(const Indices & indices) const;

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const Point & inP) const;

  /** Parameters value accessor */
  virtual Point getParameter() const;
  virtual void setParameter(const Point & parameters);

  /** Parameters description accessor */
  virtual Description getParameterDescription() const;
  virtual void setParameterDescription(const Description & description);

  /** Get the number of calls to operator() */
  UnsignedInteger getCallsNumber() const;

  /** Linearity accessors */
  Bool isLinear() const;
  Bool isLinearlyDependent(const UnsignedInteger index) const;

  /** Invalid values check accessor */
  void setCheckOutput(const Bool checkOutput);
  Bool getCheckOutput() const;

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

}; /* class Evaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EVALUATION_HXX */
