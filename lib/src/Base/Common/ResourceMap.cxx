//                                               -*- C++ -*-
/**
 *  @brief ResourceMap defines top-most resourceMap strategies
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
#include <unistd.h>                 // for sysconf
#include <string.h>                 // for strdup
#include "OTthread.hxx"
#include "OTconfig.hxx"
#include "OSS.hxx"
#include "ResourceMap.hxx"
#include "Exception.hxx"
//#include "Path.hxx"
#include "XMLToolbox.hxx"

BEGIN_NAMESPACE_OPENTURNS

static const char * ConfigurationFileName = "openturns.conf";
#if defined OPENTURNS_HAVE_LIBXML2
static const char * RootElementName = "openturns-configuration";
static const char * XMLTag_value = "value";
#endif

static pthread_mutex_t ResourceMap_InstanceMutex_;
static ResourceMap * ResourceMap_P_instance_ = 0;

ResourceMap_init::ResourceMap_init()
{
  static pthread_once_t ResourceMap_InstanceMutex_once = PTHREAD_ONCE_INIT;
  int rc = pthread_once( &ResourceMap_InstanceMutex_once, ResourceMap::Initialize );
  if (rc != 0)
  {
    perror("ResourceMap_init::ResourceMap_init once Initialization failed");
    exit(1);
  }
}

ResourceMap_init::~ResourceMap_init()
{
  ResourceMap::Release();
}



ResourceMapInstance::ResourceMapInstance( ResourceMap & rm )  throw() : rm_(rm), lock_(ResourceMap_InstanceMutex_) {}
ResourceMapInstance::ResourceMapInstance( const ResourceMapInstance & other ) : rm_(other.rm_), lock_(other.lock_) {}
String ResourceMapInstance::__repr__() const
{
  return rm_.__repr__();
}

ResourceMap & ResourceMapInstance::lock() throw()
{
  return rm_;
}

const ResourceMap & ResourceMapInstance::lock() const throw()
{
  return rm_;
}




/* Since ResourceMap is a singleton, GetInstance gives access to the object */
ResourceMapInstance ResourceMap::GetInstance()
{
#ifdef BOGUS_PTHREAD_LIBRARY
  if (!ResourceMap_P_instance_)
  {
    ResourceMap_P_instance_ = new ResourceMap;
    assert(ResourceMap_P_instance_);
  }
#endif
  return *ResourceMap_P_instance_;
}

void ResourceMap::Initialize()
{
#ifndef OT_MUTEXINIT_NOCHECK
  pthread_mutexattr_t attr;
  pthread_mutexattr_init( &attr );
  //pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_NORMAL );
  //pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
  pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
  int rc = pthread_mutex_init( &ResourceMap_InstanceMutex_, &attr );
  if (rc != 0)
  {
    perror("ResourceMap::Initialize mutex initialization failed");
    exit(1);
  }
#else
  pthread_mutex_init( &ResourceMap_InstanceMutex_, NULL );
#endif
#ifdef BOGUS_PTHREAD_LIBRARY
  ResourceMap_P_instance_ = 0;
#else
  ResourceMap_P_instance_ = new ResourceMap;
  assert(ResourceMap_P_instance_);
#endif
}



void ResourceMap::Release()
{
  delete ResourceMap_P_instance_;
  ResourceMap_P_instance_ = 0;
}

/* Get the list of keys */
std::vector<String> ResourceMap::GetKeys()
{
  //MutexLock lock( ResourceMap_InstanceMutex_ );
  return GetInstance().lock().getKeys();
}

/* Get a value in the map */
String ResourceMap::Get(String key)
{
  //MutexLock lock( ResourceMap_InstanceMutex_ );
  return GetInstance().lock().get( key );
}

Bool ResourceMap::GetAsBool(String key)
{
  //MutexLock lock( ResourceMap_InstanceMutex_ );
  return GetInstance().lock().getAsBool( key );
}

UnsignedInteger ResourceMap::GetAsUnsignedInteger(String key)
{
  //MutexLock lock( ResourceMap_InstanceMutex_ );
  return GetInstance().lock().getAsUnsignedInteger( key );
}

NumericalScalar ResourceMap::GetAsNumericalScalar(String key)
{
  //MutexLock lock( ResourceMap_InstanceMutex_ );
  return GetInstance().lock().getAsNumericalScalar( key );
}

/* Get the size of the map */
UnsignedInteger ResourceMap::GetSize()
{
  return GetInstance().lock().getSize();
}

/* Set a value in the map */
void ResourceMap::Set(String key, String value)
{
  //MutexLock lock( ResourceMap_InstanceMutex_ );
  GetInstance().lock().set( key, value );
}

void ResourceMap::SetAsBool(String key, Bool value)
{
  //MutexLock lock( ResourceMap_InstanceMutex_ );
  GetInstance().lock().setAsBool( key, value );
}

void ResourceMap::SetAsUnsignedInteger(String key, UnsignedInteger value)
{
  //MutexLock lock( ResourceMap_InstanceMutex_ );
  GetInstance().lock().setAsUnsignedInteger( key, value );
}

void ResourceMap::SetAsNumericalScalar(String key, NumericalScalar value)
{
  //MutexLock lock( ResourceMap_InstanceMutex_ );
  GetInstance().lock().setAsNumericalScalar( key, value );
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
  UnsignedInteger value;
  String st = get( key );
  std::istringstream iss( st );
  iss >> value;
  return value;
}

NumericalScalar ResourceMap::getAsNumericalScalar(String key) const
{
  NumericalScalar value;
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

void ResourceMap::setAsNumericalScalar(String key, NumericalScalar value)
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
  LOGWARN(OSS() << "Cannot parse configuration file do to lacking xml support");
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

  // Collection parameters
  setAsUnsignedInteger( "Collection-size-visible-in-str-from", 10 );

  // SpecFunc parameters
  setAsUnsignedInteger( "SpecFunc-MaximumIteration", 1000 );
  setAsNumericalScalar( "SpecFunc-Precision", 2.0e-16 );

  // GramProxy parameters
  setAsUnsignedInteger( "DesignProxy-DefaultCacheSize", 16777216 );// 2^24=16777216=128 Mio

  // KFold parameters
  setAsUnsignedInteger( "KFold-DefaultK", 10 );

  // BlendedStep parameters //
  setAsNumericalScalar( "BlendedStep-DefaultEta", 1.0 );

  // CenteredFiniteDifferenceGradient parameters //
  setAsNumericalScalar( "CenteredFiniteDifferenceGradient-DefaultEpsilon", 1.0e-5 );

  // CenteredFiniteDifferenceHessian parameters //
  setAsNumericalScalar( "CenteredFiniteDifferenceHessian-DefaultEpsilon", 1.0e-4 );

  // NonCenteredFiniteDifferenceGradient parameters //
  setAsNumericalScalar( "NonCenteredFiniteDifferenceGradient-DefaultEpsilon", 1.0e-7 );

  // PiecewiseHermiteEvaluationImplementation parameters //
  setAsNumericalScalar( "PiecewiseHermiteEvaluationImplementation-EpsilonRegular", 1.0e-12 );

  // PiecewiseLinearEvaluationImplementation parameters //
  setAsNumericalScalar( "PiecewiseLinearEvaluationImplementation-EpsilonRegular", 1.0e-12 );

  // UniVariatePolynomialImplementation parameters //
  setAsUnsignedInteger( "UniVariatePolynomialImplementation-SmallDegree", 400 );

  // Pie parameters //
  setAsNumericalScalar( "Pie-HorizontalMargin", 0.3 );
  setAsNumericalScalar( "Pie-VerticalMargin", 0.1 );
  setAsNumericalScalar( "Pie-LabelThreshold", 0.02 );

  // DrawableImplementation parameters //
  set( "DrawableImplementation-NoSpecifiedLabel", "" );
  set( "DrawableImplementation-DefaultColor", "blue" );
  set( "DrawableImplementation-DefaultSurfaceColor", "white" );
  set( "DrawableImplementation-DefaultFillStyle", "solid" );
  set( "DrawableImplementation-DefaultPointStyle", "plus" );
  set( "DrawableImplementation-DefaultLineStyle", "solid" );
  set( "DrawableImplementation-DefaultPattern", "s" );
  set( "DrawableImplementation-AlternativePattern", "S" );
  setAsUnsignedInteger( "DrawableImplementation-DefaultLineWidth", 1 );
  setAsUnsignedInteger( "DrawableImplementation-DataThreshold", 2000 );

  // GraphImplementation parameters //
  set( "GraphImplementation-NoSpecifiedLabel", "" );
  setAsUnsignedInteger( "GraphImplementation-DefaultWidth", 640 );
  setAsUnsignedInteger( "GraphImplementation-DefaultHeight", 480 );
  setAsNumericalScalar( "GraphImplementation-DefaultLegendFontSize", 1.0 );

  // Contour parameters //
  setAsUnsignedInteger( "Contour-DefaultLevelsNumber", 10 );

  // BoundConstrainedAlgorithmImplementation parameters //
  setAsUnsignedInteger( "BoundConstrainedAlgorithmImplementation-DefaultMaximumEvaluationsNumber", 100 );
  setAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumAbsoluteError", 1.0e-5 );
  setAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumRelativeError", 1.0e-5 );
  setAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumObjectiveError", 1.0e-5 );
  setAsNumericalScalar( "BoundConstrainedAlgorithmImplementation-DefaultMaximumConstraintError", 1.0e-5 );

  // SQP parameters //
  setAsNumericalScalar( "SQP-DefaultTau", 0.5 );
  setAsNumericalScalar( "SQP-DefaultOmega", 1.0e-4 );
  setAsNumericalScalar( "SQP-DefaultSmooth", 1.2 );

  // TNC parameters //
  setAsUnsignedInteger( "TNC-DefaultMaxCGit", 50 );
  setAsNumericalScalar( "TNC-DefaultEta", 0.25 );
  setAsNumericalScalar( "TNC-DefaultStepmx", 10.0 );
  setAsNumericalScalar( "TNC-DefaultAccuracy", 1.0e-4 );
  setAsNumericalScalar( "TNC-DefaultFmin", 1.0 );
  setAsNumericalScalar( "TNC-DefaultRescale", 1.3 );

  // AbdoRackwitz parameters //
  setAsNumericalScalar( "AbdoRackwitz-DefaultTau", 0.5 );
  setAsNumericalScalar( "AbdoRackwitz-DefaultOmega", 1.0e-4 );
  setAsNumericalScalar( "AbdoRackwitz-DefaultSmooth", 1.2 );

  // NearestPointAlgorithmImplementation parameters //
  setAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultLevelValue", 0.0 );
  setAsUnsignedInteger( "NearestPointAlgorithmImplementation-DefaultMaximumIteration", 100 );
  setAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumAbsoluteError", 1.0e-5 );
  setAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumRelativeError", 1.0e-5 );
  setAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumResidualError", 1.0e-5 );
  setAsNumericalScalar( "NearestPointAlgorithmImplementation-DefaultMaximumConstraintError", 1.0e-5 );

  // Cobyla parameters //
  setAsNumericalScalar( "Cobyla-DefaultRhoBeg", 0.1 );

  // SolverImplementation parameters //
  setAsNumericalScalar( "SolverImplementation-DefaultAbsoluteError",  1.0e-5 );
  setAsNumericalScalar( "SolverImplementation-DefaultRelativeError",  1.0e-5 );
  setAsNumericalScalar( "SolverImplementation-DefaultResidualError",  1.0e-8 );
  setAsUnsignedInteger( "SolverImplementation-DefaultMaximumFunctionEvaluation", 100 );

  // GaussKronrod parameters //
  setAsUnsignedInteger( "GaussKronrod-MaximumSubIntervals", 100 );
  setAsNumericalScalar( "GaussKronrod-MaximumError",  1.0e-12 );

  // IteratedQuadrature parameters //
  setAsUnsignedInteger( "IteratedQuadrature-MaximumSubIntervals", 32 );
  setAsNumericalScalar( "IteratedQuadrature-MaximumError",  1.0e-7 );

  // LinearModelFactory parameters //
  setAsNumericalScalar( "LinearModelFactory-DefaultLevelValue", 0.95 );

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

  // SensitivityAnalysis parameters //
  setAsUnsignedInteger( "SensitivityAnalysis-DefaultBlockSize", 1 );

  // FAST parameters //
  setAsUnsignedInteger( "FAST-DefaultResamplingSize", 1 );
  setAsUnsignedInteger( "FAST-DefaultInterferenceFactor", 4 );

  // RandomGenerator parameters //
  setAsUnsignedInteger( "RandomGenerator-InitialSeed", 0 );

  // CovarianceModelImplementation parameters //
  setAsNumericalScalar( "CovarianceModelImplementation-DefaultNuggetFactor", 1e-12 );

  // FieldImplementation parameters //
  setAsUnsignedInteger( "FieldImplementation-LevelNumber", 30 );

  // NumericalSampleImplementation parameters
  setAsUnsignedInteger( "NumericalSampleImplementation-SmallKendallTau", 23 );
  setAsUnsignedInteger("NumericalSampleImplementation-PrintEllipsisThreshold", 1000);
  setAsUnsignedInteger("NumericalSampleImplementation-PrintEllipsisSize", 3);

  // DomainImplementation parameters
  setAsNumericalScalar( "DomainImplementation-SmallVolume",   1.0e-12 );

  // Mesh parameters
  setAsNumericalScalar( "Mesh-VertexEpsilon", 1.0e-12 );
  setAsUnsignedInteger( "Mesh-LargeSize", 5000 );
  setAsUnsignedInteger( "Mesh-UseKDTree", 1 );

  // Matrix parameters
  setAsUnsignedInteger( "Matrix-size-visible-in-str-from", 5 );

  // Tensor parameters
  setAsUnsignedInteger( "Tensor-size-visible-in-str-from", 5 );

  // Tensor parameters
  setAsUnsignedInteger( "ComplexTensor-size-visible-in-str-from", 6 );

  // MatrixImplementation parameters //
  setAsNumericalScalar( "MatrixImplementation-DefaultSmallPivot", 1.0e-7 );

  // BurrFactory parameters //
  setAsNumericalScalar( "BurrFactory-AbsolutePrecision", 1.0e-12 );
  setAsNumericalScalar( "BurrFactory-RelativePrecision", 1.0e-12 );
  setAsNumericalScalar( "BurrFactory-ResidualPrecision", 1.0e-12 );
  setAsUnsignedInteger( "BurrFactory-MaximumIteration", 10 );

  // ConditionalDistribution parameters //
  setAsUnsignedInteger( "ConditionalDistribution-MarginalIntegrationNodesNumber", 256 );
  setAsUnsignedInteger( "ConditionalDistribution-MaximumIntegrationNodesNumber", 100000 );

  // CompositeDistribution parameters //
  setAsUnsignedInteger( "CompositeDistribution-StepNumber", 256 );
  setAsNumericalScalar( "CompositeDistribution-SolverEpsilon", 1e-10 );

  // DirichletFactory parameters //
  setAsUnsignedInteger( "Dirichlet-DefaultSamplingSize", 500000 );
  setAsUnsignedInteger( "Dirichlet-DefaultIntegrationSize", 50 );

  // DirichletFactory parameters //
  setAsNumericalScalar( "DirichletFactory-ParametersEpsilon", 1.0e-12 );
  setAsUnsignedInteger( "DirichletFactory-MaximumIteration", 10 );

  // FisherSnedecorFactory parameters //
  setAsUnsignedInteger( "FisherSnedecorFactory-MaximumEvaluationNumber", 1000 );
  setAsNumericalScalar( "FisherSnedecorFactory-MaximumAbsoluteError", 1.0e-10 );
  setAsNumericalScalar( "FisherSnedecorFactory-MaximumRelativeError", 1.0e-10 );
  setAsNumericalScalar( "FisherSnedecorFactory-MaximumObjectiveError", 1.0e-10 );
  setAsNumericalScalar( "FisherSnedecorFactory-MaximumConstraintError", 1.0e-10 );
  setAsNumericalScalar( "FisherSnedecorFactory-D1LowerBound", 1.0e-2 );
  setAsNumericalScalar( "FisherSnedecorFactory-D2LowerBound", 1.0e-2 );

  // GeneralizedParetoFactory parameters //
  setAsUnsignedInteger( "GeneralizedParetoFactory-SmallSize", 20 );
  setAsUnsignedInteger( "GeneralizedParetoFactory-MaximumEvaluationNumber", 1000 );
  setAsNumericalScalar( "GeneralizedParetoFactory-MaximumAbsoluteError", 1.0e-10 );
  setAsNumericalScalar( "GeneralizedParetoFactory-MaximumRelativeError", 1.0e-10 );
  setAsNumericalScalar( "GeneralizedParetoFactory-MaximumObjectiveError", 1.0e-10 );
  setAsNumericalScalar( "GeneralizedParetoFactory-MaximumConstraintError", 1.0e-10 );

  // InverseNormalFactory parameters //
  set( "InverseNormalFactory-Method", "MLE" );

  // KernelMixture parameters //
  setAsUnsignedInteger( "KernelMixture-SmallSize", 50 );
  setAsUnsignedInteger( "KernelMixture-LargeSize", 20 );
  setAsUnsignedInteger( "KernelMixture-PDFCDFDiscretization", 1000 );

  // KernelSmoothing parameters //
  setAsUnsignedInteger( "KernelSmoothing-SmallSize", 250 );
  setAsUnsignedInteger( "KernelSmoothing-BinNumber", 1024 );
  setAsNumericalScalar( "KernelSmoothing-CutOffPlugin", 5.0 );
  setAsNumericalScalar( "KernelSmoothing-AbsolutePrecision", 0.0 );
  setAsNumericalScalar( "KernelSmoothing-RelativePrecision", 1.0e-5 );
  setAsNumericalScalar( "KernelSmoothing-ResidualPrecision", 1.0e-10 );
  setAsUnsignedInteger( "KernelSmoothing-MaximumIteration", 50 );

  // LogNormal parameters //
  setAsUnsignedInteger( "LogNormal-CharacteristicFunctionIntegrationNodes", 256 );
  setAsNumericalScalar( "LogNormal-CharacteristicFunctionSmallSigmaThreshold", 0.2 );

  // LogNormalFactory parameters //
  setAsUnsignedInteger( "LogNormalFactory-EstimationMethod", 0 );
  setAsNumericalScalar( "LogNormalFactory-AbsolutePrecision", 1.0e-12 );
  setAsNumericalScalar( "LogNormalFactory-RelativePrecision", 1.0e-12 );
  setAsNumericalScalar( "LogNormalFactory-ResidualPrecision", 1.0e-12 );
  setAsUnsignedInteger( "LogNormalFactory-MaximumIteration", 50 );

  // Meixner parameters //
  setAsUnsignedInteger( "MeixnerDistribution-CDFIntegrationNodesNumber", 32 );
  setAsUnsignedInteger( "MeixnerDistribution-CDFDiscretization", 10000 );
  setAsNumericalScalar( "MeixnerDistribution-MaximumAbsoluteError", 1.0e-12 );
  setAsNumericalScalar( "MeixnerDistribution-MaximumRelativeError", 1.0e-12 );
  setAsNumericalScalar( "MeixnerDistribution-MaximumConstraintError", 1.0e-12 );
  setAsNumericalScalar( "MeixnerDistribution-MaximumObjectiveError", 1.0e-12 );

  // Mixture parameters //
  setAsNumericalScalar( "Mixture-SmallWeight", 1.0e-12 );
  setAsUnsignedInteger( "Mixture-SmallSize", 50 );
  setAsUnsignedInteger( "Mixture-LargeSize", 20 );
  setAsUnsignedInteger( "Mixture-PDFCDFDiscretization", 1000 );

  // Multinomial parameters //
  setAsUnsignedInteger( "Multinomial-smallA", 10 );
  setAsNumericalScalar( "Multinomial-eta", 1.0e-9 );

  // NegativeBinomialFactory parameters //
  setAsNumericalScalar( "NegativeBinomialFactory-AbsolutePrecision", 1.0e-12 );
  setAsNumericalScalar( "NegativeBinomialFactory-RelativePrecision", 1.0e-12 );
  setAsNumericalScalar( "NegativeBinomialFactory-ResidualPrecision", 1.0e-12 );
  setAsUnsignedInteger( "NegativeBinomialFactory-MaximumIteration", 50 );

  // Normal parameters //
  setAsUnsignedInteger( "Normal-MaximumNumberOfPoints", 10000000 );
  setAsUnsignedInteger( "Normal-MinimumNumberOfPoints", 100000 );
  setAsUnsignedInteger( "Normal-SmallDimension", 6 );
  setAsUnsignedInteger( "Normal-MarginalIntegrationNodesNumber", 16 );
  setAsNumericalScalar( "Normal-MaximumCDFEpsilon", 5.0e-6 );
  setAsNumericalScalar( "Normal-MinimumCDFEpsilon", 5.0e-2 );

  // ProductDistribution parameters //
  setAsNumericalScalar( "ProductDistribution-LargeCharacteristicFunctionArgument", 10.0 );

  // RiceFactory parameters //
  setAsNumericalScalar( "RiceFactory-AbsolutePrecision", 1.0e-12 );
  setAsNumericalScalar( "RiceFactory-RelativePrecision", 1.0e-12 );
  setAsNumericalScalar( "RiceFactory-ResidualPrecision", 1.0e-12 );
  setAsUnsignedInteger( "RiceFactory-MaximumIteration", 10 );

  // TrapezoidalFactory parameters //
  setAsNumericalScalar( "TrapezoidalFactory-RhoBeg", 0.1 );
  setAsNumericalScalar( "TrapezoidalFactory-RhoEnd", 1.0e-5 );
  setAsUnsignedInteger( "TrapezoidalFactory-MaximumIteration", 10000 );

  // TruncatedDistribution parameters //
  setAsNumericalScalar( "TruncatedDistribution-DefaultThresholdRealization", 0.5 );

  // TruncatedNormalFactory parameters //
  setAsUnsignedInteger( "TruncatedNormalFactory-MaximumEvaluationNumber", 1000 );
  setAsNumericalScalar( "TruncatedNormalFactory-MaximumAbsoluteError", 1.0e-10 );
  setAsNumericalScalar( "TruncatedNormalFactory-MaximumRelativeError", 1.0e-10 );
  setAsNumericalScalar( "TruncatedNormalFactory-MaximumObjectiveError", 1.0e-10 );
  setAsNumericalScalar( "TruncatedNormalFactory-MaximumConstraintError", 1.0e-10 );
  setAsNumericalScalar( "TruncatedNormalFactory-SigmaLowerBound", 1.0e-4 );

  // Student parameters //
  setAsUnsignedInteger( "Student-MaximumNumberOfPoints", 10000000 );
  setAsUnsignedInteger( "Student-MinimumNumberOfPoints", 100000 );
  setAsUnsignedInteger( "Student-SmallDimension", 6 );
  setAsUnsignedInteger( "Student-MarginalIntegrationNodesNumber", 16 );
  setAsNumericalScalar( "Student-MaximumCDFEpsilon", 5.0e-6 );
  setAsNumericalScalar( "Student-MinimumCDFEpsilon", 5.0e-2 );

  // NonCentralStudent parameters //
  setAsUnsignedInteger( "NonCentralStudent-CDFAlgo", 0 );

  // UserDefined parameters //
  setAsUnsignedInteger( "UserDefined-SmallSize", 10000 );

  // AliMikhailHaqCopulaFactory parameters //
  setAsNumericalScalar( "AliMikhailHaqCopulaFactory-ThetaEpsilon", 1.0e-14 );

  // FrankCopulaFactory parameters //
  setAsNumericalScalar( "FrankCopulaFactory-AbsolutePrecision", 1.0e-14 );
  setAsNumericalScalar( "FrankCopulaFactory-RelativePrecision", 1.0e-14 );
  setAsNumericalScalar( "FrankCopulaFactory-ResidualPrecision", 1.0e-14 );
  setAsUnsignedInteger( "FrankCopulaFactory-MaximumIteration", 100 );

  // RandomMixture parameters //
  setAsUnsignedInteger( "RandomMixture-DefaultBlockMin", 3 );
  setAsUnsignedInteger( "RandomMixture-DefaultBlockMax", 16 );
  setAsUnsignedInteger( "RandomMixture-DefaultMaxSize", 65536 );
  setAsNumericalScalar( "RandomMixture-DefaultAlpha", 5.0 );
  setAsNumericalScalar( "RandomMixture-DefaultBeta", 8.5 );
  setAsNumericalScalar( "RandomMixture-DefaultPDFEpsilon", 1.0e-10 );
  setAsNumericalScalar( "RandomMixture-DefaultCDFEpsilon", 1.0e-10 );
  setAsNumericalScalar( "RandomMixture-GraphPDFEpsilon", 1.0e-5 );
  setAsNumericalScalar( "RandomMixture-GraphCDFEpsilon", 1.0e-5 );
  setAsUnsignedInteger( "RandomMixture-SmallSize", 100 );
  setAsUnsignedInteger( "RandomMixture-ProjectionDefaultSize", 25 );
  setAsUnsignedInteger( "RandomMixture-MarginalIntegrationNodesNumber", 128 );
  setAsUnsignedInteger( "RandomMixture-MaximumIntegrationNodesNumber", 1024 );

  // NumericalMathEvaluation parameters //
  setAsUnsignedInteger( "NumericalMathEvaluationImplementation-DefaultPointNumber", 129 );

  // DualLinearCombinationEvaluationImplementation //
  setAsNumericalScalar( "DualLinearCombinationEvaluationImplementation-SmallCoefficient", 0.0);

  // LinearCombinationEvaluationImplementation //
  setAsNumericalScalar( "LinearCombinationEvaluationImplementation-SmallCoefficient", 0.0);

  // DistFunc parameters //
  setAsNumericalScalar( "DistFunc-Precision", 1.0e-14 );
  setAsUnsignedInteger( "DistFunc-MaximumIteration", 5000 );

  // RootStrategyImplementation parameters //
  setAsNumericalScalar( "RootStrategyImplementation-DefaultMaximumDistance", 8.0 );
  setAsNumericalScalar( "RootStrategyImplementation-DefaultStepSize", 1.0 );

  // Simulation parameters //
  setAsUnsignedInteger( "Simulation-DefaultMaximumOuterSampling", 1000 );
  setAsNumericalScalar( "Simulation-DefaultMaximumCoefficientOfVariation", 1.0e-1 );
  setAsNumericalScalar( "Simulation-DefaultMaximumStandardDeviation", 0.0 );
  setAsUnsignedInteger( "Simulation-DefaultBlockSize", 1 );

  // SimulationResultImplementation parameters //
  setAsNumericalScalar( "SimulationResultImplementation-DefaultConfidenceLevel", 0.95 );

  // SimulationSensitivityAnalysis parameters //
  setAsUnsignedInteger( "SimulationSensitivityAnalysis-DefaultSampleMargin", 400 );

  // AnalyticalResult parameters //
  setAsNumericalScalar( "AnalyticalResult-DefaultWidth", 1.0 );

  // StrongMaximumTest parameters //
  setAsNumericalScalar( "StrongMaximumTest-DefaultDeltaPrecision", 1.0e-7 );
  setAsNumericalScalar( "StrongMaximumTest-Epsilon", 1.0e-10 );

  // CleaningStrategy parameters //
  setAsUnsignedInteger( "CleaningStrategy-DefaultMaximumSize", 20 );
  setAsNumericalScalar( "CleaningStrategy-DefaultSignificanceFactor", 1.0e-4 );

  // FunctionalChaosAlgorithm parameters //
  setAsNumericalScalar( "FunctionalChaosAlgorithm-DefaultMaximumResidual", 1.0e-6 );
  setAsNumericalScalar( "FunctionalChaosAlgorithm-QNorm", 0.5);
  setAsNumericalScalar( "FunctionalChaosAlgorithm-PValueThreshold", 1.0e-3);
  setAsUnsignedInteger( "FunctionalChaosAlgorithm-SmallSampleSize", 1000);
  setAsUnsignedInteger( "FunctionalChaosAlgorithm-LargeSampleSize", 10000);
  setAsUnsignedInteger( "FunctionalChaosAlgorithm-MaximumTotalDegree", 10 );

  // KrigingAlgorithm parameters //
  setAsNumericalScalar( "KrigingAlgorithm-StartingScaling", 1.0e-13 );
  setAsNumericalScalar( "KrigingAlgorithm-MaximalScaling", 1.0e5 );
  set("KrigingAlgorithm-LinearAlgebra", "LAPACK");

  // SquaredExponential parameters //
  setAsNumericalScalar( "SquaredExponential-DefaultTheta", 10.0 );

  // AbsoluteExponential parameters //
  setAsNumericalScalar( "AbsoluteExponential-DefaultTheta", 10.0 );

  // GeneralizedExponential parameters //
  setAsNumericalScalar( "GeneralizedExponential-DefaultTheta", 10.0 );

  // MaternModel parameters //
  setAsNumericalScalar( "MaternModel-DefaultNu", 1.5 );
  setAsNumericalScalar( "MaternModel-DefaultTheta", 10.0 );

  // WeightedExperiment parameters //
  setAsUnsignedInteger( "WeightedExperiment-DefaultSize", 100 );

  // GaussProductExperiment parameters //
  setAsUnsignedInteger( "GaussProductExperiment-DefaultMarginalDegree", 5 );

  // HyperbolicAnisotropicEnumerateFunction parameters //
  setAsNumericalScalar( "HyperbolicAnisotropicEnumerateFunction-DefaultQ", 0.4 );

  // MarginalTransformationEvaluation parameters //
  setAsNumericalScalar( "MarginalTransformationEvaluation-DefaultTailThreshold", 0.99 );
  setAsUnsignedInteger( "MarginalTransformationEvaluation-Simplify", 1 );
  setAsNumericalScalar( "MarginalTransformationEvaluation-ParametersEpsilon", 1e-14 );

  // DistributionImplementation parameters //
  setAsUnsignedInteger( "DistributionImplementation-DefaultPointNumber", 129 );
  setAsNumericalScalar( "DistributionImplementation-DefaultQuantileEpsilon", 1.0e-12 );
  setAsNumericalScalar( "DistributionImplementation-DefaultPDFEpsilon", 1.0e-14 );
  setAsNumericalScalar( "DistributionImplementation-DefaultCDFEpsilon", 1.0e-14 );
  setAsUnsignedInteger( "DistributionImplementation-DefaultQuantileIteration", 100 );
  setAsNumericalScalar( "DistributionImplementation-QMin", 0.15 );
  setAsNumericalScalar( "DistributionImplementation-QMax", 0.85 );
  setAsUnsignedInteger( "DistributionImplementation-DefaultIntegrationNodesNumber", 255 );
  setAsUnsignedInteger( "DistributionImplementation-DefaultLevelNumber", 10 );
  setAsUnsignedInteger( "DistributionImplementation-DefaultQuantileCacheSize", 128 );
  setAsUnsignedInteger( "DistributionImplementation-Parallel", 0 );
  setAsUnsignedInteger( "DistributionImplementation-CharacteristicFunctionBlockMax", 20 );
  setAsUnsignedInteger( "DistributionImplementation-CharacteristicFunctionNMax", 1000000 );

  // ContinuousDistribution parameters //
  setAsUnsignedInteger( "ContinuousDistribution-DefaultIntegrationNodesNumber", 256 );

  // DiscreteDistribution parameters //
  setAsNumericalScalar( "DiscreteDistribution-SupportEpsilon", 1.0e-14 );

  // DistributionImplementationFactory parameters //
  setAsUnsignedInteger( "DistributionImplementationFactory-DefaultBootstrapSize", 100 );

  // OrderStatisticsMarginalChecker //
  setAsUnsignedInteger( "OrderStatisticsMarginalChecker-QuantileIteration", 100 );
  setAsNumericalScalar( "OrderStatisticsMarginalChecker-OptimizationEpsilon", 1.0e-7 );

  // MaximumEntropyOrderStatisticsDistribution //
  setAsBool( "MaximumEntropyOrderStatisticsDistribution-UseApproximation", true );
  setAsBool( "MaximumEntropyOrderStatisticsDistribution-CheckMarginals", true );
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-ExponentialFactorDiscretization", 100 );
  setAsNumericalScalar( "MaximumEntropyOrderStatisticsDistribution-SupportShift", 1.0e-15 );
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-MaximumApproximationSubdivision", 2 );
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-MaximumQuantileIteration", 10);
  setAsUnsignedInteger( "MaximumEntropyOrderStatisticsDistribution-CDFIntegrationNodesNumber", 16);

  // WhiteNoise parameters //
  setAsNumericalScalar( "WhiteNoise-MeanEpsilon", 1.0e-14 );

  // HMatrix parameters //
  setAsNumericalScalar( "HMatrix-AdmissibilityFactor", 2.0 );
  setAsUnsignedInteger( "HMatrix-CompressionMethod", 1 );
  setAsNumericalScalar( "HMatrix-AssemblyEpsilon", 1.0e-4);
  setAsNumericalScalar( "HMatrix-RecompressionEpsilon", 1.0e-4);
  setAsUnsignedInteger( "HMatrix-MaxLeafSize", 100);
  setAsUnsignedInteger( "HMatrix-MaxParallelLeaves", 5000);
  setAsNumericalScalar( "HMatrix-ValidationError", 0.0);
  setAsUnsignedInteger( "HMatrix-ValidationRerun", 0);
  setAsUnsignedInteger( "HMatrix-ValidationDump", 0);
  setAsBool( "HMatrix-ForceSequential", false);

  // TemporalNormalProcess parameters //
  setAsNumericalScalar( "TemporalNormalProcess-StartingScaling", 1.0e-13 );
  setAsNumericalScalar( "TemporalNormalProcess-MaximalScaling", 1.0e5 );
  setAsUnsignedInteger( "TemporalNormalProcess-GibbsMaximumIteration", 100 );

  // SpectralNormalProcess parameters //
  setAsNumericalScalar( "SpectralNormalProcess-StartingScaling", 1.0e-13 );
  setAsNumericalScalar( "SpectralNormalProcess-MaximalScaling", 1.0e5 );
  setAsUnsignedInteger( "SpectralNormalProcess-CholeskyCacheSize", 16384 );

  // WhittleFactory parameters //
  setAsNumericalScalar( "WhittleFactory-DefaultRhoBeg", 0.1 );
  setAsNumericalScalar( "WhittleFactory-DefaultRhoEnd", 1.0e-10 );
  setAsNumericalScalar( "WhittleFactory-DefaultMaxFun", 2000 );
  setAsNumericalScalar( "WhittleFactory-DefaultStartingPointScale", 1.0 );
  setAsNumericalScalar( "WhittleFactory-RootEpsilon", 1.0e-6);

  // BoxCoxFactory parameters //
  setAsNumericalScalar( "BoxCoxFactory-DefaultRhoBeg", 0.1 );
  setAsNumericalScalar( "BoxCoxFactory-DefaultRhoEnd", 1.0e-10 );
  setAsUnsignedInteger( "BoxCoxFactory-DefaultMaxFun", 2000 );
  setAsUnsignedInteger( "BoxCoxFactory-DefaultPointNumber", 201 );

  // VisualTest parameters //
  setAsUnsignedInteger( "VisualTest-KendallPlot-MonteCarloSize", 100 );

  // CalibrationStrategyImplementation parameters //
  setAsNumericalScalar( "CalibrationStrategyImplementation-DefaultLowerBound", 0.117 ); // = 0.5 * 0.234
  setAsNumericalScalar( "CalibrationStrategyImplementation-DefaultUpperBound", 0.468 ); // = 2.0 * 0.234
  setAsNumericalScalar( "CalibrationStrategyImplementation-DefaultShrinkFactor", 0.8 );
  setAsNumericalScalar( "CalibrationStrategyImplementation-DefaultExpansionFactor", 1.2 );
  setAsUnsignedInteger( "CalibrationStrategyImplementation-DefaultCalibrationStep", 100 );

  // MCMC parameters //
  setAsUnsignedInteger("MCMC-DefaultBurnIn", 0);
  setAsUnsignedInteger("MCMC-DefaultThinning", 1);


  // ARMALikelihoodFactory parameters //
  setAsNumericalScalar( "ARMALikelihoodFactory-StartingScaling", 1.0e-13 );
  setAsNumericalScalar( "ARMALikelihoodFactory-MaximalScaling", 1.0e5 );
  setAsNumericalScalar( "ARMALikelihoodFactory-DefaultRhoBeg", 0.01 );
  setAsNumericalScalar( "ARMALikelihoodFactory-DefaultRhoEnd", 1.0e-10 );
  setAsUnsignedInteger( "ARMALikelihoodFactory-DefaultMaxFun", 10000 );
  setAsNumericalScalar( "ARMALikelihoodFactory-DefaultStartingPointScale", 1.0 );
  setAsNumericalScalar( "ARMALikelihoodFactory-RootEpsilon", 1.0e-6);

  // FittingTest parameters //
  setAsUnsignedInteger( "FittingTest-ChiSquaredMinFrequency", 5 );

  // LeastSquaresMetaModelSelection parameters //
  set("LeastSquaresMetaModelSelection-DecompositionMethod", "SVD");
  setAsNumericalScalar("LeastSquaresMetaModelSelection-MaximumErrorFactor", 2.0);
  setAsNumericalScalar("LeastSquaresMetaModelSelection-ErrorThreshold", 0.0);

  // CholeskyMethod parameters //
  setAsUnsignedInteger("CholeskyMethod-LargeCase", 128);

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
std::ostream & operator <<(std::ostream & os, const ResourceMapInstance & obj)
{
  return os << obj.lock().__repr__();
}

OStream & operator <<(OStream & OS, const ResourceMapInstance & obj)
{
  return OS << obj.lock().__repr__();
}





END_NAMESPACE_OPENTURNS
