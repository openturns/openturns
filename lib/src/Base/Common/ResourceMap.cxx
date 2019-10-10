//                                               -*- C++ -*-
/**
 *  @brief ResourceMap defines top-most resourceMap strategies
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include <unistd.h>                 // for sysconf
#include "openturns/OTthread.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/OSS.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Path.hxx"
#include "openturns/Collection.hxx"
#include "openturns/XMLToolbox.hxx"

BEGIN_NAMESPACE_OPENTURNS

static const char * ConfigurationFileName = "openturns.conf";
#if defined OPENTURNS_HAVE_LIBXML2
static const char * RootElementName = "openturns-configuration";
static const char * XMLTag_value_str              = "value_str";
static const char * XMLTag_value_float            = "value_float";
static const char * XMLTag_value_int              = "value_int";
static const char * XMLTag_value_bool             = "value_bool";
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
  return GetInstance().lock().map_.getKeys();
}

std::vector<String> ResourceMap::GetStringKeys()
{
  return GetInstance().lock().map_.getStringKeys();
}

std::vector<String> ResourceMap::GetScalarKeys()
{
  return GetInstance().lock().map_.getScalarKeys();
}

std::vector<String> ResourceMap::GetUnsignedIntegerKeys()
{
  return GetInstance().lock().map_.getUnsignedIntegerKeys();
}

std::vector<String> ResourceMap::GetBoolKeys()
{
  return GetInstance().lock().map_.getBoolKeys();
}

/* Get a value in the map */
String ResourceMap::GetType(const String & key)
{
  return GetInstance().lock().map_.getType( key );
}

/* Get a value in the map */
String ResourceMap::Get(const String & key)
{
  return GetInstance().lock().map_.get( key );
}

String ResourceMap::GetAsString(const String & key)
{
  return GetInstance().lock().map_.getAsString( key );
}

Bool ResourceMap::GetAsBool(const String & key)
{
  return GetInstance().lock().map_.getAsBool( key );
}

UnsignedInteger ResourceMap::GetAsUnsignedInteger(const String & key)
{
  return GetInstance().lock().map_.getAsUnsignedInteger( key );
}

Scalar ResourceMap::GetAsScalar(const String & key)
{
  return GetInstance().lock().map_.getAsScalar( key );
}

UnsignedInteger ResourceMap::GetSize()
{
  return GetInstance().lock().map_.getSize();
}

UnsignedInteger ResourceMap::GetStringSize()
{
  return GetInstance().lock().map_.getStringSize();
}

UnsignedInteger ResourceMap::GetScalarSize()
{
  return GetInstance().lock().map_.getScalarSize();
}

UnsignedInteger ResourceMap::GetUnsignedIntegerSize()
{
  return GetInstance().lock().map_.getUnsignedIntegerSize();
}

UnsignedInteger ResourceMap::GetBoolSize()
{
  return GetInstance().lock().map_.getBoolSize();
}

/* Set a value in the map */
void ResourceMap::Set(const String & key, const String & value)
{
  GetInstance().lock().map_.set( key, value );
}

void ResourceMap::SetAsString(const String & key, const String & value)
{
  GetInstance().lock().map_.setAsString( key, value );
}

void ResourceMap::SetAsBool(const String & key, Bool value)
{
  GetInstance().lock().map_.setAsBool( key, value );
}

void ResourceMap::SetAsUnsignedInteger(const String & key, UnsignedInteger value)
{
  GetInstance().lock().map_.setAsUnsignedInteger( key, value );
}

void ResourceMap::SetAsScalar(const String & key, Scalar value)
{
  GetInstance().lock().map_.setAsScalar( key, value );
}

Bool ResourceMap::HasKey(const String & key)
{
  return GetInstance().lock().map_.hasKey(key);
}

void ResourceMap::Reload()
{
  return GetInstance().lock().reload();
}

void ResourceMap::RemoveKey(const String & key)
{
  GetInstance().lock().removeKey(key);
}

/* Default constructor */
ResourceMap::ResourceMap()
  : map_()
{
  reload();
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
  if (xmlStrcmp(rootElt->name, reinterpret_cast<const xmlChar *>(RootElementName)))
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
        const String key = XML::ToString(current->name);
        // Try to get a String value
        {
          String value = XML::GetAttributeByName(current, XMLTag_value_str);
          if (value.size() > 0)
          {
            map_.setAsString(key, value);
            break;
          }
        } // String
        // Try to get a Scalar value
        {
          String value = XML::GetAttributeByName(current, XMLTag_value_float);
          if (value.size() > 0)
          {
            Scalar scalarValue = -1.0;
            std::istringstream iss( value );
            iss >> scalarValue;
            map_.setAsScalar(key, scalarValue);
            break;
          }
        } // Scalar
        // Try to get an UnsignedInteger value
        {
          String value = XML::GetAttributeByName(current, XMLTag_value_int);
          if (value.size() > 0)
          {
            UnsignedInteger unsignedIntegerValue = 0;
            std::istringstream iss( value );
            iss >> unsignedIntegerValue;
            map_.setAsUnsignedInteger(key, unsignedIntegerValue);
            break;
          }
        } // UnsignedInteger
        // Try to get a Bool value
        {
          String value = XML::GetAttributeByName( current, XMLTag_value_bool );
          if (value.size() > 0)
          {
            Bool boolValue = false;
            // First, try to recover the bool value from the "true" or "false" strings
            if (value == "true") boolValue = true;
            else if (value == "false") boolValue = false;
            // Second, try to recover the bool from the litteral value
            else
            {
              std::istringstream iss( value );
              iss >> boolValue;
            }
            map_.setAsBool(key, boolValue);
            break;
          }
        } // Bool
      } // if XML::IsElement
    } // for xmlNodePtr
  } // if root
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
  map_.setAsString( "R-executable-command", R_EXECUTABLE );
#else
  map_.setAsString( "R-executable-command", "" );
#endif
  map_.setAsString( "csv-file-separator", ";" );
#ifndef WIN32
  map_.setAsString( "temporary-directory", "/tmp" );
  map_.setAsUnsignedInteger( "parallel-threads", sysconf(_SC_NPROCESSORS_CONF) );
#else
  map_.setAsString( "temporary-directory", "TEMP" );
  UnsignedInteger numberOfProcessors = 0;
  std::istringstream iss( getenv("NUMBER_OF_PROCESSORS") );
  iss >> numberOfProcessors;
  map_.setAsUnsignedInteger( "parallel-threads", numberOfProcessors );
#endif
  map_.setAsUnsignedInteger( "cache-max-size", 1024 );
  map_.setAsUnsignedInteger( "output-files-timeout", 2 );

  // Os parameters
  map_.setAsBool( "Os-create-process", "false" );
  map_.setAsBool( "Os-RemoveFiles", "true" );

  // XMLStorageManager parameters
  map_.setAsUnsignedInteger( "XMLStorageManager-DefaultCompressionLevel", 0 );

  // Collection parameters
  map_.setAsUnsignedInteger( "Collection-size-visible-in-str-from", 10 );

  // SpecFunc parameters
  map_.setAsScalar( "SpecFunc-Precision", 2.0e-16 );
  map_.setAsUnsignedInteger( "SpecFunc-MaximumIteration", 1000 );

  // SymbolicParser parameters
  map_.setAsBool( "SymbolicParser-CheckResult", true );
  map_.setAsString( "SymbolicParser-Backend", SYMBOLICPARSER_DEFAULT_BACKEND );

  // DesignProxy parameters
  map_.setAsUnsignedInteger( "DesignProxy-DefaultCacheSize", 16777216 );// 2^24=16777216=128 Mio

  // KFold parameters
  map_.setAsUnsignedInteger( "KFold-DefaultK", 10 );

  // BlendedStep parameters //
  map_.setAsScalar( "BlendedStep-DefaultEta", 1.0 );

  // CenteredFiniteDifferenceGradient parameters //
  map_.setAsScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon", 1.0e-5 );

  // CenteredFiniteDifferenceHessian parameters //
  map_.setAsScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon", 1.0e-4 );

  // NonCenteredFiniteDifferenceGradient parameters //
  map_.setAsScalar( "NonCenteredFiniteDifferenceGradient-DefaultEpsilon", 1.0e-7 );

  // PiecewiseHermiteEvaluation parameters //
  map_.setAsScalar( "PiecewiseHermiteEvaluation-EpsilonRegular", 1.0e-12 );

  // PiecewiseLinearEvaluation parameters //
  map_.setAsScalar( "PiecewiseLinearEvaluation-EpsilonRegular", 1.0e-12 );

  // UniVariatePolynomialImplementation parameters //
  map_.setAsUnsignedInteger( "UniVariatePolynomial-SmallDegree", 400 );

  // Pie parameters //
  map_.setAsScalar( "Pie-HorizontalMargin", 0.3 );
  map_.setAsScalar( "Pie-LabelThreshold", 0.02 );
  map_.setAsScalar( "Pie-VerticalMargin", 0.1 );

  // DrawableImplementation parameters //
  map_.setAsScalar( "Drawable-DefaultLineWidth", 1.0 );
  map_.setAsString( "Drawable-AlternativePattern", "S" );
  map_.setAsString( "Drawable-DefaultColor", "blue" );
  map_.setAsString( "Drawable-DefaultFillStyle", "solid" );
  map_.setAsString( "Drawable-DefaultLineStyle", "solid" );
  map_.setAsString( "Drawable-DefaultPattern", "s" );
  map_.setAsString( "Drawable-DefaultPointStyle", "plus" );
  map_.setAsString( "Drawable-DefaultSurfaceColor", "white" );
  map_.setAsString( "Drawable-NoSpecifiedLabel", "" );
  map_.setAsUnsignedInteger( "Drawable-DataThreshold", 2000 );
  map_.setAsUnsignedInteger( "Drawable-DefaultPalettePhase", 12 );

  // Text drawable
  map_.setAsScalar( "Text-DefaultTextSize", 0.75 );

  // GraphImplementation parameters //
  map_.setAsScalar( "Graph-DefaultHorizontalMargin", 0.05 );
  map_.setAsScalar( "Graph-DefaultLegendFontSize", 1.0 );
  map_.setAsScalar( "Graph-DefaultVerticalMargin", 0.05 );
  map_.setAsString( "Graph-NoSpecifiedLabel", "" );
  map_.setAsUnsignedInteger( "Graph-DefaultHeight", 480 );
  map_.setAsUnsignedInteger( "Graph-DefaultWidth", 640 );

  // Contour parameters //
  map_.setAsUnsignedInteger( "Contour-DefaultLevelsNumber", 10 );

  // IntervalMesher parameters //
  map_.setAsBool( "IntervalMesher-UseDiamond", false );

  // PointToPointEvaluation parameters //
  map_.setAsUnsignedInteger( "PointToPointEvaluation-BlockSize", 256);

  // SQP parameters //
  map_.setAsScalar( "SQP-DefaultOmega", 1.0e-4 );
  map_.setAsScalar( "SQP-DefaultSmooth", 1.2 );
  map_.setAsScalar( "SQP-DefaultTau", 0.5 );
  map_.setAsUnsignedInteger ( "SQP-DefaultMaximumEvaluationNumber", 100000);

  // TNC parameters //
  map_.setAsScalar( "TNC-DefaultAccuracy", 1.0e-4 );
  map_.setAsScalar( "TNC-DefaultEta", 0.25 );
  map_.setAsScalar( "TNC-DefaultFmin", 1.0 );
  map_.setAsScalar( "TNC-DefaultRescale", 1.3 );
  map_.setAsScalar( "TNC-DefaultStepmx", 10.0 );
  map_.setAsUnsignedInteger( "TNC-DefaultMaxCGit", 50 );

  // AbdoRackwitz parameters //
  map_.setAsScalar( "AbdoRackwitz-DefaultOmega", 1.0e-4 );
  map_.setAsScalar( "AbdoRackwitz-DefaultSmooth", 1.2 );
  map_.setAsScalar( "AbdoRackwitz-DefaultTau", 0.5 );
  map_.setAsUnsignedInteger( "AbdoRackwitz-DefaultMaximumEvaluationNumber", 100000);

  // OptimizationAlgorithm parameters //
  map_.setAsScalar( "OptimizationAlgorithm-DefaultLevelValue", 0.0 );
  map_.setAsScalar( "OptimizationAlgorithm-DefaultMaximumAbsoluteError", 1.0e-5 );
  map_.setAsScalar( "OptimizationAlgorithm-DefaultMaximumConstraintError", 1.0e-5 );
  map_.setAsScalar( "OptimizationAlgorithm-DefaultMaximumRelativeError", 1.0e-5 );
  map_.setAsScalar( "OptimizationAlgorithm-DefaultMaximumResidualError", 1.0e-5 );
  map_.setAsUnsignedInteger( "OptimizationAlgorithm-DefaultMaximumEvaluationNumber", 100);
  map_.setAsUnsignedInteger( "OptimizationAlgorithm-DefaultMaximumIterationNumber", 100 );
  
  // Dlib optimization parameters //
  map_.setAsScalar( "Dlib-DefaultInitialTrustRegionRadius", 1.0);
  map_.setAsScalar( "Dlib-DefaultWolfeRho", 0.01);
  map_.setAsScalar( "Dlib-DefaultWolfeSigma", 0.9);
  map_.setAsUnsignedInteger( "Dlib-DefaultMaxLineSearchIterations", 100);
  map_.setAsUnsignedInteger( "Dlib-DefaultMaxSize", 10);
  

  // EfficientGlobalOptimization parameters //
  map_.setAsScalar( "EfficientGlobalOptimization-DefaultAEITradeoff", 1.0);
  map_.setAsScalar( "EfficientGlobalOptimization-DefaultCorrelationLengthFactor", 1.0);
  map_.setAsScalar( "EfficientGlobalOptimization-DefaultImprovementFactor", 1.0);
  map_.setAsUnsignedInteger( "EfficientGlobalOptimization-DefaultMultiStartExperimentSize", 100);
  map_.setAsUnsignedInteger( "EfficientGlobalOptimization-DefaultMultiStartNumber", 20);
  map_.setAsUnsignedInteger( "EfficientGlobalOptimization-DefaultParameterEstimationPeriod", 1);

  // Cobyla parameters //
  map_.setAsScalar( "Cobyla-DefaultRhoBeg", 0.1 );

  // OPTpp parameters //
  map_.setAsScalar("OPTpp-DefaultLineSearchTolerance", 1.0e-4);
  map_.setAsScalar("OPTpp-DefaultMaximumStep", 1.0e3);
  map_.setAsScalar("OPTpp-DefaultMinimumStep", 2.0e-16);
  map_.setAsScalar("OPTpp-DefaultTrustRegionSize", 1.0e3);
  map_.setAsUnsignedInteger("OPTpp-DefaultMaximumLineSearchIterationNumber", 5);

  // SolverImplementation parameters //
  map_.setAsScalar( "Solver-DefaultAbsoluteError",  1.0e-5 );
  map_.setAsScalar( "Solver-DefaultRelativeError",  1.0e-5 );
  map_.setAsScalar( "Solver-DefaultResidualError",  1.0e-8 );
  map_.setAsUnsignedInteger( "Solver-DefaultMaximumFunctionEvaluation", 100 );

  // GaussKronrod parameters //
  map_.setAsScalar( "GaussKronrod-MaximumError",  1.0e-12 );
  map_.setAsUnsignedInteger( "GaussKronrod-MaximumSubIntervals", 100 );

  // GaussLegendre parameters //
  map_.setAsUnsignedInteger( "GaussLegendre-DefaultMarginalIntegrationPointsNumber", 64);

  // IteratedQuadrature parameters //
  map_.setAsScalar( "IteratedQuadrature-MaximumError",    1.0e-7 );
  map_.setAsUnsignedInteger( "IteratedQuadrature-MaximumSubIntervals", 32 );

  // Fehlberg parameters //
  map_.setAsScalar( "Fehlberg-InitialStep", 1.0e-7);
  map_.setAsScalar( "Fehlberg-LocalPrecision", 1.0e-10);
  map_.setAsUnsignedInteger( "Fehlberg-DefaultOrder", 4);

  // KarhunenLoeveQuadratureAlgorithm parameters //
  map_.setAsScalar( "KarhunenLoeveQuadratureAlgorithm-RegularizationFactor", 0.0);

  // KarhunenLoeveSVDAlgorithm parameters //
  map_.setAsBool( "KarhunenLoeveSVDAlgorithm-UseRandomSVD", false);
  map_.setAsString("KarhunenLoeveSVDAlgorithm-RandomSVDVariant", "Halko2010");
  map_.setAsUnsignedInteger( "KarhunenLoeveSVDAlgorithm-Halko2011Margin", 2);
  map_.setAsUnsignedInteger( "KarhunenLoeveSVDAlgorithm-Halko2011Iterations", 2);
  map_.setAsUnsignedInteger( "KarhunenLoeveSVDAlgorithm-RandomSVDMaximumRank", 1000);

  // KarhunenLoeveP1Algorithm parameters //
  map_.setAsScalar( "KarhunenLoeveP1Algorithm-RegularizationFactor", 0.0);

  // AdaptiveStieltjesAlgorithm parameters //
  map_.setAsScalar( "AdaptiveStieltjesAlgorithm-MaximumError",  1.0e-12 );
  map_.setAsUnsignedInteger( "AdaptiveStieltjesAlgorithm-MaximumSubIntervalsBetweenRoots", 64 );

  // LinearModelTest parameters //
  map_.setAsScalar( "LinearModelTest-DefaultHarrisonMcCabeBreakpoint", 0.5 );
  map_.setAsScalar( "LinearModelTest-DefaultLevel", 0.05 );
  map_.setAsString( "LinearModelTest-DefaultDurbinWatsonHypothesis", "Equal" );
  map_.setAsUnsignedInteger( "LinearModelTest-DefaultHarrisonMcCabeSimulationSize", 1000 );

  // Last parameters //
  map_.setAsUnsignedInteger( "Last-DefaultMaximumSize", 65536 );

  // Compact parameters //
  map_.setAsUnsignedInteger( "Compact-DefaultHalfMaximumSize", 1024 );

  // FaureSequence parameters //
  map_.setAsUnsignedInteger( "FaureSequence-InitialSeed", 1 );

  // HaltonSequence parameters //
  map_.setAsUnsignedInteger( "HaltonSequence-InitialSeed", 1 );

  // HaselgroveSequence parameters //
  map_.setAsUnsignedInteger( "HaselgroveSequence-InitialSeed", 1 );

  // ReverseHaltonSequence parameters //
  map_.setAsUnsignedInteger( "ReverseHaltonSequence-InitialSeed", 1 );

  // SobolSequence parameters //
  map_.setAsUnsignedInteger( "SobolSequence-InitialSeed", 1 );

  // SobolIndicesExperiment parameters //
  map_.setAsString( "SobolIndicesExperiment-SamplingMethod", "MonteCarlo");

  // SobolIndicesAlgorithm parameters //
  map_.setAsBool( "SobolIndicesAlgorithm-DefaultUseAsymptoticDistribution", false );
  map_.setAsScalar( "SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel", 0.95 );
  map_.setAsUnsignedInteger( "SobolIndicesAlgorithm-DefaultBlockSize", 1 );
  map_.setAsUnsignedInteger( "SobolIndicesAlgorithm-DefaultBootstrapSize", 100 );

  // FAST parameters //
  map_.setAsUnsignedInteger( "FAST-DefaultInterferenceFactor", 4 );
  map_.setAsUnsignedInteger( "FAST-DefaultResamplingSize", 1 );

  // RandomGenerator parameters //
  map_.setAsUnsignedInteger( "RandomGenerator-InitialSeed", 0 );

  // CovarianceModelImplementation parameters //
  map_.setAsScalar( "CovarianceModel-DefaultNuggetFactor", 1e-12 );
  map_.setAsScalar( "CovarianceModel-DefaultTMax",  5.0 );
  map_.setAsScalar( "CovarianceModel-DefaultTMin", -5.0 );
  map_.setAsUnsignedInteger( "CovarianceModel-DefaultPointNumber",  129 );

  // SpectralModel parameters //
  map_.setAsScalar( "SpectralModel-DefaultMaximumFrequency",  5.0 );
  map_.setAsScalar( "SpectralModel-DefaultMinimumFrequency", -5.0 );
  map_.setAsUnsignedInteger( "SpectralModel-DefaultFrequencyNumber",  129 );

  // FieldImplementation parameters //
  map_.setAsBool( "Field-AutomaticScaling", true );
  map_.setAsScalar( "Field-ArrowRatio", 0.01 );
  map_.setAsScalar( "Field-ArrowScaling", 1.0 );
  map_.setAsUnsignedInteger( "Field-LevelNumber", 30 );

  // SampleImplementation parameters
  map_.setAsString( "Sample-CommentMarkers", "#");
  map_.setAsUnsignedInteger( "Sample-PrintEllipsisSize", 3);
  map_.setAsUnsignedInteger( "Sample-PrintEllipsisThreshold", 1000);
  map_.setAsUnsignedInteger( "Sample-SmallKendallTau", 23 );

  // DomainImplementation parameters
  map_.setAsScalar( "Domain-SmallVolume",   1.0e-12 );

  // Mesh parameters
  map_.setAsBool( "Mesh-BackfaceCulling", false );
  map_.setAsScalar( "Mesh-AmbientFactor", 0.1 );
  map_.setAsScalar( "Mesh-DiffuseFactor", 0.7 );
  map_.setAsScalar( "Mesh-Shininess", 100.0 );
  map_.setAsScalar( "Mesh-SpecularFactor", 0.2 );
  map_.setAsScalar( "Mesh-VertexEpsilon", 1.0e-6 );
  map_.setAsUnsignedInteger( "Mesh-LargeSize", 5000 );

  // BoundingVolumeHierarchy parameters
  map_.setAsString( "BoundingVolumeHierarchy-Strategy", "Mean" );
  map_.setAsUnsignedInteger( "BoundingVolumeHierarchy-BinNumber", 50 );

  // EnclosingSimplexAlgorithm parameters
  map_.setAsUnsignedInteger( "EnclosingSimplexAlgorithm-LargeDimension", 5);

  // Matrix parameters
  map_.setAsUnsignedInteger( "Matrix-size-visible-in-str-from", 5 );

  // Tensor parameters
  map_.setAsUnsignedInteger( "Tensor-size-visible-in-str-from", 5 );

  // Tensor parameters
  map_.setAsUnsignedInteger( "ComplexTensor-size-visible-in-str-from", 6 );

  // MatrixImplementation parameters //
  map_.setAsScalar( "Matrix-DefaultSmallPivot", 1.0e-7  );
  map_.setAsScalar( "Matrix-SymmetryThreshold", 1.0e-12 );

  // BernsteinCopulaFactory parameters //
  map_.setAsScalar( "BernsteinCopulaFactory-alpha", 1.0 );
  map_.setAsString( "BernsteinCopulaFactory-BinNumberSelectionMethod", "LogLikelihood" );
  map_.setAsUnsignedInteger( "BernsteinCopulaFactory-kFraction", 2 );
  map_.setAsUnsignedInteger( "BernsteinCopulaFactory-MaxM", 1 );
  map_.setAsUnsignedInteger( "BernsteinCopulaFactory-MinM", 2 );
  map_.setAsUnsignedInteger( "BernsteinCopulaFactory-SamplingSize", 10000 );

  // BurrFactory parameters //
  map_.setAsScalar( "BurrFactory-AbsolutePrecision", 1.0e-12 );
  map_.setAsScalar( "BurrFactory-RelativePrecision", 1.0e-12 );
  map_.setAsScalar( "BurrFactory-ResidualPrecision", 1.0e-12 );
  map_.setAsUnsignedInteger( "BurrFactory-MaximumIteration", 10 );

  // ConditionalDistribution parameters //
  map_.setAsUnsignedInteger( "ConditionalDistribution-MarginalIntegrationNodesNumber", 256 );
  map_.setAsUnsignedInteger( "ConditionalDistribution-MaximumIntegrationNodesNumber", 100000 );

  // ComposedDistribution parameters //
  map_.setAsBool( "ComposedDistribution-UseGenericCovarianceAlgorithm", false);

  // CompositeDistribution parameters //
  map_.setAsScalar( "CompositeDistribution-SolverEpsilon", 1.0e-10 );
  map_.setAsUnsignedInteger( "CompositeDistribution-StepNumber", 256 );

  // Dirichlet parameters //
  map_.setAsUnsignedInteger( "Dirichlet-DefaultIntegrationSize", 50 );
  map_.setAsUnsignedInteger( "Dirichlet-DefaultSamplingSize", 500000 );

  // DirichletFactory parameters //
  map_.setAsScalar( "DirichletFactory-ParametersEpsilon", 1.0e-12 );
  map_.setAsUnsignedInteger( "DirichletFactory-MaximumIteration", 10 );

  // DiscreteCompoundDistribution parameters //
  map_.setAsScalar( "DiscreteCompoundDistribution-DefaultEpsilon", 1.0e-12);

  // ExtremeValueCopula parameters //
  map_.setAsBool( "ExtremeValueCopula-CheckPickandFunction", true );
  map_.setAsUnsignedInteger( "ExtremeValueCopula-CheckGridSize", 11 );

  // FisherSnedecorFactory parameters //
  map_.setAsScalar( "FisherSnedecorFactory-D1LowerBound", 1.0e-2 );
  map_.setAsScalar( "FisherSnedecorFactory-D2LowerBound", 1.0e-2 );

  // FrechetFactory parameters //
  map_.setAsScalar( "FrechetFactory-BoundMargin", 10.0 );

  // GeneralizedExtremeValue parameters //
  map_.setAsScalar( "GeneralizedExtremeValue-XiThreshold", 1.0e-6 );

  // GeneralizedParetoFactory parameters //
  map_.setAsScalar( "GeneralizedParetoFactory-MaximumAbsoluteError", 1.0e-10 );
  map_.setAsScalar( "GeneralizedParetoFactory-MaximumConstraintError", 1.0e-10 );
  map_.setAsScalar( "GeneralizedParetoFactory-MaximumObjectiveError", 1.0e-10 );
  map_.setAsScalar( "GeneralizedParetoFactory-MaximumRelativeError", 1.0e-10 );
  map_.setAsUnsignedInteger( "GeneralizedParetoFactory-MaximumEvaluationNumber", 1000 );
  map_.setAsUnsignedInteger( "GeneralizedParetoFactory-SmallSize", 20 );

  // InverseNormalFactory parameters //
  map_.setAsString( "InverseNormalFactory-Method", "MLE" );

  // KernelMixture parameters //
  map_.setAsUnsignedInteger( "KernelMixture-LargeSize", 20 );
  map_.setAsUnsignedInteger( "KernelMixture-PDFCDFDiscretization", 1000 );
  map_.setAsUnsignedInteger( "KernelMixture-SmallSize", 50 );

  // KernelSmoothing parameters //
  map_.setAsScalar( "KernelSmoothing-AbsolutePrecision", 0.0 );
  map_.setAsScalar( "KernelSmoothing-CutOffPlugin", 5.0 );
  map_.setAsScalar( "KernelSmoothing-RelativePrecision", 1.0e-5 );
  map_.setAsScalar( "KernelSmoothing-ResidualPrecision", 1.0e-10 );
  map_.setAsUnsignedInteger( "KernelSmoothing-BinNumber", 1024 );
  map_.setAsUnsignedInteger( "KernelSmoothing-MaximumIteration", 50 );
  map_.setAsUnsignedInteger( "KernelSmoothing-SmallSize", 250 );

  // LogNormal parameters //
  map_.setAsScalar( "LogNormal-CharacteristicFunctionSmallSigmaThreshold", 0.2 );
  map_.setAsUnsignedInteger( "LogNormal-CharacteristicFunctionIntegrationNodes", 256 );

  // LogNormalFactory parameters //
  map_.setAsScalar( "LogNormalFactory-AbsolutePrecision", 1.0e-12 );
  map_.setAsScalar( "LogNormalFactory-RelativePrecision", 1.0e-12 );
  map_.setAsScalar( "LogNormalFactory-ResidualPrecision", 1.0e-12 );
  map_.setAsUnsignedInteger( "LogNormalFactory-EstimationMethod", 0 );
  map_.setAsUnsignedInteger( "LogNormalFactory-MaximumIteration", 50 );

  // Meixner parameters //
  map_.setAsScalar( "MeixnerDistribution-MaximumAbsoluteError", 1.0e-12 );
  map_.setAsScalar( "MeixnerDistribution-MaximumConstraintError", 1.0e-12 );
  map_.setAsScalar( "MeixnerDistribution-MaximumObjectiveError", 1.0e-12 );
  map_.setAsScalar( "MeixnerDistribution-MaximumRelativeError", 1.0e-12 );
  map_.setAsUnsignedInteger( "MeixnerDistribution-CDFDiscretization", 10000 );
  map_.setAsUnsignedInteger( "MeixnerDistribution-CDFIntegrationNodesNumber", 32 );

  // Mixture parameters //
  map_.setAsScalar( "Mixture-SmallWeight", 1.0e-12 );
  map_.setAsUnsignedInteger( "Mixture-LargeSize", 20 );
  map_.setAsUnsignedInteger( "Mixture-PDFCDFDiscretization", 1000 );
  map_.setAsUnsignedInteger( "Mixture-SmallSize", 50 );

  // Multinomial parameters //
  map_.setAsScalar( "Multinomial-eta", 1.0e-9 );
  map_.setAsScalar( "Multinomial-smallA", 10.0 );

  // NegativeBinomialFactory parameters //
  map_.setAsScalar( "NegativeBinomialFactory-AbsolutePrecision", 1.0e-12 );
  map_.setAsScalar( "NegativeBinomialFactory-RelativePrecision", 1.0e-12 );
  map_.setAsScalar( "NegativeBinomialFactory-ResidualPrecision", 1.0e-12 );
  map_.setAsUnsignedInteger( "NegativeBinomialFactory-MaximumIteration", 50 );

  // Normal parameters //
  map_.setAsScalar( "Normal-MaximumCDFEpsilon", 5.0e-6 );
  map_.setAsScalar( "Normal-MinimumCDFEpsilon", 5.0e-2 );
  map_.setAsUnsignedInteger( "Normal-MarginalIntegrationNodesNumber", 16 );
  map_.setAsUnsignedInteger( "Normal-MaximumNumberOfPoints", 10000000 );
  map_.setAsUnsignedInteger( "Normal-MinimumNumberOfPoints", 100000 );
  map_.setAsUnsignedInteger( "Normal-SmallDimension", 6 );

  // ParetoFactory parameters //
  map_.setAsScalar( "ParetoFactory-AlphaUpperBound", 1000.0 );

  // ProductDistribution parameters //
  map_.setAsScalar( "ProductDistribution-LargeCharacteristicFunctionArgument", 10.0 );

  // RiceFactory parameters //
  map_.setAsScalar( "RiceFactory-AbsolutePrecision", 1.0e-12 );
  map_.setAsScalar( "RiceFactory-RelativePrecision", 1.0e-12 );
  map_.setAsScalar( "RiceFactory-ResidualPrecision", 1.0e-12 );
  map_.setAsUnsignedInteger( "RiceFactory-MaximumIteration", 10 );

  // TrapezoidalFactory parameters //
  map_.setAsScalar( "TrapezoidalFactory-RhoBeg", 0.1 );
  map_.setAsScalar( "TrapezoidalFactory-RhoEnd", 1.0e-5 );
  map_.setAsUnsignedInteger( "TrapezoidalFactory-MaximumIteration", 2000 );

  // TruncatedDistribution parameters //
  map_.setAsScalar( "TruncatedDistribution-DefaultThresholdRealization", 0.5 );

  // TruncatedNormalFactory parameters //
  map_.setAsScalar( "TruncatedNormalFactory-SigmaLowerBound", 1.0e-4 );

  // MaximumLikelihoodFactory parameters //
  map_.setAsScalar( "MaximumLikelihoodFactory-MaximumAbsoluteError", 1.0e-10 );
  map_.setAsScalar( "MaximumLikelihoodFactory-MaximumConstraintError", 1.0e-10 );
  map_.setAsScalar( "MaximumLikelihoodFactory-MaximumObjectiveError", 1.0e-10 );
  map_.setAsScalar( "MaximumLikelihoodFactory-MaximumRelativeError", 1.0e-10 );
  map_.setAsString( "MaximumLikelihoodFactory-DefaultOptimizationAlgorithm", "TNC" );
  map_.setAsUnsignedInteger( "MaximumLikelihoodFactory-MaximumEvaluationNumber", 1000 );

  // MethodOfMomentsFactory parameters //
  map_.setAsScalar( "MethodOfMomentsFactory-MaximumAbsoluteError", 1.0e-10 );
  map_.setAsScalar( "MethodOfMomentsFactory-MaximumConstraintError", 1.0e-10 );
  map_.setAsScalar( "MethodOfMomentsFactory-MaximumObjectiveError", 1.0e-10 );
  map_.setAsScalar( "MethodOfMomentsFactory-MaximumRelativeError", 1.0e-10 );
  map_.setAsUnsignedInteger( "MethodOfMomentsFactory-MaximumEvaluationNumber", 1000 );

  // Student parameters //
  map_.setAsScalar( "Student-MaximumCDFEpsilon", 5.0e-6 );
  map_.setAsScalar( "Student-MinimumCDFEpsilon", 5.0e-2 );
  map_.setAsUnsignedInteger( "Student-MarginalIntegrationNodesNumber", 16 );
  map_.setAsUnsignedInteger( "Student-MaximumNumberOfPoints", 10000000 );
  map_.setAsUnsignedInteger( "Student-MinimumNumberOfPoints", 100000 );
  map_.setAsUnsignedInteger( "Student-SmallDimension", 6 );

  // StudentFactory parameters //
  map_.setAsScalar( "StudentFactory-NuMax", 1.0e3 );

  // NonCentralStudent parameters //
  map_.setAsUnsignedInteger( "NonCentralStudent-CDFAlgo", 0 );

  // UserDefined parameters //
  map_.setAsUnsignedInteger( "UserDefined-SmallSize", 10000 );

  // AliMikhailHaqCopulaFactory parameters //
  map_.setAsScalar( "AliMikhailHaqCopulaFactory-ThetaEpsilon", 1.0e-14 );

  // FrankCopulaFactory parameters //
  map_.setAsScalar( "FrankCopulaFactory-AbsolutePrecision", 1.0e-14 );
  map_.setAsScalar( "FrankCopulaFactory-RelativePrecision", 1.0e-14 );
  map_.setAsScalar( "FrankCopulaFactory-ResidualPrecision", 1.0e-14 );
  map_.setAsUnsignedInteger( "FrankCopulaFactory-MaximumIteration", 100 );

  // RandomMixture parameters //
  map_.setAsBool( "RandomMixture-SimplifyAtoms", true );
  map_.setAsScalar( "RandomMixture-DefaultAlpha", 5.0 );
  map_.setAsScalar( "RandomMixture-DefaultBeta", 8.5 );
  map_.setAsScalar( "RandomMixture-DefaultCDFEpsilon", 1.0e-10 );
  map_.setAsScalar( "RandomMixture-DefaultPDFEpsilon", 1.0e-10 );
  map_.setAsScalar( "RandomMixture-GraphCDFEpsilon", 1.0e-5 );
  map_.setAsScalar( "RandomMixture-GraphPDFEpsilon", 1.0e-5 );
  map_.setAsUnsignedInteger( "RandomMixture-DefaultBlockMax", 16 );
  map_.setAsUnsignedInteger( "RandomMixture-DefaultBlockMin", 3 );
  map_.setAsUnsignedInteger( "RandomMixture-DefaultMaxSize", 65536 );
  map_.setAsUnsignedInteger( "RandomMixture-MarginalIntegrationNodesNumber", 128 );
  map_.setAsUnsignedInteger( "RandomMixture-MaximumIntegrationNodesNumber", 1024 );
  map_.setAsUnsignedInteger( "RandomMixture-MaximumSupportSize", 2048 );
  map_.setAsUnsignedInteger( "RandomMixture-ProjectionDefaultSize", 25 );
  map_.setAsUnsignedInteger( "RandomMixture-SmallSize", 100 );

  // NumericalMathEvaluation parameters //
  map_.setAsScalar( "NumericalMathEvaluation-ParameterEpsilon", 1.0e-7 );
  map_.setAsUnsignedInteger( "NumericalMathEvaluation-DefaultPointNumber", 129 );

  // DualLinearCombinationEvaluation //
  map_.setAsScalar( "DualLinearCombinationEvaluation-SmallCoefficient", 0.0);

  // LinearCombinationEvaluation //
  map_.setAsScalar( "LinearCombinationEvaluation-SmallCoefficient", 0.0);

  // DistFunc parameters //
  map_.setAsScalar( "DistFunc-Precision", 1.0e-14 );
  map_.setAsUnsignedInteger( "DistFunc-MaximumIteration", 5000 );

  // KFactor parameters //
  map_.setAsScalar( "KFactor-Precision", 1.0e-8 );
  map_.setAsUnsignedInteger( "KFactor-DefaultIntegrationNodesNumber", 256 );
  map_.setAsUnsignedInteger( "KFactor-MaximumIteration", 32 );

  // RootStrategyImplementation parameters //
  map_.setAsScalar( "RootStrategy-DefaultMaximumDistance", 8.0 );
  map_.setAsScalar( "RootStrategy-DefaultStepSize", 1.0 );

  // SimulationAlgorithm parameters //
  map_.setAsScalar( "SimulationAlgorithm-DefaultMaximumCoefficientOfVariation", 1.0e-1 );
  map_.setAsScalar( "SimulationAlgorithm-DefaultMaximumStandardDeviation", 0.0 );
  map_.setAsUnsignedInteger( "SimulationAlgorithm-DefaultBlockSize", 1 );
  map_.setAsUnsignedInteger( "SimulationAlgorithm-DefaultMaximumOuterSampling", 1000 );

  // ProbabilitySimulationResult parameters //
  map_.setAsBool( "ProbabilitySimulationResult-CheckPositiveVariance", false );
  map_.setAsScalar( "ProbabilitySimulationResult-DefaultConfidenceLevel", 0.95 );

  // ExpectationSimulationAlgorithm parameters //
  map_.setAsString( "ExpectationSimulationAlgorithm-DefaultCoefficientOfVariationCriterionType", "MAX");
  map_.setAsString( "ExpectationSimulationAlgorithm-DefaultStandardDeviationCriterionType", "NONE");

  // SobolSimulationAlgorithm parameters //
  map_.setAsScalar( "SobolSimulationAlgorithm-DefaultIndexQuantileLevel", 0.05 );
  map_.setAsScalar( "SobolSimulationAlgorithm-DefaultIndexQuantileEpsilon", 1e-2 );
  map_.setAsUnsignedInteger( "SobolSimulationAlgorithm-DefaultBlockSize", 1000 );
  map_.setAsUnsignedInteger( "SobolSimulationAlgorithm-DefaultBatchSize", 1 );

  // SimulationSensitivityAnalysis parameters //
  map_.setAsUnsignedInteger( "SimulationSensitivityAnalysis-DefaultSampleMargin", 400 );

  // SubsetSampling parameters //
  map_.setAsScalar( "SubsetSampling-DefaultBetaMin", 2.0 );
  map_.setAsScalar( "SubsetSampling-DefaultConditionalProbability", 0.1 );
  map_.setAsScalar( "SubsetSampling-DefaultProposalRange", 2.0 );
  map_.setAsUnsignedInteger( "SubsetSampling-DefaultMaximumOuterSampling", 10000 );

  // DirectionalSampling parameters //
  map_.setAsUnsignedInteger( "DirectionalSampling-MeanContributionIntegrationNodesNumber", 255 );

  // AdaptiveDirectionalSampling parameters //
  map_.setAsScalar( "AdaptiveDirectionalSampling-DefaultGamma", 0.5 );
  map_.setAsUnsignedInteger( "AdaptiveDirectionalSampling-DefaultMaximumStratificationDimension", 3 );
  map_.setAsUnsignedInteger( "AdaptiveDirectionalSampling-DefaultNumberOfSteps", 2 );

  // AnalyticalResult parameters //
  map_.setAsScalar( "AnalyticalResult-DefaultWidth", 1.0 );
  map_.setAsUnsignedInteger( "AnalyticalResult-MeanPointIntegrationNodesNumber", 255 );

  // SystemFORM parameters //
  setAsScalar( "SystemFORM-MaximalScaling", 1.0e5 );
  setAsScalar( "SystemFORM-StartingScaling", 1.0e-13 );

  // MultiFORM parameters //
  setAsScalar( "MultiFORM-DefaultGamma", 1.1 );
  setAsScalar( "MultiFORM-DefaultDelta", 0.75 );
  setAsUnsignedInteger( "MultiFORM-DefaultMaximumNumberOfDesignPoints", 4 );

  // StrongMaximumTest parameters //
  map_.setAsScalar( "StrongMaximumTest-DefaultDeltaPrecision", 1.0e-7 );
  map_.setAsScalar( "StrongMaximumTest-Epsilon", 1.0e-10 );

  // CleaningStrategy parameters //
  map_.setAsScalar( "CleaningStrategy-DefaultSignificanceFactor", 1.0e-4 );
  map_.setAsUnsignedInteger( "CleaningStrategy-DefaultMaximumSize", 20 );

  // FunctionalChaosAlgorithm parameters //
  map_.setAsScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual", 1.0e-6 );
  map_.setAsScalar( "FunctionalChaosAlgorithm-PValueThreshold", 1.0e-3);
  map_.setAsScalar( "FunctionalChaosAlgorithm-QNorm", 0.5);
  map_.setAsUnsignedInteger( "FunctionalChaosAlgorithm-LargeSampleSize", 10000);
  map_.setAsUnsignedInteger( "FunctionalChaosAlgorithm-MaximumTotalDegree", 10 );
  map_.setAsUnsignedInteger( "FunctionalChaosAlgorithm-SmallSampleSize", 1000);

  // FunctionalChaosSobolIndices parameters //
  map_.setAsScalar( "FunctionalChaosSobolIndices-VariancePartThreshold", 1.0e-2);

  // LinearModelAlgorithm parameters //
  map_.setAsString("LinearModelAlgorithm-DecompositionMethod", "QR");
  map_.setAsUnsignedInteger("LinearModelAnalysis-Identifiers", 3);

  // GeneralLinearModelAlgorithm parameters //
  map_.setAsBool( "GeneralLinearModelAlgorithm-KeepCovariance", true);
  map_.setAsBool( "GeneralLinearModelAlgorithm-NormalizeData", false);
  map_.setAsBool( "GeneralLinearModelAlgorithm-OptimizeParameters", true);
  map_.setAsBool( "GeneralLinearModelAlgorithm-UnbiasedVariance", true);
  map_.setAsBool( "GeneralLinearModelAlgorithm-UseAnalyticalAmplitudeEstimate", true);
  map_.setAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationLowerBound", 1.0e-2 );
  map_.setAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationScaleFactor", 2.0 );
  map_.setAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationUpperBound", 1.0e2 );
  map_.setAsScalar( "GeneralLinearModelAlgorithm-MaximalScaling", 1.0e5 );
  map_.setAsScalar( "GeneralLinearModelAlgorithm-MeanEpsilon", 1.0e-12 );
  map_.setAsScalar( "GeneralLinearModelAlgorithm-StartingScaling", 1.0e-13 );
  map_.setAsString( "GeneralLinearModelAlgorithm-DefaultOptimizationAlgorithm", "TNC");
  map_.setAsString( "GeneralLinearModelAlgorithm-LinearAlgebra", "LAPACK");

  // KrigingAlgorithm parameters //
  map_.setAsScalar( "KrigingAlgorithm-MaximalScaling", 1.0e5 );
  map_.setAsScalar( "KrigingAlgorithm-StartingScaling", 1.0e-13 );
  map_.setAsString( "KrigingAlgorithm-LinearAlgebra", "LAPACK");

  // SquaredExponential parameters //
  map_.setAsScalar( "SquaredExponential-DefaultTheta", 1.0 );

  // AbsoluteExponential parameters //
  map_.setAsScalar( "AbsoluteExponential-DefaultTheta", 1.0 );

  // GeneralizedExponential parameters //
  map_.setAsScalar( "GeneralizedExponential-DefaultTheta", 1.0 );

  // MaternModel parameters //
  map_.setAsScalar( "MaternModel-DefaultNu", 1.5 );
  map_.setAsScalar( "MaternModel-DefaultTheta", 1.0 );

  // WeightedExperimentImplementation parameters //
  map_.setAsUnsignedInteger( "WeightedExperiment-DefaultSize", 100 );

  // GaussProductExperiment parameters //
  map_.setAsUnsignedInteger( "GaussProductExperiment-DefaultMarginalDegree", 5 );

  // HyperbolicAnisotropicEnumerateFunction parameters //
  map_.setAsScalar( "HyperbolicAnisotropicEnumerateFunction-DefaultQ", 0.4 );

  // MarginalTransformationEvaluation parameters //
  map_.setAsBool( "MarginalTransformationEvaluation-Simplify", true );
  map_.setAsScalar( "MarginalTransformationEvaluation-DefaultTailThreshold", 0.99 );
  map_.setAsScalar( "MarginalTransformationEvaluation-ParametersEpsilon", 1.0e-14 );

  // DistributionImplementation parameters //
  map_.setAsBool( "Distribution-MinimumVolumeLevelSetBySampling", false );
  map_.setAsBool( "Distribution-Parallel", true);
  map_.setAsBool( "Distribution-ScaleColorsDiscretePDF", true);
  map_.setAsBool( "Distribution-ShowSupportDiscretePDF", true);
  map_.setAsBool( "Distribution-UseCovarianceAdaptiveAlgorithm", true );
  map_.setAsScalar( "Distribution-QMin", 0.15 );
  map_.setAsScalar( "Distribution-QMax", 0.85 );
  map_.setAsScalar( "Distribution-DefaultQuantileEpsilon", 1.0e-12 );
  map_.setAsScalar( "Distribution-DefaultPDFEpsilon", 1.0e-14 );
  map_.setAsScalar( "Distribution-DefaultCDFEpsilon", 1.0e-14 );
  map_.setAsScalar( "Distribution-DiscreteDrawPDFScaling", 0.25 );
  map_.setAsUnsignedInteger( "Distribution-CharacteristicFunctionBlockMax", 20 );
  map_.setAsUnsignedInteger( "Distribution-CharacteristicFunctionNMax", 1000000 );
  map_.setAsUnsignedInteger( "Distribution-DefaultIntegrationNodesNumber", 255 );
  map_.setAsUnsignedInteger( "Distribution-DefaultLevelNumber", 10 );
  map_.setAsUnsignedInteger( "Distribution-DefaultPointNumber", 129 );
  map_.setAsUnsignedInteger( "Distribution-DefaultQuantileCacheSize", 128 );
  map_.setAsUnsignedInteger( "Distribution-DefaultQuantileIteration", 100 );
  map_.setAsUnsignedInteger( "Distribution-EntropySamplingSize", 1048576 );
  map_.setAsUnsignedInteger( "Distribution-MinimumVolumeLevelSetSamplingSize", 1048576 );
  map_.setAsUnsignedInteger( "Distribution-SmallDimensionEntropy", 3 );

  // ContinuousDistribution parameters //
  map_.setAsUnsignedInteger( "ContinuousDistribution-DefaultIntegrationNodesNumber", 256 );

  // DiscreteDistribution parameters //
  map_.setAsScalar( "DiscreteDistribution-SupportEpsilon", 1.0e-14 );
  
  // DiscreteMarkovChain parameters //
  map_.setAsString( "DiscreteMarkovChain-DOTArcColor", "black");
  map_.setAsString( "DiscreteMarkovChain-DOTLayout", "dot");
  map_.setAsString( "DiscreteMarkovChain-DOTNodeColor", "black");
  map_.setAsString( "DiscreteMarkovChain-DOTNodeShape", "oval");
  map_.setAsScalar( "DiscreteMarkovChain-ProbabilitySumPrecision", 1.0e-6 );  

  // DistributionFactoryImplementation parameters //
  map_.setAsScalar( "DistributionFactory-BootstrapErrorTolerance", 0.1 );
  map_.setAsUnsignedInteger( "DistributionFactory-DefaultBootstrapSize", 100 );

  // OrderStatisticsMarginalChecker //
  map_.setAsScalar( "OrderStatisticsMarginalChecker-OptimizationEpsilon", 1.0e-7 );
  map_.setAsUnsignedInteger( "OrderStatisticsMarginalChecker-QuantileIteration", 100 );

  // MaximumEntropyOrderStatisticsDistribution //
  map_.setAsBool( "MaximumEntropyOrderStatisticsDistribution-CheckMarginals", true );
  map_.setAsBool( "MaximumEntropyOrderStatisticsDistribution-UseApproximation", true );
  map_.setAsScalar( "MaximumEntropyOrderStatisticsDistribution-SupportShift", 1.0e-15 );
  map_.setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-CDFIntegrationNodesNumber", 16);
  map_.setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-ExponentialFactorDiscretization", 100 );
  map_.setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-MaximumApproximationSubdivision", 2 );
  map_.setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-MaximumQuantileIteration", 10);

  // HMatrix parameters //
  map_.setAsBool( "HMatrix-ForceSequential", false);
  map_.setAsScalar( "HMatrix-AdmissibilityFactor", 2.0 );
  map_.setAsScalar( "HMatrix-AssemblyEpsilon", 1.0e-4);
  map_.setAsScalar( "HMatrix-RecompressionEpsilon", 1.0e-4);
  map_.setAsScalar( "HMatrix-ValidationError", 0.0);
  map_.setAsString( "HMatrix-ClusteringAlgorithm", "median" );
  map_.setAsUnsignedInteger( "HMatrix-CompressionMethod", 1 );
  map_.setAsUnsignedInteger( "HMatrix-MaxLeafSize", 100);
  map_.setAsUnsignedInteger( "HMatrix-ValidationDump", 0);
  map_.setAsUnsignedInteger( "HMatrix-ValidationRerun", 0);

  // GaussianProcess parameters //
  map_.setAsScalar( "GaussianProcess-MaximalScaling", 1.0e5 );
  map_.setAsScalar( "GaussianProcess-StartingScaling", 1.0e-13 );
  map_.setAsUnsignedInteger( "GaussianProcess-GibbsMaximumIteration", 100 );

  // SpectralGaussianProcess parameters //
  map_.setAsScalar( "SpectralGaussianProcess-StartingScaling", 1.0e-13 );
  map_.setAsScalar( "SpectralGaussianProcess-MaximalScaling", 1.0e5 );
  map_.setAsUnsignedInteger( "SpectralGaussianProcess-CholeskyCacheSize", 16384 );

  // WhittleFactory parameters //
  map_.setAsScalar( "WhittleFactory-DefaultRhoBeg", 0.1 );
  map_.setAsScalar( "WhittleFactory-DefaultRhoEnd", 1.0e-10 );
  map_.setAsScalar( "WhittleFactory-DefaultStartingPointScale", 1.0 );
  map_.setAsScalar( "WhittleFactory-RootEpsilon", 1.0e-6);
  map_.setAsUnsignedInteger( "WhittleFactory-DefaultMaxFun", 2000 );

  // BoxCoxFactory parameters //
  map_.setAsScalar( "BoxCoxFactory-DefaultRhoBeg", 0.1 );
  map_.setAsScalar( "BoxCoxFactory-DefaultRhoEnd", 1.0e-10 );
  map_.setAsUnsignedInteger( "BoxCoxFactory-DefaultMaxFun", 2000 );
  map_.setAsUnsignedInteger( "BoxCoxFactory-DefaultPointNumber", 201 );

  // VisualTest parameters //
  map_.setAsUnsignedInteger( "VisualTest-KendallPlot-MonteCarloSize", 100 );

  // CalibrationStrategyImplementation parameters //
  map_.setAsScalar( "CalibrationStrategy-DefaultExpansionFactor", 1.2 );
  map_.setAsScalar( "CalibrationStrategy-DefaultLowerBound", 0.117 ); // = 0.5 * 0.234
  map_.setAsScalar( "CalibrationStrategy-DefaultShrinkFactor", 0.8 );
  map_.setAsScalar( "CalibrationStrategy-DefaultUpperBound", 0.468 ); // = 2.0 * 0.234
  map_.setAsUnsignedInteger( "CalibrationStrategy-DefaultCalibrationStep", 100 );

  // MCMC parameters //
  map_.setAsUnsignedInteger( "MCMC-DefaultBurnIn", 0 );
  map_.setAsUnsignedInteger( "MCMC-DefaultThinning", 1 );

  // GaussianLinearCalibration parameters //
  map_.setAsString( "GaussianLinearCalibration-Method", "QR" );

  // LinearLeastSquaresCalibration parameters //
  map_.setAsScalar( "LinearLeastSquaresCalibration-Regularization", 1e-12 );
  map_.setAsString( "LinearLeastSquaresCalibration-Method", "QR" );

  // NonLinearLeastSquaresCalibration parameters //
  map_.setAsUnsignedInteger( "NonLinearLeastSquaresCalibration-BootstrapSize", 100 );
  map_.setAsUnsignedInteger( "NonLinearLeastSquaresCalibration-MultiStartSize", 100 );

  // GaussianNonLinearCalibration parameters //
  map_.setAsUnsignedInteger( "GaussianNonLinearCalibration-BootstrapSize", 100 );
  map_.setAsUnsignedInteger( "GaussianNonLinearCalibration-MultiStartSize", 100 );

  // ARMA parameters //
  map_.setAsScalar( "ARMA-MeanEpsilon", 1.0e-14 );

  // ARMALikelihoodFactory parameters //
  map_.setAsScalar( "ARMALikelihoodFactory-DefaultRhoBeg", 0.01 );
  map_.setAsScalar( "ARMALikelihoodFactory-DefaultRhoEnd", 1.0e-10 );
  map_.setAsScalar( "ARMALikelihoodFactory-DefaultStartingPointScale", 1.0 );
  map_.setAsScalar( "ARMALikelihoodFactory-MaximalScaling", 1.0e5 );
  map_.setAsScalar( "ARMALikelihoodFactory-RootEpsilon", 1.0e-6);
  map_.setAsScalar( "ARMALikelihoodFactory-StartingScaling", 1.0e-13 );
  map_.setAsUnsignedInteger( "ARMALikelihoodFactory-DefaultMaxFun", 10000 );

  // FittingTest parameters //
  map_.setAsBool( "FittingTest-ChiSquaredCheckSample", true );
  map_.setAsUnsignedInteger( "FittingTest-ChiSquaredMinimumBinCount", 5 );
  map_.setAsUnsignedInteger( "FittingTest-KolmogorovSamplingSize", 10 );

  // LeastSquaresMetaModelSelection parameters //
  map_.setAsScalar( "LeastSquaresMetaModelSelection-ErrorThreshold", 0.0 );
  map_.setAsScalar( "LeastSquaresMetaModelSelection-MaximumErrorFactor", 2.0 );
  map_.setAsString( "LeastSquaresMetaModelSelection-DecompositionMethod", "SVD" );

  // SparseMethod parameters //
  map_.setAsScalar( "SparseMethod-ErrorThreshold", 1.0e-3 );
  map_.setAsScalar( "SparseMethod-MaximumErrorFactor", 2.0 );

  // CholeskyMethod parameters //
  map_.setAsUnsignedInteger( "CholeskyMethod-LargeCase", 128 );

  // Classifier parameters //
  map_.setAsBool( "Classifier-Parallel", true );

  // TensorApproximationAlgorithm parameters //
  map_.setAsScalar( "TensorApproximationAlgorithm-DefaultMaximumRadiusError", 1.0e-5 );
  map_.setAsScalar( "TensorApproximationAlgorithm-DefaultMaximumResidualError", 1.0e-5 );
  map_.setAsString( "TensorApproximationAlgorithm-DecompositionMethod", "SVD" );
  map_.setAsString( "TensorApproximationAlgorithm-Method", "GreedyRankOne" );
  map_.setAsUnsignedInteger( "TensorApproximationAlgorithm-DefaultMaximumAlternatingLeastSquaresIteration", 100 );

  // viewer.View parameters //
  map_.setAsString( "View-ImageFormat", "png" );
}

void ResourceMap::reload()
{
  loadDefaultConfiguration();
  loadConfigurationFile();
}

/* String converter */
String ResourceMap::__repr__() const
{
  OSS oss;
  oss << map_;
  return oss;
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
