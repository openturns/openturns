// SWIG file statistics_module.i

%module(package="openturns", docstring="Statistical classes.") statistics
//%feature("autodoc","1");

%{
#include "openturns/OTconfig.hxx"
#include "openturns/OTCommon.hxx"
#include "openturns/OTType.hxx"
#include "openturns/OTStat.hxx"
#include "openturns/OTGeom.hxx"
#include "openturns/OTFunc.hxx"
%}

%include typemaps.i
%include OTtypes.i
%include OTexceptions.i
%include std_vector.i

/* Base/Common */
%import common_module.i

/* Base/Type */
%import typ_module.i
%import BaseTypTemplateDefs.i

/* Base/Graph */
%import graph_module.i
%import BaseGraphTemplateDefs.i

/* Base / Func */
%import func_module.i
%import BaseFuncTemplateDefs.i

/* Base/Geom */
%import geom_module.i
%import BaseGeomTemplateDefs.i
%include LevelSet.i

/* Base/Stat */
%include Compact.i  
%include Full.i  
%include Last.i  
%include Null.i  
%include HMatrixParameters.i
%include LinearModel.i  
%include LinearModelFactory.i  
%include TestResult.i  
%include RandomGeneratorState.i
%include RandomGenerator.i
%include LowDiscrepancySequenceImplementation.i 
%include LowDiscrepancySequence.i 
%include SobolSequence.i 
%include FaureSequence.i 
%include HaltonSequence.i 
%include HaselgroveSequence.i 
%include ReverseHaltonSequence.i 
%include CorrelationAnalysis.i 
%include FilteringWindowsImplementation.i
%include FilteringWindows.i
%include Hamming.i
%include Hanning.i
%include FFTImplementation.i
%include FFT.i
%include SpectralModelImplementation.i
%include SpectralModel.i
%include CovarianceModelImplementation.i
%include CovarianceModel.i
%include StationaryCovarianceModel.i
%include SecondOrderModelImplementation.i 
%include ExponentialCauchy.i 
%include SecondOrderModel.i 
%include SpectralModelFactoryImplementation.i
%include SpectralModelFactory.i 
%include UserDefinedSpectralModel.i
%include WelchFactory.i
%include ExponentialModel.i
%include ExponentiallyDampedCosineModel.i
%include SphericalModel.i
%include CauchyModel.i
%include CovarianceModelFactoryImplementation.i
%include CovarianceModelFactory.i
%include UserDefinedCovarianceModel.i
%include NonStationaryCovarianceModelFactory.i
%include UserDefinedStationaryCovarianceModel.i
%include StationaryCovarianceModelFactory.i
%include SquaredExponential.i
%include GeneralizedExponential.i
%include AbsoluteExponential.i
%include MaternModel.i
%include DiracCovarianceModel.i
%include ProductCovarianceModel.i
%include TensorizedCovarianceModel.i
%include HMatrixFactory.i
%include HMatrixImplementation.i
%include HMatrix.i

/* At last we include template definitions */
%include BaseStatisticsTemplateDefs.i
