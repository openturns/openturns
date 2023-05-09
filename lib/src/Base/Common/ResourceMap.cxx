//                                               -*- C++ -*-
/**
 *  @brief ResourceMap defines top-most resourceMap strategies
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <mutex>
#include "openturns/OTconfig.hxx"
#ifdef OPENTURNS_HAVE_UNISTD_H
#include <unistd.h>                 // for sysconf
#endif
#include "openturns/OSS.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Path.hxx"
#include "openturns/Collection.hxx"
#include "openturns/XMLToolbox.hxx"
#ifdef OPENTURNS_HAVE_LIBXML2
#include <libxml/tree.h>
#endif

BEGIN_NAMESPACE_OPENTURNS

static const char * ConfigurationFileName = "openturns.conf";
#if defined OPENTURNS_HAVE_LIBXML2
static const char * RootElementName = "openturns-configuration";
static const char * XMLTag_value_str              = "value_str";
static const char * XMLTag_value_float            = "value_float";
static const char * XMLTag_value_int              = "value_int";
static const char * XMLTag_value_bool             = "value_bool";
#endif

static std::mutex ResourceMap_InstanceMutex_;
static ResourceMap * ResourceMap_P_instance_ = 0;
static const ResourceMap_init static_initializer_ResourceMap;

ResourceMap_init::ResourceMap_init()
{
  static std::once_flag flag;
  std::call_once(flag, [&]() {
    ResourceMap_P_instance_ = new ResourceMap;
  });
  assert(ResourceMap_P_instance_);
}

ResourceMap_init::~ResourceMap_init()
{
  static std::once_flag flag;
  std::call_once(flag, [&]() {
    delete ResourceMap_P_instance_;
    ResourceMap_P_instance_ = 0;
  });
}


template<>
MutexLockSingleton<ResourceMap>::MutexLockSingleton(ResourceMap & singleton)
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
  return GetInstance().lock().getType(key);
}

/* Get a value in the map */
String ResourceMap::Get(const String & key)
{
  return GetInstance().lock().get(key);
}

String ResourceMap::GetAsString(const String & key)
{
  return GetInstance().lock().getAsString(key);
}

Bool ResourceMap::GetAsBool(const String & key)
{
  return GetInstance().lock().getAsBool( key);
}

UnsignedInteger ResourceMap::GetAsUnsignedInteger(const String & key)
{
  return GetInstance().lock().getAsUnsignedInteger(key);
}

Scalar ResourceMap::GetAsScalar(const String & key)
{
  return GetInstance().lock().getAsScalar(key);
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
  GetInstance().lock().set(key, value);
}

void ResourceMap::SetAsString(const String & key, const String & value)
{
  GetInstance().lock().setAsString(key, value);
}

void ResourceMap::SetAsBool(const String & key, const Bool value)
{
  GetInstance().lock().setAsBool(key, value);
}

void ResourceMap::SetAsUnsignedInteger(const String & key, const UnsignedInteger value)
{
  GetInstance().lock().setAsUnsignedInteger(key, value);
}

void ResourceMap::SetAsScalar(const String & key, const Scalar value)
{
  GetInstance().lock().setAsScalar(key, value);
}

void ResourceMap::AddAsString(const String & key, const String & value)
{
  GetInstance().lock().addAsString(key, value);
}

void ResourceMap::AddAsBool(const String & key, const Bool value)
{
  GetInstance().lock().addAsBool(key, value);
}

void ResourceMap::AddAsUnsignedInteger(const String & key, const UnsignedInteger value)
{
  GetInstance().lock().addAsUnsignedInteger(key, value);
}

void ResourceMap::AddAsScalar(const String & key, const Scalar value)
{
  GetInstance().lock().addAsScalar(key, value);
}

Bool ResourceMap::HasKey(const String & key)
{
  return GetInstance().lock().hasKey(key);
}

void ResourceMap::Reload()
{
  return GetInstance().lock().reload();
}

void ResourceMap::RemoveKey(const String & key)
{
  GetInstance().lock().removeKey(key);
}

std::vector<String> ResourceMap::FindKeys(const String & substr)
{
  return GetInstance().lock().findKeys(substr);
}

/* Default constructor */
ResourceMap::ResourceMap()
  : mapString_()
  , mapScalar_()
  , mapUnsignedInteger_()
  , mapBool_()
{
  reload();
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
    if (it != mapString_.end()) return "str";
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
    if (it != mapUnsignedInteger_.end()) return "int";
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
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap.";
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
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap.";
}

Bool ResourceMap::hasKey(const String & key) const
{
  return ((mapString_.find(key) != mapString_.end())
          || (mapScalar_.find(key) != mapScalar_.end())
          || (mapUnsignedInteger_.find(key) != mapUnsignedInteger_.end())
          || (mapBool_.find(key) != mapBool_.end()));
}

void ResourceMap::removeKey(const String & key)
{
  if (!hasKey(key))
    throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap";

  String keyType(getType(key));
  if (keyType == "str")
    mapString_.erase(mapString_.find(key));
  if (keyType == "float")
    mapScalar_.erase(mapScalar_.find(key));
  if (keyType == "int")
    mapUnsignedInteger_.erase(mapUnsignedInteger_.find(key));
  if (keyType == "bool")
    mapBool_.erase(mapBool_.find(key));
}

String ResourceMap::getAsString(const String & key) const
{
  MapStringType::const_iterator it = mapString_.find(key);
  if (it != mapString_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as a String.";
}

Scalar ResourceMap::getAsScalar(const String & key) const
{
  MapScalarType::const_iterator it = mapScalar_.find(key);
  if (it != mapScalar_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as a Scalar.";
}

UnsignedInteger ResourceMap::getAsUnsignedInteger(const String & key) const
{
  MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.find(key);
  if (it != mapUnsignedInteger_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as an UnsignedInteger.";
}

Bool ResourceMap::getAsBool(const String & key) const
{
  MapBoolType::const_iterator it = mapBool_.find(key);
  if (it != mapBool_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as a Bool.";
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
    MapStringType::iterator it = mapString_.find(key);
    if (it != mapString_.end())
    {
      it->second = value;
      return;
    }
  }
  // Second, try to retrieve the value from the Scalar map
  {
    MapScalarType::iterator it = mapScalar_.find(key);
    if (it != mapScalar_.end())
    {
      Scalar scalarValue = -1.0;
      std::istringstream iss(value);
      iss >> scalarValue;
      it->second = scalarValue;
      return;
    }
  }
  // Third, try to retrieve the value from the UnsignedInteger map
  {
    MapUnsignedIntegerType::iterator it = mapUnsignedInteger_.find(key);
    if (it != mapUnsignedInteger_.end())
    {
      UnsignedInteger unsignedIntegerValue = 0;
      std::istringstream iss(value);
      iss >> unsignedIntegerValue;
      it->second = unsignedIntegerValue;
      return;
    }
  }
  // Fourth, try to retrieve the value from the Bool map
  {
    MapBoolType::iterator it = mapBool_.find(key);
    if (it != mapBool_.end())
    {
      Bool boolValue = false;
      // First, try to recover the bool value from the "true" or "false" strings
      if (value == "true") boolValue = true;
      else if (value == "false") boolValue = false;
      // Second, try to recover the bool from the litteral value
      else
      {
        std::istringstream iss(value);
        iss >> boolValue;
      }
      it->second = boolValue;
    }
  }
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap.";
}

void ResourceMap::setAsString(const String & key, const String & value)
{
  MapStringType::iterator it = mapString_.find(key);
  if (it == mapString_.end())
    throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as a String.";
  it->second = value;
}

void ResourceMap::setAsBool(const String & key, const Bool value)
{
  MapBoolType::iterator it = mapBool_.find(key);
  if (it == mapBool_.end())
    throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as a Bool.";
  it->second = value;
}

void ResourceMap::setAsUnsignedInteger(const String & key, const UnsignedInteger value)
{
  MapUnsignedIntegerType::iterator it = mapUnsignedInteger_.find(key);
  if (it == mapUnsignedInteger_.end())
    throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as an UnsignedInteger.";
  it->second = value;
}

void ResourceMap::setAsScalar(const String & key, const Scalar value)
{
  MapScalarType::iterator it = mapScalar_.find(key);
  if (it == mapScalar_.end())
    throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap as a Scalar.";
  it->second = value;
}

void ResourceMap::addAsString(const String & key, const String & value)
{
  if (mapString_.find(key) != mapString_.end())
    throw InternalException(HERE) << "Key '" << key << "' is already in ResourceMap as a String.";
  mapString_[key] = value;
}

void ResourceMap::addAsBool(const String & key, const Bool value)
{
  if (mapBool_.find(key) != mapBool_.end())
    throw InternalException(HERE) << "Key '" << key << "' is already in ResourceMap as a Bool.";
  mapBool_[key] = value;
}

void ResourceMap::addAsUnsignedInteger(const String & key, const UnsignedInteger value)
{
  if (mapUnsignedInteger_.find(key) != mapUnsignedInteger_.end())
    throw InternalException(HERE) << "Key '" << key << "' is already in ResourceMap as an UnsignedInteger.";
  mapUnsignedInteger_[key] = value;
}

void ResourceMap::addAsScalar(const String & key, const Scalar value)
{
  if (mapScalar_.find(key) != mapScalar_.end())
    throw InternalException(HERE) << "Key '" << key << "' is already in ResourceMap as a Scalar.";
  mapScalar_[key] = value;
}

/* Update the ResourceMap with information from the configuration file */
void ResourceMap::readConfigurationFile(const FileName & configurationFile)
{
#if defined OPENTURNS_HAVE_LIBXML2
  LOGINFO(OSS() << "Using configuration file " << configurationFile);

  // Open the configuration file and load it in a DOM document for later analysis
  XMLDoc document(configurationFile);

  // Check it is an OpenTURNS' one
  xmlNodePtr rootElt = xmlDocGetRootElement(document);
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
            mapString_[key] = value;
            continue;
          }
        } // String
        // Try to get a Scalar value
        {
          String value = XML::GetAttributeByName(current, XMLTag_value_float);
          if (value.size() > 0)
          {
            Scalar scalarValue = -1.0;
            std::istringstream iss(value);
            iss >> scalarValue;
            mapScalar_[key] = scalarValue;
            continue;
          }
        } // Scalar
        // Try to get an UnsignedInteger value
        {
          String value = XML::GetAttributeByName(current, XMLTag_value_int);
          if (value.size() > 0)
          {
            UnsignedInteger unsignedIntegerValue = 0;
            std::istringstream iss(value);
            iss >> unsignedIntegerValue;
            mapUnsignedInteger_[key] = unsignedIntegerValue;
            continue;
          }
        } // UnsignedInteger
        // Try to get a Bool value
        {
          String value = XML::GetAttributeByName( current, XMLTag_value_bool);
          if (value.size() > 0)
          {
            Bool boolValue = false;
            // First, try to recover the bool value from the "true" or "false" strings
            if (value == "true") boolValue = true;
            else if (value == "false") boolValue = false;
            // Second, try to recover the bool from the litteral value
            else
            {
              std::istringstream iss(value);
              iss >> boolValue;
            }
            mapBool_[key] = boolValue;
            continue;
          }
        } // Bool
      } // if XML::IsElement
    } // for xmlNodePtr
  } // if root
#else
  (void)configurationFile;
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
  catch (const FileNotFoundException &)
  {
    LOGWARN(OSS() << "The configuration file has not been found, using default parameters.");
  }
}

/* Load the configuration defined at installation time */
void ResourceMap::loadDefaultConfiguration()
{
#ifndef _WIN32
  addAsString("Path-TemporaryDirectory", "/tmp");
  addAsUnsignedInteger("TBB-ThreadsNumber", sysconf(_SC_NPROCESSORS_CONF));
#else
  addAsString("Path-TemporaryDirectory", "TEMP");
  UnsignedInteger numberOfProcessors = 0;
  std::istringstream iss(getenv("NUMBER_OF_PROCESSORS"));
  iss >> numberOfProcessors;
  addAsUnsignedInteger("TBB-ThreadsNumber", numberOfProcessors);
#endif
  if (const char* env_num_threads = std::getenv("OPENTURNS_NUM_THREADS"))
  {
    try
    {
      setAsUnsignedInteger("TBB-ThreadsNumber", std::stoi(env_num_threads));
    }
    catch (const std::invalid_argument &)
    {
      throw InternalException(HERE) << "OPENTURNS_NUM_THREADS must be an integer, got " << env_num_threads;
    }
  }
  addAsUnsignedInteger("Cache-MaxSize", 1024);

  // Os parameters
  addAsBool("Os-CreateProcess", false);
  addAsBool("Os-RemoveFiles", true);
  addAsUnsignedInteger("OS-DeleteTimeout", 2);

  // XMLStorageManager parameters
  addAsUnsignedInteger("XMLStorageManager-DefaultCompressionLevel", 0);

  // Collection parameters
  addAsUnsignedInteger("Collection-size-visible-in-str-from", 10);

  // SpecFunc parameters
  addAsScalar("SpecFunc-Precision", 2.0e-16);
  addAsUnsignedInteger("SpecFunc-MaximumIteration", 1000);

  // SymbolicParser parameters
  addAsString("SymbolicParser-Backend", SYMBOLICPARSER_DEFAULT_BACKEND);

  // SymbolicParserExprTk parameters
  addAsUnsignedInteger("SymbolicParserExprTk-SmallSize", 100);
  addAsUnsignedInteger("SymbolicParserExprTk-MaxStackDepth", 400);
  addAsUnsignedInteger("SymbolicParserExprTk-MaxNodeDepth", 10000);

  // SymbolicParserMuParser parameters
  addAsUnsignedInteger("SymbolicParserMuParser-SmallSize", 1000);

  // DesignProxy parameters
  addAsUnsignedInteger("DesignProxy-DefaultCacheSize", 16777216);// 2^24=16777216=128 Mio

  // KFold parameters
  addAsUnsignedInteger("KFold-DefaultK", 10);

  // KFold parameters
  addAsBool("KFoldSplitter-Randomize", false);

  // BlendedStep parameters //
  addAsScalar("BlendedStep-DefaultEta", 1.0);

  // CenteredFiniteDifferenceGradient parameters //
  addAsScalar("CenteredFiniteDifferenceGradient-DefaultEpsilon", 1.0e-5);

  // CenteredFiniteDifferenceHessian parameters //
  addAsScalar("CenteredFiniteDifferenceHessian-DefaultEpsilon", 1.0e-4);

  // NonCenteredFiniteDifferenceGradient parameters //
  addAsScalar("NonCenteredFiniteDifferenceGradient-DefaultEpsilon", 1.0e-7);

  // PiecewiseHermiteEvaluation parameters //
  addAsScalar("PiecewiseHermiteEvaluation-EpsilonRegular", 1.0e-12);

  // PiecewiseLinearEvaluation parameters //
  addAsScalar("PiecewiseLinearEvaluation-EpsilonRegular", 1.0e-12);

  // UniVariatePolynomialImplementation parameters //
  addAsUnsignedInteger("UniVariatePolynomial-SmallDegree", 400);

  // Pie parameters //
  addAsScalar("Pie-HorizontalMargin", 0.3);
  addAsScalar("Pie-LabelThreshold", 0.02);
  addAsScalar("Pie-VerticalMargin", 0.1);

  // DrawableImplementation parameters //
  addAsScalar("Drawable-DefaultLineWidth", 1.0);
  addAsString("Drawable-AlternativePattern", "S");
  addAsString("Drawable-DefaultColor", "blue");
  addAsString("Drawable-DefaultFillStyle", "solid");
  addAsString("Drawable-DefaultLineStyle", "solid");
  addAsString("Drawable-DefaultPaletteName", "Tableau");
  addAsString("Drawable-DefaultPattern", "s");
  addAsString("Drawable-DefaultPointStyle", "plus");
  addAsString("Drawable-DefaultSurfaceColor", "white");
  addAsUnsignedInteger("Drawable-DataThreshold", 2000);
  addAsUnsignedInteger("Drawable-DefaultPalettePhase", 12);

  // Text drawable
  addAsScalar("Text-DefaultTextSize", 0.75);

  // GraphImplementation parameters //
  addAsScalar("Graph-DefaultHorizontalMargin", 0.05);
  addAsScalar("Graph-DefaultLegendFontSize", 1.0);
  addAsScalar("Graph-DefaultVerticalMargin", 0.05);
  addAsUnsignedInteger("Graph-DefaultHeight", 480);
  addAsUnsignedInteger("Graph-DefaultWidth", 640);

  // Contour parameters //
  addAsUnsignedInteger("Contour-DefaultLevelsNumber", 10);

  // IntervalMesher parameters //
  addAsBool("IntervalMesher-UseDiamond", false);

  // PointToPointEvaluation parameters //
  addAsUnsignedInteger("PointToPointEvaluation-BlockSize", 256);

  // FieldToPointConnection parameters //
  addAsUnsignedInteger("FieldToPointConnection-BlockSize", 256);

  // FieldToPointFunctionalChaosAlgorithm
  addAsBool("FieldToPointFunctionalChaosAlgorithm-DefaultRecompress", false);
  addAsString("FieldToPointFunctionalChaosAlgorithm-CopulaType", "Normal");

  // SQP parameters //
  addAsScalar("SQP-DefaultOmega", 1.0e-4);
  addAsScalar("SQP-DefaultSmooth", 1.2);
  addAsScalar("SQP-DefaultTau", 0.5);

  // TNC parameters //
  addAsScalar("TNC-DefaultAccuracy", 1.0e-4);
  addAsScalar("TNC-DefaultEta", 0.25);
  addAsScalar("TNC-DefaultFmin", 1.0);
  addAsScalar("TNC-DefaultRescale", 1.3);
  addAsScalar("TNC-DefaultStepmx", 10.0);
  addAsUnsignedInteger("TNC-DefaultMaxCGit", 50);

  // AbdoRackwitz parameters //
  addAsScalar("AbdoRackwitz-DefaultOmega", 1.0e-4);
  addAsScalar("AbdoRackwitz-DefaultSmooth", 1.2);
  addAsScalar("AbdoRackwitz-DefaultTau", 0.5);

  // MultiStart parameters //
  addAsBool("MultiStart-KeepResults", true);

  // OptimizationAlgorithm parameters //
  addAsScalar("OptimizationAlgorithm-DefaultLevelValue", 0.0);
  addAsScalar("OptimizationAlgorithm-DefaultMaximumAbsoluteError", 1.0e-5);
  addAsScalar("OptimizationAlgorithm-DefaultMaximumConstraintError", 1.0e-5);
  addAsScalar("OptimizationAlgorithm-DefaultMaximumRelativeError", 1.0e-5);
  addAsScalar("OptimizationAlgorithm-DefaultMaximumResidualError", 1.0e-5);
  addAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumEvaluationNumber", 1000);
  addAsUnsignedInteger("OptimizationAlgorithm-DefaultMaximumIterationNumber", 100);

  // Pagmo parameters //
  addAsString("Pagmo-UnconstrainMethod", "death penalty");
  addAsBool("Pagmo-memory", false);
  // gaco
  addAsUnsignedInteger("Pagmo-gaco-ker", 63);
  addAsScalar("Pagmo-gaco-q", 1.0);
  addAsScalar("Pagmo-gaco-oracle", 0.0);
  addAsScalar("Pagmo-gaco-acc", 0.01);
  addAsUnsignedInteger("Pagmo-gaco-threshold", 1);
  addAsUnsignedInteger("Pagmo-gaco-n_gen_mark", 7);
  addAsUnsignedInteger("Pagmo-gaco-impstop", 100000);
  addAsScalar("Pagmo-gaco-focus", 0.0);
  // de
  addAsScalar("Pagmo-de-F", 0.8);
  addAsScalar("Pagmo-de-CR", 0.9);
  addAsUnsignedInteger("Pagmo-de-variant", 2);
  addAsUnsignedInteger("Pagmo-sade-variant", 2);
  addAsUnsignedInteger("Pagmo-sade-variant_adptv", 1);
  addAsUnsignedInteger("Pagmo-de1220-variant_adptv", 1);
  // ihs
  addAsScalar("Pagmo-ihs-phmcr", 0.85);
  addAsScalar("Pagmo-ihs-ppar_min", 0.35);
  addAsScalar("Pagmo-ihs-ppar_max", 0.99);
  addAsScalar("Pagmo-ihs-bw_min", 1e-5);
  addAsScalar("Pagmo-ihs-bw_max", 1.0);
  // pso
  addAsScalar("Pagmo-pso-omega", 0.7298);
  addAsScalar("Pagmo-pso-eta1", 2.05);
  addAsScalar("Pagmo-pso-eta2", 2.05);
  addAsScalar("Pagmo-pso-max_vel", 0.5);
  addAsUnsignedInteger("Pagmo-pso-variant", 5);
  addAsUnsignedInteger("Pagmo-pso-neighb_type", 2);
  addAsUnsignedInteger("Pagmo-pso-neighb_param", 4);
  // sga
  addAsScalar("Pagmo-sga-cr", 0.9);
  addAsScalar("Pagmo-sga-eta_c", 1.0);
  addAsScalar("Pagmo-sga-m", 0.01);
  addAsScalar("Pagmo-sga-param_m", 1.0);
  addAsUnsignedInteger("Pagmo-sga-param_s", 2);
  addAsString("Pagmo-sga-crossover", "exponential");
  addAsString("Pagmo-sga-mutation", "polynomial");
  addAsString("Pagmo-sga-selection", "tournament");
  // simulated_annealing
  addAsScalar("Pagmo-simulated_annealing-Ts", 10.0);
  addAsScalar("Pagmo-simulated_annealing-Tf", 0.1);
  addAsUnsignedInteger("Pagmo-simulated_annealing-n_T_adj", 10);
  addAsUnsignedInteger("Pagmo-simulated_annealing-n_range_adj", 1);
  addAsUnsignedInteger("Pagmo-simulated_annealing-bin_size", 20);
  addAsScalar("Pagmo-simulated_annealing-start_range", 1.0);
  // bee_colony
  addAsUnsignedInteger("Pagmo-bee_colony-limit", 20);
  // cmaes
  addAsScalar("Pagmo-cmaes-cc", -1.0);
  addAsScalar("Pagmo-cmaes-cs", -1.0);
  addAsScalar("Pagmo-cmaes-c1", -1.0);
  addAsScalar("Pagmo-cmaes-cmu", -1.0);
  addAsScalar("Pagmo-cmaes-sigma0", 0.5);
  // xnes
  addAsScalar("Pagmo-xnes-eta_mu", -1.0);
  addAsScalar("Pagmo-xnes-eta_sigma", -1.0);
  addAsScalar("Pagmo-xnes-eta_b", -1.0);
  addAsScalar("Pagmo-xnes-sigma0", -1.0);
  // nsga2
  addAsScalar("Pagmo-nsga2-cr", 0.95);
  addAsScalar("Pagmo-nsga2-eta_c", 10.0);
  addAsScalar("Pagmo-nsga2-m", 0.01);
  addAsScalar("Pagmo-nsga2-eta_m", 50.0);
  // moead
  addAsString("Pagmo-moead-weight_generation", "grid");
  addAsString("Pagmo-moead-decomposition", "tchebycheff");
  addAsUnsignedInteger("Pagmo-moead-neighbours", 20);
  addAsScalar("Pagmo-moead-CR", 1.0);
  addAsScalar("Pagmo-moead-F", 0.5);
  addAsScalar("Pagmo-moead-eta_m", 20.0);
  addAsScalar("Pagmo-moead-realb", 0.9);
  addAsUnsignedInteger("Pagmo-moead-limit", 2);
  addAsBool("Pagmo-moead-preserve_diversity", true);
  // mhaco
  addAsUnsignedInteger("Pagmo-mhaco-ker", 63);
  addAsScalar("Pagmo-mhaco-q", 1.0);
  addAsUnsignedInteger("Pagmo-mhaco-threshold", 1);
  addAsUnsignedInteger("Pagmo-mhaco-n_gen_mark", 7);
  addAsScalar("Pagmo-mhaco-focus", 0.0);
  // nspso
  addAsScalar("Pagmo-nspso-omega", 0.6);
  addAsScalar("Pagmo-nspso-c1", 2.0);
  addAsScalar("Pagmo-nspso-c2", 2.0);
  addAsScalar("Pagmo-nspso-chi", 1.0);
  addAsScalar("Pagmo-nspso-v_coeff", 0.5);
  addAsUnsignedInteger("Pagmo-nspso-leader_selection_range", 60);
  addAsString("Pagmo-nspso-diversity_mechanism", "crowding distance");

  // Dlib optimization parameters //
  addAsScalar("Dlib-DefaultInitialTrustRegionRadius", 1.0);
  addAsScalar("Dlib-DefaultWolfeRho", 0.01);
  addAsScalar("Dlib-DefaultWolfeSigma", 0.9);
  addAsUnsignedInteger("Dlib-DefaultMaxLineSearchIterations", 100);
  addAsUnsignedInteger("Dlib-DefaultMaxSize", 10);

  // EfficientGlobalOptimization parameters //
  addAsScalar("EfficientGlobalOptimization-DefaultAEITradeoff", 1.0);
  addAsScalar("EfficientGlobalOptimization-DefaultCorrelationLengthFactor", 1.0);
  addAsScalar("EfficientGlobalOptimization-DefaultImprovementFactor", 0.0);
  addAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartExperimentSize", 100);
  addAsUnsignedInteger("EfficientGlobalOptimization-DefaultMultiStartNumber", 20);
  addAsUnsignedInteger("EfficientGlobalOptimization-DefaultParameterEstimationPeriod", 1);

  // Cobyla parameters //
  addAsScalar("Cobyla-DefaultRhoBeg", 0.1);

  // SolverImplementation parameters //
  addAsScalar("Solver-DefaultAbsoluteError",  1.0e-5);
  addAsScalar("Solver-DefaultRelativeError",  1.0e-5);
  addAsScalar("Solver-DefaultResidualError",  0.0   );
  addAsUnsignedInteger("Solver-DefaultMaximumFunctionEvaluation", 100);

  // GaussKronrod parameters //
  addAsScalar("GaussKronrod-MaximumError",  1.0e-12);
  addAsUnsignedInteger("GaussKronrod-MaximumSubIntervals", 100);

  // GaussLegendre parameters //
  addAsUnsignedInteger("GaussLegendre-DefaultMarginalIntegrationPointsNumber", 64);

  // FejerAlgorithm parameters //
  addAsUnsignedInteger("FejerAlgorithm-DefaultMarginalIntegrationPointsNumber", 64);

  // IteratedQuadrature parameters //
  addAsScalar("IteratedQuadrature-MaximumError",    1.0e-7);
  addAsUnsignedInteger("IteratedQuadrature-MaximumSubIntervals", 32);

  // Fehlberg parameters //
  addAsScalar("Fehlberg-InitialStep", 1.0e-7);
  addAsScalar("Fehlberg-LocalPrecision", 1.0e-10);
  addAsScalar("Fehlberg-MinimalStep", 1.0e-7);
  addAsUnsignedInteger("Fehlberg-DefaultOrder", 4);

  // KarhunenLoeveQuadratureAlgorithm parameters //
  addAsScalar("KarhunenLoeveQuadratureAlgorithm-RegularizationFactor", 0.0);

  // KarhunenLoeveSVDAlgorithm parameters //
  addAsBool("KarhunenLoeveSVDAlgorithm-UseRandomSVD", false);
  addAsString("KarhunenLoeveSVDAlgorithm-RandomSVDVariant", "Halko2010");
  addAsUnsignedInteger("KarhunenLoeveSVDAlgorithm-Halko2011Margin", 2);
  addAsUnsignedInteger("KarhunenLoeveSVDAlgorithm-Halko2011Iterations", 2);
  addAsUnsignedInteger("KarhunenLoeveSVDAlgorithm-RandomSVDMaximumRank", 1000);

  // KarhunenLoeveP1Algorithm parameters //
  addAsScalar("KarhunenLoeveP1Algorithm-RegularizationFactor", 0.0);
  addAsString("KarhunenLoeveP1Algorithm-CovarianceMatrixStorage", "DENSE");
  addAsString("KarhunenLoeveP1Algorithm-EigenvaluesSolver", "LAPACK");

  // AdaptiveStieltjesAlgorithm parameters //
  addAsScalar("AdaptiveStieltjesAlgorithm-MaximumError",  1.0e-12);
  addAsUnsignedInteger("AdaptiveStieltjesAlgorithm-MaximumSubIntervalsBetweenRoots", 64);

  // LinearModelTest parameters //
  addAsScalar("LinearModelTest-DefaultHarrisonMcCabeBreakpoint", 0.5);
  addAsScalar("LinearModelTest-DefaultLevel", 0.05);
  addAsString("LinearModelTest-DefaultDurbinWatsonHypothesis", "Equal");
  addAsUnsignedInteger("LinearModelTest-DefaultHarrisonMcCabeSimulationSize", 1000);

  // Last parameters //
  addAsUnsignedInteger("Last-DefaultMaximumSize", 65536);

  // Compact parameters //
  addAsUnsignedInteger("Compact-DefaultHalfMaximumSize", 1024);

  // FaureSequence parameters //
  addAsUnsignedInteger("FaureSequence-InitialSeed", 1);

  // HaltonSequence parameters //
  addAsString("HaltonSequence-Scrambling", "NONE");
  addAsUnsignedInteger("HaltonSequence-InitialSeed", 1);

  // HaselgroveSequence parameters //
  addAsUnsignedInteger("HaselgroveSequence-InitialSeed", 1);

  // LowDiscrepancySequence parameters //
  addAsUnsignedInteger("LowDiscrepancySequence-ScramblingSeed", 0);

  // ReverseHaltonSequence parameters //
  addAsUnsignedInteger("ReverseHaltonSequence-InitialSeed", 1);

  // SobolSequence parameters //
  addAsUnsignedInteger("SobolSequence-InitialSeed", 1);

  // SobolIndicesExperiment parameters //
  addAsString("SobolIndicesExperiment-SamplingMethod", "MonteCarlo");

  // SobolIndicesAlgorithm parameters //
  addAsBool("SobolIndicesAlgorithm-DefaultUseAsymptoticDistribution", false);
  addAsScalar("SobolIndicesAlgorithm-DefaultBootstrapConfidenceLevel", 0.95);
  addAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBlockSize", 1);
  addAsUnsignedInteger("SobolIndicesAlgorithm-DefaultBootstrapSize", 100);

  // FAST parameters //
  addAsUnsignedInteger("FAST-DefaultInterferenceFactor", 4);
  addAsUnsignedInteger("FAST-DefaultResamplingSize", 1);

  // HSIC parameters //
  addAsBool("HSICEstimator-ParallelPValues", true);
  addAsUnsignedInteger("HSICEstimator-PermutationSize", 100);

  // RandomGenerator parameters //
  addAsUnsignedInteger("RandomGenerator-InitialSeed", 0);

  // CovarianceModelImplementation parameters //
  addAsScalar("CovarianceModel-DefaultNuggetFactor", 1e-12);
  addAsScalar("CovarianceModel-DefaultTMax",  5.0);
  addAsScalar("CovarianceModel-DefaultTMin", -5.0);
  addAsUnsignedInteger("CovarianceModel-DefaultPointNumber",  129);

  // SpectralModel parameters //
  addAsScalar("SpectralModel-DefaultMaximumFrequency",  5.0);
  addAsScalar("SpectralModel-DefaultMinimumFrequency", -5.0);
  addAsUnsignedInteger("SpectralModel-DefaultFrequencyNumber",  129);

  // FieldImplementation parameters //
  addAsBool("Field-AutomaticScaling", true);
  addAsScalar("Field-ArrowRatio", 0.01);
  addAsScalar("Field-ArrowScaling", 1.0);
  addAsUnsignedInteger("Field-LevelNumber", 30);

  // SampleImplementation parameters
  addAsString("Sample-CSVFileSeparator", ";");
  addAsUnsignedInteger("Sample-CSVPrecision", 16);
  addAsString("Sample-CSVFormat", "scientific");
  addAsString("Sample-CommentMarkers", "#");
  addAsUnsignedInteger("Sample-PrintEllipsisSize", 3);
  addAsUnsignedInteger("Sample-PrintEllipsisThreshold", 1000);
  addAsUnsignedInteger("Sample-SmallKendallTau", 23);

  // DomainImplementation parameters
  addAsScalar("Domain-SmallVolume",   1.0e-12);

  // Mesh parameters
  addAsBool("Mesh-BackfaceCulling", false);
  addAsScalar("Mesh-AmbientFactor", 0.1);
  addAsScalar("Mesh-DiffuseFactor", 0.7);
  addAsScalar("Mesh-Shininess", 100.0);
  addAsScalar("Mesh-SpecularFactor", 0.2);
  addAsScalar("Mesh-VertexEpsilon", 1.0e-6);
  addAsScalar("Mesh-CoordinateEpsilon", 1.0e-14);
  addAsUnsignedInteger("Mesh-LargeSize", 5000);
  addAsBool("Mesh-CheckValidity", false);

  // BoundingVolumeHierarchy parameters
  addAsString("BoundingVolumeHierarchy-Strategy", "Mean");
  addAsUnsignedInteger("BoundingVolumeHierarchy-BinNumber", 50);

  // EnclosingSimplexAlgorithm parameters
  addAsUnsignedInteger("EnclosingSimplexAlgorithm-LargeDimension", 5);

  // Matrix parameters
  addAsUnsignedInteger("Matrix-size-visible-in-str-from", 5);
  addAsScalar("Matrix-DefaultSmallPivot", 1.0e-7 );
  addAsScalar("Matrix-LargestEigenValueRelativeError", 1.0e-4);
  addAsScalar("Matrix-SymmetryThreshold", 1.0e-12);
  addAsUnsignedInteger("Matrix-LargestEigenValueIterations", 50);
  addAsScalar("Matrix-MaximalScaling", 1.0e-5);
  addAsScalar("Matrix-StartingScaling", 1.0e-13);

  // Tensor parameters
  addAsUnsignedInteger("Tensor-size-visible-in-str-from", 5);

  // Tensor parameters
  addAsUnsignedInteger("ComplexTensor-size-visible-in-str-from", 6);

  // BernsteinCopulaFactory parameters //
  addAsScalar("BernsteinCopulaFactory-alpha", 1.0);
  addAsString("BernsteinCopulaFactory-BinNumberSelectionMethod", "LogLikelihood");
  addAsUnsignedInteger("BernsteinCopulaFactory-kFraction", 2);
  addAsUnsignedInteger("BernsteinCopulaFactory-MaxM", 1);
  addAsUnsignedInteger("BernsteinCopulaFactory-MinM", 2);
  addAsUnsignedInteger("BernsteinCopulaFactory-SamplingSize", 10000);

  // BurrFactory parameters //
  addAsScalar("BurrFactory-AbsolutePrecision", 1.0e-12);
  addAsScalar("BurrFactory-RelativePrecision", 1.0e-12);
  addAsScalar("BurrFactory-ResidualPrecision", 1.0e-12);
  addAsUnsignedInteger("BurrFactory-MaximumIteration", 10);

  // ConditionalDistribution parameters //
  addAsUnsignedInteger("ConditionalDistribution-MarginalIntegrationNodesNumber", 256);
  addAsUnsignedInteger("ConditionalDistribution-MaximumIntegrationNodesNumber", 100000);
  addAsString("ConditionalDistribution-ContinuousDiscretizationMethod", "GaussProduct");

  // ComposedDistribution parameters //
  addAsBool("ComposedDistribution-UseGenericCovarianceAlgorithm", false);

  // CompositeDistribution parameters //
  addAsScalar("CompositeDistribution-SolverEpsilon", 1.0e-14);
  addAsUnsignedInteger("CompositeDistribution-StepNumber", 256);

  // Dirichlet parameters //
  addAsUnsignedInteger("Dirichlet-DefaultIntegrationSize", 50);
  addAsUnsignedInteger("Dirichlet-DefaultSamplingSize", 500000);

  // DirichletFactory parameters //
  addAsScalar("DirichletFactory-ParametersEpsilon", 1.0e-12);
  addAsUnsignedInteger("DirichletFactory-MaximumIteration", 10);

  // DiscreteCompoundDistribution parameters //
  addAsScalar("DiscreteCompoundDistribution-DefaultEpsilon", 1.0e-12);

  // ExtremeValueCopula parameters //
  addAsBool("ExtremeValueCopula-CheckPickandFunction", true);
  addAsUnsignedInteger("ExtremeValueCopula-CheckGridSize", 11);

  // FisherSnedecorFactory parameters //
  addAsScalar("FisherSnedecorFactory-D1LowerBound", 1.0e-2);
  addAsScalar("FisherSnedecorFactory-D2LowerBound", 1.0e-2);

  // FrechetFactory parameters //
  addAsScalar("FrechetFactory-BoundMargin", 10.0);

  // GeneralizedExtremeValue parameters //
  addAsScalar("GeneralizedExtremeValue-XiThreshold", 1.0e-6);

  // GeneralizedParetoFactory parameters //
  addAsScalar("GeneralizedParetoFactory-MaximumAbsoluteError", 1.0e-10);
  addAsScalar("GeneralizedParetoFactory-MaximumConstraintError", 1.0e-10);
  addAsScalar("GeneralizedParetoFactory-MaximumObjectiveError", 1.0e-10);
  addAsScalar("GeneralizedParetoFactory-MaximumRelativeError", 1.0e-10);
  addAsUnsignedInteger("GeneralizedParetoFactory-MaximumEvaluationNumber", 1000);
  addAsUnsignedInteger("GeneralizedParetoFactory-SmallSize", 20);

  // Gibbs parameters //
  addAsUnsignedInteger("Gibbs-DefaultUpdatingMethod", 0);

  // InverseNormalFactory parameters //
  addAsString("InverseNormalFactory-Method", "MLE");

  // KernelMixture parameters //
  addAsUnsignedInteger("KernelMixture-LargeSize", 20);
  addAsUnsignedInteger("KernelMixture-PDFCDFDiscretization", 1000);
  addAsUnsignedInteger("KernelMixture-SmallSize", 50);

  // KernelSmoothing parameters //
  addAsScalar("KernelSmoothing-AbsolutePrecision", 0.0);
  addAsScalar("KernelSmoothing-CutOffPlugin", 5.0);
  addAsScalar("KernelSmoothing-RelativePrecision", 1.0e-5);
  addAsScalar("KernelSmoothing-ResidualPrecision", 1.0e-10);
  addAsUnsignedInteger("KernelSmoothing-BinNumber", 1024);
  addAsUnsignedInteger("KernelSmoothing-MaximumIteration", 50);
  addAsUnsignedInteger("KernelSmoothing-SmallSize", 250);

  // LogNormal parameters //
  addAsScalar("LogNormal-CharacteristicFunctionSmallSigmaThreshold", 0.2);
  addAsUnsignedInteger("LogNormal-CharacteristicFunctionIntegrationNodes", 256);

  // LogNormalFactory parameters //
  addAsScalar("LogNormalFactory-AbsolutePrecision", 1.0e-12);
  addAsScalar("LogNormalFactory-RelativePrecision", 1.0e-12);
  addAsScalar("LogNormalFactory-ResidualPrecision", 1.0e-12);
  addAsUnsignedInteger("LogNormalFactory-EstimationMethod", 0);
  addAsUnsignedInteger("LogNormalFactory-MaximumIteration", 50);

  // Meixner parameters //
  addAsScalar("MeixnerDistribution-MaximumAbsoluteError", 1.0e-12);
  addAsScalar("MeixnerDistribution-MaximumConstraintError", 1.0e-12);
  addAsScalar("MeixnerDistribution-MaximumObjectiveError", 1.0e-12);
  addAsScalar("MeixnerDistribution-MaximumRelativeError", 1.0e-12);
  addAsUnsignedInteger("MeixnerDistribution-CDFDiscretization", 10000);
  addAsUnsignedInteger("MeixnerDistribution-CDFIntegrationNodesNumber", 32);

  // Mixture parameters //
  addAsScalar("Mixture-SmallWeight", 1.0e-12);
  addAsUnsignedInteger("Mixture-LargeSize", 20);
  addAsUnsignedInteger("Mixture-PDFCDFDiscretization", 1000);
  addAsUnsignedInteger("Mixture-SmallSize", 50);

  // Multinomial parameters //
  addAsScalar("Multinomial-eta", 1.0e-9);
  addAsScalar("Multinomial-smallA", 10.0);

  // NegativeBinomialFactory parameters //
  addAsScalar("NegativeBinomialFactory-AbsolutePrecision", 1.0e-12);
  addAsScalar("NegativeBinomialFactory-RelativePrecision", 1.0e-12);
  addAsScalar("NegativeBinomialFactory-ResidualPrecision", 1.0e-12);
  addAsUnsignedInteger("NegativeBinomialFactory-MaximumIteration", 50);

  // Normal parameters //
  addAsScalar("Normal-MaximumCDFEpsilon", 5.0e-6);
  addAsScalar("Normal-MinimumCDFEpsilon", 5.0e-2);
  addAsUnsignedInteger("Normal-MarginalIntegrationNodesNumber", 16);
  addAsUnsignedInteger("Normal-MaximumNumberOfPoints", 10000000);
  addAsUnsignedInteger("Normal-MinimumNumberOfPoints", 100000);
  addAsUnsignedInteger("Normal-SmallDimension", 6);

  // ParetoFactory parameters //
  addAsScalar("ParetoFactory-AlphaUpperBound", 1000.0);

  // ProductDistribution parameters //
  addAsScalar("ProductDistribution-LargeCharacteristicFunctionArgument", 10.0);

  // RiceFactory parameters //
  addAsScalar("RiceFactory-AbsolutePrecision", 1.0e-12);
  addAsScalar("RiceFactory-RelativePrecision", 1.0e-12);
  addAsScalar("RiceFactory-ResidualPrecision", 1.0e-12);
  addAsUnsignedInteger("RiceFactory-MaximumIteration", 10);

  // TrapezoidalFactory parameters //
  addAsScalar("TrapezoidalFactory-RhoBeg", 0.1);
  addAsScalar("TrapezoidalFactory-RhoEnd", 1.0e-5);
  addAsUnsignedInteger("TrapezoidalFactory-MaximumIteration", 2000);

  // TruncatedDistribution parameters //
  addAsScalar("TruncatedDistribution-DefaultThresholdRealization", 0.5);

  // TruncatedNormalFactory parameters //
  addAsScalar("TruncatedNormalFactory-SigmaLowerBound", 1.0e-4);

  // MaximumLikelihoodFactory parameters //
  addAsScalar("MaximumLikelihoodFactory-MaximumAbsoluteError", 1.0e-10);
  addAsScalar("MaximumLikelihoodFactory-MaximumConstraintError", 1.0e-10);
  addAsScalar("MaximumLikelihoodFactory-MaximumObjectiveError", 1.0e-10);
  addAsScalar("MaximumLikelihoodFactory-MaximumRelativeError", 1.0e-10);
  addAsString("MaximumLikelihoodFactory-DefaultOptimizationAlgorithm", "TNC");
  addAsUnsignedInteger("MaximumLikelihoodFactory-MaximumEvaluationNumber", 1000);

  // MethodOfMomentsFactory parameters //
  addAsScalar("MethodOfMomentsFactory-MaximumAbsoluteError", 1.0e-10);
  addAsScalar("MethodOfMomentsFactory-MaximumConstraintError", 1.0e-10);
  addAsScalar("MethodOfMomentsFactory-MaximumObjectiveError", 1.0e-10);
  addAsScalar("MethodOfMomentsFactory-MaximumRelativeError", 1.0e-10);
  addAsUnsignedInteger("MethodOfMomentsFactory-MaximumEvaluationNumber", 1000);

  // QuantileMatchingFactory parameters //
  addAsScalar("QuantileMatchingFactory-MaximumAbsoluteError", 1.0e-10);
  addAsScalar("QuantileMatchingFactory-MaximumConstraintError", 1.0e-10);
  addAsScalar("QuantileMatchingFactory-MaximumObjectiveError", 1.0e-10);
  addAsScalar("QuantileMatchingFactory-MaximumRelativeError", 1.0e-10);
  addAsScalar("QuantileMatchingFactory-QuantileEpsilon", 1.0e-2);
  addAsUnsignedInteger("QuantileMatchingFactory-MaximumEvaluationNumber", 1000);

  // Student parameters //
  addAsScalar("Student-MaximumCDFEpsilon", 5.0e-6);
  addAsScalar("Student-MinimumCDFEpsilon", 5.0e-2);
  addAsUnsignedInteger("Student-MarginalIntegrationNodesNumber", 16);
  addAsUnsignedInteger("Student-MaximumNumberOfPoints", 10000000);
  addAsUnsignedInteger("Student-MinimumNumberOfPoints", 100000);
  addAsUnsignedInteger("Student-SmallDimension", 6);

  // StudentFactory parameters //
  addAsScalar("StudentFactory-NuMax", 1.0e3);

  // NonCentralStudent parameters //
  addAsUnsignedInteger("NonCentralStudent-CDFAlgo", 0);

  // UserDefined parameters //
  addAsUnsignedInteger("UserDefined-SmallSize", 10000);

  // VonMisesFactory parameters //
  addAsScalar("VonMisesFactory-AbsolutePrecision", 1.0e-12);
  addAsScalar("VonMisesFactory-RelativePrecision", 1.0e-12);
  addAsScalar("VonMisesFactory-ResidualPrecision", 1.0e-12);
  addAsUnsignedInteger("VonMisesFactory-MaximumIteration", 10);

  // AliMikhailHaqCopulaFactory parameters //
  addAsScalar("AliMikhailHaqCopulaFactory-ThetaEpsilon", 1.0e-14);

  // FrankCopulaFactory parameters //
  addAsScalar("FrankCopulaFactory-AbsolutePrecision", 1.0e-14);
  addAsScalar("FrankCopulaFactory-RelativePrecision", 1.0e-14);
  addAsScalar("FrankCopulaFactory-ResidualPrecision", 1.0e-14);
  addAsUnsignedInteger("FrankCopulaFactory-MaximumIteration", 100);

  // RandomMixture parameters //
  addAsBool("RandomMixture-SimplifyAtoms", true);
  addAsScalar("RandomMixture-DefaultAlpha", 5.0);
  addAsScalar("RandomMixture-DefaultBeta", 8.5);
  addAsScalar("RandomMixture-DefaultCDFEpsilon", 1.0e-10);
  addAsScalar("RandomMixture-DefaultPDFEpsilon", 1.0e-10);
  addAsScalar("RandomMixture-GraphCDFEpsilon", 1.0e-5);
  addAsScalar("RandomMixture-GraphPDFEpsilon", 1.0e-5);
  addAsUnsignedInteger("RandomMixture-DefaultBlockMax", 16);
  addAsUnsignedInteger("RandomMixture-DefaultBlockMin", 3);
  addAsUnsignedInteger("RandomMixture-DefaultMaxSize", 65536);
  addAsUnsignedInteger("RandomMixture-MarginalIntegrationNodesNumber", 128);
  addAsUnsignedInteger("RandomMixture-MaximumIntegrationNodesNumber", 1024);
  addAsUnsignedInteger("RandomMixture-MaximumSupportSize", 2048);
  addAsUnsignedInteger("RandomMixture-ProjectionDefaultSize", 25);
  addAsUnsignedInteger("RandomMixture-SmallSize", 100);

  // Evaluation parameters //
  addAsScalar("Evaluation-ParameterEpsilon", 1.0e-7);
  addAsUnsignedInteger("Evaluation-DefaultPointNumber", 129);

  // DualLinearCombinationEvaluation //
  addAsScalar("DualLinearCombinationEvaluation-SmallCoefficient", 0.0);

  // LinearCombinationEvaluation //
  addAsScalar("LinearCombinationEvaluation-SmallCoefficient", 0.0);

  // DistFunc parameters //
  addAsScalar("DistFunc-Precision", 1.0e-14);
  addAsUnsignedInteger("DistFunc-MaximumIteration", 5000);

  // KFactor parameters //
  addAsScalar("KFactor-Precision", 1.0e-8);
  addAsUnsignedInteger("KFactor-DefaultIntegrationNodesNumber", 256);
  addAsUnsignedInteger("KFactor-MaximumIteration", 32);

  // RootStrategyImplementation parameters //
  addAsScalar("RootStrategy-DefaultMaximumDistance", 8.0);
  addAsScalar("RootStrategy-DefaultStepSize", 1.0);

  // SimulationAlgorithm parameters //
  addAsScalar("SimulationAlgorithm-DefaultMaximumCoefficientOfVariation", 1.0e-1);
  addAsScalar("SimulationAlgorithm-DefaultMaximumStandardDeviation", 0.0);
  addAsUnsignedInteger("SimulationAlgorithm-DefaultBlockSize", 1);
  addAsUnsignedInteger("SimulationAlgorithm-DefaultMaximumOuterSampling", 1000);

  // ProbabilitySimulationResult parameters //
  addAsBool("ProbabilitySimulationResult-CheckPositiveVariance", false);
  addAsScalar("ProbabilitySimulationResult-DefaultConfidenceLevel", 0.95);

  // ExpectationSimulationAlgorithm parameters //
  addAsString("ExpectationSimulationAlgorithm-DefaultCoefficientOfVariationCriterionType", "MAX");
  addAsString("ExpectationSimulationAlgorithm-DefaultStandardDeviationCriterionType", "NONE");

  // SobolSimulationAlgorithm parameters //
  addAsScalar("SobolSimulationAlgorithm-DefaultIndexQuantileLevel", 0.05);
  addAsScalar("SobolSimulationAlgorithm-DefaultIndexQuantileEpsilon", 1e-2);
  addAsUnsignedInteger("SobolSimulationAlgorithm-DefaultBlockSize", 1000);
  addAsUnsignedInteger("SobolSimulationAlgorithm-DefaultBatchSize", 1);

  // SimulationSensitivityAnalysis parameters //
  addAsUnsignedInteger("SimulationSensitivityAnalysis-DefaultSampleMargin", 400);

  // SubsetSampling parameters //
  addAsScalar("SubsetSampling-DefaultBetaMin", 2.0);
  addAsScalar("SubsetSampling-DefaultConditionalProbability", 0.1);
  addAsScalar("SubsetSampling-DefaultProposalRange", 2.0);
  addAsUnsignedInteger("SubsetSampling-DefaultMaximumOuterSampling", 10000);

  // NAIS parameters //
  addAsScalar("NAIS-DefaultQuantileLevel", 0.25);

  // DirectionalSampling parameters //
  addAsUnsignedInteger("DirectionalSampling-MeanContributionIntegrationNodesNumber", 255);

  // AdaptiveDirectionalStratification parameters //
  addAsScalar("AdaptiveDirectionalStratification-DefaultGamma", 0.5);
  addAsUnsignedInteger("AdaptiveDirectionalStratification-DefaultMaximumStratificationDimension", 3);
  addAsUnsignedInteger("AdaptiveDirectionalStratification-DefaultNumberOfSteps", 2);

  // AnalyticalResult parameters //
  addAsScalar("AnalyticalResult-DefaultWidth", 1.0);
  addAsUnsignedInteger("AnalyticalResult-MeanPointIntegrationNodesNumber", 255);

  // SystemFORM parameters //
  addAsScalar("SystemFORM-MaximalScaling", 1.0e-5);
  addAsScalar("SystemFORM-StartingScaling", 1.0e-13);

  // MultiFORM parameters //
  addAsScalar("MultiFORM-DefaultGamma", 1.1);
  addAsScalar("MultiFORM-DefaultDelta", 0.75);
  addAsUnsignedInteger("MultiFORM-DefaultMaximumDesignPointsNumber", 4);

  // StrongMaximumTest parameters //
  addAsScalar("StrongMaximumTest-DefaultDeltaPrecision", 1.0e-7);
  addAsScalar("StrongMaximumTest-Epsilon", 1.0e-10);

  // CleaningStrategy parameters //
  addAsScalar("CleaningStrategy-DefaultSignificanceFactor", 1.0e-4);
  addAsUnsignedInteger("CleaningStrategy-DefaultMaximumSize", 20);

  // MetaModelAlgorithm parameters //
  addAsScalar("MetaModelAlgorithm-PValueThreshold", 1.0e-3);
  addAsString("MetaModelAlgorithm-ModelSelectionCriterion", "BIC");
  addAsString("MetaModelAlgorithm-NonParametricModel", "Histogram");

  // FunctionalChaosAlgorithm parameters //
  addAsScalar("FunctionalChaosAlgorithm-DefaultMaximumResidual", 1.0e-6);
  addAsScalar("FunctionalChaosAlgorithm-QNorm", 0.5);
  addAsUnsignedInteger("FunctionalChaosAlgorithm-MaximumTotalDegree", 10);
  addAsUnsignedInteger("FunctionalChaosAlgorithm-BasisSize", 0);
  addAsBool("FunctionalChaosAlgorithm-Sparse", false);
  addAsString("FunctionalChaosAlgorithm-FittingAlgorithm", "CorrectedLeaveOneOut");

  // LeastSquaresExpansion parameters //
  addAsString("LeastSquaresExpansion-DecompositionMethod", "QR");

  // FunctionalChaosSobolIndices parameters //
  addAsScalar("FunctionalChaosSobolIndices-VariancePartThreshold", 1.0e-2);

  // LinearModelAlgorithm parameters //
  addAsString("LinearModelAlgorithm-DecompositionMethod", "QR");
  addAsUnsignedInteger("LinearModelAnalysis-Identifiers", 3);

  // LinearModelStepwiseAlgorithm parameters //
  addAsScalar("LinearModelStepwiseAlgorithm-Penalty", 2.0);
  addAsUnsignedInteger("LinearModelStepwiseAlgorithm-MaximumIterationNumber", 1000);

  // GeneralLinearModelAlgorithm parameters //
  addAsBool("GeneralLinearModelAlgorithm-KeepCovariance", true);
  addAsBool("GeneralLinearModelAlgorithm-OptimizeParameters", true);
  addAsBool("GeneralLinearModelAlgorithm-UnbiasedVariance", true);
  addAsBool("GeneralLinearModelAlgorithm-UseAnalyticalAmplitudeEstimate", true);
  addAsScalar("GeneralLinearModelAlgorithm-DefaultOptimizationLowerBound", 1.0e-2);
  addAsScalar("GeneralLinearModelAlgorithm-DefaultOptimizationScaleFactor", 2.0);
  addAsScalar("GeneralLinearModelAlgorithm-DefaultOptimizationUpperBound", 1.0e2);
  addAsScalar("GeneralLinearModelAlgorithm-MeanEpsilon", 1.0e-12);
  addAsString("GeneralLinearModelAlgorithm-DefaultOptimizationAlgorithm", "TNC");
  addAsString("GeneralLinearModelAlgorithm-LinearAlgebra", "LAPACK");

  // KrigingAlgorithm parameters //
  addAsString("KrigingAlgorithm-LinearAlgebra", "LAPACK");

  // SquaredExponential parameters //
  addAsScalar("SquaredExponential-DefaultTheta", 1.0);

  // AbsoluteExponential parameters //
  addAsScalar("AbsoluteExponential-DefaultTheta", 1.0);

  // GeneralizedExponential parameters //
  addAsScalar("GeneralizedExponential-DefaultTheta", 1.0);

  // MaternModel parameters //
  addAsScalar("MaternModel-DefaultNu", 1.5);
  addAsScalar("MaternModel-DefaultTheta", 1.0);

  // WeightedExperimentImplementation parameters //
  addAsUnsignedInteger("WeightedExperiment-DefaultSize", 100);

  // GaussProductExperiment parameters //
  addAsUnsignedInteger("GaussProductExperiment-DefaultMarginalSize", 5);

  // HyperbolicAnisotropicEnumerateFunction parameters //
  addAsScalar("HyperbolicAnisotropicEnumerateFunction-DefaultQ", 0.4);

  // MarginalTransformationEvaluation parameters //
  addAsBool("MarginalTransformationEvaluation-Simplify", true);
  addAsScalar("MarginalTransformationEvaluation-DefaultTailThreshold", 0.99);
  addAsScalar("MarginalTransformationEvaluation-ParametersEpsilon", 1.0e-14);

  // DistributionImplementation parameters //
  addAsBool("Distribution-MinimumVolumeLevelSetBySampling", false);
  addAsBool("Distribution-Parallel", true);
  addAsBool("Distribution-ScaleColorsDiscretePDF", true);
  addAsBool("Distribution-ShowSupportDiscretePDF", true);
  addAsBool("Distribution-UseCovarianceAdaptiveAlgorithm", true);
  addAsScalar("Distribution-QMin", 0.15);
  addAsScalar("Distribution-QMax", 0.85);
  addAsScalar("Distribution-DefaultQuantileEpsilon", 1.0e-12);
  addAsScalar("Distribution-DefaultPDFEpsilon", 1.0e-14);
  addAsScalar("Distribution-DefaultCDFEpsilon", 1.0e-14);
  addAsScalar("Distribution-DiscreteDrawPDFScaling", 0.25);
  addAsString("Distribution-EntropySamplingMethod", "MonteCarlo");
  addAsUnsignedInteger("Distribution-CharacteristicFunctionBlockMax", 20);
  addAsUnsignedInteger("Distribution-CharacteristicFunctionNMax", 1000000);
  addAsUnsignedInteger("Distribution-DefaultIntegrationNodesNumber", 255);
  addAsUnsignedInteger("Distribution-DefaultLevelNumber", 10);
  addAsUnsignedInteger("Distribution-DefaultPointNumber", 129);
  addAsUnsignedInteger("Distribution-DefaultQuantileCacheSize", 128);
  addAsUnsignedInteger("Distribution-DefaultQuantileIteration", 100);
  addAsUnsignedInteger("Distribution-EntropySamplingSize", 524288);
  addAsUnsignedInteger("Distribution-MinimumVolumeLevelSetSamplingSize", 16384);
  addAsUnsignedInteger("Distribution-SmallDimensionEntropy", 3);
  addAsString("Distribution-RoughnessSamplingMethod", "MonteCarlo");
  addAsUnsignedInteger("Distribution-RoughnessSamplingSize", 524288);
  addAsUnsignedInteger("Distribution-SmallDimensionRoughness", 3);

  // ContinuousDistribution parameters //
  addAsUnsignedInteger("ContinuousDistribution-DefaultIntegrationNodesNumber", 256);

  // DiscreteDistribution parameters //
  addAsScalar("DiscreteDistribution-SupportEpsilon", 1.0e-14);

  // DiscreteMarkovChain parameters //
  addAsString("DiscreteMarkovChain-DOTArcColor", "black");
  addAsString("DiscreteMarkovChain-DOTLayout", "dot");
  addAsString("DiscreteMarkovChain-DOTNodeColor", "black");
  addAsString("DiscreteMarkovChain-DOTNodeShape", "oval");
  addAsScalar("DiscreteMarkovChain-ProbabilitySumPrecision", 1.0e-6);

  // DistributionFactoryImplementation parameters //
  addAsScalar("DistributionFactory-BootstrapErrorTolerance", 0.1);
  addAsUnsignedInteger("DistributionFactory-DefaultBootstrapSize", 100);

  // OrderStatisticsMarginalChecker //
  addAsScalar("OrderStatisticsMarginalChecker-OptimizationEpsilon", 1.0e-7);
  addAsUnsignedInteger("OrderStatisticsMarginalChecker-QuantileIteration", 100);

  // MaximumEntropyOrderStatisticsDistribution //
  addAsBool("MaximumEntropyOrderStatisticsDistribution-CheckMarginals", true);
  addAsBool("MaximumEntropyOrderStatisticsDistribution-UseApproximation", false);
  addAsScalar("MaximumEntropyOrderStatisticsDistribution-SupportShift", 1.0e-15);
  addAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-CDFIntegrationNodesNumber", 16);
  addAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-ExponentialFactorDiscretization", 100);
  addAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-MaximumApproximationSubdivision", 2);
  addAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-MaximumQuantileIteration", 10);

  // HMatrix parameters //
  addAsBool("HMatrix-ForceSequential", false);
  addAsScalar("HMatrix-AdmissibilityFactor", 100.0);
  addAsScalar("HMatrix-AssemblyEpsilon", 1.0e-4);
  addAsScalar("HMatrix-LargestEigenValueRelativeError", 1.0e-1);
  addAsScalar("HMatrix-RegularizationEpsilon", 1.0e-4);
  addAsScalar("HMatrix-RecompressionEpsilon", 1.0e-4);
  addAsScalar("HMatrix-ValidationError", 0.0);
  addAsString("HMatrix-ClusteringAlgorithm", "median");
  addAsString("HMatrix-CompressionMethod", "AcaRandom");
  addAsString("HMatrix-FactorizationMethod", "LLt");
  addAsUnsignedInteger("HMatrix-FactorizationIterations", 10);
  addAsUnsignedInteger("HMatrix-LargestEigenValueIterations", 10);
  addAsUnsignedInteger("HMatrix-MaxLeafSize", 250);
  addAsUnsignedInteger("HMatrix-ValidationDump", 0);
  addAsUnsignedInteger("HMatrix-ValidationRerun", 0);

  // GaussianProcess parameters //
  addAsUnsignedInteger("GaussianProcess-GibbsMaximumIteration", 100);

  // SpectralGaussianProcess parameters //
  addAsScalar("SpectralGaussianProcess-StartingScaling", 1.0e-13);
  addAsScalar("SpectralGaussianProcess-MaximalScaling", 1.0e-5);
  addAsUnsignedInteger("SpectralGaussianProcess-CholeskyCacheSize", 16384);

  // WhittleFactory parameters //
  addAsScalar("WhittleFactory-DefaultRhoBeg", 0.1);
  addAsScalar("WhittleFactory-DefaultRhoEnd", 1.0e-10);
  addAsScalar("WhittleFactory-DefaultStartingPointScale", 1.0);
  addAsScalar("WhittleFactory-RootEpsilon", 1.0e-6);
  addAsUnsignedInteger("WhittleFactory-DefaultMaximumEvaluationNumber", 2000);

  // BoxCoxFactory parameters //
  addAsScalar("BoxCoxFactory-DefaultRhoBeg", 0.1);
  addAsScalar("BoxCoxFactory-DefaultRhoEnd", 1.0e-10);
  addAsUnsignedInteger("BoxCoxFactory-DefaultMaximumEvaluationNumber", 2000);
  addAsUnsignedInteger("BoxCoxFactory-DefaultPointNumber", 201);

  // VisualTest parameters //
  addAsUnsignedInteger("VisualTest-KendallPlot-MonteCarloSize", 100);

  // RandomWalkMetropolisHastings parameters //
  addAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationExpansionFactor", 1.2);
  addAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationLowerBound", 0.117); // = 0.5 * 0.234
  addAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationShrinkFactor", 0.8);
  addAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationUpperBound", 0.468); // = 2.0 * 0.234
  addAsUnsignedInteger("RandomWalkMetropolisHastings-DefaultAdaptationPeriod", 30);

  // MetropolisHastingsImplementation parameters //
  addAsUnsignedInteger("RandomWalkMetropolisHastings-DefaultBurnIn", 1000);

  // GaussianLinearCalibration parameters //
  addAsString("GaussianLinearCalibration-Method", "QR");

  // LinearLeastSquaresCalibration parameters //
  addAsScalar("LinearLeastSquaresCalibration-Regularization", 1e-12);
  addAsString("LinearLeastSquaresCalibration-Method", "QR");

  // NonLinearLeastSquaresCalibration parameters //
  addAsUnsignedInteger("NonLinearLeastSquaresCalibration-BootstrapSize", 100);
  addAsUnsignedInteger("NonLinearLeastSquaresCalibration-MultiStartSize", 100);

  // GaussianNonLinearCalibration parameters //
  addAsUnsignedInteger("GaussianNonLinearCalibration-BootstrapSize", 100);
  addAsUnsignedInteger("GaussianNonLinearCalibration-MultiStartSize", 100);

  // CalibrationResult parameters //
  addAsScalar("CalibrationResult-xRangeMarginFactor", 0.2);
  addAsString("CalibrationResult-PriorLineStyle", "dashed");
  addAsString("CalibrationResult-PosteriorLineStyle", "dotdash");
  addAsString("CalibrationResult-ObservationLineStyle", "solid");
  addAsString("CalibrationResult-PriorPointStyle", "circle");
  addAsString("CalibrationResult-PosteriorPointStyle", "diamond");
  addAsString("CalibrationResult-ObservationPointStyle", "fsquare");

  // ARMA parameters //
  addAsScalar("ARMA-MeanEpsilon", 1.0e-14);

  // ARMALikelihoodFactory parameters //
  addAsScalar("ARMALikelihoodFactory-DefaultRhoBeg", 0.01);
  addAsScalar("ARMALikelihoodFactory-DefaultRhoEnd", 1.0e-10);
  addAsScalar("ARMALikelihoodFactory-RootEpsilon", 1.0e-6);
  addAsUnsignedInteger("ARMALikelihoodFactory-DefaultMaximumEvaluationNumber", 10000);

  // FittingTest parameters //
  addAsBool("FittingTest-ChiSquaredCheckSample", true);
  addAsScalar("FittingTest-LillieforsPrecision", 0.01);
  addAsUnsignedInteger("FittingTest-ChiSquaredMinimumBinCount", 5);
  addAsUnsignedInteger("FittingTest-LillieforsMinimumSamplingSize", 10);
  addAsUnsignedInteger("FittingTest-LillieforsMaximumSamplingSize", 100000);

  // PenalizedLeastSquaresAlgorithm parameters //
  addAsBool("PenalizedLeastSquaresAlgorithm-UseNormal", false);

  // LeastSquaresMetaModelSelection parameters //
  addAsScalar("LeastSquaresMetaModelSelection-ErrorThreshold", 0.0);
  addAsScalar("LeastSquaresMetaModelSelection-MaximumError", 0.5);
  addAsScalar("LeastSquaresMetaModelSelection-MaximumErrorFactor", 2.0);
  addAsString("LeastSquaresMetaModelSelection-DecompositionMethod", "SVD");

  // SparseMethod parameters //
  addAsScalar("SparseMethod-ErrorThreshold", 1.0e-3);
  addAsScalar("SparseMethod-MaximumErrorFactor", 2.0);

  // CholeskyMethod parameters //
  addAsUnsignedInteger("CholeskyMethod-LargeCase", 128);

  // Classifier parameters //
  addAsBool("Classifier-Parallel", true);

  // viewer.View parameters //
  addAsString("View-ImageFormat", "png");

  // SmolyakExperiment parameters //
  addAsScalar("SmolyakExperiment-MergeRelativeEpsilon", 1.0e-8);
  addAsScalar("SmolyakExperiment-MergeAbsoluteEpsilon", 1.0e-8);
  addAsBool("SmolyakExperiment-MergeQuadrature", true);

}

void ResourceMap::reload()
{
  std::vector<String> allKeys(getKeys());
  for (UnsignedInteger i = 0; i < allKeys.size(); ++ i)
    removeKey(allKeys[i]);
  loadDefaultConfiguration();
  loadConfigurationFile();
}

/** Get the list of keys associated to a class */
std::vector<String> ResourceMap::findKeys(const String & substr)
{
  std::vector<String> allKeys(getKeys());
  std::vector<String> result;
  for (UnsignedInteger i = 0; i < allKeys.size(); ++ i)
  {
    if(allKeys[i].find(substr) != std::string::npos)
      result.push_back(allKeys[i]);
  }
  return result;
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
