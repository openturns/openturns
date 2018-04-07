//                                               -*- C++ -*-
/**
 *  @brief ResourceMap defines top-most resourceMap strategies
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
#include <unistd.h>                 // for sysconf
#include "openturns/OTthread.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/OSS.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Path.hxx"
#include "openturns/XMLToolbox.hxx"

BEGIN_NAMESPACE_OPENTURNS

static const char * ConfigurationFileName = "openturns.conf";
#if defined OPENTURNS_HAVE_LIBXML2
static const char * RootElementName = "openturns-configuration";
static const char * XMLTag_value = "value";
#endif

static pthread_mutex_t ResourceMap_InstanceMutex_;
static ResourceMap * ResourceMap_P_instance_ = 0;
static const ResourceMap_init static_initializer_ResourceMap;

ResourceMap_init::ResourceMap_init()
{
  if (!ResourceMap_P_instance_)
  {
#ifndef OT_MUTEXINIT_NOCHECK
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
    pthread_mutex_init( &ResourceMap_InstanceMutex_, &attr );
#else
    pthread_mutex_init( &ResourceMap_InstanceMutex_, NULL );
#endif
    ResourceMap_P_instance_ = new ResourceMap;
  }
  assert(ResourceMap_P_instance_);
}

ResourceMap_init::~ResourceMap_init()
{
  if (ResourceMap_P_instance_)
    pthread_mutex_destroy(&ResourceMap_InstanceMutex_);
  delete ResourceMap_P_instance_;
  ResourceMap_P_instance_ = 0;
}


template<>
MutexLockSingleton<ResourceMap>::MutexLockSingleton( ResourceMap & singleton )  throw()
  : singleton_(singleton)
  , lock_(ResourceMap_InstanceMutex_) {}


/* GetInstance gives a locked access to the singleton */
MutexLockSingleton<ResourceMap> ResourceMap::GetInstance()
{
  static const ResourceMap_init force_instantiation;
  // ResourceMap_InstanceMutex_ is always initialized
  return *ResourceMap_P_instance_;
}

/* Get the list of keys */
std::vector<String> ResourceMap::GetKeys()
{
  return GetInstance().lock().getKeys();
}

/* Get a value in the map */
String ResourceMap::Get(String key)
{
  return GetInstance().lock().get( key );
}

Bool ResourceMap::GetAsBool(String key)
{
  return GetInstance().lock().getAsBool( key );
}

UnsignedInteger ResourceMap::GetAsUnsignedInteger(String key)
{
  return GetInstance().lock().getAsUnsignedInteger( key );
}

Scalar ResourceMap::GetAsScalar(String key)
{
  return GetInstance().lock().getAsScalar( key );
}

UnsignedInteger ResourceMap::GetSize()
{
  return GetInstance().lock().getSize();
}

/* Set a value in the map */
void ResourceMap::Set(String key, String value)
{
  GetInstance().lock().set( key, value );
}

void ResourceMap::SetAsBool(String key, Bool value)
{
  GetInstance().lock().setAsBool( key, value );
}

void ResourceMap::SetAsUnsignedInteger(String key, UnsignedInteger value)
{
  GetInstance().lock().setAsUnsignedInteger( key, value );
}

void ResourceMap::SetAsScalar(String key, Scalar value)
{
  GetInstance().lock().setAsScalar( key, value );
}

/* Default constructor */
ResourceMap::ResourceMap()
  : map_()
{
  loadDefaultConfiguration();
  loadConfigurationFile();
}

/* Method for retrieving information from the resource map */
String ResourceMap::get(String key) const
{
  MapType::const_iterator it = map_.find(key);
  if (it == map_.end()) throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap. Report Bug";
  return it->second;
}

Bool ResourceMap::getAsBool(String key) const
{
  Bool value = false;
  String st = get( key );
  if (st == String(OSS() << true)) value = true;
  else
  {
    std::istringstream iss( st );
    iss >> value;
  }
  return value;
}

UnsignedInteger ResourceMap::getAsUnsignedInteger(String key) const
{
  UnsignedInteger value = 0;
  String st = get( key );
  std::istringstream iss( st );
  iss >> value;
  return value;
}

Scalar ResourceMap::getAsScalar(String key) const
{
  Scalar value = -1.0;
  String st = get( key );
  std::istringstream iss( st );
  iss >> value;
  return value;
}

/* Get the size of the map */
UnsignedInteger ResourceMap::getSize() const
{
  return map_.size();
}

/* Method for setting information into the resource map */
void ResourceMap::set(String key, String value)
{
  map_[ key ] = value;
}

void ResourceMap::setAsBool(String key, Bool value)
{
  set( key, OSS() << value );
}

void ResourceMap::setAsUnsignedInteger(String key, UnsignedInteger value)
{
  set( key, OSS() << value );
}

void ResourceMap::setAsScalar(String key, Scalar value)
{
  set( key, OSS() << value );
}

/* Update the ResourceMap with information from the configuration file */
void ResourceMap::readConfigurationFile(const FileName & configurationFile)
{
#if defined OPENTURNS_HAVE_LIBXML2
  LOGINFO(OSS() << "Using configuration file " << configurationFile);

  // Open the configuration file and load it in a DOM document for later analysis
  XMLDoc document( configurationFile );

  // Check it is an OpenTURNS' one
  xmlNodePtr rootElt = xmlDocGetRootElement( document );
  if (rootElt == NULL) throw ConfigurationFileParsingException(HERE) << "Wrapper file has no root element" << configurationFile;
  if (xmlStrcmp( rootElt->name, REINTERPRET_CAST(const xmlChar *, RootElementName) ))
    throw ConfigurationFileParsingException(HERE) << "Wrapper file " << configurationFile
        << " has an invalid root element (" << rootElt->name << ")"
        << " at line " << xmlGetLineNo(rootElt);


  // Walk through the DOM document to read the data
  if (rootElt && rootElt->children)
  {
    for (xmlNodePtr current = rootElt->children; current; current = current->next)
    {
      if (XML::IsElement(current))
      {
        String key = XML::ToString(current->name);
        String value = XML::GetAttributeByName( current, XMLTag_value );
        //if (value.empty()) value = XMLAttr_undefined;
        map_[ key ] = value;
      }
    }
  }
#else
  LOGWARN(OSS() << "Cannot parse configuration file due to lacking xml support");
#endif
}


/* Find the configuration file in specific path (see Path class for algorithm) */
FileName ResourceMap::findConfigurationFile() const
{
  Path::DirectoryList dirList = Path::GetConfigDirectoryList();
  return Path::FindFileByNameInDirectoryList(ConfigurationFileName, dirList);
}

/* Load the configuration file */
void ResourceMap::loadConfigurationFile()
{
  try
  {
    readConfigurationFile(findConfigurationFile());
  }
  catch (FileNotFoundException &)
  {
    LOGWARN(OSS() << "The configuration file has not been found, using default parameters.");
  }
}



/* Load the configuration defined at installation time */
void ResourceMap::loadDefaultConfiguration()
{
#ifdef R_EXECUTABLE
  set( "R-executable-command", R_EXECUTABLE );
#else
  set( "R-executable-command", "" );
#endif
  set( "csv-file-separator", ";" );
#ifndef WIN32
  set( "temporary-directory", "/tmp" );
  setAsUnsignedInteger( "parallel-threads", sysconf(_SC_NPROCESSORS_CONF) );
#else
  set( "temporary-directory", "TEMP" );
  set( "parallel-threads", getenv("NUMBER_OF_PROCESSORS") );
#endif
  setAsUnsignedInteger( "cache-max-size", 1024 );
  setAsUnsignedInteger( "output-files-timeout", 2 );

  // Os parameters
  setAsUnsignedInteger( "Os-create-process", 0 );
  setAsUnsignedInteger( "Os-RemoveFiles", 1 );

  // XMLStorageManager parameters
  setAsUnsignedInteger( "XMLStorageManager-DefaultCompressionLevel", 0 );

  // Collection parameters
  setAsUnsignedInteger( "Collection-size-visible-in-str-from", 10 );

  // SpecFunc parameters
  setAsUnsignedInteger( "SpecFunc-MaximumIteration", 1000 );
  setAsScalar( "SpecFunc-Precision", 2.0e-16 );

  // SymbolicParser parameters
  set( "SymbolicParser-Backend", SYMBOLICPARSER_DEFAULT_BACKEND );
  setAsBool( "SymbolicParser-CheckResult", true );

  // GramProxy parameters
  setAsUnsignedInteger( "DesignProxy-DefaultCacheSize", 16777216 );// 2^24=16777216=128 Mio

  // KFold parameters
  setAsUnsignedInteger( "KFold-DefaultK", 10 );

  // BlendedStep parameters //
  setAsScalar( "BlendedStep-DefaultEta", 1.0 );

  // CenteredFiniteDifferenceGradient parameters //
  setAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon", 1.0e-5 );

  // CenteredFiniteDifferenceHessian parameters //
  setAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon", 1.0e-4 );

  // NonCenteredFiniteDifferenceGradient parameters //
  setAsScalar( "NonCenteredFiniteDifferenceGradient-DefaultEpsilon", 1.0e-7 );

  // PiecewiseHermiteEvaluation parameters //
  setAsScalar( "PiecewiseHermiteEvaluation-EpsilonRegular", 1.0e-12 );

  // PiecewiseLinearEvaluation parameters //
  setAsScalar( "PiecewiseLinearEvaluation-EpsilonRegular", 1.0e-12 );

  // UniVariatePolynomialImplementation parameters //
  setAsUnsignedInteger( "UniVariatePolynomial-SmallDegree", 400 );

  // Pie parameters //
  setAsScalar( "Pie-HorizontalMargin", 0.3 );
  setAsScalar( "Pie-VerticalMargin", 0.1 );
  setAsScalar( "Pie-LabelThreshold", 0.02 );

  // DrawableImplementation parameters //
  set( "Drawable-NoSpecifiedLabel", "" );
  set( "Drawable-DefaultColor", "blue" );
  set( "Drawable-DefaultSurfaceColor", "white" );
  set( "Drawable-DefaultFillStyle", "solid" );
  set( "Drawable-DefaultPointStyle", "plus" );
  set( "Drawable-DefaultLineStyle", "solid" );
  set( "Drawable-DefaultPattern", "s" );
  set( "Drawable-AlternativePattern", "S" );
  setAsUnsignedInteger( "Drawable-DefaultLineWidth", 1 );
  setAsUnsignedInteger( "Drawable-DataThreshold", 2000 );

  // GraphImplementation parameters //
  set( "Graph-NoSpecifiedLabel", "" );
  setAsUnsignedInteger( "Graph-DefaultWidth", 640 );
  setAsUnsignedInteger( "Graph-DefaultHeight", 480 );
  setAsScalar( "Graph-DefaultLegendFontSize", 1.0 );
  setAsScalar( "Graph-DefaultHorizontalMargin", 0.05 );
  setAsScalar( "Graph-DefaultVerticalMargin", 0.05 );

  // Contour parameters //
  setAsUnsignedInteger( "Contour-DefaultLevelsNumber", 10 );

  // IntervalMesher parameters //
  setAsBool( "IntervalMesher-UseDiamond", false );

  // PointToPointEvaluation parameters //
  setAsUnsignedInteger("PointToPointEvaluation-BlockSize", 256);

  // SQP parameters //
  setAsScalar( "SQP-DefaultTau", 0.5 );
  setAsScalar( "SQP-DefaultOmega", 1.0e-4 );
  setAsScalar( "SQP-DefaultSmooth", 1.2 );
  setAsUnsignedInteger ("SQP-DefaultMaximumEvaluationNumber", 100000);

  // TNC parameters //
  setAsUnsignedInteger( "TNC-DefaultMaxCGit", 50 );
  setAsScalar( "TNC-DefaultEta", 0.25 );
  setAsScalar( "TNC-DefaultStepmx", 10.0 );
  setAsScalar( "TNC-DefaultAccuracy", 1.0e-4 );
  setAsScalar( "TNC-DefaultFmin", 1.0 );
  setAsScalar( "TNC-DefaultRescale", 1.3 );

  // AbdoRackwitz parameters //
  setAsScalar( "AbdoRackwitz-DefaultTau", 0.5 );
  setAsScalar( "AbdoRackwitz-DefaultOmega", 1.0e-4 );
  setAsScalar( "AbdoRackwitz-DefaultSmooth", 1.2 );
  setAsUnsignedInteger ("AbdoRackwitz-DefaultMaximumEvaluationNumber", 100000);

  // OptimizationAlgorithm parameters //
  setAsScalar( "OptimizationAlgorithm-DefaultLevelValue", 0.0 );
  setAsUnsignedInteger( "OptimizationAlgorithm-DefaultMaximumIterationNumber", 100 );
  setAsUnsignedInteger( "OptimizationAlgorithm-DefaultMaximumEvaluationNumber", 100);
  setAsScalar( "OptimizationAlgorithm-DefaultMaximumAbsoluteError", 1.0e-5 );
  setAsScalar( "OptimizationAlgorithm-DefaultMaximumRelativeError", 1.0e-5 );
  setAsScalar( "OptimizationAlgorithm-DefaultMaximumResidualError", 1.0e-5 );
  setAsScalar( "OptimizationAlgorithm-DefaultMaximumConstraintError", 1.0e-5 );

  // EfficientGlobalOptimization parameters //
  setAsUnsignedInteger( "EfficientGlobalOptimization-DefaultMultiStartExperimentSize", 100);
  setAsUnsignedInteger( "EfficientGlobalOptimization-DefaultMultiStartNumber", 20);
  setAsUnsignedInteger( "EfficientGlobalOptimization-DefaultParameterEstimationPeriod", 1);
  setAsScalar( "EfficientGlobalOptimization-DefaultImprovementFactor", 1.0);
  setAsScalar( "EfficientGlobalOptimization-DefaultCorrelationLengthFactor", 1.0);
  setAsScalar( "EfficientGlobalOptimization-DefaultAEITradeoff", 1.0);

  // Cobyla parameters //
  setAsScalar( "Cobyla-DefaultRhoBeg", 0.1 );

  // SolverImplementation parameters //
  setAsScalar( "Solver-DefaultAbsoluteError",  1.0e-5 );
  setAsScalar( "Solver-DefaultRelativeError",  1.0e-5 );
  setAsScalar( "Solver-DefaultResidualError",  1.0e-8 );
  setAsUnsignedInteger( "Solver-DefaultMaximumFunctionEvaluation", 100 );

  // GaussKronrod parameters //
  setAsUnsignedInteger( "GaussKronrod-MaximumSubIntervals", 100 );
  setAsScalar( "GaussKronrod-MaximumError",  1.0e-12 );

  // GaussLegendre parameters //
  setAsUnsignedInteger( "GaussLegendre-DefaultMarginalIntegrationPointsNumber", 64);

  // IteratedQuadrature parameters //
  setAsUnsignedInteger( "IteratedQuadrature-MaximumSubIntervals", 32 );
  setAsScalar( "IteratedQuadrature-MaximumError",    1.0e-7 );

  // Fehlberg parameters //
  setAsScalar("Fehlberg-LocalPrecision", 1.0e-10);
  setAsScalar("Fehlberg-InitialStep", 1.0e-7);
  setAsUnsignedInteger("Fehlberg-DefaultOrder", 4);
  
  // KarhunenLoeveQuadratureAlgorithm parameters //
  setAsScalar( "KarhunenLoeveQuadratureAlgorithm-RegularizationFactor", 0.0);

  // KarhunenLoeveP1Algorithm parameters //
  setAsScalar( "KarhunenLoeveP1Algorithm-RegularizationFactor", 0.0);

  // AdaptiveStieltjesAlgorithm parameters //
  setAsUnsignedInteger( "AdaptiveStieltjesAlgorithm-MaximumSubIntervalsBetweenRoots", 64 );
  setAsScalar( "AdaptiveStieltjesAlgorithm-MaximumError",  1.0e-12 );

  // LinearModelFactory parameters //
  setAsScalar( "LinearModelFactory-DefaultLevelValue", 0.95 );

  // LinearModelTest parameters //
  setAsScalar( "LinearModelTest-DefaultLevel", 0.95 );
  setAsScalar( "LinearModelTest-DefaultHarrisonMcCabeBreakpoint", 0.5 );
  setAsUnsignedInteger( "LinearModelTest-DefaultHarrisonMcCabeSimulationSize", 1000 );
  set( "LinearModelTest-DefaultDurbinWatsonHypothesis", "Equal" );

  // Last parameters //
  setAsUnsignedInteger( "Last-DefaultMaximumSize", 65536 );

  // Compact parameters //
  setAsUnsignedInteger( "Compact-DefaultHalfMaximumSize", 1024 );

  // FaureSequence parameters //
  setAsUnsignedInteger( "FaureSequence-InitialSeed", 1 );

  // HaltonSequence parameters //
  setAsUnsignedInteger( "HaltonSequence-InitialSeed", 1 );

  // HaselgroveSequence parameters //
  setAsUnsignedInteger( "HaselgroveSequence-InitialSeed", 1 );

  // ReverseHaltonSequence parameters //
  setAsUnsignedInteger( "ReverseHaltonSequence-InitialSeed", 1 );

  // SobolSequence parameters //
  setAsUnsignedInteger( "SobolSequence-InitialSeed", 1 );

  // SensitivityAlgorithm parameters //
  setAsBool( "SobolIndicesAlgorithm-DefaultUseAsymptoticDistribution", false );
  setAsUnsignedInteger( "SobolIndicesAlgorithm-DefaultBlockSize", 1 );
  setAsUnsignedInteger( "SobolIndicesAlgorithm-DefaultBootstrapSize", 100 );
  setAsScalar( "SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel", 0.95 );

  // FAST parameters //
  setAsUnsignedInteger( "FAST-DefaultResamplingSize", 1 );
  setAsUnsignedInteger( "FAST-DefaultInterferenceFactor", 4 );

  // RandomGenerator parameters //
  setAsUnsignedInteger( "RandomGenerator-InitialSeed", 0 );

  // CovarianceModelImplementation parameters //
  setAsScalar( "CovarianceModel-DefaultNuggetFactor", 1e-12 );
  setAsScalar( "SpectralModel-DefaultTMin", -5.0 );
  setAsScalar( "SpectralModel-DefaultTMax",  5.0 );
  setAsUnsignedInteger( "SpectralModel-DefaultPointNumber",  129 );

  // SpectralModel parameters //
  setAsScalar( "SpectralModel-DefaultMinimumFrequency", -5.0 );
  setAsScalar( "SpectralModel-DefaultMaximumFrequency",  5.0 );
  setAsUnsignedInteger( "SpectralModel-DefaultFrequencyNumber",  129 );

  // FieldImplementation parameters //
  setAsUnsignedInteger( "Field-LevelNumber", 30 );
  setAsScalar( "Field-ArrowRatio", 0.01 );
  setAsScalar( "Field-ArrowScaling", 1.0 );

  // SampleImplementation parameters
  setAsUnsignedInteger( "Sample-SmallKendallTau", 23 );
  setAsUnsignedInteger("Sample-PrintEllipsisThreshold", 1000);
  setAsUnsignedInteger("Sample-PrintEllipsisSize", 3);
  set("Sample-CommentMarkers", "#");

  // DomainImplementation parameters
  setAsScalar( "Domain-SmallVolume",   1.0e-12 );

  // Mesh parameters
  setAsScalar( "Mesh-VertexEpsilon", 1.0e-12 );
  setAsUnsignedInteger( "Mesh-LargeSize", 5000 );
  setAsBool( "Mesh-BackfaceCulling", false );
  setAsScalar( "Mesh-DiffuseFactor", 0.7 );
  setAsScalar( "Mesh-AmbientFactor", 0.1 );
  setAsScalar( "Mesh-SpecularFactor", 0.2 );
  setAsScalar( "Mesh-Shininess", 100.0 );

  // BoundingVolumeHierarchy parameters
  setAsUnsignedInteger( "BoundingVolumeHierarchy-BinNumber", 50 );
  set( "BoundingVolumeHierarchy-Strategy", "Mean" );

  // EnclosingSimplexAlgorithm parameters
  setAsUnsignedInteger( "EnclosingSimplexAlgorithm-LargeDimension", 5);

  // Matrix parameters
  setAsUnsignedInteger( "Matrix-size-visible-in-str-from", 5 );

  // Tensor parameters
  setAsUnsignedInteger( "Tensor-size-visible-in-str-from", 5 );

  // Tensor parameters
  setAsUnsignedInteger( "ComplexTensor-size-visible-in-str-from", 6 );

  // MatrixImplementation parameters //
  setAsScalar( "Matrix-DefaultSmallPivot", 1.0e-7  );
  setAsScalar( "Matrix-SymmetryThreshold", 1.0e-12 );

  // BernsteinCopulaFactory parameters //
  setAsUnsignedInteger( "BernsteinCopulaFactory-kFraction", 2 );
  setAsScalar( "BernsteinCopulaFactory-alpha", 1.0 );
  set( "BernsteinCopulaFactory-BinNumberSelectionMethod", "LogLikelihood" );
  setAsUnsignedInteger( "BernsteinCopulaFactory-MinM", 2 );
  setAsUnsignedInteger( "BernsteinCopulaFactory-MaxM", 1 );
  setAsUnsignedInteger( "BernsteinCopulaFactory-SamplingSize", 10000 );

  // BurrFactory parameters //
  setAsScalar( "BurrFactory-AbsolutePrecision", 1.0e-12 );
  setAsScalar( "BurrFactory-RelativePrecision", 1.0e-12 );
  setAsScalar( "BurrFactory-ResidualPrecision", 1.0e-12 );
  setAsUnsignedInteger( "BurrFactory-MaximumIteration", 10 );

  // ConditionalDistribution parameters //
  setAsUnsignedInteger( "ConditionalDistribution-MarginalIntegrationNodesNumber", 256 );
  setAsUnsignedInteger( "ConditionalDistribution-MaximumIntegrationNodesNumber", 100000 );

  // CompositeDistribution parameters //
  setAsUnsignedInteger( "CompositeDistribution-StepNumber", 256 );
  setAsScalar( "CompositeDistribution-SolverEpsilon", 1.0e-10 );

  // ComposedDistribution parameters //
  setAsBool("ComposedDistribution-UseGenericCovarianceAlgorithm", false);

  // DirichletFactory parameters //
  setAsUnsignedInteger( "Dirichlet-DefaultSamplingSize", 500000 );
  setAsUnsignedInteger( "Dirichlet-DefaultIntegrationSize", 50 );

  // DirichletFactory parameters //
  setAsScalar( "DirichletFactory-ParametersEpsilon", 1.0e-12 );
  setAsUnsignedInteger( "DirichletFactory-MaximumIteration", 10 );

  // FisherSnedecorFactory parameters //
  setAsScalar( "FisherSnedecorFactory-D1LowerBound", 1.0e-2 );
  setAsScalar( "FisherSnedecorFactory-D2LowerBound", 1.0e-2 );

  // FrechetFactory parameters //
  setAsScalar( "FrechetFactory-BoundMargin", 10.0 );

  // GeneralizedExtremeValue parameters //
  setAsScalar( "GeneralizedExtremeValue-XiThreshold", 1.0e-6 );

  // GeneralizedParetoFactory parameters //
  setAsUnsignedInteger( "GeneralizedParetoFactory-SmallSize", 20 );
  setAsUnsignedInteger( "GeneralizedParetoFactory-MaximumEvaluationNumber", 1000 );
  setAsScalar( "GeneralizedParetoFactory-MaximumAbsoluteError", 1.0e-10 );
  setAsScalar( "GeneralizedParetoFactory-MaximumRelativeError", 1.0e-10 );
  setAsScalar( "GeneralizedParetoFactory-MaximumObjectiveError", 1.0e-10 );
  setAsScalar( "GeneralizedParetoFactory-MaximumConstraintError", 1.0e-10 );

  // InverseNormalFactory parameters //
  set( "InverseNormalFactory-Method", "MLE" );

  // KernelMixture parameters //
  setAsUnsignedInteger( "KernelMixture-SmallSize", 50 );
  setAsUnsignedInteger( "KernelMixture-LargeSize", 20 );
  setAsUnsignedInteger( "KernelMixture-PDFCDFDiscretization", 1000 );

  // KernelSmoothing parameters //
  setAsUnsignedInteger( "KernelSmoothing-SmallSize", 250 );
  setAsUnsignedInteger( "KernelSmoothing-BinNumber", 1024 );
  setAsScalar( "KernelSmoothing-CutOffPlugin", 5.0 );
  setAsScalar( "KernelSmoothing-AbsolutePrecision", 0.0 );
  setAsScalar( "KernelSmoothing-RelativePrecision", 1.0e-5 );
  setAsScalar( "KernelSmoothing-ResidualPrecision", 1.0e-10 );
  setAsUnsignedInteger( "KernelSmoothing-MaximumIteration", 50 );

  // LogNormal parameters //
  setAsUnsignedInteger( "LogNormal-CharacteristicFunctionIntegrationNodes", 256 );
  setAsScalar( "LogNormal-CharacteristicFunctionSmallSigmaThreshold", 0.2 );

  // LogNormalFactory parameters //
  setAsUnsignedInteger( "LogNormalFactory-EstimationMethod", 0 );
  setAsScalar( "LogNormalFactory-AbsolutePrecision", 1.0e-12 );
  setAsScalar( "LogNormalFactory-RelativePrecision", 1.0e-12 );
  setAsScalar( "LogNormalFactory-ResidualPrecision", 1.0e-12 );
  setAsUnsignedInteger( "LogNormalFactory-MaximumIteration", 50 );

  // Meixner parameters //
  setAsUnsignedInteger( "MeixnerDistribution-CDFIntegrationNodesNumber", 32 );
  setAsUnsignedInteger( "MeixnerDistribution-CDFDiscretization", 10000 );
  setAsScalar( "MeixnerDistribution-MaximumAbsoluteError", 1.0e-12 );
  setAsScalar( "MeixnerDistribution-MaximumRelativeError", 1.0e-12 );
  setAsScalar( "MeixnerDistribution-MaximumConstraintError", 1.0e-12 );
  setAsScalar( "MeixnerDistribution-MaximumObjectiveError", 1.0e-12 );

  // Mixture parameters //
  setAsScalar( "Mixture-SmallWeight", 1.0e-12 );
  setAsUnsignedInteger( "Mixture-SmallSize", 50 );
  setAsUnsignedInteger( "Mixture-LargeSize", 20 );
  setAsUnsignedInteger( "Mixture-PDFCDFDiscretization", 1000 );

  // Multinomial parameters //
  setAsUnsignedInteger( "Multinomial-smallA", 10 );
  setAsScalar( "Multinomial-eta", 1.0e-9 );

  // NegativeBinomialFactory parameters //
  setAsScalar( "NegativeBinomialFactory-AbsolutePrecision", 1.0e-12 );
  setAsScalar( "NegativeBinomialFactory-RelativePrecision", 1.0e-12 );
  setAsScalar( "NegativeBinomialFactory-ResidualPrecision", 1.0e-12 );
  setAsUnsignedInteger( "NegativeBinomialFactory-MaximumIteration", 50 );

  // Normal parameters //
  setAsUnsignedInteger( "Normal-MaximumNumberOfPoints", 10000000 );
  setAsUnsignedInteger( "Normal-MinimumNumberOfPoints", 100000 );
  setAsUnsignedInteger( "Normal-SmallDimension", 6 );
  setAsUnsignedInteger( "Normal-MarginalIntegrationNodesNumber", 16 );
  setAsScalar( "Normal-MaximumCDFEpsilon", 5.0e-6 );
  setAsScalar( "Normal-MinimumCDFEpsilon", 5.0e-2 );

  // ProductDistribution parameters //
  setAsScalar( "ProductDistribution-LargeCharacteristicFunctionArgument", 10.0 );

  // RiceFactory parameters //
  setAsScalar( "RiceFactory-AbsolutePrecision", 1.0e-12 );
  setAsScalar( "RiceFactory-RelativePrecision", 1.0e-12 );
  setAsScalar( "RiceFactory-ResidualPrecision", 1.0e-12 );
  setAsUnsignedInteger( "RiceFactory-MaximumIteration", 10 );

  // TrapezoidalFactory parameters //
  setAsScalar( "TrapezoidalFactory-RhoBeg", 0.1 );
  setAsScalar( "TrapezoidalFactory-RhoEnd", 1.0e-5 );
  setAsUnsignedInteger( "TrapezoidalFactory-MaximumIteration", 2000 );

  // TruncatedDistribution parameters //
  setAsScalar( "TruncatedDistribution-DefaultThresholdRealization", 0.5 );

  // TruncatedNormalFactory parameters //
  setAsScalar( "TruncatedNormalFactory-SigmaLowerBound", 1.0e-4 );

  // MaximumLikelihoodFactory parameters //
  set( "MaximumLikelihoodFactory-DefaultOptimizationAlgorithm", "TNC" );
  setAsUnsignedInteger( "MaximumLikelihoodFactory-MaximumEvaluationNumber", 1000 );
  setAsScalar( "MaximumLikelihoodFactory-MaximumAbsoluteError", 1.0e-10 );
  setAsScalar( "MaximumLikelihoodFactory-MaximumRelativeError", 1.0e-10 );
  setAsScalar( "MaximumLikelihoodFactory-MaximumObjectiveError", 1.0e-10 );
  setAsScalar( "MaximumLikelihoodFactory-MaximumConstraintError", 1.0e-10 );

  // MethodOfMomentsFactory parameters //
  setAsUnsignedInteger( "MethodOfMomentsFactory-MaximumEvaluationNumber", 1000 );
  setAsScalar( "MethodOfMomentsFactory-MaximumAbsoluteError", 1.0e-10 );
  setAsScalar( "MethodOfMomentsFactory-MaximumRelativeError", 1.0e-10 );
  setAsScalar( "MethodOfMomentsFactory-MaximumObjectiveError", 1.0e-10 );
  setAsScalar( "MethodOfMomentsFactory-MaximumConstraintError", 1.0e-10 );

  // Student parameters //
  setAsUnsignedInteger( "Student-MaximumNumberOfPoints", 10000000 );
  setAsUnsignedInteger( "Student-MinimumNumberOfPoints", 100000 );
  setAsUnsignedInteger( "Student-SmallDimension", 6 );
  setAsUnsignedInteger( "Student-MarginalIntegrationNodesNumber", 16 );
  setAsScalar( "Student-MaximumCDFEpsilon", 5.0e-6 );
  setAsScalar( "Student-MinimumCDFEpsilon", 5.0e-2 );

  // NonCentralStudent parameters //
  setAsUnsignedInteger( "NonCentralStudent-CDFAlgo", 0 );

  // UserDefined parameters //
  setAsUnsignedInteger( "UserDefined-SmallSize", 10000 );

  // AliMikhailHaqCopulaFactory parameters //
  setAsScalar( "AliMikhailHaqCopulaFactory-ThetaEpsilon", 1.0e-14 );

  // FrankCopulaFactory parameters //
  setAsScalar( "FrankCopulaFactory-AbsolutePrecision", 1.0e-14 );
  setAsScalar( "FrankCopulaFactory-RelativePrecision", 1.0e-14 );
  setAsScalar( "FrankCopulaFactory-ResidualPrecision", 1.0e-14 );
  setAsUnsignedInteger( "FrankCopulaFactory-MaximumIteration", 100 );

  // RandomMixture parameters //
  setAsUnsignedInteger( "RandomMixture-DefaultBlockMin", 3 );
  setAsUnsignedInteger( "RandomMixture-DefaultBlockMax", 16 );
  setAsUnsignedInteger( "RandomMixture-DefaultMaxSize", 65536 );
  setAsScalar( "RandomMixture-DefaultAlpha", 5.0 );
  setAsScalar( "RandomMixture-DefaultBeta", 8.5 );
  setAsScalar( "RandomMixture-DefaultPDFEpsilon", 1.0e-10 );
  setAsScalar( "RandomMixture-DefaultCDFEpsilon", 1.0e-10 );
  setAsScalar( "RandomMixture-GraphPDFEpsilon", 1.0e-5 );
  setAsScalar( "RandomMixture-GraphCDFEpsilon", 1.0e-5 );
  setAsUnsignedInteger( "RandomMixture-SmallSize", 100 );
  setAsUnsignedInteger( "RandomMixture-ProjectionDefaultSize", 25 );
  setAsUnsignedInteger( "RandomMixture-MarginalIntegrationNodesNumber", 128 );
  setAsUnsignedInteger( "RandomMixture-MaximumIntegrationNodesNumber", 1024 );
  setAsUnsignedInteger( "RandomMixture-MaximumSupportSize", 2048 );
  setAsBool( "RandomMixture-SimplifyAtoms", true );

  // NumericalMathEvaluation parameters //
  setAsUnsignedInteger( "NumericalMathEvaluation-DefaultPointNumber", 129 );
  setAsScalar( "NumericalMathEvaluation-ParameterEpsilon", 1.0e-7 );

  // DualLinearCombinationEvaluation //
  setAsScalar( "DualLinearCombinationEvaluation-SmallCoefficient", 0.0);

  // LinearCombinationEvaluation //
  setAsScalar( "LinearCombinationEvaluation-SmallCoefficient", 0.0);

  // DistFunc parameters //
  setAsScalar( "DistFunc-Precision", 1.0e-14 );
  setAsUnsignedInteger( "DistFunc-MaximumIteration", 5000 );

  // KFactor parameters //
  setAsScalar( "KFactor-Precision", 1.0e-8 );
  setAsUnsignedInteger( "KFactor-MaximumIteration", 32 );
  setAsUnsignedInteger( "KFactor-DefaultIntegrationNodesNumber", 256 );

  // RootStrategyImplementation parameters //
  setAsScalar( "RootStrategy-DefaultMaximumDistance", 8.0 );
  setAsScalar( "RootStrategy-DefaultStepSize", 1.0 );

  // Simulation parameters //
  setAsUnsignedInteger( "Simulation-DefaultMaximumOuterSampling", 1000 );
  setAsScalar( "Simulation-DefaultMaximumCoefficientOfVariation", 1.0e-1 );
  setAsScalar( "Simulation-DefaultMaximumStandardDeviation", 0.0 );
  setAsUnsignedInteger( "Simulation-DefaultBlockSize", 1 );

  // SimulationResultImplementation parameters //
  setAsScalar( "SimulationResult-DefaultConfidenceLevel", 0.95 );

  // SimulationSensitivityAnalysis parameters //
  setAsUnsignedInteger( "SimulationSensitivityAnalysis-DefaultSampleMargin", 400 );

  // SubsetSampling parameters //
  setAsUnsignedInteger( "SubsetSampling-DefaultMaximumOuterSampling", 10000 );
  setAsScalar( "SubsetSampling-DefaultConditionalProbability", 0.1 );
  setAsScalar( "SubsetSampling-DefaultProposalRange", 2.0 );
  setAsScalar( "SubsetSampling-DefaultBetaMin", 2.0 );

  // DirectionalSampling parameters //
  setAsUnsignedInteger( "DirectionalSampling-MeanContributionIntegrationNodesNumber", 255 );

  // AdaptiveDirectionalSampling parameters //
  setAsUnsignedInteger( "AdaptiveDirectionalSampling-DefaultMaximumStratificationDimension", 3 );
  setAsScalar( "AdaptiveDirectionalSampling-DefaultGamma", 0.5 );
  setAsUnsignedInteger( "AdaptiveDirectionalSampling-DefaultNumberOfSteps", 2 );

  // AnalyticalResult parameters //
  setAsScalar( "AnalyticalResult-DefaultWidth", 1.0 );
  setAsUnsignedInteger( "AnalyticalResult-MeanPointIntegrationNodesNumber", 255 );

  // StrongMaximumTest parameters //
  setAsScalar( "StrongMaximumTest-DefaultDeltaPrecision", 1.0e-7 );
  setAsScalar( "StrongMaximumTest-Epsilon", 1.0e-10 );

  // CleaningStrategy parameters //
  setAsUnsignedInteger( "CleaningStrategy-DefaultMaximumSize", 20 );
  setAsScalar( "CleaningStrategy-DefaultSignificanceFactor", 1.0e-4 );

  // FunctionalChaosAlgorithm parameters //
  setAsScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual", 1.0e-6 );
  setAsScalar( "FunctionalChaosAlgorithm-QNorm", 0.5);
  setAsScalar( "FunctionalChaosAlgorithm-PValueThreshold", 1.0e-3);
  setAsUnsignedInteger( "FunctionalChaosAlgorithm-SmallSampleSize", 1000);
  setAsUnsignedInteger( "FunctionalChaosAlgorithm-LargeSampleSize", 10000);
  setAsUnsignedInteger( "FunctionalChaosAlgorithm-MaximumTotalDegree", 10 );

  // FunctionalChaosSobolIndices parameters //
  setAsScalar( "FunctionalChaosSobolIndices-VariancePartThreshold", 1.0e-2);

  // GeneralLinearModelAlgorithm parameters //
  set("GeneralLinearModelAlgorithm-LinearAlgebra", "LAPACK");
  setAsBool("GeneralLinearModelAlgorithm-NormalizeData", false);
  setAsBool("GeneralLinearModelAlgorithm-KeepCovariance", true);
  setAsBool("GeneralLinearModelAlgorithm-OptimizeParameters", true);
  setAsBool("GeneralLinearModelAlgorithm-UseAnalyticalAmplitudeEstimate", true);
  setAsBool("GeneralLinearModelAlgorithm-UnbiasedVariance", true);
  setAsScalar( "GeneralLinearModelAlgorithm-MeanEpsilon", 1.0e-12 );
  setAsScalar( "GeneralLinearModelAlgorithm-StartingScaling", 1.0e-13 );
  setAsScalar( "GeneralLinearModelAlgorithm-MaximalScaling", 1.0e5 );
  setAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationLowerBound", 1.0e-2 );
  setAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationUpperBound", 1.0e2 );
  setAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationScaleFactor", 2.0 );
  set("GeneralLinearModelAlgorithm-DefaultOptimizationAlgorithm", "TNC");

  // KrigingAlgorithm parameters //
  setAsScalar( "KrigingAlgorithm-StartingScaling", 1.0e-13 );
  setAsScalar( "KrigingAlgorithm-MaximalScaling", 1.0e5 );
  set("KrigingAlgorithm-LinearAlgebra", "LAPACK");

  // SquaredExponential parameters //
  setAsScalar( "SquaredExponential-DefaultTheta", 1.0 );

  // AbsoluteExponential parameters //
  setAsScalar( "AbsoluteExponential-DefaultTheta", 1.0 );

  // GeneralizedExponential parameters //
  setAsScalar( "GeneralizedExponential-DefaultTheta", 1.0 );

  // MaternModel parameters //
  setAsScalar( "MaternModel-DefaultNu", 1.5 );
  setAsScalar( "MaternModel-DefaultTheta", 1.0 );

  // WeightedExperimentImplementation parameters //
  setAsUnsignedInteger( "WeightedExperiment-DefaultSize", 100 );

  // GaussProductExperiment parameters //
  setAsUnsignedInteger( "GaussProductExperiment-DefaultMarginalDegree", 5 );

  // HyperbolicAnisotropicEnumerateFunction parameters //
  setAsScalar( "HyperbolicAnisotropicEnumerateFunction-DefaultQ", 0.4 );

  // MarginalTransformationEvaluation parameters //
  setAsScalar( "MarginalTransformationEvaluation-DefaultTailThreshold", 0.99 );
  setAsBool( "MarginalTransformationEvaluation-Simplify", true );
  setAsScalar( "MarginalTransformationEvaluation-ParametersEpsilon", 1.0e-14 );

  // DistributionImplementation parameters //
  setAsUnsignedInteger( "Distribution-DefaultPointNumber", 129 );
  setAsScalar( "Distribution-DefaultQuantileEpsilon", 1.0e-12 );
  setAsScalar( "Distribution-DefaultPDFEpsilon", 1.0e-14 );
  setAsScalar( "Distribution-DefaultCDFEpsilon", 1.0e-14 );
  setAsUnsignedInteger( "Distribution-DefaultQuantileIteration", 100 );
  setAsScalar( "Distribution-QMin", 0.15 );
  setAsScalar( "Distribution-QMax", 0.85 );
  setAsUnsignedInteger( "Distribution-DefaultIntegrationNodesNumber", 255 );
  setAsUnsignedInteger( "Distribution-DefaultLevelNumber", 10 );
  setAsUnsignedInteger( "Distribution-DefaultQuantileCacheSize", 128 );
  setAsBool("Distribution-Parallel", true);
  setAsUnsignedInteger( "Distribution-CharacteristicFunctionBlockMax", 20 );
  setAsUnsignedInteger( "Distribution-CharacteristicFunctionNMax", 1000000 );
  setAsUnsignedInteger( "Distribution-MinimumVolumeLevelSetSamplingSize", 1048576 );
  setAsBool( "Distribution-MinimumVolumeLevelSetBySampling", false );
  setAsBool( "Distribution-UseCovarianceAdaptiveAlgorithm", true );
  setAsUnsignedInteger( "Distribution-SmallDimensionEntropy", 3 );
  setAsUnsignedInteger( "Distribution-EntropySamplingSize", 1048576 );

  // ContinuousDistribution parameters //
  setAsUnsignedInteger( "ContinuousDistribution-DefaultIntegrationNodesNumber", 256 );

  // DiscreteDistribution parameters //
  setAsScalar( "DiscreteDistribution-SupportEpsilon", 1.0e-14 );

  // DistributionFactoryImplementation parameters //
  setAsUnsignedInteger( "DistributionFactory-DefaultBootstrapSize", 100 );
  setAsScalar( "DistributionFactory-BootstrapErrorTolerance", 0.1 );

  // OrderStatisticsMarginalChecker //
  setAsUnsignedInteger( "OrderStatisticsMarginalChecker-QuantileIteration", 100 );
  setAsScalar( "OrderStatisticsMarginalChecker-OptimizationEpsilon", 1.0e-7 );

  // MaximumEntropyOrderStatisticsDistribution //
  setAsBool( "MaximumEntropyOrderStatisticsDistribution-UseApproximation", true );
  setAsBool( "MaximumEntropyOrderStatisticsDistribution-CheckMarginals", true );
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-ExponentialFactorDiscretization", 100 );
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-MaximumApproximationSubdivision", 2 );
  setAsScalar( "MaximumEntropyOrderStatisticsDistribution-SupportShift", 1.0e-15 );
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-MaximumQuantileIteration", 10);
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-CDFIntegrationNodesNumber", 16);

  // HMatrix parameters //
  set( "HMatrix-ClusteringAlgorithm", "median" );
  setAsScalar( "HMatrix-AdmissibilityFactor", 2.0 );
  setAsUnsignedInteger( "HMatrix-CompressionMethod", 1 );
  setAsScalar( "HMatrix-AssemblyEpsilon", 1.0e-4);
  setAsScalar( "HMatrix-RecompressionEpsilon", 1.0e-4);
  setAsUnsignedInteger( "HMatrix-MaxLeafSize", 100);
  setAsUnsignedInteger( "HMatrix-MaxParallelLeaves", 5000);
  setAsScalar( "HMatrix-ValidationError", 0.0);
  setAsUnsignedInteger( "HMatrix-ValidationRerun", 0);
  setAsUnsignedInteger( "HMatrix-ValidationDump", 0);
  setAsBool( "HMatrix-ForceSequential", false);

  // GaussianProcess parameters //
  setAsScalar( "GaussianProcess-StartingScaling", 1.0e-13 );
  setAsScalar( "GaussianProcess-MaximalScaling", 1.0e5 );
  setAsUnsignedInteger( "GaussianProcess-GibbsMaximumIteration", 100 );

  // SpectralGaussianProcess parameters //
  setAsScalar( "SpectralGaussianProcess-StartingScaling", 1.0e-13 );
  setAsScalar( "SpectralGaussianProcess-MaximalScaling", 1.0e5 );
  setAsUnsignedInteger( "SpectralGaussianProcess-CholeskyCacheSize", 16384 );

  // WhittleFactory parameters //
  setAsScalar( "WhittleFactory-DefaultRhoBeg", 0.1 );
  setAsScalar( "WhittleFactory-DefaultRhoEnd", 1.0e-10 );
  setAsScalar( "WhittleFactory-DefaultMaxFun", 2000 );
  setAsScalar( "WhittleFactory-DefaultStartingPointScale", 1.0 );
  setAsScalar( "WhittleFactory-RootEpsilon", 1.0e-6);

  // BoxCoxFactory parameters //
  setAsScalar( "BoxCoxFactory-DefaultRhoBeg", 0.1 );
  setAsScalar( "BoxCoxFactory-DefaultRhoEnd", 1.0e-10 );
  setAsUnsignedInteger( "BoxCoxFactory-DefaultMaxFun", 2000 );
  setAsUnsignedInteger( "BoxCoxFactory-DefaultPointNumber", 201 );

  // VisualTest parameters //
  setAsUnsignedInteger( "VisualTest-KendallPlot-MonteCarloSize", 100 );

  // CalibrationStrategyImplementation parameters //
  setAsScalar( "CalibrationStrategy-DefaultLowerBound", 0.117 ); // = 0.5 * 0.234
  setAsScalar( "CalibrationStrategy-DefaultUpperBound", 0.468 ); // = 2.0 * 0.234
  setAsScalar( "CalibrationStrategy-DefaultShrinkFactor", 0.8 );
  setAsScalar( "CalibrationStrategy-DefaultExpansionFactor", 1.2 );
  setAsUnsignedInteger( "CalibrationStrategy-DefaultCalibrationStep", 100 );

  // MCMC parameters //
  setAsUnsignedInteger("MCMC-DefaultBurnIn", 0);
  setAsUnsignedInteger("MCMC-DefaultThinning", 1);

  // ARMA parameters //
  setAsScalar( "ARMA-MeanEpsilon", 1.0e-14 );

  // ARMALikelihoodFactory parameters //
  setAsScalar( "ARMALikelihoodFactory-StartingScaling", 1.0e-13 );
  setAsScalar( "ARMALikelihoodFactory-MaximalScaling", 1.0e5 );
  setAsScalar( "ARMALikelihoodFactory-DefaultRhoBeg", 0.01 );
  setAsScalar( "ARMALikelihoodFactory-DefaultRhoEnd", 1.0e-10 );
  setAsUnsignedInteger( "ARMALikelihoodFactory-DefaultMaxFun", 10000 );
  setAsScalar( "ARMALikelihoodFactory-DefaultStartingPointScale", 1.0 );
  setAsScalar( "ARMALikelihoodFactory-RootEpsilon", 1.0e-6);

  // FittingTest parameters //
  setAsUnsignedInteger( "FittingTest-ChiSquaredMinFrequency", 5 );

  // LeastSquaresMetaModelSelection parameters //
  set("LeastSquaresMetaModelSelection-DecompositionMethod", "SVD");
  setAsScalar("LeastSquaresMetaModelSelection-MaximumErrorFactor", 2.0);
  setAsScalar("LeastSquaresMetaModelSelection-ErrorThreshold", 0.0);

  // SparseMethod parameters //
  setAsScalar("SparseMethod-MaximumErrorFactor", 2.0);
  setAsScalar("SparseMethod-ErrorThreshold", 1.0e-3);

  // CholeskyMethod parameters //
  setAsUnsignedInteger("CholeskyMethod-LargeCase", 128);

  // Classifier parameters //
  setAsBool( "Classifier-Parallel", true);

  // TensorApproximationAlgorithm parameters //
  set("TensorApproximationAlgorithm-Method", "GreedyRankOne");
  set("TensorApproximationAlgorithm-DecompositionMethod", "SVD");
  setAsUnsignedInteger("TensorApproximationAlgorithm-DefaultMaximumAlternatingLeastSquaresIteration", 100);
  setAsScalar("TensorApproximationAlgorithm-DefaultMaximumRadiusError", 1.0e-5);
  setAsScalar("TensorApproximationAlgorithm-DefaultMaximumResidualError", 1.0e-5);

  // viewer.View parameters //
  set("View-ImageFormat", "png");
}

/* String converter */
String ResourceMap::__repr__() const
{
  OSS oss;
  oss << "ResourceMap={";
  const char * separator = "\n  ";
  for(MapType::const_iterator it = map_.begin(); it != map_.end(); ++it, separator = ",\n  ")
    oss << separator
        << it->first << " => " << it->second;
  oss << "\n}";
  return oss;
}

std::vector<String> ResourceMap::getKeys() const
{
  std::vector<String> keys;
  for(MapType::const_iterator it = map_.begin(); it != map_.end(); ++it)
  {
    keys.push_back(it->first);
  }
  return keys;
}

/*
 * @fn std::ostream & operator <<(std::ostream & os, const ResourceMap & obj)
 * @brief Output stream converter
 * @param os A STL output stream resourceMap
 * @param obj The resourceMap read by \em os
 * @return A reference to \em os
 *
 * Operator << converts the ResourceMap object to an output stream
 * so it is easy to show the content of the resourceMap.
 */
std::ostream & operator <<(std::ostream & os, const MutexLockSingleton<ResourceMap> & obj)
{
  return os << obj.lock().__repr__();
}

OStream & operator <<(OStream & OS, const MutexLockSingleton<ResourceMap> & obj)
{
  return OS << obj.lock().__repr__();
}





END_NAMESPACE_OPENTURNS
