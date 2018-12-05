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
  return GetInstance().lock().getKeys();
}

std::vector<String> ResourceMap::GetStringKeys()
{
  return GetInstance().lock().getStringKeys();
}

std::vector<String> ResourceMap::GetScalarKeys()
{
  return GetInstance().lock().getScalarKeys();
}

std::vector<String> ResourceMap::GetUnsignedIntegerKeys()
{
  return GetInstance().lock().getUnsignedIntegerKeys();
}

std::vector<String> ResourceMap::GetBoolKeys()
{
  return GetInstance().lock().getBoolKeys();
}

/* Get a value in the map */
String ResourceMap::GetType(const String & key)
{
  return GetInstance().lock().getType( key );
}

/* Get a value in the map */
String ResourceMap::Get(const String & key)
{
  return GetInstance().lock().get( key );
}

String ResourceMap::GetAsString(const String & key)
{
  return GetInstance().lock().getAsString( key );
}

Bool ResourceMap::GetAsBool(const String & key)
{
  return GetInstance().lock().getAsBool( key );
}

UnsignedInteger ResourceMap::GetAsUnsignedInteger(const String & key)
{
  return GetInstance().lock().getAsUnsignedInteger( key );
}

Scalar ResourceMap::GetAsScalar(const String & key)
{
  return GetInstance().lock().getAsScalar( key );
}

UnsignedInteger ResourceMap::GetSize()
{
  return GetInstance().lock().getSize();
}

UnsignedInteger ResourceMap::GetStringSize()
{
  return GetInstance().lock().getStringSize();
}

UnsignedInteger ResourceMap::GetScalarSize()
{
  return GetInstance().lock().getScalarSize();
}

UnsignedInteger ResourceMap::GetUnsignedIntegerSize()
{
  return GetInstance().lock().getUnsignedIntegerSize();
}

UnsignedInteger ResourceMap::GetBoolSize()
{
  return GetInstance().lock().getBoolSize();
}

/* Set a value in the map */
void ResourceMap::Set(const String & key, const String & value)
{
  GetInstance().lock().set( key, value );
}

void ResourceMap::SetAsString(const String & key, const String & value)
{
  GetInstance().lock().setAsString( key, value );
}

void ResourceMap::SetAsBool(const String & key, Bool value)
{
  GetInstance().lock().setAsBool( key, value );
}

void ResourceMap::SetAsUnsignedInteger(const String & key, UnsignedInteger value)
{
  GetInstance().lock().setAsUnsignedInteger( key, value );
}

void ResourceMap::SetAsScalar(const String & key, Scalar value)
{
  GetInstance().lock().setAsScalar( key, value );
}

/* Default constructor */
ResourceMap::ResourceMap()
  : mapString_()
  , mapScalar_()
  , mapUnsignedInteger_()
  , mapBool_()
{
  loadDefaultConfiguration();
  loadConfigurationFile();
}

/* Method for retrieving information from the resource map */
String ResourceMap::getType(const String & key) const
{
  // First, try to retrieve the value from the String map
  {
    Collection<String> keys;
    for (MapStringType::const_iterator it = mapString_.begin(); it != mapString_.end(); ++it)
    {
      keys.add(it->first);
    }
    MapStringType::const_iterator it = mapString_.find(key);
    if (it != mapString_.end()) return "string";
  }
  // Second, try to retrieve the value from the Scalar map
  {
    Collection<String> keys;
    for (MapScalarType::const_iterator it = mapScalar_.begin(); it != mapScalar_.end(); ++it)
    {
      keys.add(it->first);
    }
    MapScalarType::const_iterator it = mapScalar_.find(key);
    if (it != mapScalar_.end()) return "float";
  }
  // Third, try to retrieve the value from the UnsignedInteger map
  {
    Collection<String> keys;
    for (MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.begin(); it != mapUnsignedInteger_.end(); ++it)
    {
      keys.add(it->first);
    }
    MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.find(key);
    if (it != mapUnsignedInteger_.end()) return "unsigned int";
  }
  // Fourth, try to retrieve the value from the Bool map
  {
    Collection<String> keys;
    for (MapBoolType::const_iterator it = mapBool_.begin(); it != mapBool_.end(); ++it)
    {
      keys.add(it->first);
    }
    MapBoolType::const_iterator it = mapBool_.find(key);
    if (it != mapBool_.end()) return "bool";
  }
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap. Report Bug";
}

String ResourceMap::get(const String & key) const
{
  // First, try to retrieve the value from the String map
  {
    MapStringType::const_iterator it = mapString_.find(key);
    if (it != mapString_.end()) return it->second;
  }
  // Second, try to retrieve the value from the Scalar map
  {
    MapScalarType::const_iterator it = mapScalar_.find(key);
    if (it != mapScalar_.end()) return (OSS() << it->second);
  }
  // Third, try to retrieve the value from the UnsignedInteger map
  {
    MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.find(key);
    if (it != mapUnsignedInteger_.end()) return (OSS() << it->second);
  }
  // Fourth, try to retrieve the value from the Bool map
  {
    MapBoolType::const_iterator it = mapBool_.find(key);
    if (it != mapBool_.end()) return (OSS() << it->second);
  }
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap. Report Bug";
}

String ResourceMap::getAsString(const String & key) const
{
  MapStringType::const_iterator it = mapString_.find(key);
  if (it != mapString_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as a String. Report Bug";
}

Scalar ResourceMap::getAsScalar(const String & key) const
{
  MapScalarType::const_iterator it = mapScalar_.find(key);
  if (it != mapScalar_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as a Scalar. Report Bug";
}

UnsignedInteger ResourceMap::getAsUnsignedInteger(const String & key) const
{
  MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.find(key);
  if (it != mapUnsignedInteger_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as an UnsignedInteger. Report Bug";
}

Bool ResourceMap::getAsBool(const String & key) const
{
  MapBoolType::const_iterator it = mapBool_.find(key);
  if (it != mapBool_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as a Bool. Report Bug";
}

/* Get the size of the map */
UnsignedInteger ResourceMap::getSize() const
{
  return getStringSize() + getScalarSize() + getUnsignedIntegerSize() + getBoolSize();
}

UnsignedInteger ResourceMap::getStringSize() const
{
  return mapString_.size();
}

UnsignedInteger ResourceMap::getScalarSize() const
{
  return mapScalar_.size();
}

UnsignedInteger ResourceMap::getUnsignedIntegerSize() const
{
  return mapUnsignedInteger_.size();
}

UnsignedInteger ResourceMap::getBoolSize() const
{
  return mapBool_.size();
}

/* Method for setting information into the resource map */
void ResourceMap::set(const String & key, const String & value)
{
  // First, check if the given key is already defined in one of the maps
  // to convert it to the correct type
  // First, try to retrieve the value from the String map
  {
    MapStringType::const_iterator it = mapString_.find(key);
    if (it != mapString_.end())
    {
      mapString_[key] = value;
      return;
    }
  }
  // Second, try to retrieve the value from the Scalar map
  {
    MapScalarType::const_iterator it = mapScalar_.find(key);
    if (it != mapScalar_.end())
    {
      Scalar scalarValue = -1.0;
      std::istringstream iss( value );
      iss >> scalarValue;
      mapScalar_[ key ] = scalarValue;
      return;
    }
  }
  // Third, try to retrieve the value from the UnsignedInteger map
  {
    MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.find(key);
    if (it != mapUnsignedInteger_.end())
    {
      UnsignedInteger unsignedIntegerValue = 0;
      std::istringstream iss( value );
      iss >> unsignedIntegerValue;
      mapUnsignedInteger_[ key ] = unsignedIntegerValue;
      return;
    }
  }
  // Fourth, try to retrieve the value from the Bool map
  {
    MapBoolType::const_iterator it = mapBool_.find(key);
    if (it != mapBool_.end())
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
      mapBool_[ key ] = boolValue;
    }
  }
  // Second, set the new value as a string
  mapString_[ key ] = value;
}

void ResourceMap::setAsString(const String & key, const String & value)
{
  mapString_[ key ] = value;
}

void ResourceMap::setAsBool(const String & key, Bool value)
{
  mapBool_[ key ] = value;
}

void ResourceMap::setAsUnsignedInteger(const String & key, UnsignedInteger value)
{
  mapUnsignedInteger_[ key ] = value;
}

void ResourceMap::setAsScalar(const String & key, Scalar value)
{
  mapScalar_[ key ] = value;
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
        const String key = XML::ToString(current->name);
        // Try to get a String value
        {
          String value = XML::GetAttributeByName(current, XMLTag_value_str);
          if (value.size() > 0)
          {
            mapString_[ key ] = value;
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
            mapScalar_[ key ] = scalarValue;
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
            mapUnsignedInteger_[ key ] = unsignedIntegerValue;
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
            mapBool_[ key ] = boolValue;
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
  setAsString( "R-executable-command", R_EXECUTABLE );
#else
  setAsString( "R-executable-command", "" );
#endif
  setAsString( "csv-file-separator", ";" );
#ifndef WIN32
  setAsString( "temporary-directory", "/tmp" );
  setAsUnsignedInteger( "parallel-threads", sysconf(_SC_NPROCESSORS_CONF) );
#else
  setAsString( "temporary-directory", "TEMP" );
  UnsignedInteger numberOfProcessors = 0;
  std::istringstream iss( getenv("NUMBER_OF_PROCESSORS") );
  iss >> numberOfProcessors;
  setAsUnsignedInteger( "parallel-threads", numberOfProcessors );
#endif
  setAsUnsignedInteger( "cache-max-size", 1024 );
  setAsUnsignedInteger( "output-files-timeout", 2 );

  // Os parameters
  setAsBool( "Os-create-process", "false" );
  setAsBool( "Os-RemoveFiles", "true" );

  // XMLStorageManager parameters
  setAsUnsignedInteger( "XMLStorageManager-DefaultCompressionLevel", 0 );

  // Collection parameters
  setAsUnsignedInteger( "Collection-size-visible-in-str-from", 10 );

  // SpecFunc parameters
  setAsScalar( "SpecFunc-Precision", 2.0e-16 );
  setAsUnsignedInteger( "SpecFunc-MaximumIteration", 1000 );

  // SymbolicParser parameters
  setAsBool( "SymbolicParser-CheckResult", true );
  setAsString( "SymbolicParser-Backend", SYMBOLICPARSER_DEFAULT_BACKEND );

  // DesignProxy parameters
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
  setAsScalar( "Pie-LabelThreshold", 0.02 );
  setAsScalar( "Pie-VerticalMargin", 0.1 );

  // DrawableImplementation parameters //
  setAsScalar( "Drawable-DefaultLineWidth", 1.0 );
  setAsString( "Drawable-AlternativePattern", "S" );
  setAsString( "Drawable-DefaultColor", "blue" );
  setAsString( "Drawable-DefaultFillStyle", "solid" );
  setAsString( "Drawable-DefaultLineStyle", "solid" );
  setAsString( "Drawable-DefaultPattern", "s" );
  setAsString( "Drawable-DefaultPointStyle", "plus" );
  setAsString( "Drawable-DefaultSurfaceColor", "white" );
  setAsString( "Drawable-NoSpecifiedLabel", "" );
  setAsUnsignedInteger( "Drawable-DataThreshold", 2000 );
  setAsUnsignedInteger( "Drawable-DefaultPalettePhase", 12 );

  // GraphImplementation parameters //
  setAsScalar( "Graph-DefaultHorizontalMargin", 0.05 );
  setAsScalar( "Graph-DefaultLegendFontSize", 1.0 );
  setAsScalar( "Graph-DefaultVerticalMargin", 0.05 );
  setAsString( "Graph-NoSpecifiedLabel", "" );
  setAsUnsignedInteger( "Graph-DefaultHeight", 480 );
  setAsUnsignedInteger( "Graph-DefaultWidth", 640 );

  // Contour parameters //
  setAsUnsignedInteger( "Contour-DefaultLevelsNumber", 10 );

  // IntervalMesher parameters //
  setAsBool( "IntervalMesher-UseDiamond", false );

  // PointToPointEvaluation parameters //
  setAsUnsignedInteger( "PointToPointEvaluation-BlockSize", 256);

  // SQP parameters //
  setAsScalar( "SQP-DefaultOmega", 1.0e-4 );
  setAsScalar( "SQP-DefaultSmooth", 1.2 );
  setAsScalar( "SQP-DefaultTau", 0.5 );
  setAsUnsignedInteger ( "SQP-DefaultMaximumEvaluationNumber", 100000);

  // TNC parameters //
  setAsScalar( "TNC-DefaultAccuracy", 1.0e-4 );
  setAsScalar( "TNC-DefaultEta", 0.25 );
  setAsScalar( "TNC-DefaultFmin", 1.0 );
  setAsScalar( "TNC-DefaultRescale", 1.3 );
  setAsScalar( "TNC-DefaultStepmx", 10.0 );
  setAsUnsignedInteger( "TNC-DefaultMaxCGit", 50 );

  // AbdoRackwitz parameters //
  setAsScalar( "AbdoRackwitz-DefaultOmega", 1.0e-4 );
  setAsScalar( "AbdoRackwitz-DefaultSmooth", 1.2 );
  setAsScalar( "AbdoRackwitz-DefaultTau", 0.5 );
  setAsUnsignedInteger( "AbdoRackwitz-DefaultMaximumEvaluationNumber", 100000);

  // OptimizationAlgorithm parameters //
  setAsScalar( "OptimizationAlgorithm-DefaultLevelValue", 0.0 );
  setAsScalar( "OptimizationAlgorithm-DefaultMaximumAbsoluteError", 1.0e-5 );
  setAsScalar( "OptimizationAlgorithm-DefaultMaximumConstraintError", 1.0e-5 );
  setAsScalar( "OptimizationAlgorithm-DefaultMaximumRelativeError", 1.0e-5 );
  setAsScalar( "OptimizationAlgorithm-DefaultMaximumResidualError", 1.0e-5 );
  setAsUnsignedInteger( "OptimizationAlgorithm-DefaultMaximumEvaluationNumber", 100);
  setAsUnsignedInteger( "OptimizationAlgorithm-DefaultMaximumIterationNumber", 100 );

  // EfficientGlobalOptimization parameters //
  setAsScalar( "EfficientGlobalOptimization-DefaultAEITradeoff", 1.0);
  setAsScalar( "EfficientGlobalOptimization-DefaultCorrelationLengthFactor", 1.0);
  setAsScalar( "EfficientGlobalOptimization-DefaultImprovementFactor", 1.0);
  setAsUnsignedInteger( "EfficientGlobalOptimization-DefaultMultiStartExperimentSize", 100);
  setAsUnsignedInteger( "EfficientGlobalOptimization-DefaultMultiStartNumber", 20);
  setAsUnsignedInteger( "EfficientGlobalOptimization-DefaultParameterEstimationPeriod", 1);

  // Cobyla parameters //
  setAsScalar( "Cobyla-DefaultRhoBeg", 0.1 );

  // OPTpp parameters //
  setAsScalar("OPTpp-DefaultLineSearchTolerance", 1.0e-4);
  setAsScalar("OPTpp-DefaultMaximumStep", 1.0e3);
  setAsScalar("OPTpp-DefaultMinimumStep", 2.0e-16);
  setAsScalar("OPTpp-DefaultTrustRegionSize", 1.0e3);
  setAsUnsignedInteger("OPTpp-DefaultMaximumLineSearchIterationNumber", 5);

  // SolverImplementation parameters //
  setAsScalar( "Solver-DefaultAbsoluteError",  1.0e-5 );
  setAsScalar( "Solver-DefaultRelativeError",  1.0e-5 );
  setAsScalar( "Solver-DefaultResidualError",  1.0e-8 );
  setAsUnsignedInteger( "Solver-DefaultMaximumFunctionEvaluation", 100 );

  // GaussKronrod parameters //
  setAsScalar( "GaussKronrod-MaximumError",  1.0e-12 );
  setAsUnsignedInteger( "GaussKronrod-MaximumSubIntervals", 100 );

  // GaussLegendre parameters //
  setAsUnsignedInteger( "GaussLegendre-DefaultMarginalIntegrationPointsNumber", 64);

  // IteratedQuadrature parameters //
  setAsScalar( "IteratedQuadrature-MaximumError",    1.0e-7 );
  setAsUnsignedInteger( "IteratedQuadrature-MaximumSubIntervals", 32 );

  // Fehlberg parameters //
  setAsScalar( "Fehlberg-InitialStep", 1.0e-7);
  setAsScalar( "Fehlberg-LocalPrecision", 1.0e-10);
  setAsUnsignedInteger( "Fehlberg-DefaultOrder", 4);

  // KarhunenLoeveQuadratureAlgorithm parameters //
  setAsScalar( "KarhunenLoeveQuadratureAlgorithm-RegularizationFactor", 0.0);

  // KarhunenLoeveSVDAlgorithm parameters //
  setAsBool( "KarhunenLoeveSVDAlgorithm-UseRandomSVD", false);
  setAsString("KarhunenLoeveSVDAlgorithm-RandomSVDVariant", "Halko2010");
  setAsUnsignedInteger( "KarhunenLoeveSVDAlgorithm-Halko2011Margin", 2);
  setAsUnsignedInteger( "KarhunenLoeveSVDAlgorithm-Halko2011Iterations", 2);
  setAsUnsignedInteger( "KarhunenLoeveSVDAlgorithm-RandomSVDMaximumRank", 1000);

  // KarhunenLoeveP1Algorithm parameters //
  setAsScalar( "KarhunenLoeveP1Algorithm-RegularizationFactor", 0.0);

  // AdaptiveStieltjesAlgorithm parameters //
  setAsScalar( "AdaptiveStieltjesAlgorithm-MaximumError",  1.0e-12 );
  setAsUnsignedInteger( "AdaptiveStieltjesAlgorithm-MaximumSubIntervalsBetweenRoots", 64 );

  // LinearModelFactory parameters //
  setAsScalar( "LinearModelFactory-DefaultLevelValue", 0.95 );

  // LinearModelTest parameters //
  setAsScalar( "LinearModelTest-DefaultHarrisonMcCabeBreakpoint", 0.5 );
  setAsScalar( "LinearModelTest-DefaultLevel", 0.05 );
  setAsString( "LinearModelTest-DefaultDurbinWatsonHypothesis", "Equal" );
  setAsUnsignedInteger( "LinearModelTest-DefaultHarrisonMcCabeSimulationSize", 1000 );

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

  // SobolIndicesExperiment parameters //
  setAsString( "SobolIndicesExperiment-SamplingMethod", "MonteCarlo");

  // SobolIndicesAlgorithm parameters //
  setAsBool( "SobolIndicesAlgorithm-DefaultUseAsymptoticDistribution", false );
  setAsScalar( "SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel", 0.95 );
  setAsUnsignedInteger( "SobolIndicesAlgorithm-DefaultBlockSize", 1 );
  setAsUnsignedInteger( "SobolIndicesAlgorithm-DefaultBootstrapSize", 100 );

  // FAST parameters //
  setAsUnsignedInteger( "FAST-DefaultInterferenceFactor", 4 );
  setAsUnsignedInteger( "FAST-DefaultResamplingSize", 1 );

  // RandomGenerator parameters //
  setAsUnsignedInteger( "RandomGenerator-InitialSeed", 0 );

  // CovarianceModelImplementation parameters //
  setAsScalar( "CovarianceModel-DefaultNuggetFactor", 1e-12 );
  setAsScalar( "SpectralModel-DefaultTMax",  5.0 );
  setAsScalar( "SpectralModel-DefaultTMin", -5.0 );
  setAsUnsignedInteger( "SpectralModel-DefaultPointNumber",  129 );

  // SpectralModel parameters //
  setAsScalar( "SpectralModel-DefaultMaximumFrequency",  5.0 );
  setAsScalar( "SpectralModel-DefaultMinimumFrequency", -5.0 );
  setAsUnsignedInteger( "SpectralModel-DefaultFrequencyNumber",  129 );

  // FieldImplementation parameters //
  setAsScalar( "Field-ArrowRatio", 0.01 );
  setAsScalar( "Field-ArrowScaling", 1.0 );
  setAsUnsignedInteger( "Field-LevelNumber", 30 );

  // SampleImplementation parameters
  setAsString( "Sample-CommentMarkers", "#");
  setAsUnsignedInteger( "Sample-PrintEllipsisSize", 3);
  setAsUnsignedInteger( "Sample-PrintEllipsisThreshold", 1000);
  setAsUnsignedInteger( "Sample-SmallKendallTau", 23 );

  // DomainImplementation parameters
  setAsScalar( "Domain-SmallVolume",   1.0e-12 );

  // Mesh parameters
  setAsBool( "Mesh-BackfaceCulling", false );
  setAsScalar( "Mesh-AmbientFactor", 0.1 );
  setAsScalar( "Mesh-DiffuseFactor", 0.7 );
  setAsScalar( "Mesh-Shininess", 100.0 );
  setAsScalar( "Mesh-SpecularFactor", 0.2 );
  setAsScalar( "Mesh-VertexEpsilon", 1.0e-6 );
  setAsUnsignedInteger( "Mesh-LargeSize", 5000 );

  // BoundingVolumeHierarchy parameters
  setAsString( "BoundingVolumeHierarchy-Strategy", "Mean" );
  setAsUnsignedInteger( "BoundingVolumeHierarchy-BinNumber", 50 );

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
  setAsScalar( "BernsteinCopulaFactory-alpha", 1.0 );
  setAsString( "BernsteinCopulaFactory-BinNumberSelectionMethod", "LogLikelihood" );
  setAsUnsignedInteger( "BernsteinCopulaFactory-kFraction", 2 );
  setAsUnsignedInteger( "BernsteinCopulaFactory-MaxM", 1 );
  setAsUnsignedInteger( "BernsteinCopulaFactory-MinM", 2 );
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
  setAsScalar( "CompositeDistribution-SolverEpsilon", 1.0e-10 );
  setAsUnsignedInteger( "CompositeDistribution-StepNumber", 256 );

  // ComposedDistribution parameters //
  setAsBool( "ComposedDistribution-UseGenericCovarianceAlgorithm", false);

  // Dirichlet parameters //
  setAsUnsignedInteger( "Dirichlet-DefaultIntegrationSize", 50 );
  setAsUnsignedInteger( "Dirichlet-DefaultSamplingSize", 500000 );

  // DirichletFactory parameters //
  setAsScalar( "DirichletFactory-ParametersEpsilon", 1.0e-12 );
  setAsUnsignedInteger( "DirichletFactory-MaximumIteration", 10 );

  // ExtremeValueCopula parameters //
  setAsBool( "ExtremeValueCopula-CheckPickandFunction", true );
  setAsUnsignedInteger( "ExtremeValueCopula-CheckGridSize", 11 );

  // FisherSnedecorFactory parameters //
  setAsScalar( "FisherSnedecorFactory-D1LowerBound", 1.0e-2 );
  setAsScalar( "FisherSnedecorFactory-D2LowerBound", 1.0e-2 );

  // FrechetFactory parameters //
  setAsScalar( "FrechetFactory-BoundMargin", 10.0 );

  // GeneralizedExtremeValue parameters //
  setAsScalar( "GeneralizedExtremeValue-XiThreshold", 1.0e-6 );

  // GeneralizedParetoFactory parameters //
  setAsScalar( "GeneralizedParetoFactory-MaximumAbsoluteError", 1.0e-10 );
  setAsScalar( "GeneralizedParetoFactory-MaximumConstraintError", 1.0e-10 );
  setAsScalar( "GeneralizedParetoFactory-MaximumObjectiveError", 1.0e-10 );
  setAsScalar( "GeneralizedParetoFactory-MaximumRelativeError", 1.0e-10 );
  setAsUnsignedInteger( "GeneralizedParetoFactory-MaximumEvaluationNumber", 1000 );
  setAsUnsignedInteger( "GeneralizedParetoFactory-SmallSize", 20 );

  // InverseNormalFactory parameters //
  setAsString( "InverseNormalFactory-Method", "MLE" );

  // KernelMixture parameters //
  setAsUnsignedInteger( "KernelMixture-LargeSize", 20 );
  setAsUnsignedInteger( "KernelMixture-PDFCDFDiscretization", 1000 );
  setAsUnsignedInteger( "KernelMixture-SmallSize", 50 );

  // KernelSmoothing parameters //
  setAsScalar( "KernelSmoothing-AbsolutePrecision", 0.0 );
  setAsScalar( "KernelSmoothing-CutOffPlugin", 5.0 );
  setAsScalar( "KernelSmoothing-RelativePrecision", 1.0e-5 );
  setAsScalar( "KernelSmoothing-ResidualPrecision", 1.0e-10 );
  setAsUnsignedInteger( "KernelSmoothing-BinNumber", 1024 );
  setAsUnsignedInteger( "KernelSmoothing-MaximumIteration", 50 );
  setAsUnsignedInteger( "KernelSmoothing-SmallSize", 250 );

  // LogNormal parameters //
  setAsScalar( "LogNormal-CharacteristicFunctionSmallSigmaThreshold", 0.2 );
  setAsUnsignedInteger( "LogNormal-CharacteristicFunctionIntegrationNodes", 256 );

  // LogNormalFactory parameters //
  setAsScalar( "LogNormalFactory-AbsolutePrecision", 1.0e-12 );
  setAsScalar( "LogNormalFactory-RelativePrecision", 1.0e-12 );
  setAsScalar( "LogNormalFactory-ResidualPrecision", 1.0e-12 );
  setAsUnsignedInteger( "LogNormalFactory-EstimationMethod", 0 );
  setAsUnsignedInteger( "LogNormalFactory-MaximumIteration", 50 );

  // Meixner parameters //
  setAsScalar( "MeixnerDistribution-MaximumAbsoluteError", 1.0e-12 );
  setAsScalar( "MeixnerDistribution-MaximumConstraintError", 1.0e-12 );
  setAsScalar( "MeixnerDistribution-MaximumObjectiveError", 1.0e-12 );
  setAsScalar( "MeixnerDistribution-MaximumRelativeError", 1.0e-12 );
  setAsUnsignedInteger( "MeixnerDistribution-CDFDiscretization", 10000 );
  setAsUnsignedInteger( "MeixnerDistribution-CDFIntegrationNodesNumber", 32 );

  // Mixture parameters //
  setAsScalar( "Mixture-SmallWeight", 1.0e-12 );
  setAsUnsignedInteger( "Mixture-LargeSize", 20 );
  setAsUnsignedInteger( "Mixture-PDFCDFDiscretization", 1000 );
  setAsUnsignedInteger( "Mixture-SmallSize", 50 );

  // Multinomial parameters //
  setAsScalar( "Multinomial-eta", 1.0e-9 );
  setAsScalar( "Multinomial-smallA", 10.0 );

  // NegativeBinomialFactory parameters //
  setAsScalar( "NegativeBinomialFactory-AbsolutePrecision", 1.0e-12 );
  setAsScalar( "NegativeBinomialFactory-RelativePrecision", 1.0e-12 );
  setAsScalar( "NegativeBinomialFactory-ResidualPrecision", 1.0e-12 );
  setAsUnsignedInteger( "NegativeBinomialFactory-MaximumIteration", 50 );

  // Normal parameters //
  setAsScalar( "Normal-MaximumCDFEpsilon", 5.0e-6 );
  setAsScalar( "Normal-MinimumCDFEpsilon", 5.0e-2 );
  setAsUnsignedInteger( "Normal-MarginalIntegrationNodesNumber", 16 );
  setAsUnsignedInteger( "Normal-MaximumNumberOfPoints", 10000000 );
  setAsUnsignedInteger( "Normal-MinimumNumberOfPoints", 100000 );
  setAsUnsignedInteger( "Normal-SmallDimension", 6 );

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
  setAsScalar( "MaximumLikelihoodFactory-MaximumAbsoluteError", 1.0e-10 );
  setAsScalar( "MaximumLikelihoodFactory-MaximumConstraintError", 1.0e-10 );
  setAsScalar( "MaximumLikelihoodFactory-MaximumObjectiveError", 1.0e-10 );
  setAsScalar( "MaximumLikelihoodFactory-MaximumRelativeError", 1.0e-10 );
  setAsString( "MaximumLikelihoodFactory-DefaultOptimizationAlgorithm", "TNC" );
  setAsUnsignedInteger( "MaximumLikelihoodFactory-MaximumEvaluationNumber", 1000 );

  // MethodOfMomentsFactory parameters //
  setAsScalar( "MethodOfMomentsFactory-MaximumAbsoluteError", 1.0e-10 );
  setAsScalar( "MethodOfMomentsFactory-MaximumConstraintError", 1.0e-10 );
  setAsScalar( "MethodOfMomentsFactory-MaximumObjectiveError", 1.0e-10 );
  setAsScalar( "MethodOfMomentsFactory-MaximumRelativeError", 1.0e-10 );
  setAsUnsignedInteger( "MethodOfMomentsFactory-MaximumEvaluationNumber", 1000 );

  // Student parameters //
  setAsScalar( "Student-MaximumCDFEpsilon", 5.0e-6 );
  setAsScalar( "Student-MinimumCDFEpsilon", 5.0e-2 );
  setAsUnsignedInteger( "Student-MarginalIntegrationNodesNumber", 16 );
  setAsUnsignedInteger( "Student-MaximumNumberOfPoints", 10000000 );
  setAsUnsignedInteger( "Student-MinimumNumberOfPoints", 100000 );
  setAsUnsignedInteger( "Student-SmallDimension", 6 );

  // StudentFactory parameters //
  setAsScalar( "StudentFactory-NuMax", 1.0e3 );

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
  setAsBool( "RandomMixture-SimplifyAtoms", true );
  setAsScalar( "RandomMixture-DefaultAlpha", 5.0 );
  setAsScalar( "RandomMixture-DefaultBeta", 8.5 );
  setAsScalar( "RandomMixture-DefaultCDFEpsilon", 1.0e-10 );
  setAsScalar( "RandomMixture-DefaultPDFEpsilon", 1.0e-10 );
  setAsScalar( "RandomMixture-GraphCDFEpsilon", 1.0e-5 );
  setAsScalar( "RandomMixture-GraphPDFEpsilon", 1.0e-5 );
  setAsUnsignedInteger( "RandomMixture-DefaultBlockMax", 16 );
  setAsUnsignedInteger( "RandomMixture-DefaultBlockMin", 3 );
  setAsUnsignedInteger( "RandomMixture-DefaultMaxSize", 65536 );
  setAsUnsignedInteger( "RandomMixture-MarginalIntegrationNodesNumber", 128 );
  setAsUnsignedInteger( "RandomMixture-MaximumIntegrationNodesNumber", 1024 );
  setAsUnsignedInteger( "RandomMixture-MaximumSupportSize", 2048 );
  setAsUnsignedInteger( "RandomMixture-ProjectionDefaultSize", 25 );
  setAsUnsignedInteger( "RandomMixture-SmallSize", 100 );

  // NumericalMathEvaluation parameters //
  setAsScalar( "NumericalMathEvaluation-ParameterEpsilon", 1.0e-7 );
  setAsUnsignedInteger( "NumericalMathEvaluation-DefaultPointNumber", 129 );

  // DualLinearCombinationEvaluation //
  setAsScalar( "DualLinearCombinationEvaluation-SmallCoefficient", 0.0);

  // LinearCombinationEvaluation //
  setAsScalar( "LinearCombinationEvaluation-SmallCoefficient", 0.0);

  // DistFunc parameters //
  setAsScalar( "DistFunc-Precision", 1.0e-14 );
  setAsUnsignedInteger( "DistFunc-MaximumIteration", 5000 );

  // KFactor parameters //
  setAsScalar( "KFactor-Precision", 1.0e-8 );
  setAsUnsignedInteger( "KFactor-DefaultIntegrationNodesNumber", 256 );
  setAsUnsignedInteger( "KFactor-MaximumIteration", 32 );

  // RootStrategyImplementation parameters //
  setAsScalar( "RootStrategy-DefaultMaximumDistance", 8.0 );
  setAsScalar( "RootStrategy-DefaultStepSize", 1.0 );

  // SimulationAlgorithm parameters //
  setAsScalar( "SimulationAlgorithm-DefaultMaximumCoefficientOfVariation", 1.0e-1 );
  setAsScalar( "SimulationAlgorithm-DefaultMaximumStandardDeviation", 0.0 );
  setAsUnsignedInteger( "SimulationAlgorithm-DefaultBlockSize", 1 );
  setAsUnsignedInteger( "SimulationAlgorithm-DefaultMaximumOuterSampling", 1000 );

  // ProbabilitySimulationResult parameters //
  setAsBool( "ProbabilitySimulationResult-CheckPositiveVariance", false );
  setAsScalar( "ProbabilitySimulationResult-DefaultConfidenceLevel", 0.95 );

  // ExpectationSimulationAlgorithm parameters //
  setAsString( "ExpectationSimulationAlgorithm-DefaultCoefficientOfVariationCriterionType", "MAX");
  setAsString( "ExpectationSimulationAlgorithm-DefaultStandardDeviationCriterionType", "NONE");

  // SobolSimulationAlgorithm parameters //
  setAsScalar( "SobolSimulationAlgorithm-DefaultIndexQuantileLevel", 0.05 );
  setAsScalar( "SobolSimulationAlgorithm-DefaultIndexQuantileEpsilon", 1e-2 );
  setAsUnsignedInteger( "SobolSimulationAlgorithm-DefaultBlockSize", 1000 );
  setAsUnsignedInteger( "SobolSimulationAlgorithm-DefaultBatchSize", 1 );

  // SimulationSensitivityAnalysis parameters //
  setAsUnsignedInteger( "SimulationSensitivityAnalysis-DefaultSampleMargin", 400 );

  // SubsetSampling parameters //
  setAsScalar( "SubsetSampling-DefaultBetaMin", 2.0 );
  setAsScalar( "SubsetSampling-DefaultConditionalProbability", 0.1 );
  setAsScalar( "SubsetSampling-DefaultProposalRange", 2.0 );
  setAsUnsignedInteger( "SubsetSampling-DefaultMaximumOuterSampling", 10000 );

  // DirectionalSampling parameters //
  setAsUnsignedInteger( "DirectionalSampling-MeanContributionIntegrationNodesNumber", 255 );

  // AdaptiveDirectionalSampling parameters //
  setAsScalar( "AdaptiveDirectionalSampling-DefaultGamma", 0.5 );
  setAsUnsignedInteger( "AdaptiveDirectionalSampling-DefaultMaximumStratificationDimension", 3 );
  setAsUnsignedInteger( "AdaptiveDirectionalSampling-DefaultNumberOfSteps", 2 );

  // AnalyticalResult parameters //
  setAsScalar( "AnalyticalResult-DefaultWidth", 1.0 );
  setAsUnsignedInteger( "AnalyticalResult-MeanPointIntegrationNodesNumber", 255 );

  // StrongMaximumTest parameters //
  setAsScalar( "StrongMaximumTest-DefaultDeltaPrecision", 1.0e-7 );
  setAsScalar( "StrongMaximumTest-Epsilon", 1.0e-10 );

  // CleaningStrategy parameters //
  setAsScalar( "CleaningStrategy-DefaultSignificanceFactor", 1.0e-4 );
  setAsUnsignedInteger( "CleaningStrategy-DefaultMaximumSize", 20 );

  // FunctionalChaosAlgorithm parameters //
  setAsScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual", 1.0e-6 );
  setAsScalar( "FunctionalChaosAlgorithm-PValueThreshold", 1.0e-3);
  setAsScalar( "FunctionalChaosAlgorithm-QNorm", 0.5);
  setAsUnsignedInteger( "FunctionalChaosAlgorithm-LargeSampleSize", 10000);
  setAsUnsignedInteger( "FunctionalChaosAlgorithm-MaximumTotalDegree", 10 );
  setAsUnsignedInteger( "FunctionalChaosAlgorithm-SmallSampleSize", 1000);

  // FunctionalChaosSobolIndices parameters //
  setAsScalar( "FunctionalChaosSobolIndices-VariancePartThreshold", 1.0e-2);

  // LinearModelAlgorithm parameters //
  setAsString("LinearModelAlgorithm-DecompositionMethod", "QR");
  setAsUnsignedInteger("LinearModelAnalysis-Identifiers", 3);

  // GeneralLinearModelAlgorithm parameters //
  setAsBool( "GeneralLinearModelAlgorithm-KeepCovariance", true);
  setAsBool( "GeneralLinearModelAlgorithm-NormalizeData", false);
  setAsBool( "GeneralLinearModelAlgorithm-OptimizeParameters", true);
  setAsBool( "GeneralLinearModelAlgorithm-UnbiasedVariance", true);
  setAsBool( "GeneralLinearModelAlgorithm-UseAnalyticalAmplitudeEstimate", true);
  setAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationLowerBound", 1.0e-2 );
  setAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationScaleFactor", 2.0 );
  setAsScalar( "GeneralLinearModelAlgorithm-DefaultOptimizationUpperBound", 1.0e2 );
  setAsScalar( "GeneralLinearModelAlgorithm-MaximalScaling", 1.0e5 );
  setAsScalar( "GeneralLinearModelAlgorithm-MeanEpsilon", 1.0e-12 );
  setAsScalar( "GeneralLinearModelAlgorithm-StartingScaling", 1.0e-13 );
  setAsString( "GeneralLinearModelAlgorithm-DefaultOptimizationAlgorithm", "TNC");
  setAsString( "GeneralLinearModelAlgorithm-LinearAlgebra", "LAPACK");

  // KrigingAlgorithm parameters //
  setAsScalar( "KrigingAlgorithm-MaximalScaling", 1.0e5 );
  setAsScalar( "KrigingAlgorithm-StartingScaling", 1.0e-13 );
  setAsString( "KrigingAlgorithm-LinearAlgebra", "LAPACK");

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
  setAsBool( "MarginalTransformationEvaluation-Simplify", true );
  setAsScalar( "MarginalTransformationEvaluation-DefaultTailThreshold", 0.99 );
  setAsScalar( "MarginalTransformationEvaluation-ParametersEpsilon", 1.0e-14 );

  // DistributionImplementation parameters //
  setAsBool( "Distribution-MinimumVolumeLevelSetBySampling", false );
  setAsBool( "Distribution-Parallel", true);
  setAsBool( "Distribution-ShowSupportDiscretePDF", true);
  setAsBool( "Distribution-UseCovarianceAdaptiveAlgorithm", true );
  setAsScalar( "Distribution-QMin", 0.15 );
  setAsScalar( "Distribution-QMax", 0.85 );
  setAsScalar( "Distribution-DefaultQuantileEpsilon", 1.0e-12 );
  setAsScalar( "Distribution-DefaultPDFEpsilon", 1.0e-14 );
  setAsScalar( "Distribution-DefaultCDFEpsilon", 1.0e-14 );
  setAsScalar( "Distribution-DiscreteDrawPDFScaling", 0.25 );
  setAsUnsignedInteger( "Distribution-CharacteristicFunctionBlockMax", 20 );
  setAsUnsignedInteger( "Distribution-CharacteristicFunctionNMax", 1000000 );
  setAsUnsignedInteger( "Distribution-DefaultIntegrationNodesNumber", 255 );
  setAsUnsignedInteger( "Distribution-DefaultLevelNumber", 10 );
  setAsUnsignedInteger( "Distribution-DefaultPointNumber", 129 );
  setAsUnsignedInteger( "Distribution-DefaultQuantileCacheSize", 128 );
  setAsUnsignedInteger( "Distribution-DefaultQuantileIteration", 100 );
  setAsUnsignedInteger( "Distribution-EntropySamplingSize", 1048576 );
  setAsUnsignedInteger( "Distribution-MinimumVolumeLevelSetSamplingSize", 1048576 );
  setAsUnsignedInteger( "Distribution-SmallDimensionEntropy", 3 );

  // ContinuousDistribution parameters //
  setAsUnsignedInteger( "ContinuousDistribution-DefaultIntegrationNodesNumber", 256 );

  // DiscreteDistribution parameters //
  setAsScalar( "DiscreteDistribution-SupportEpsilon", 1.0e-14 );

  // DistributionFactoryImplementation parameters //
  setAsScalar( "DistributionFactory-BootstrapErrorTolerance", 0.1 );
  setAsUnsignedInteger( "DistributionFactory-DefaultBootstrapSize", 100 );

  // OrderStatisticsMarginalChecker //
  setAsScalar( "OrderStatisticsMarginalChecker-OptimizationEpsilon", 1.0e-7 );
  setAsUnsignedInteger( "OrderStatisticsMarginalChecker-QuantileIteration", 100 );

  // MaximumEntropyOrderStatisticsDistribution //
  setAsBool( "MaximumEntropyOrderStatisticsDistribution-CheckMarginals", true );
  setAsBool( "MaximumEntropyOrderStatisticsDistribution-UseApproximation", true );
  setAsScalar( "MaximumEntropyOrderStatisticsDistribution-SupportShift", 1.0e-15 );
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-CDFIntegrationNodesNumber", 16);
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-ExponentialFactorDiscretization", 100 );
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-MaximumApproximationSubdivision", 2 );
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-MaximumQuantileIteration", 10);

  // HMatrix parameters //
  setAsBool( "HMatrix-ForceSequential", false);
  setAsScalar( "HMatrix-AdmissibilityFactor", 2.0 );
  setAsScalar( "HMatrix-AssemblyEpsilon", 1.0e-4);
  setAsScalar( "HMatrix-RecompressionEpsilon", 1.0e-4);
  setAsScalar( "HMatrix-ValidationError", 0.0);
  setAsString( "HMatrix-ClusteringAlgorithm", "median" );
  setAsUnsignedInteger( "HMatrix-CompressionMethod", 1 );
  setAsUnsignedInteger( "HMatrix-MaxLeafSize", 100);
  setAsUnsignedInteger( "HMatrix-ValidationDump", 0);
  setAsUnsignedInteger( "HMatrix-ValidationRerun", 0);

  // GaussianProcess parameters //
  setAsScalar( "GaussianProcess-MaximalScaling", 1.0e5 );
  setAsScalar( "GaussianProcess-StartingScaling", 1.0e-13 );
  setAsUnsignedInteger( "GaussianProcess-GibbsMaximumIteration", 100 );

  // SpectralGaussianProcess parameters //
  setAsScalar( "SpectralGaussianProcess-StartingScaling", 1.0e-13 );
  setAsScalar( "SpectralGaussianProcess-MaximalScaling", 1.0e5 );
  setAsUnsignedInteger( "SpectralGaussianProcess-CholeskyCacheSize", 16384 );

  // WhittleFactory parameters //
  setAsScalar( "WhittleFactory-DefaultRhoBeg", 0.1 );
  setAsScalar( "WhittleFactory-DefaultRhoEnd", 1.0e-10 );
  setAsScalar( "WhittleFactory-DefaultStartingPointScale", 1.0 );
  setAsScalar( "WhittleFactory-RootEpsilon", 1.0e-6);
  setAsUnsignedInteger( "WhittleFactory-DefaultMaxFun", 2000 );

  // BoxCoxFactory parameters //
  setAsScalar( "BoxCoxFactory-DefaultRhoBeg", 0.1 );
  setAsScalar( "BoxCoxFactory-DefaultRhoEnd", 1.0e-10 );
  setAsUnsignedInteger( "BoxCoxFactory-DefaultMaxFun", 2000 );
  setAsUnsignedInteger( "BoxCoxFactory-DefaultPointNumber", 201 );

  // VisualTest parameters //
  setAsUnsignedInteger( "VisualTest-KendallPlot-MonteCarloSize", 100 );

  // CalibrationStrategyImplementation parameters //
  setAsScalar( "CalibrationStrategy-DefaultExpansionFactor", 1.2 );
  setAsScalar( "CalibrationStrategy-DefaultLowerBound", 0.117 ); // = 0.5 * 0.234
  setAsScalar( "CalibrationStrategy-DefaultShrinkFactor", 0.8 );
  setAsScalar( "CalibrationStrategy-DefaultUpperBound", 0.468 ); // = 2.0 * 0.234
  setAsUnsignedInteger( "CalibrationStrategy-DefaultCalibrationStep", 100 );

  // MCMC parameters //
  setAsUnsignedInteger( "MCMC-DefaultBurnIn", 0 );
  setAsUnsignedInteger( "MCMC-DefaultThinning", 1 );

  // ARMA parameters //
  setAsScalar( "ARMA-MeanEpsilon", 1.0e-14 );

  // ARMALikelihoodFactory parameters //
  setAsScalar( "ARMALikelihoodFactory-DefaultRhoBeg", 0.01 );
  setAsScalar( "ARMALikelihoodFactory-DefaultRhoEnd", 1.0e-10 );
  setAsScalar( "ARMALikelihoodFactory-DefaultStartingPointScale", 1.0 );
  setAsScalar( "ARMALikelihoodFactory-MaximalScaling", 1.0e5 );
  setAsScalar( "ARMALikelihoodFactory-RootEpsilon", 1.0e-6);
  setAsScalar( "ARMALikelihoodFactory-StartingScaling", 1.0e-13 );
  setAsUnsignedInteger( "ARMALikelihoodFactory-DefaultMaxFun", 10000 );

  // FittingTest parameters //
  setAsBool( "FittingTest-ChiSquaredCheckSample", true );
  setAsUnsignedInteger( "FittingTest-ChiSquaredMinimumBinCount", 5 );
  setAsUnsignedInteger( "FittingTest-KolmogorovSamplingSize", 10 );

  // LeastSquaresMetaModelSelection parameters //
  setAsScalar( "LeastSquaresMetaModelSelection-ErrorThreshold", 0.0 );
  setAsScalar( "LeastSquaresMetaModelSelection-MaximumErrorFactor", 2.0 );
  setAsString( "LeastSquaresMetaModelSelection-DecompositionMethod", "SVD" );

  // SparseMethod parameters //
  setAsScalar( "SparseMethod-ErrorThreshold", 1.0e-3 );
  setAsScalar( "SparseMethod-MaximumErrorFactor", 2.0 );

  // CholeskyMethod parameters //
  setAsUnsignedInteger( "CholeskyMethod-LargeCase", 128 );

  // Classifier parameters //
  setAsBool( "Classifier-Parallel", true );

  // TensorApproximationAlgorithm parameters //
  setAsScalar( "TensorApproximationAlgorithm-DefaultMaximumRadiusError", 1.0e-5 );
  setAsScalar( "TensorApproximationAlgorithm-DefaultMaximumResidualError", 1.0e-5 );
  setAsString( "TensorApproximationAlgorithm-DecompositionMethod", "SVD" );
  setAsString( "TensorApproximationAlgorithm-Method", "GreedyRankOne" );
  setAsUnsignedInteger( "TensorApproximationAlgorithm-DefaultMaximumAlternatingLeastSquaresIteration", 100 );

  // viewer.View parameters //
  setAsString( "View-ImageFormat", "png" );
}

/* String converter */
String ResourceMap::__repr__() const
{
  // First, build a global map to sort keys
  MapStringType globalMap(mapString_);
  for (MapScalarType::const_iterator it = mapScalar_.begin(); it != mapScalar_.end(); ++it)
    globalMap[it->first] = Get(it->first);
  for (MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.begin(); it != mapUnsignedInteger_.end(); ++it)
    globalMap[it->first] = Get(it->first);
  for (MapBoolType::const_iterator it = mapBool_.begin(); it != mapBool_.end(); ++it)
    globalMap[it->first] = Get(it->first);
  OSS oss;
  oss << "ResourceMap={";
  const char * separator = "\n  ";
  for(MapStringType::const_iterator it = globalMap.begin(); it != globalMap.end(); ++it, separator = ",\n  ")
    oss << separator
        << it->first << " => " << it->second;
  oss << "\n}";
  return oss;
}

std::vector<String> ResourceMap::getKeys() const
{
  // Initialize with the string keys
  std::vector<String> keys(getStringKeys());
  // Append the scalar keys
  {
    std::vector<String> scalarKeys(getScalarKeys());
    keys.insert(keys.end(), scalarKeys.begin(), scalarKeys.end());
  }
  // Append the unsigned integer keys
  {
    std::vector<String> unsignedIntegerKeys(getUnsignedIntegerKeys());
    keys.insert(keys.end(), unsignedIntegerKeys.begin(), unsignedIntegerKeys.end());
  }
  // Append the bool keys
  {
    std::vector<String> boolKeys(getBoolKeys());
    keys.insert(keys.end(), boolKeys.begin(), boolKeys.end());
  }
  // Then sort the keys
  std::sort(keys.begin(), keys.end());
  return keys;
}

std::vector<String> ResourceMap::getStringKeys() const
{
  std::vector<String> keys;
  for(MapStringType::const_iterator it = mapString_.begin(); it != mapString_.end(); ++it)
  {
    keys.push_back(it->first);
  }
  return keys;
}

std::vector<String> ResourceMap::getBoolKeys() const
{
  std::vector<String> keys;
  for(MapBoolType::const_iterator it = mapBool_.begin(); it != mapBool_.end(); ++it)
  {
    keys.push_back(it->first);
  }
  return keys;
}

std::vector<String> ResourceMap::getScalarKeys() const
{
  std::vector<String> keys;
  for(MapScalarType::const_iterator it = mapScalar_.begin(); it != mapScalar_.end(); ++it)
  {
    keys.push_back(it->first);
  }
  return keys;
}

std::vector<String> ResourceMap::getUnsignedIntegerKeys() const
{
  std::vector<String> keys;
  for(MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.begin(); it != mapUnsignedInteger_.end(); ++it)
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
