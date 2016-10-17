// SWIG file algo_module.i

%module(package="openturns", docstring="Approximation algorithms.") algo
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTAlgo.hxx"
#include "openturns/OTStat.hxx"
#include "openturns/OTFunc.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Common */
%import common_module.i
%import BaseCommonTemplateDefs.i

/* Base/Type */
%import typ_module.i
%import BaseTypTemplateDefs.i

/* Base/Func */
%include BaseFuncCollection.i
%import func_module.i
%import BaseFuncTemplateDefs.i

/* Base/Stat */
%import statistics_module.i
%import BaseStatisticsTemplateDefs.i

/* Base/Algo */
%include ApproximationAlgorithmImplementation.i
%include ApproximationAlgorithm.i
%include ApproximationAlgorithmImplementationFactory.i
%include ClassifierImplementation.i
%include Classifier.i
%include FittingAlgorithmImplementation.i
%include FittingAlgorithm.i
%include KDTree.i
%include KFold.i
%include CorrectedLeaveOneOut.i
%include DesignProxy.i
%include LeastSquaresMethodImplementation.i
%include LeastSquaresMethod.i
%include CholeskyMethod.i
%include QRMethod.i
%include SVDMethod.i
%include LeastSquaresMetaModelSelection.i
%include LeastSquaresMetaModelSelectionFactory.i
%include PenalizedLeastSquaresAlgorithm.i
%include PenalizedLeastSquaresAlgorithmFactory.i
%include KissFFT.i
%include IntegrationAlgorithmImplementation.i
%include IntegrationAlgorithm.i
%include GaussKronrodRule.i
%include GaussKronrod.i
%include IteratedQuadrature.i
%include ExpertMixture.i
%include KarhunenLoeveP1Factory.i

/* At last we include template definitions */
%include BaseAlgoTemplateDefs.i
