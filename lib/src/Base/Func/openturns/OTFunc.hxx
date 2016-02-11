//                                               -*- C++ -*-
/**
 *  @brief The external header file of OpenTURNS for subdir Func
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_OTFUNC_HXX
#define OPENTURNS_OTFUNC_HXX

#include "AggregatedNumericalMathEvaluationImplementation.hxx"
#include "AggregatedNumericalMathGradientImplementation.hxx"
#include "AggregatedNumericalMathHessianImplementation.hxx"
#ifdef OPENTURNS_HAVE_MUPARSER
#include "AnalyticalNumericalMathEvaluationImplementation.hxx"
#include "AnalyticalNumericalMathGradientImplementation.hxx"
#include "AnalyticalNumericalMathHessianImplementation.hxx"
#endif
#include "Basis.hxx"
#include "BasisSequence.hxx"
#include "BasisSequenceImplementation.hxx"
#include "BasisSequenceFactory.hxx"
#include "BasisSequenceFactoryImplementation.hxx"
#include "ComposedNumericalMathEvaluationImplementation.hxx"
#include "ComposedNumericalMathFunction.hxx"
#include "ComposedNumericalMathGradientImplementation.hxx"
#include "ComposedNumericalMathHessianImplementation.hxx"
#include "ConstantNumericalMathGradientImplementation.hxx"
#include "ConstantNumericalMathHessianImplementation.hxx"
#include "DatabaseNumericalMathEvaluationImplementation.hxx"
#include "DualLinearCombinationEvaluationImplementation.hxx"
#include "DualLinearCombinationGradientImplementation.hxx"
#include "DualLinearCombinationHessianImplementation.hxx"
#include "DynamicalFunction.hxx"
#include "DynamicalFunctionImplementation.hxx"
#include "EnumerateFunction.hxx"
#include "EnumerateFunctionImplementation.hxx"
#include "LinearEnumerateFunction.hxx"
#include "HyperbolicAnisotropicEnumerateFunction.hxx"
#include "ExpertMixture.hxx"
#include "IndicatorNumericalMathEvaluationImplementation.hxx"
#include "LAR.hxx"
#include "LinearCombinationEvaluationImplementation.hxx"
#include "LinearCombinationGradientImplementation.hxx"
#include "LinearCombinationHessianImplementation.hxx"
#include "LinearNumericalMathEvaluationImplementation.hxx"
#include "LinearNumericalMathFunction.hxx"
#include "QuadraticNumericalMathFunction.hxx"
#include "LinearNumericalMathGradientImplementation.hxx"
#include "MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "NoNumericalMathEvaluationImplementation.hxx"
#include "NoNumericalMathGradientImplementation.hxx"
#include "NoNumericalMathHessianImplementation.hxx"
#include "NumericalMathEvaluationImplementation.hxx"
#include "NumericalMathFunction.hxx"
#include "NumericalMathFunctionImplementation.hxx"
#include "NumericalMathGradientImplementation.hxx"
#include "NumericalMathHessianImplementation.hxx"
#include "ParametricEvaluationImplementation.hxx"
#include "ParametricGradientImplementation.hxx"
#include "ParametricHessianImplementation.hxx"
#include "P1LagrangeEvaluationImplementation.hxx"
#include "PiecewiseLinearEvaluationImplementation.hxx"
#include "PiecewiseHermiteEvaluationImplementation.hxx"
#include "ProductNumericalMathEvaluationImplementation.hxx"
#include "ProductNumericalMathFunction.hxx"
#include "ProductNumericalMathGradientImplementation.hxx"
#include "ProductNumericalMathHessianImplementation.hxx"
#include "ProductPolynomialEvaluationImplementation.hxx"
#include "ProductPolynomialGradientImplementation.hxx"
#include "ProductPolynomialHessianImplementation.hxx"
#include "QuadraticNumericalMathEvaluationImplementation.hxx"
#include "SpatialFunction.hxx"
#include "TemporalFunction.hxx"
#include "UniVariatePolynomial.hxx"
#include "UniVariatePolynomialImplementation.hxx"
#include "XMLTags.hxx"
#include "OTSpecFunc.hxx"
#include "BoxCoxEvaluationImplementation.hxx"
#include "BoxCoxGradientImplementation.hxx"
#include "BoxCoxHessianImplementation.hxx"
#include "BoxCoxTransform.hxx"
#include "InverseBoxCoxEvaluationImplementation.hxx"
#include "InverseBoxCoxGradientImplementation.hxx"
#include "InverseBoxCoxHessianImplementation.hxx"
#include "InverseBoxCoxTransform.hxx"
#include "TrendEvaluationImplementation.hxx"
#include "TrendTransform.hxx"
#include "InverseTrendEvaluationImplementation.hxx"
#include "InverseTrendTransform.hxx"
#include "FunctionalBasis.hxx"
#include "FunctionalBasisImplementation.hxx"
#include "FiniteBasis.hxx"
#include "ConstantBasisFactory.hxx"
#include "LinearBasisFactory.hxx"
#include "QuadraticBasisFactory.hxx"

#endif /* OPENTURNS_OTFUNC_HXX */
