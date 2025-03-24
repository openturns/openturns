//                                               -*- C++ -*-
/**
 *  @brief The header file that declares exit codes for tests
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_OTTESTCODE_HXX
#define OPENTURNS_OTTESTCODE_HXX

#include "openturns/OTconfig.hxx"

#include <exception>
#include <string>         // for std::string
#include <sstream>        // for std::ostringstream
#include <cstdlib>        // for exit codes

#include "openturns/RandomGenerator.hxx"
#include "openturns/OStream.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/PlatformInfo.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TBB.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Mesh.hxx"
#include "openturns/Field.hxx"
#include "openturns/ProcessSample.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/FittingTest.hxx"
#include "openturns/DomainEvent.hxx"
#include "openturns/ProbabilitySimulationAlgorithm.hxx"
#include "openturns/RosenblattEvaluation.hxx"
#include "openturns/InverseRosenblattEvaluation.hxx"

#define TESTPREAMBLE { OT::TBB::Enable(); }

BEGIN_NAMESPACE_OPENTURNS

/**
 * @brief      The namespace for test specific definitions
 */

namespace Test
{

/**
 * @typedef int ExitCodeValue
 * ExitCodeValue is the type of the exit code to the operating
 * system. The actual type is determined at configuration time.
 */
typedef int ExitCodeValue;


/**
 * @class ExitCode
 * Class ExitCode is a compound that declares exit codes to the
 * operating system when doing the test suite of the project.
 * Each code has a special meaning to the operating system, and
 * precisely to the compilation tools (automake et al.) that use
 * them to know if the test succeeded or failed.
 */

class ExitCode
{

public:
  /** You should return Success when it's OK */
  static const ExitCodeValue Success = EXIT_SUCCESS;

  /** You should return Error when something went wrong */
  static const ExitCodeValue Error = EXIT_FAILURE;

  /** You should return ExpectedToFail when it failed as it should */
  static const ExitCodeValue ExpectedToFail = 77;

}; /* class ExitCode */




/**
 * @class TestFailed
 *
 * Class TestFailed is an exception derived class that should be
 * used to throw exception in unitary test code.
 */
class TestFailed : public std::exception
{
public:
  TestFailed(const std::string & message) : message_(message) {}
  ~TestFailed() throw() {}
  const char * what() const throw()
  {
    return message_.c_str();
  }

private:

  const std::string message_;
};

inline std::ostream & operator <<(std::ostream & os, const TestFailed & obj)
{
  return os << "*** EXCEPTION ***" << std::endl
         << "TestFailed : " << obj.what() << std::endl
         << "*****************";
}


/**
 * @fn streamObject(const T & anObject)
 *
 * Try to stream an object onto an ostream object (std::cout).
 * This method tests the operator << of the object.
 */
template <class T>
void streamObject(const T & anObject)
{
  std::cout << "streamObject(const T & anObject)"
            << std::endl;
  OT::OStream fullprint(std::cout);
  fullprint << anObject << std::endl;
}


/**
 * @fn show_className()
 *
 * Try to get the name of the class. Check the GetClassName static
 * method.
 */
template <class T>
void showClassName()
{
  std::cout << "Testing class "
            << T::GetClassName()
            << std::endl;
}


/**
 * @fn checkConstructorAndDestructor()
 *
 * Try to instantiate an object and delete it. This method tests the
 * default constructor and the destructor ot the class.
 */
template <class T>
void checkConstructorAndDestructor()
{
  std::cout << "checkConstructorAndDestructor()"
            << std::endl;

  // Call the default constructor
  T anObject;
}


/**
 * @fn checkCopyConstructor()
 *
 * Try to instantiate an object and copy-construct it.
 */
template <class T>
void checkCopyConstructor()
{
  std::cout << "checkCopyConstructor()"
            << std::endl;

  // Call the default constructor
  T anObject;

  // Call the copy-constructor
  T anCopiedObject( anObject );
}


/**
 * @fn areSameObjects(const T & firstObject, const T & secondObject)
 *
 * Try to compare two objects supposed to be identical.
 * This method tests the operator == of the object.
 */
template <class T>
OT::Bool areSameObjects(const T & firstObject,
                        const T & secondObject)
{
  std::cout << "areSameObjects(const T & firstObject, const T & secondObject)"
            << std::endl;

  return (firstObject == secondObject);
}


/**
 * @fn areDifferentObjects(const T & firstObject, const T & secondObject)
 *
 * Try to compare two objects supposed to be different.
 * This method tests the operator != of the object.
 */
template <class T>
OT::Bool areDifferentObjects(const T & firstObject,
                             const T & secondObject)
{
  std::cout << "areDifferentObjects(const T & firstObject, const T & secondObject)"
            << std::endl;

  return (firstObject != secondObject);
}


/**
 * @fn checkClassWithoutClassName()
 *
 * Try to check the standard functionnalities of the class
 */
template <class T>
void checkClassWithoutClassName()
{
  checkConstructorAndDestructor<T>();
  checkCopyConstructor<T>();

  T object1;
  streamObject<T>(object1);

  T object2(object1);
  streamObject<T>(object2);

  // default compares pointers, but we want semantic comparison
  if (T::GetClassName() == "PersistentObject")
    return;

  if (! areSameObjects<T>(object1, object2))
  {
    throw TestFailed("areSameObjects<T>(object1, object2)");
  }

  if (areDifferentObjects<T>(object1, object2))
  {
    throw TestFailed("areDifferentObjects<T>(object1, object2)");
  }
}


/**
 * @fn checkClassWithClassName()
 *
 * Try to check some basic functionnalities of the class
 */
template <class T>
void checkClassWithClassName()
{
  showClassName<T>();
  checkClassWithoutClassName<T>();
}


inline void assert_almost_equal(const Scalar a, const Scalar b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  if (!SpecFunc::IsNormal(a) || !SpecFunc::IsNormal(b))
    throw TestFailed(OSS() << "Value a: " << a << " or b: " << b << " are invalid " << errMsg);
  if (std::abs(a - b) > atol + rtol * std::abs(b))
  {
    throw TestFailed(OSS() << "Value " << a << " is not close enough to " << b << " " << errMsg);
  }
}

inline void assert_almost_equal(const Complex & a, const Complex & b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  if (!SpecFunc::IsNormal(a.real()) || !SpecFunc::IsNormal(b.real()) ||
      !SpecFunc::IsNormal(a.imag()) || !SpecFunc::IsNormal(b.imag()))
    throw TestFailed(OSS() << "Value a: " << a << " or b: " << b << " are invalid " << errMsg);
  if (std::abs(a - b) > atol + rtol * std::abs(b))
  {
    throw TestFailed(OSS() << "Value " << a << " is not close enough to " << b << " " << errMsg);
  }
}

inline void assert_almost_equal(const Indices &a, const Indices &b, const String errMsg = "")
{
  if (a.getSize() != b.getSize())
    throw InvalidArgumentException(HERE) << "A and B must have the same size " << a.getSize() << " vs " << b.getSize();
  const UnsignedInteger size = a.getSize();
  for (UnsignedInteger j = 0; j < size; ++j)
  {
    assert_almost_equal(a[j], b[j], 0, 0, errMsg);
  }
}

inline void assert_almost_equal(const Point & a, const Point & b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  if (a.getDimension() != b.getDimension())
    throw InvalidArgumentException(HERE) << "A and B must have the same dimension " << a.getDimension() << " vs " << b.getDimension();
  const UnsignedInteger dimension = a.getDimension();
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    assert_almost_equal(a[j], b[j], rtol, atol, errMsg);
  }
}


inline void assert_almost_equal(const Sample & a, const Sample & b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  if (a.getSize() != b.getSize())
    throw InvalidArgumentException(HERE) << "A and B must have the same size " << a.getSize() << " vs " << b.getSize();
  if (a.getDimension() != b.getDimension())
    throw InvalidArgumentException(HERE) << "A and B must have the same dimension " << a.getDimension() << " vs " << b.getDimension();
  const UnsignedInteger size = a.getSize();
  const UnsignedInteger dimension = a.getDimension();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      assert_almost_equal(a(i, j), b(i, j), rtol, atol, errMsg);
    }
  }
}

inline void assert_almost_equal(const Matrix &a, const Matrix &b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  if (a.getNbRows() != b.getNbRows())
    throw InvalidArgumentException(HERE) << "A and B must have the same row number " << a.getNbRows() << " vs " << b.getNbRows();
  if (a.getNbColumns() != b.getNbColumns())
    throw InvalidArgumentException(HERE) << "A and B must have the same column number " << a.getNbColumns() << " vs " << b.getNbColumns();
  const UnsignedInteger rows = a.getNbRows();
  const UnsignedInteger columns = a.getNbColumns();

  for (UnsignedInteger j = 0; j < columns; ++ j)
  {
    for (UnsignedInteger i = 0; i < rows; ++ i)
    {
      assert_almost_equal(a(i, j), b(i, j), rtol, atol, errMsg);
    }
  }
}

inline void assert_almost_equal(const SymmetricMatrix &a, const SymmetricMatrix &b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  if (a.getDimension() != b.getDimension())
    throw InvalidArgumentException(HERE) << "A and B must have the same dimension " << a.getDimension() << " vs " << b.getDimension();
  const UnsignedInteger dimension = a.getDimension();

  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    for (UnsignedInteger i = j; i < dimension; ++i)
    {
      assert_almost_equal(a(i, j), b(i, j), rtol, atol, errMsg);
    }
  }
}

inline void assert_almost_equal(const CovarianceMatrix &a, const CovarianceMatrix &b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  assert_almost_equal(SymmetricMatrix(*a.getImplementation()), SymmetricMatrix(*b.getImplementation()), rtol, atol, errMsg);
}

inline void assert_almost_equal(const Tensor & a, const Tensor & b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  if (a.getNbSheets() != b.getNbSheets())
    throw InvalidArgumentException(HERE) << "A and B must have the same sheet number " << a.getNbSheets() << " vs " << b.getNbSheets();
  const UnsignedInteger sheets = a.getNbSheets();
  for (UnsignedInteger k = 0; k < sheets; ++ k)
    assert_almost_equal(a.getSheet(k), b.getSheet(k), rtol, atol, errMsg);
}

inline void assert_almost_equal(const Distribution &a, const Distribution &b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  if (a.getImplementation()->getClassName() != b.getImplementation()->getClassName())
    throw InvalidArgumentException(HERE) << "A and B must be the same distribution. A is a " << a.getImplementation()->getClassName() << " whereas B is a " << b.getImplementation()->getClassName();

  if (a.getParameterDimension() != b.getParameterDimension())
    throw InvalidArgumentException(HERE) << "A and B must have the same number of parameters. A has " << a.getParameterDimension() << " parameters whereas B has " << b.getParameterDimension() << " parameters.";

  assert_almost_equal(a.getParameter(), b.getParameter(), rtol, atol, errMsg);
}

inline void assert_almost_equal(const Mesh & a, const Mesh & b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  assert_almost_equal(a.getVertices(), b.getVertices(), rtol, atol, errMsg);
  if (a.getSimplices() != b.getSimplices())
    throw InvalidArgumentException(HERE) << "A and B must have the same simplices";
}

inline void assert_almost_equal(const Field & a, const Field & b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  assert_almost_equal(a.getMesh(), b.getMesh(), rtol, atol, errMsg);
  assert_almost_equal(a.getValues(), b.getValues(), rtol, atol, errMsg);
}

inline void assert_almost_equal(const ProcessSample & a, const ProcessSample & b, const Scalar rtol = 1.0e-5, const Scalar atol = 1.0e-8, const String errMsg = "")
{
  assert_almost_equal(a.getMesh(), b.getMesh(), rtol, atol, errMsg);
  if (a.getSize() != b.getSize())
    throw InvalidArgumentException(HERE) << "A and B must have the same size";
  for (UnsignedInteger j = 0; j < a.getSize(); ++j)
    assert_almost_equal(a.getField(j).getValues(), b.getField(j).getValues(), rtol, atol, errMsg);
}

template <typename T>
void assert_equal(const T & a, const T & b, const String errMsg = "")
{
  if (a != b)
  {
    throw TestFailed(OSS() << "Value " << a << " is not equal to " << b << " " << errMsg);
  }
}


class DistributionValidation
{
public:
  DistributionValidation() {}

  explicit DistributionValidation(const Distribution & distribution)
    : distribution_(distribution) {}

  void run() const
  {
    checkPrint();
    checkGeneral();
    checkRealization();
    checkComparison();
    if (enablePDF_)
    {
      checkPDF();
      checkLogPDF();
    }
    if (enableCDF_)
      checkCDF();
    if (enableDDF_)
      checkDDF();
    if (enableComplementaryCDF_)
      checkComplementaryCDF();
    checkSurvival();
    checkInverseSurvival();
    checkQuantile();
    if (enableProbability_)
      checkProbability();
    checkFitting();
    if (enableEntropy_)
      checkEntropy();
    checkMoments();
    if (enableParameters_)
      checkParameters();
    if (enableGradient_)
    {
      checkPDFGradient();
      checkCDFGradient();
    }
    if (enableMinimumVolumeInterval_)
      checkMinimumVolumeInterval();
    if (enableMinimumVolumeLevelSet_)
      checkMinimumVolumeLevelSet();
    if (enableConfidenceInterval_)
      checkConfidenceInterval();
    if (enableCharacteristicFunction_)
      checkCharacteristicFunction();
    if (enableGeneratingFunction_)
      checkGeneratingFunction();
    if (enableConditional_)
      checkConditional();
    if (enableTransformation_)
      checkTransformation();
  }

  void skipPDF()
  {
    enablePDF_ = false;
  }
  void skipCDF()
  {
    enableCDF_ = false;
  }
  void skipComplementaryCDF()
  {
    enableComplementaryCDF_ = false;
  }
  void skipDDF()
  {
    enableDDF_ = false;
  }
  void skipMoments()
  {
    enableMoments_ = false;
  }
  void skipCorrelation()
  {
    enableCorrelation_ = false;
  }
  void skipDependenceMeasures()
  {
    enableDependenceMeasures_ = false;
  }
  void skipMinimumVolumeLevelSet()
  {
    enableMinimumVolumeLevelSet_ = false;
  }
  void skipMinimumVolumeInterval()
  {
    enableMinimumVolumeInterval_ = false;
  }
  void skipConfidenceInterval()
  {
    enableConfidenceInterval_ = false;
  }
  void skipParameters()
  {
    enableParameters_ = false;
  }
  void skipProbability()
  {
    enableProbability_ = false;
  }
  void skipCharacteristicFunction()
  {
    enableCharacteristicFunction_ = false;
  }
  void skipGeneratingFunction()
  {
    enableGeneratingFunction_ = false;
  }
  void skipGradient()
  {
    enableGradient_ = false;
  }
  void skipEntropy()
  {
    enableEntropy_ = false;
  }
  void skipConditional()
  {
    enableConditional_ = false;
  }
  void skipTransformation()
  {
    enableTransformation_ = false;
  }
  void setEntropyTolerance(const Scalar entropyTolerance)
  {
    entropyTolerance_ = entropyTolerance;
  }
  void setCDFTolerance(const Scalar cdfTolerance)
  {
    cdfTolerance_ = cdfTolerance;
  }
  void setPDFTolerance(const Scalar pdfTolerance)
  {
    pdfTolerance_ = pdfTolerance;
  }
  void setDDFTolerance(const Scalar ddfTolerance)
  {
    ddfTolerance_ = ddfTolerance;
  }
  void setParameterTolerance(const Scalar parameterTolerance)
  {
    parameterTolerance_ = parameterTolerance;
  }
  void setQuantileTolerance(const Scalar quantileTolerance)
  {
    quantileTolerance_ = quantileTolerance;
  }
  void setDomainTolerance(const Scalar domainTolerance)
  {
    domainTolerance_ = domainTolerance;
  }
  void setEntropySamplingSize(const UnsignedInteger entropySamplingSize)
  {
    entropySamplingSize_ = entropySamplingSize;
  }
  void setDomainSamplingSize(const UnsignedInteger domainSamplingSize)
  {
    domainSamplingSize_ = domainSamplingSize;
  }
  void setMeanTolerance (const Scalar meanTolerance)
  {
    meanTolerance_ = meanTolerance;
  }
  void setStandardDeviationTolerance (const Scalar standardDeviationTolerance)
  {
    standardDeviationTolerance_ = standardDeviationTolerance;
  }
  void setSkewnessTolerance (const Scalar skewnessTolerance)
  {
    skewnessTolerance_ = skewnessTolerance;
  }
  void setKurtosisTolerance (const Scalar kurtosisTolerance)
  {
    kurtosisTolerance_ = kurtosisTolerance;
  }
  void setCorrelationTolerance (const Scalar correlationTolerance)
  {
    correlationTolerance_ = correlationTolerance;
  }
  void setMomentsSamplingSize(const Scalar momentsSamplingSize)
  {
    momentsSamplingSize_ = momentsSamplingSize;
  }
  void setPDFSamplingSize(const Scalar pdfSamplingSize)
  {
    pdfSamplingSize_ = pdfSamplingSize;
  }
  void setCDFSamplingSize(const Scalar cdfSamplingSize)
  {
    cdfSamplingSize_ = cdfSamplingSize;
  }
  void setFittingSamplingSize(const Scalar fittingSamplingSize)
  {
    fittingSamplingSize_ = fittingSamplingSize;
  }

private:

  void checkPrint() const
  {
    LOGTRACE(OSS() << distribution_.__repr__());
    LOGTRACE(OSS() << distribution_.__str__());
    LOGTRACE(OSS() << distribution_.__repr_markdown__());
    LOGTRACE(OSS() << distribution_._repr_html_());
  }

  void checkGeneral() const
  {
    LOGTRACE(OSS() << "elliptical=" << distribution_.isElliptical());
    LOGTRACE(OSS() << "continuous=" << distribution_.isContinuous());
    LOGTRACE(OSS() << "discrete=" << distribution_.isDiscrete());
    LOGTRACE(OSS() << "integral=" << distribution_.isIntegral());
    LOGTRACE(OSS() << "copula=" << distribution_.isCopula());

    Distribution standardRep(distribution_.getStandardRepresentative());
    LOGTRACE(OSS() << "Standard representative=" << standardRep);
  }

  void checkRealization() const
  {
    LOGTRACE(OSS() << "checking realization...");
    RandomGeneratorState initialState(RandomGenerator::GetState());
    const Point x(distribution_.getRealization());
    RandomGenerator::SetState(initialState);
    LOGTRACE(OSS() << "x=" << x << " dim=" << distribution_.getDimension());
    if (x.getDimension() != distribution_.getDimension())
      throw TestFailed(OSS() << "dim(realization)==dimension failed for " << distribution_);
  }

  void checkGeneratingFunction() const
  {
    if (distribution_.isDiscrete() && (distribution_.getDimension() == 1))
    {
      LOGTRACE(OSS() << "checking generating function...");
      const Complex z(0.3, 0.7);
      Complex GF = distribution_.computeGeneratingFunction(z);
      LOGTRACE(OSS() << "generating function=" << GF);
      Complex LGF = distribution_.computeLogGeneratingFunction(z);
      LOGTRACE(OSS() << "log generating function=" << LGF);
    }
  }

  void checkCharacteristicFunction() const
  {
    if (distribution_.getDimension() == 1)
    {
      LOGTRACE(OSS() << "checking characteristic function...");
      const Scalar t = 0.0;
      Complex CF = distribution_.computeCharacteristicFunction(t);
      LOGTRACE(OSS() << "characteristic function=" << CF);
      assert_almost_equal(CF, Complex(1.0, 0.0));
      Complex LCF = distribution_.computeLogCharacteristicFunction(t);
      LOGTRACE(OSS() << "log characteristic function=" << LCF);
      assert_almost_equal(LCF, Complex(0.0, 0.0));
    }
  }

  void checkComparison() const
  {
    LOGTRACE(OSS() << "checking comparison operators...");
    if (!(distribution_ == distribution_))
      throw TestFailed(OSS() << "operator==(self) failed for " << distribution_);
    if (distribution_ != distribution_)
      throw TestFailed(OSS() << "operator==(self) failed for " << distribution_);
    const Distribution distribution2(distribution_);
    if (!(distribution2 == distribution_))
      throw TestFailed(OSS() << "operator==(copy) failed for " << distribution_);
    if (distribution2 != distribution_)
      throw TestFailed(OSS() << "operator!=(copy) failed for " << distribution_);
    const Dirac dirac(42.0);
    if (distribution_ == dirac)
      throw TestFailed(OSS() << "operator==(other) failed for " << distribution_);
    if (!(distribution2 != dirac))
      throw TestFailed(OSS() << "operator!=(other) failed for " << distribution_);
  }

  void checkPDF() const
  {
    LOGTRACE(OSS() << "checking PDF...");
    const Sample sample(distribution_.getSample(pdfSamplingSize_));
    sample.exportToCSVFile("sample_pdf.csv");
    for (UnsignedInteger i = 0; i < sample.getSize(); ++ i)
    {
      const Point x(sample[i]);
      const Scalar pdf = distribution_.computePDF(x);
      LOGTRACE(OSS() << "x=" << x << " pdf=" << pdf);
      if (!(pdf > 0.0))
        throw TestFailed(OSS() << "pdf(x) failed for " << distribution_);
    }
  }

  void checkLogPDF() const
  {
    LOGTRACE(OSS() << "checking LogPDF...");
    const Sample sample(distribution_.getSample(pdfSamplingSize_));
    sample.exportToCSVFile("sample_logpdf.csv");
    for (UnsignedInteger i = 0; i < sample.getSize(); ++ i)
    {
      const Point x(sample[i]);
      const Scalar lpdf2 = std::log(distribution_.computePDF(x));
      LOGTRACE(OSS() << "log(pdf)=" << lpdf2);
      const Scalar lpdf1 = distribution_.computeLogPDF(x);
      LOGTRACE(OSS() << "logpdf  =" << lpdf1);
      assert_almost_equal(lpdf1, lpdf2);
    }
  }

  void checkCDF() const
  {
    LOGTRACE(OSS() << "checking CDF...");
    // check CDF at bounds
    const UnsignedInteger dimension = distribution_.getDimension();
    const Point epsilon(dimension, std::pow(ResourceMap::GetAsScalar("Distribution-DefaultCDFEpsilon"), 1.0 / 3.0));
    const Scalar cdflb = distribution_.computeCDF(distribution_.getRange().getLowerBound() - epsilon);
    LOGTRACE(OSS() << "cdf(lb)=" << cdflb);
    assert_almost_equal(cdflb, 0.0, cdfTolerance_, cdfTolerance_,  "cdf(lb) " + distribution_.__repr__());
    const Scalar cdfub = distribution_.computeCDF(distribution_.getRange().getUpperBound() + epsilon);
    LOGTRACE(OSS() << "cdf(ub)=" << cdfub);
    assert_almost_equal(cdfub, 1.0, cdfTolerance_, cdfTolerance_,  "cdf(ub) " + distribution_.__repr__());

    if (distribution_.isContinuous() && (dimension == 1))
    {
      const Sample sample(distribution_.getSample(cdfSamplingSize_));
      sample.exportToCSVFile("sample_cdf.csv");
      for (UnsignedInteger i = 0; i < sample.getSize(); ++ i)
      {
        const Point x(sample[i]);
        const Scalar pdf1 = distribution_.computePDF(x);
        LOGTRACE(OSS() << "pdf    =" << pdf1);
        const Scalar cdf1 = distribution_.computeCDF(Point({x[0] + epsilon[0]}));
        const Scalar cdf2 = distribution_.computeCDF(Point({x[0] - epsilon[0]}));
        const Scalar pdf2 = (cdf1 - cdf2) / (2.0 * epsilon[0]);
        LOGTRACE(OSS() << "pdf(fd)=" << pdf2);
        assert_almost_equal(pdf1, pdf2, pdfTolerance_, pdfTolerance_,  "cdf " + distribution_.__repr__());
      }
    }
  }

  void checkDDF() const
  {
    if (distribution_.isContinuous())
    {
      LOGTRACE(OSS() << "checking DDF...");
      const Sample sample(distribution_.getSample(10));
      sample.exportToCSVFile("sample_ddf.csv");
      const Scalar epsilon = std::pow(ResourceMap::GetAsScalar("DistFunc-Precision"), 1.0 / 3.0);
      for (UnsignedInteger i = 0; i < sample.getSize(); ++ i)
      {
        const Point x(sample[i]);
        const Point ddf1 = distribution_.computeDDF(x);
        LOGTRACE(OSS() << "ddf    =" << ddf1 << " x=" << x);
        Point ddf2(distribution_.getDimension());
        if (ddf1.getDimension() != distribution_.getDimension())
          throw TestFailed(OSS() << "expected ddf of dimension " << distribution_.getDimension() << " got " << ddf1.getDimension() << " for " << distribution_);
        for (UnsignedInteger j = 0; j < distribution_.getDimension(); ++ j)
        {
          Point xp(x);
          xp[j] += epsilon;
          const Scalar pdf1 = distribution_.computePDF(xp);
          Point xm(x);
          xm[j] -= epsilon;
          const Scalar pdf2 = distribution_.computePDF(xm);
          ddf2[j] = (pdf1 - pdf2) / (2.0 * epsilon);
        }
        LOGTRACE(OSS() << "ddf(fd)=" << ddf2);
        assert_almost_equal(ddf1, ddf2, ddfTolerance_, ddfTolerance_, "ddf " + distribution_.__repr__());
      }
    }
  }

  void checkComplementaryCDF() const
  {
    if (distribution_.isContinuous() && !distribution_.isCopula() && (distribution_.getDimension() < 4))
    {
      LOGTRACE(OSS() << "checking CCDF...");
      const Sample sample(distribution_.getSample(cdfSamplingSize_));
      sample.exportToCSVFile("sample_ccdf.csv");
      for (UnsignedInteger i = 0; i < sample.getSize(); ++ i)
      {
        const Point x(sample[i]);
        const Scalar ccdf2 = 1.0 - distribution_.computeCDF(x);
        LOGTRACE(OSS() << " 1-cdf=" << ccdf2);
        const Scalar ccdf1 = distribution_.computeComplementaryCDF(x);
        LOGTRACE(OSS() << "  ccdf=" << ccdf1);
        assert_almost_equal(ccdf1, ccdf2, cdfTolerance_, cdfTolerance_, "ccdf " + distribution_.__repr__());
      }
    }
  }

  void checkSurvival() const
  {
    if (distribution_.getDimension() == 1)
    {
      LOGTRACE(OSS() << "checking survival...");
      const Point x(distribution_.getRealization());
      const Scalar survival = distribution_.computeSurvivalFunction(x);
      LOGTRACE(OSS() << "survival=" << survival);
      const Scalar survival2 = 1.0 - distribution_.computeCDF(x);
      LOGTRACE(OSS() << "   1-cdf=" << survival2);
      assert_almost_equal(survival, survival2, cdfTolerance_, cdfTolerance_, "survival " + distribution_.__repr__());
    }
  }

  void checkInverseSurvival() const
  {
    if (distribution_.isContinuous() && distribution_.getDimension() == 1)
    {
      LOGTRACE(OSS() << "checking inverse survival...");
      const Point inverseSurvival(distribution_.computeInverseSurvivalFunction(0.95));
      const Scalar survival = distribution_.computeSurvivalFunction(inverseSurvival);
      LOGTRACE(OSS() << "inverseSurvival=" << inverseSurvival << " survival=" << survival);
      assert_almost_equal(survival, 0.95, quantileTolerance_, quantileTolerance_, "inverse survival " + distribution_.__repr__());
    }
  }

  void checkQuantile() const
  {
    if (distribution_.isContinuous() && distribution_.getDimension() == 1)
    {
      LOGTRACE(OSS() << "checking quantile...");
      const Point quantile1 = distribution_.computeQuantile(0.95);
      if (!distribution_.getRange().contains(quantile1))
        throw TestFailed(OSS() << "quantile not in range for " << distribution_);
      const Scalar cdf1 = distribution_.computeCDF(quantile1);
      LOGTRACE(OSS() << "quantile=" << quantile1 << " cdf=" << cdf1);
      assert_almost_equal(cdf1, 0.95, quantileTolerance_, quantileTolerance_, "quantile " + distribution_.__repr__());

      const Point quantile2 = distribution_.computeQuantile(0.95, true);
      if (!distribution_.getRange().contains(quantile2))
        throw TestFailed(OSS() << "quantile not in range for " << distribution_);
      const Scalar cdf2 = distribution_.computeCDF(quantile2);
      LOGTRACE(OSS() << "quantile=" << quantile2 << " cdf=" << cdf2);
      assert_almost_equal(cdf2, 0.05, quantileTolerance_, quantileTolerance_, "quantile(tail) " + distribution_.__repr__());
    }
  }

  void checkProbability() const
  {
    if (distribution_.getDimension() == 1)
    {
      LOGTRACE(OSS() << "checking probability...");
      const Scalar proba1 = distribution_.computeProbability(distribution_.getRange());
      LOGTRACE(OSS() << "proba(range)=" << proba1);
      assert_almost_equal(proba1, 1.0, cdfTolerance_, cdfTolerance_, "proba(range) " + distribution_.__repr__());

      const Scalar proba2 = distribution_.computeProbability(Interval(-SpecFunc::Infinity, SpecFunc::Infinity));
      LOGTRACE(OSS() << "proba(R)=" << proba2);
      assert_almost_equal(proba2, 1.0, cdfTolerance_, cdfTolerance_, "proba(R) " + distribution_.__repr__());
    }
  }

  void checkPDFGradient() const
  {
    if (!distribution_.isContinuous())
      return;

    const Point x(distribution_.getRealization());
    const Point parameter(distribution_.getParameter());
    LOGTRACE(OSS() << "checking PDF gradient...");
    const Point pdfGr = distribution_.computePDFGradient(x);
    LOGTRACE(OSS() << "pdfgrad=    " << pdfGr.__str__() << " x=" << x.__str__() << " params=" << parameter.__str__());
    if (pdfGr.getDimension() != parameter.getDimension())
      throw TestFailed(OSS() << "wrong pdfGradient(x) dimension (" << pdfGr.getDimension() << ") expected (" << parameter.getDimension() << ") for " << distribution_);
    const Scalar epsilon = std::pow(ResourceMap::GetAsScalar("DistFunc-Precision"), 1.0 / 3.0);
    Point pdfgrfd(parameter.getDimension());
    for (UnsignedInteger j = 0; j < parameter.getDimension(); ++ j)
    {
      Distribution distributionClone(distribution_);
      Point param1(parameter);
      param1[j] += epsilon;
      distributionClone.setParameter(param1);
      const Scalar pdf1 = distributionClone.computePDF(x);
      Point param2(parameter);
      param2[j] -= epsilon;
      distributionClone.setParameter(param2);
      const Scalar pdf2 = distributionClone.computePDF(x);
      pdfgrfd[j] = (pdf1 - pdf2) / (2.0 * epsilon);

    }
    LOGTRACE(OSS() << "pdfgrad(fd)=" << pdfgrfd.__str__());
    assert_almost_equal(pdfGr, pdfgrfd, parameterTolerance_, parameterTolerance_, "wrong pdf gradient for " + distribution_.__repr__());
  }

  void checkCDFGradient() const
  {
    if (!distribution_.isContinuous())
      return;
    if (distribution_.getDimension() < 4)
    {
      LOGTRACE(OSS() << "checking CDF gradient...");
      const Point x(distribution_.getRealization());
      const Point parameter(distribution_.getParameter());
      const Point cdfGr = distribution_.computeCDFGradient(x);
      LOGTRACE(OSS() << "cdfgrad=    " << cdfGr.__str__() << " x=" << x.__str__() << " params=" << parameter.__str__());
      if (cdfGr.getDimension() != parameter.getDimension())
        throw TestFailed(OSS() << "wrong cdfGradient(x) dimension (" << cdfGr.getDimension() << ") expected (" << parameter.getDimension() << ") for " << distribution_);
      const Scalar epsilon = std::pow(ResourceMap::GetAsScalar("DistFunc-Precision"), 1.0 / 3.0);
      Point cdfgrfd(parameter.getDimension());
      for (UnsignedInteger j = 0; j < parameter.getDimension(); ++ j)
      {
        Distribution distributionClone(distribution_);
        Point param1(parameter);
        param1[j] += epsilon;
        distributionClone.setParameter(param1);
        const Scalar cdf1 = distributionClone.computeCDF(x);
        Point param2(parameter);
        param2[j] -= epsilon;
        distributionClone.setParameter(param2);
        const Scalar cdf2 = distributionClone.computeCDF(x);
        cdfgrfd[j] = (cdf1 - cdf2) / (2.0 * epsilon);
      }
      LOGTRACE(OSS() << "cdfgrad(fd)=" << cdfgrfd.__str__());
      assert_almost_equal(cdfGr, cdfgrfd, parameterTolerance_, parameterTolerance_, "wrong cdf gradient for " + distribution_.__repr__());
    }
  }

  void checkFitting() const
  {
    const Point x = distribution_.getRealization();
    LOGTRACE(OSS() << "oneRealization=" << x);
    if (x.getDimension() != distribution_.getDimension())
      throw TestFailed(OSS() << "wrong realization dimension for " << distribution_);

    if (distribution_.isContinuous())
    {
      if (!distribution_.getRange().contains(x))
        throw TestFailed(OSS() << "realization not in range for " << distribution_);
    }
    else if (distribution_.isDiscrete())
    {
      const Sample support(distribution_.getSupport());
      if (support.find(x) >= support.getSize())
        throw TestFailed(OSS() << "realization not in support for " << distribution_);
    }

    if (distribution_.getDimension() == 1)
    {
      LOGTRACE(OSS() << "checking fit with Kolmogorov/ChiSquared ...");
      UnsignedInteger size = fittingSamplingSize_;
      // Iteratively increase the sample size.
      for (UnsignedInteger i = 0; i < 2; ++ i)
      {
        Bool accepted = true;
        if (distribution_.isContinuous())
          accepted = FittingTest::Kolmogorov(distribution_.getSample(size), distribution_).getBinaryQualityMeasure();
        else if (distribution_.isDiscrete())
          accepted = FittingTest::ChiSquared(distribution_.getSample(size), distribution_).getBinaryQualityMeasure();
        LOGTRACE(OSS() << "Kolmogorov test for size " << size << " accepted=" << accepted);
        if (!accepted)
          throw TestFailed(OSS() << "kolmogorov test failed for " << distribution_);
        size *= 10;
      }
    }
  }

  void checkEntropy() const
  {
    if (distribution_.getDimension() == 1)
    {
      LOGTRACE(OSS() << "checking entropy...");
      const Scalar entropy = distribution_.computeEntropy();
      LOGTRACE(OSS() << "entropy=" << entropy);
      const Scalar entropyMC  = -distribution_.computeLogPDF(distribution_.getSample(entropySamplingSize_)).computeMean()[0];
      LOGTRACE(OSS() << "entropy(MC)=" << entropyMC);
      assert_almost_equal(entropy, entropyMC, entropyTolerance_, entropyTolerance_, "entropy " + distribution_.__repr__());
    }
  }

  void checkMoments() const
  {
    if (!enableMoments_ && !enableCorrelation_)
      return;

    LOGTRACE(OSS() << "generating big sample...");
    const Sample sample(distribution_.getSample(momentsSamplingSize_));
    sample.exportToCSVFile("sample_moments.csv");
    if (enableMoments_)
    {
      LOGTRACE(OSS() << "checking moments...");
      const Point mean(distribution_.getMean());
      LOGTRACE(OSS() << "mean    =" << mean);
      const Point meanMC(sample.computeMean());
      LOGTRACE(OSS() << "mean(MC)=" << meanMC);
      assert_almost_equal(mean, meanMC, meanTolerance_, meanTolerance_, "mean " + distribution_.__repr__());

      const Point stddev(distribution_.getStandardDeviation());
      LOGTRACE(OSS() << "stddev    =" << stddev);
      const Point stddevMC(sample.computeStandardDeviation());
      LOGTRACE(OSS() << "stddev(MC)=" << stddevMC);
      assert_almost_equal(stddev, stddevMC, standardDeviationTolerance_, standardDeviationTolerance_, "stddev " + distribution_.__repr__());

      const Point skewness(distribution_.getSkewness());
      LOGTRACE(OSS() << "skewness    =" << skewness);
      const Point skewnessMC(sample.computeSkewness());
      LOGTRACE(OSS() << "skewness(MC)=" << skewnessMC);
      assert_almost_equal(skewness, skewnessMC, skewnessTolerance_, skewnessTolerance_, "skewness " + distribution_.__repr__());

      const Point kurtosis(distribution_.getKurtosis());
      LOGTRACE(OSS() << "kurtosis    =" << kurtosis);
      const Point kurtosisMC(sample.computeKurtosis());
      LOGTRACE(OSS() << "kurtosis(MC)=" << kurtosisMC);
      assert_almost_equal(kurtosis, kurtosisMC, kurtosisTolerance_, kurtosisTolerance_, "kurtosis " + distribution_.__repr__());
    }

    if (enableCorrelation_)
    {
      LOGTRACE(OSS() << "checking correlation...");
      const CovarianceMatrix covariance(distribution_.getCovariance());
      LOGTRACE(OSS() << "covariance    =" << covariance);
      const CovarianceMatrix covarianceMC(sample.computeCovariance());
      LOGTRACE(OSS() << "covariance(MC)=" << covarianceMC);
      assert_almost_equal(covariance, covarianceMC, correlationTolerance_, correlationTolerance_, "covariance " + distribution_.__repr__());

      const CorrelationMatrix correlation(distribution_.getCorrelation());
      LOGTRACE(OSS() << "correlation    =" << correlation);
      const CorrelationMatrix correlationMC(sample.computeLinearCorrelation());
      LOGTRACE(OSS() << "correlation(MC)=" << correlationMC);
      assert_almost_equal(correlation, correlationMC, correlationTolerance_, correlationTolerance_, "correlation " + distribution_.__repr__());
    }

    if (enableDependenceMeasures_)
    {
      const CorrelationMatrix spearman(distribution_.getSpearmanCorrelation());
      LOGTRACE(OSS() << "spearman    =" << spearman);
      if (distribution_.isContinuous())
      {
        const CorrelationMatrix spearmanMC(sample.computeSpearmanCorrelation());
        LOGTRACE(OSS() << "spearman(MC)=" << spearmanMC);
        assert_almost_equal(spearman, spearmanMC, correlationTolerance_, correlationTolerance_, "spearman " + distribution_.__repr__());
      }

      const CorrelationMatrix kendall(distribution_.getKendallTau());
      LOGTRACE(OSS() << "kendall    =" << kendall);
      if (distribution_.isContinuous())
      {
        const CorrelationMatrix kendallMC(sample.computeKendallTau());
        LOGTRACE(OSS() << "kendall(MC)=" << kendallMC);
        assert_almost_equal(kendall, kendallMC, correlationTolerance_, correlationTolerance_, "kendall " + distribution_.__repr__());
      }
    }
  }

  void checkParameters() const
  {
    const Point parameter1(distribution_.getParameter());
    LOGTRACE(OSS() << "parameter =" << parameter1 << " pdim=" << parameter1.getDimension());
    for (UnsignedInteger j = 0; j < parameter1.getDimension(); ++ j)
    {
      Point parameter2(parameter1);
      parameter2[j] += 1e-2;
      Distribution distribution2(distribution_);
      try
      {
        distribution2.setParameter(parameter2);
      }
      catch (const InvalidArgumentException &)
      {
        // try increment integer parameter
        parameter2[j] = parameter1[j] + 1.0;
        distribution2.setParameter(parameter2);
      }
      LOGTRACE(OSS() << "parameter2=" << parameter2);
      Point parameter3(distribution2.getParameter());
      LOGTRACE(OSS() << "parameter3=" << parameter3);
      assert_almost_equal(parameter3, parameter2);
    }

    const Distribution::PointWithDescriptionCollection parameters(distribution_.getParametersCollection());
    LOGTRACE(OSS() << "parameters=" << parameters);
    if (!parameters.getSize())
      throw TestFailed(OSS() << "null parameter collection size for " << distribution_);
  }

  void checkMinimumVolumeInterval() const
  {
    if (distribution_.isContinuous() && (distribution_.getDimension() == 1))
    {
      LOGTRACE(OSS() << "checking min volume interval...");
      const Scalar probability = 0.9;
      Scalar threshold = 0.0;
      Interval interval(distribution_.computeMinimumVolumeIntervalWithMarginalProbability(probability, threshold));
      LOGTRACE(OSS() << "minvol interval=" << interval);
      const Scalar computedProbability = distribution_.computeProbability(interval);
      LOGTRACE(OSS() << "proba(minvol interval)=" << computedProbability);
      assert_almost_equal(probability, computedProbability, domainTolerance_, domainTolerance_, "proba(minvol interval) " + distribution_.__repr__());
    }
  }

  Scalar computeDomainProbabilityMC(const Domain & domain) const
  {
    const DomainEvent event(RandomVector(distribution_), domain);
    ProbabilitySimulationAlgorithm algo(event);
    algo.setBlockSize(domainSamplingSize_);
    algo.setMaximumOuterSampling(1);
    algo.run();
    return algo.getResult().getProbabilityEstimate();
  }

  void checkMinimumVolumeLevelSet() const
  {
    if (distribution_.isContinuous() && (distribution_.getDimension() == 1))
    {
      LOGTRACE(OSS() << "checking min volume levelset...");
      const Scalar probability = 0.9;
      Scalar threshold = 0.0;
      LevelSet levelSet(distribution_.computeMinimumVolumeLevelSetWithThreshold(probability, threshold));
      const Scalar mcProbability = computeDomainProbabilityMC(levelSet);
      LOGTRACE(OSS() << "proba(minvol levelset)=" << mcProbability);
      assert_almost_equal(mcProbability, probability, domainTolerance_, domainTolerance_, "proba(minvol levelset) " + distribution_.__repr__());
    }
  }

  void checkConfidenceInterval() const
  {
    if (distribution_.isContinuous() && (distribution_.getDimension() == 1))
    {
      LOGTRACE(OSS() << "checking confidence interval...");
      const Scalar probability = 0.95;
      Scalar beta = 0.0;
      const Interval interval1(distribution_.computeBilateralConfidenceIntervalWithMarginalProbability(probability, beta));
      LOGTRACE(OSS() << "Bilateral confidence interval=" << interval1);
      const Scalar mcProbability1 = computeDomainProbabilityMC(interval1);
      LOGTRACE(OSS() << "proba(bilateral)=" << mcProbability1);
      assert_almost_equal(mcProbability1, probability, domainTolerance_, domainTolerance_, "proba(ci bilateral) " + distribution_.__repr__());

      const Interval interval2(distribution_.computeUnilateralConfidenceIntervalWithMarginalProbability(probability, false, beta));
      LOGTRACE(OSS() << "Unilateral confidence interval (lower tail)=" << interval2);
      const Scalar mcProbability2 = computeDomainProbabilityMC(interval2);
      LOGTRACE(OSS() << "proba(lower tail)=" << mcProbability2);
      assert_almost_equal(mcProbability2, probability, domainTolerance_, domainTolerance_, "proba(ci lower tail) " + distribution_.__repr__());

      const Interval interval3(distribution_.computeUnilateralConfidenceIntervalWithMarginalProbability(probability, true, beta));
      LOGTRACE(OSS() << "Unilateral confidence interval (upper tail)=" << interval3);
      const Scalar mcProbability3 = computeDomainProbabilityMC(interval3);
      LOGTRACE(OSS() << "proba(upper tail)=" << mcProbability3);
      assert_almost_equal(mcProbability3, probability, domainTolerance_, domainTolerance_, "proba(ci upper tail) " + distribution_.__repr__());
    }
  }

  void checkConditional() const
  {
    if (distribution_.isIntegral())
      return;

    LOGTRACE(OSS() << "checking conditional PDF...");
    const Point point(distribution_.getRange().getLowerBound() * 0.4 + distribution_.getRange().getUpperBound() * 0.6);
    LOGTRACE(OSS() << "point=" << point << " dim=" << distribution_.getDimension());
    const Point seqPDF(distribution_.computeSequentialConditionalPDF(point));
    LOGTRACE(OSS() << "sequential conditional PDF=" << seqPDF.__str__());
    if (seqPDF.getDimension() != distribution_.getDimension())
      throw TestFailed(OSS() << "wrong seq PDF dim (" << seqPDF.getDimension() << ") for " << distribution_);

    if (distribution_.getDimension() == 1)
      assert_almost_equal(seqPDF[0], distribution_.computePDF(point), pdfTolerance_, pdfTolerance_, "seq PDF (1d) " + distribution_.__repr__());

    // check consistency with computeSequentialConditionalPDF
    for (UnsignedInteger i = 0; i < distribution_.getDimension(); ++ i)
    {
      Point y(i);
      std::copy(point.begin(), point.begin() + i, y.begin());
      const Scalar x = point[i];
      const Scalar condPDF = distribution_.computeConditionalPDF(x, y);
      LOGTRACE(OSS() << "i=" << i << " x=" << x << " y=" << y << " conditional PDF=" << condPDF);
      assert_almost_equal(condPDF, seqPDF[i], pdfTolerance_, pdfTolerance_, "seq pdf " + distribution_.__repr__());
    }

    LOGTRACE(OSS() << "checking conditional CDF...");
    const Point seqCDF(distribution_.computeSequentialConditionalCDF(point));
    LOGTRACE(OSS() << "sequential conditional CDF=" << seqCDF.__str__());
    if (seqCDF.getDimension() != distribution_.getDimension())
      throw TestFailed(OSS() << "wrong seq CDF dim (" << seqCDF.getDimension() << ") for " << distribution_);

    if (distribution_.getDimension() == 1)
      assert_almost_equal(seqCDF[0], distribution_.computeCDF(point), quantileTolerance_, quantileTolerance_, "seq CDF (1d) " + distribution_.__repr__());

    // check consistency with computeSequentialConditionalCDF
    for (UnsignedInteger i = 0; i < distribution_.getDimension(); ++ i)
    {
      Point y(i);
      std::copy(point.begin(), point.begin() + i, y.begin());
      const Scalar x = point[i];
      const Scalar condCDF = distribution_.computeConditionalCDF(x, y);
      LOGTRACE(OSS() << "i=" << i << " x=" << x << " y=" << y << " conditional CDF=" << condCDF);
      assert_almost_equal(condCDF, seqCDF[i], cdfTolerance_, cdfTolerance_, "seq cdf " + distribution_.__repr__());
    }

    LOGTRACE(OSS() << "checking conditional quantile...");
    const Scalar p = 0.1;
    const Point seqQ(distribution_.computeSequentialConditionalQuantile(Point(distribution_.getDimension(), p)));
    LOGTRACE(OSS() << "sequential conditional quantile=" << seqQ.__str__());
    if (seqQ.getDimension() != distribution_.getDimension())
      throw TestFailed(OSS() << "wrong seq quantile dim (" << seqCDF.getDimension() << ") for " << distribution_);
    
    // check consistency with computeSequentialConditionalQuantile
    for (UnsignedInteger i = 0; i < distribution_.getDimension(); ++ i)
    {
      Point y(i);
      std::copy(seqQ.begin(), seqQ.begin() + i, y.begin());
      const Scalar condQuantile = distribution_.computeConditionalQuantile(p, y);
      LOGTRACE(OSS() << "i=" << i << " y=" << y << " conditional quantile=" << condQuantile);
      assert_almost_equal(condQuantile, seqQ[i], quantileTolerance_, quantileTolerance_, "seq quantile " + distribution_.__repr__());
    }

    // check consistency between computeConditionalQuantile and computeConditionalCDF
    for (UnsignedInteger i = 0; i < distribution_.getDimension(); ++ i)
    {
      Point y(i);
      std::copy(seqQ.begin(), seqQ.begin() + i, y.begin());
      for (UnsignedInteger j = 1; j < 10; ++j)
	{
	  const Scalar pj = 0.1 * j;
	  const Scalar condQuantile = distribution_.computeConditionalQuantile(pj, y);
	  const Scalar condCDF = distribution_.computeConditionalCDF(condQuantile, y);
	  LOGTRACE(OSS() << "i=" << i << " y=" << y << " pj=" << pj << " conditional quantile=" << condQuantile << " condCDF=" << condCDF);
	  assert_almost_equal(condCDF, pj, cdfTolerance_, cdfTolerance_, "cond. quantile vs cond. cdf " + distribution_.__repr__());
	}
    }
    
  }

  void checkTransformation() const
  {
    // FIXME: should it work for discrete ?
    if (!distribution_.isContinuous())
      return;

    LOGTRACE(OSS() << "checking transformation...");
    const Function transform(distribution_.getIsoProbabilisticTransformation());
    if (transform.getInputDimension() != distribution_.getDimension())
      throw TestFailed(OSS() << "wrong transform input dim (" << transform.getInputDimension() << ") for " << distribution_);
    if (transform.getOutputDimension() != distribution_.getDimension())
      throw TestFailed(OSS() << "wrong transform output dim for " << distribution_);
    const Function inverseTransform(distribution_.getInverseIsoProbabilisticTransformation());
    if (inverseTransform.getInputDimension() != distribution_.getDimension())
      throw TestFailed(OSS() << "wrong inverse transform input (" << inverseTransform.getInputDimension() << ") dim for " << distribution_);
    if (inverseTransform.getOutputDimension() != distribution_.getDimension())
      throw TestFailed(OSS() << "wrong inverse transform output dim for " << distribution_);

    // check ToT-1(u)=u
    const Point u0(distribution_.getDimension(), 0.125);
    LOGTRACE(OSS() << "u0=" << u0.__str__());
    const Point x1(inverseTransform(u0));
    LOGTRACE(OSS() << "x1=" << x1.__str__());
    const Point u2(transform(x1));
    LOGTRACE(OSS() << "u2=" << u2.__str__());
    assert_almost_equal(u2, u0, quantileTolerance_, quantileTolerance_, "ToT-1(u) " + distribution_.__repr__());

    // with rosenblatt
    const Function ros(new RosenblattEvaluation(distribution_));
    const Function invRos(new InverseRosenblattEvaluation(distribution_));
    const Point r2(ros(invRos(u0)));
    LOGTRACE(OSS() << "r2=" << r2.__str__());
    assert_almost_equal(ros(invRos(u0)), u0, quantileTolerance_, quantileTolerance_, "Tros o Tros-1(u) " + distribution_.__repr__());

    // check inv transform gradient by FD
    const Function inverseTransformFD(inverseTransform.getEvaluation());
    const Matrix uGrad(inverseTransform.gradient(u0));
    const Matrix uGradFD(inverseTransformFD.gradient(u0));
    LOGTRACE(OSS() << "uGrad=" << uGrad.__str__() << " uGradFD=" << uGradFD.__str__());
    assert_almost_equal(uGrad, uGradFD, quantileTolerance_, quantileTolerance_, "inv transform grad " + distribution_.__repr__());

    // check transform gradient by FD
    const Function transformFD(transform.getEvaluation());
    const Matrix xGrad(transform.gradient(x1));
    const Matrix xGradFD(transformFD.gradient(x1));
    LOGTRACE(OSS() << "xGrad=" << uGrad.__str__() << " xGradFD=" << uGradFD.__str__());
    assert_almost_equal(xGrad, xGradFD, cdfTolerance_, cdfTolerance_, "transform grad " + distribution_.__repr__());
  }

  Distribution distribution_;
  Bool enablePDF_ = true;
  Bool enableCDF_ = true;
  Bool enableComplementaryCDF_ = true;
  Bool enableDDF_ = true;
  Bool enableMoments_ = true;
  Scalar meanTolerance_ = 1e-2;
  Scalar standardDeviationTolerance_ = 1e-2;
  Scalar skewnessTolerance_ = 1e-1;
  Scalar kurtosisTolerance_ = 5.0;
  Bool enableCorrelation_ = true;
  Bool enableDependenceMeasures_ = true;
  Scalar correlationTolerance_ = 2e-2;
  mutable Sample sample_;
  Bool enableMinimumVolumeInterval_ = true;
  Bool enableMinimumVolumeLevelSet_ = true;
  Bool enableConfidenceInterval_ = true;
  Bool enableParameters_ = true;
  Bool enableProbability_ = true;
  Bool enableGeneratingFunction_ = true;
  Bool enableCharacteristicFunction_ = true;
  Bool enableGradient_ = true;
  Bool enableEntropy_ = true;
  Bool enableTransformation_ = true;
  Bool enableConditional_ = true;
  Scalar entropyTolerance_ = 2e-3;
  Scalar cdfTolerance_ = 1e-5;
  Scalar pdfTolerance_ = 1e-3;
  Scalar ddfTolerance_ = 1e-3;
  Scalar quantileTolerance_ = 1e-5;
  Scalar parameterTolerance_ = 1e-5;
  Scalar domainTolerance_ = 1e-2;
  UnsignedInteger momentsSamplingSize_ = 1000000;
  UnsignedInteger entropySamplingSize_ = 1000000;
  UnsignedInteger domainSamplingSize_ = 1000000;
  UnsignedInteger pdfSamplingSize_ = 10;
  UnsignedInteger cdfSamplingSize_ = 5;
  UnsignedInteger fittingSamplingSize_ = 100;
};

} /* namespace Test */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OTTESTCODE_HXX */
