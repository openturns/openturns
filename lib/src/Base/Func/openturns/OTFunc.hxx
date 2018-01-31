//                                               -*- C++ -*-
/**
 *  @brief The external header file of OpenTURNS for subdir Func
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

#ifndef OPENTURNS_OTFUNC_HXX
#define OPENTURNS_OTFUNC_HXX

#include "openturns/AggregatedFunction.hxx"
#include "openturns/AggregatedEvaluation.hxx"
#include "openturns/AggregatedGradient.hxx"
#include "openturns/AggregatedHessian.hxx"
#ifdef OPENTURNS_HAVE_MUPARSER
#include "openturns/SymbolicEvaluation.hxx"
#include "openturns/SymbolicGradient.hxx"
#include "openturns/SymbolicHessian.hxx"
#endif
#include "openturns/Basis.hxx"
#include "openturns/BasisSequence.hxx"
#include "openturns/BasisSequenceImplementation.hxx"
#include "openturns/BasisSequenceFactory.hxx"
#include "openturns/BasisSequenceFactoryImplementation.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/ComposedEvaluation.hxx"
#include "openturns/ComposedGradient.hxx"
#include "openturns/ComposedHessian.hxx"
#include "openturns/ConstantGradient.hxx"
#include "openturns/ConstantHessian.hxx"
#include "openturns/DatabaseFunction.hxx"
#include "openturns/DatabaseEvaluation.hxx"
#include "openturns/DualLinearCombinationFunction.hxx"
#include "openturns/DualLinearCombinationEvaluation.hxx"
#include "openturns/DualLinearCombinationGradient.hxx"
#include "openturns/DualLinearCombinationHessian.hxx"
#include "openturns/FieldFunction.hxx"
#include "openturns/FieldFunctionImplementation.hxx"
#include "openturns/FieldToPointFunction.hxx"
#include "openturns/FieldToPointFunctionImplementation.hxx"
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/PointToFieldFunctionImplementation.hxx"
#include "openturns/PointToPointEvaluation.hxx"
#include "openturns/FieldToFieldConnection.hxx"
#include "openturns/FieldToPointConnection.hxx"
#include "openturns/PointToFieldConnection.hxx"
#include "openturns/PointToPointConnection.hxx"
#include "openturns/EnumerateFunction.hxx"
#include "openturns/EnumerateFunctionImplementation.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/LinearEnumerateFunction.hxx"
#include "openturns/HyperbolicAnisotropicEnumerateFunction.hxx"
#include "openturns/ExpertMixture.hxx"
#include "openturns/IndicatorEvaluation.hxx"
#include "openturns/LARS.hxx"
#include "openturns/LinearCombinationEvaluation.hxx"
#include "openturns/LinearCombinationGradient.hxx"
#include "openturns/LinearCombinationHessian.hxx"
#include "openturns/LinearEvaluation.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/IdentityEvaluation.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/IndicatorFunction.hxx"
#include "openturns/QuadraticFunction.hxx"
#include "openturns/LinearGradient.hxx"
#include "openturns/MethodBoundEvaluation.hxx"
#include "openturns/NoEvaluation.hxx"
#include "openturns/NoGradient.hxx"
#include "openturns/NoHessian.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/Function.hxx"
#include "openturns/FunctionImplementation.hxx"
#include "openturns/GradientImplementation.hxx"
#include "openturns/HessianImplementation.hxx"
#include "openturns/ParametricFunction.hxx"
#include "openturns/ParametricEvaluation.hxx"
#include "openturns/ParametricGradient.hxx"
#include "openturns/ParametricHessian.hxx"
#include "openturns/P1LagrangeEvaluation.hxx"
#include "openturns/PiecewiseLinearEvaluation.hxx"
#include "openturns/PiecewiseHermiteEvaluation.hxx"
#include "openturns/ProductEvaluation.hxx"
#include "openturns/ProductFunction.hxx"
#include "openturns/ProductGradient.hxx"
#include "openturns/ProductHessian.hxx"
#include "openturns/ProductPolynomialEvaluation.hxx"
#include "openturns/ProductPolynomialGradient.hxx"
#include "openturns/ProductPolynomialHessian.hxx"
#include "openturns/QuadraticEvaluation.hxx"
#include "openturns/ValueFunction.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/VertexFunction.hxx"
#include "openturns/VertexValueFunction.hxx"
#include "openturns/UniVariatePolynomial.hxx"
#include "openturns/UniVariatePolynomialImplementation.hxx"
#include "openturns/UniVariateFunction.hxx"
#include "openturns/UniVariateFunctionImplementation.hxx"
#include "openturns/FourierSeries.hxx"
#include "openturns/HaarWavelet.hxx"
#include "openturns/ProductUniVariateFunctionEvaluation.hxx"
#include "openturns/ProductUniVariateFunctionGradient.hxx"
#include "openturns/ProductUniVariateFunctionHessian.hxx"
#include "openturns/UniVariateFunctionFamily.hxx"
#include "openturns/UniVariateFunctionFactory.hxx"
#include "openturns/TensorizedUniVariateFunctionFactory.hxx"
#include "openturns/MonomialFunction.hxx"
#include "openturns/MonomialFunctionFactory.hxx"
#include "openturns/OTSpecFunc.hxx"
#include "openturns/BoxCoxEvaluation.hxx"
#include "openturns/BoxCoxGradient.hxx"
#include "openturns/BoxCoxHessian.hxx"
#include "openturns/BoxCoxTransform.hxx"
#include "openturns/InverseBoxCoxEvaluation.hxx"
#include "openturns/InverseBoxCoxGradient.hxx"
#include "openturns/InverseBoxCoxHessian.hxx"
#include "openturns/InverseBoxCoxTransform.hxx"
#include "openturns/TrendEvaluation.hxx"
#include "openturns/TrendTransform.hxx"
#include "openturns/InverseTrendEvaluation.hxx"
#include "openturns/InverseTrendTransform.hxx"
#include "openturns/ConstantBasisFactory.hxx"
#include "openturns/LinearBasisFactory.hxx"
#include "openturns/QuadraticBasisFactory.hxx"

#endif /* OPENTURNS_OTFUNC_HXX */
