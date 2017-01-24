//                                               -*- C++ -*-
/**
 *  @brief The external header file of OpenTURNS for subdir Func
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

#ifndef OPENTURNS_OTFUNC_HXX
#define OPENTURNS_OTFUNC_HXX

#include "openturns/AggregatedFunction.hxx"
#include "openturns/AggregatedNumericalMathEvaluationImplementation.hxx"
#include "openturns/AggregatedNumericalMathGradientImplementation.hxx"
#include "openturns/AggregatedNumericalMathHessianImplementation.hxx"
#ifdef OPENTURNS_HAVE_MUPARSER
#include "openturns/AnalyticalNumericalMathEvaluationImplementation.hxx"
#include "openturns/AnalyticalNumericalMathGradientImplementation.hxx"
#include "openturns/AnalyticalNumericalMathHessianImplementation.hxx"
#endif
#include "openturns/Basis.hxx"
#include "openturns/BasisSequence.hxx"
#include "openturns/BasisSequenceImplementation.hxx"
#include "openturns/BasisSequenceFactory.hxx"
#include "openturns/BasisSequenceFactoryImplementation.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/ComposedNumericalMathEvaluationImplementation.hxx"
#include "openturns/ComposedNumericalMathGradientImplementation.hxx"
#include "openturns/ComposedNumericalMathHessianImplementation.hxx"
#include "openturns/ConstantNumericalMathGradientImplementation.hxx"
#include "openturns/ConstantNumericalMathHessianImplementation.hxx"
#include "openturns/DatabaseFunction.hxx"
#include "openturns/DatabaseNumericalMathEvaluationImplementation.hxx"
#include "openturns/DualLinearCombinationFunction.hxx"
#include "openturns/DualLinearCombinationEvaluationImplementation.hxx"
#include "openturns/DualLinearCombinationGradientImplementation.hxx"
#include "openturns/DualLinearCombinationHessianImplementation.hxx"
#include "openturns/DynamicalFunction.hxx"
#include "openturns/DynamicalFunctionImplementation.hxx"
#include "openturns/EnumerateFunction.hxx"
#include "openturns/EnumerateFunctionImplementation.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/LinearEnumerateFunction.hxx"
#include "openturns/HyperbolicAnisotropicEnumerateFunction.hxx"
#include "openturns/ExpertMixture.hxx"
#include "openturns/IndicatorNumericalMathEvaluationImplementation.hxx"
#include "openturns/LARS.hxx"
#include "openturns/LinearCombinationEvaluationImplementation.hxx"
#include "openturns/LinearCombinationGradientImplementation.hxx"
#include "openturns/LinearCombinationHessianImplementation.hxx"
#include "openturns/LinearNumericalMathEvaluationImplementation.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/IdentityEvaluation.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/QuadraticFunction.hxx"
#include "openturns/LinearNumericalMathGradientImplementation.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/NoNumericalMathEvaluationImplementation.hxx"
#include "openturns/NoNumericalMathGradientImplementation.hxx"
#include "openturns/NoNumericalMathHessianImplementation.hxx"
#include "openturns/NumericalMathEvaluationImplementation.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/NumericalMathFunctionImplementation.hxx"
#include "openturns/NumericalMathGradientImplementation.hxx"
#include "openturns/NumericalMathHessianImplementation.hxx"
#include "openturns/ParametricFunction.hxx"
#include "openturns/ParametricEvaluationImplementation.hxx"
#include "openturns/ParametricGradientImplementation.hxx"
#include "openturns/ParametricHessianImplementation.hxx"
#include "openturns/P1LagrangeEvaluationImplementation.hxx"
#include "openturns/PiecewiseLinearEvaluationImplementation.hxx"
#include "openturns/PiecewiseHermiteEvaluationImplementation.hxx"
#include "openturns/ProductNumericalMathEvaluationImplementation.hxx"
#include "openturns/ProductNumericalMathFunction.hxx"
#include "openturns/ProductNumericalMathGradientImplementation.hxx"
#include "openturns/ProductNumericalMathHessianImplementation.hxx"
#include "openturns/ProductPolynomialEvaluationImplementation.hxx"
#include "openturns/ProductPolynomialGradientImplementation.hxx"
#include "openturns/ProductPolynomialHessianImplementation.hxx"
#include "openturns/QuadraticNumericalMathEvaluationImplementation.hxx"
#include "openturns/SpatialFunction.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/TemporalFunction.hxx"
#include "openturns/UniVariatePolynomial.hxx"
#include "openturns/UniVariatePolynomialImplementation.hxx"
#include "openturns/UniVariateFunction.hxx"
#include "openturns/UniVariateFunctionImplementation.hxx"
#include "openturns/FourierSeries.hxx"
#include "openturns/HaarWavelet.hxx"
#include "openturns/ProductUniVariateFunctionEvaluationImplementation.hxx"
#include "openturns/ProductUniVariateFunctionGradientImplementation.hxx"
#include "openturns/ProductUniVariateFunctionHessianImplementation.hxx"
#include "openturns/OTSpecFunc.hxx"
#include "openturns/BoxCoxEvaluationImplementation.hxx"
#include "openturns/BoxCoxGradientImplementation.hxx"
#include "openturns/BoxCoxHessianImplementation.hxx"
#include "openturns/BoxCoxTransform.hxx"
#include "openturns/InverseBoxCoxEvaluationImplementation.hxx"
#include "openturns/InverseBoxCoxGradientImplementation.hxx"
#include "openturns/InverseBoxCoxHessianImplementation.hxx"
#include "openturns/InverseBoxCoxTransform.hxx"
#include "openturns/TrendEvaluationImplementation.hxx"
#include "openturns/TrendTransform.hxx"
#include "openturns/InverseTrendEvaluationImplementation.hxx"
#include "openturns/InverseTrendTransform.hxx"
#include "openturns/FunctionalBasis.hxx"
#include "openturns/FunctionalBasisImplementation.hxx"
#include "openturns/FiniteBasis.hxx"
#include "openturns/ConstantBasisFactory.hxx"
#include "openturns/LinearBasisFactory.hxx"
#include "openturns/QuadraticBasisFactory.hxx"

#endif /* OPENTURNS_OTFUNC_HXX */
