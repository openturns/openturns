//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DiracCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/HMatrixFactory.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DiracCovarianceModel)

static const Factory<DiracCovarianceModel> Factory_DiracCovarianceModel;

class DiracAssemblyFunction : public HMatrixTensorRealAssemblyFunction
{
private:
  const SquareMatrix covarianceMatrix_;

public:
  DiracAssemblyFunction(const DiracCovarianceModel & covarianceModel)
    : HMatrixTensorRealAssemblyFunction(covarianceModel.getOutputDimension())
    , covarianceMatrix_(covarianceModel(Point(covarianceModel.getInputDimension())))
  {
    // Nothing to do
  }

  void compute(UnsignedInteger i, UnsignedInteger j, Matrix* localValues) const
  {
    if (i == j)
    {
      std::copy(&covarianceMatrix_.getImplementation()->operator[](0), &covarianceMatrix_.getImplementation()->operator[](0) + dimension_ * dimension_, &localValues->getImplementation()->operator[](0));
    }
  }
};

/* Default constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
  , covarianceFactor_()
{
  // Remove the scale from the active parameter
  activeParameter_ = Indices(outputDimension_);
  activeParameter_.fill();
  isStationary_ = true;
}

/* Parameters constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger inputDimension,
    const Point & amplitude)
  : CovarianceModelImplementation(inputDimension)
  , covarianceFactor_()
{
  outputDimension_ = amplitude.getDimension();
  setAmplitude(amplitude);

  // Remove the scale from the active parameter
  activeParameter_ = Indices(outputDimension_);
  activeParameter_.fill();
  isStationary_ = true;
}

/** Parameters constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger inputDimension,
    const Point & amplitude,
    const CorrelationMatrix & correlation)
  : CovarianceModelImplementation(Point(inputDimension, 1.0), Point(amplitude.getDimension(), 1.0))
  , covarianceFactor_()
{
  outputDimension_ = amplitude.getDimension();
  // Set spatial correlation
  setOutputCorrelation(correlation);
  // set amplitude & compute covariance
  setAmplitude(amplitude);

  // Remove the scale from the active parameter
  activeParameter_ = Indices(outputDimension_);
  activeParameter_.fill();
  isStationary_ = true;
}

/** Parameters constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger inputDimension,
    const CovarianceMatrix & covariance)
  : CovarianceModelImplementation(inputDimension)
{
  outputDimension_ = covariance.getDimension();
  amplitude_ = Point(outputDimension_);
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    amplitude_[i] = sqrt(covariance(i, i));
  if (!covariance.isDiagonal())
  {
    CorrelationMatrix correlation(outputDimension_);
    for(UnsignedInteger j = 0; j < outputDimension_; ++j)
      for(UnsignedInteger i = j; i < outputDimension_; ++i)
        outputCorrelation_(i, j) = covariance(i, j) / (amplitude_[i] * amplitude_[j]);
  }
  // Copy covariance
  outputCovariance_ = covariance;

  // Remove the scale from the active parameter
  activeParameter_ = Indices(outputDimension_);
  activeParameter_.fill();
  isStationary_ = true;
}

void DiracCovarianceModel::computeCovariance()
{
  // Method that helps to compute outputCovariance_ (for tau=0)
  // after setAmplitude, setOutputCorrelation
  outputCovariance_ = CovarianceMatrix(outputDimension_);
  for(UnsignedInteger j = 0; j < outputDimension_; ++j) outputCovariance_(j, j) = amplitude_[j] * amplitude_[j] * (1.0 + nuggetFactor_);
  if (!outputCorrelation_.isDiagonal())
  {
    for(UnsignedInteger j = 0; j < outputDimension_; ++j)
      for(UnsignedInteger i = j + 1; i < outputDimension_; ++i)
        outputCovariance_(i, j) = outputCorrelation_(i, j) * amplitude_[i] * amplitude_[j];
  }
  // Compute once the Cholesky factor
  covarianceFactor_ = outputCovariance_.computeCholesky();
}

/* Virtual constructor */
DiracCovarianceModel * DiracCovarianceModel::clone() const
{
  return new DiracCovarianceModel(*this);
}

/* Computation of the covariance density function */
SquareMatrix DiracCovarianceModel::operator() (const Point & tau) const
{
  if (tau.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::operator(), the point tau has dimension=" << tau.getDimension() << ", expected dimension=" << inputDimension_;
  // If tau.norm1 is zero we compute the covariance matrix
  // Otherwise the returned value is 0
  if (tau.norm() == 0)
    return outputCovariance_;
  else
    return SquareMatrix(outputDimension_).getImplementation();
}

Scalar DiracCovarianceModel::computeAsScalar(const Point &tau) const
{
  if (outputDimension_ > 1)
    throw InvalidArgumentException(HERE) << "Error : DiracCovarianceModel::computeAsScalar(tau) should be only used if output dimension is 1. Here, output dimension = " << outputDimension_;
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::computeStandardRepresentative: expected a shift of dimension=" << getInputDimension() << ", got dimension=" << tau.getDimension();
  if (tau.norm() == 0)
    return outputCovariance_(0, 0);
  else
    return 0.0;
}

Scalar DiracCovarianceModel::computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
    const Collection<Scalar>::const_iterator &t_begin) const
{
  if (outputDimension_ > 1)
    throw InvalidArgumentException(HERE) << "Error : DiracCovarianceModel::computeAsScalar(tau) should be only used if output dimension is 1. Here, output dimension = " << outputDimension_;

  Scalar tauNorm = 0;
  Collection<Scalar>::const_iterator s_it = s_begin;
  Collection<Scalar>::const_iterator t_it = t_begin;
  for (UnsignedInteger i = 0; i < inputDimension_; ++i, ++s_it, ++t_it)
  {
    const Scalar dx = (*s_it - *t_it) / scale_[i];
    tauNorm += dx * dx;
  }
  tauNorm = sqrt(tauNorm);
  if (tauNorm == 0)
    return outputCovariance_(0, 0);
  else
    return 0.0;
}

Scalar DiracCovarianceModel::computeAsScalar(const Scalar tau) const
{
  if (inputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has input dimension=" << inputDimension_ << ", expected input dimension=1.";
  if (outputDimension_ != 1)
    throw NotDefinedException(HERE) << "Error: the covariance model has output dimension=" << outputDimension_ << ", expected dimension=1.";
  if (std::abs(tau) <= SpecFunc::ScalarEpsilon)
    return outputCovariance_(0, 0);
  else
    return 0.0;
}

// The following structure helps to compute the full covariance matrix
struct DiracCovarianceModelDiscretizePolicy
{
  const Sample & input_;
  CovarianceMatrix & output_;
  const DiracCovarianceModel & model_;
  const UnsignedInteger dimension_;

  DiracCovarianceModelDiscretizePolicy(const Sample & input,
                                       CovarianceMatrix & output,
                                       const DiracCovarianceModel & model)
    : input_(input)
    , output_(output)
    , model_(model)
    , dimension_(model.getOutputDimension())
  {}

  inline void operator()(const TBB::BlockedRange<UnsignedInteger> & r) const
  {
    for (UnsignedInteger index = r.begin(); index != r.end(); ++index)
    {
      const UnsignedInteger indexBlock = index * dimension_;
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        for (UnsignedInteger i = 0; i < dimension_; ++i)
          output_(indexBlock + i, indexBlock + j) = model_.outputCovariance_(i, j);
    }
  }
}; /* end struct DiracCovarianceModelDiscretizePolicy */

CovarianceMatrix DiracCovarianceModel::discretize(const Sample & vertices) const
{
  if (vertices.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a dimension=" << vertices.getDimension()
                                         << " different from the input dimension=" << inputDimension_;

  if (!(vertices.getSize() > 0))
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a size 0";

  const UnsignedInteger size = vertices.getSize();
  const UnsignedInteger fullSize = size * outputDimension_;
  CovarianceMatrix covarianceMatrix(fullSize);

  const DiracCovarianceModelDiscretizePolicy policy( vertices, covarianceMatrix, *this );
  // The loop is over the lower block-triangular part
  TBB::ParallelFor( 0, size, policy );

  return covarianceMatrix;
}

// The following structure helps to compute the full covariance matrix
struct DiracCovarianceModelDiscretizeAndFactorizePolicy
{
  const Sample & input_;
  TriangularMatrix & output_;
  const DiracCovarianceModel & model_;
  const UnsignedInteger dimension_;

  DiracCovarianceModelDiscretizeAndFactorizePolicy(const Sample & input,
      TriangularMatrix & output,
      const DiracCovarianceModel & model)
    : input_(input)
    , output_(output)
    , model_(model)
    , dimension_(model.getOutputDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger index = r.begin(); index != r.end(); ++index)
    {
      const UnsignedInteger indexBlock = index * dimension_;
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        for (UnsignedInteger i = j; i < dimension_; ++i)
          output_(indexBlock + i, indexBlock + j) = model_.covarianceFactor_(i, j);
    }
  }
}; /* end struct DiracCovarianceModelDiscretizeAndFactorizePolicy */

TriangularMatrix DiracCovarianceModel::discretizeAndFactorize(const Sample & vertices) const
{
  if (vertices.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a dimension=" << vertices.getDimension()
                                         << " different from the input dimension=" << inputDimension_;

  if (!(vertices.getSize() > 0))
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a size 0";

  const UnsignedInteger size = vertices.getSize();
  const UnsignedInteger fullSize = size * outputDimension_;
  TriangularMatrix covarianceFactor(fullSize);

  const DiracCovarianceModelDiscretizeAndFactorizePolicy policy( vertices, covarianceFactor, *this );
  // The loop is over the lower block-triangular part
  TBB::ParallelFor( 0, size, policy );

  return covarianceFactor;
}

Sample DiracCovarianceModel::discretizeRow(const Sample & vertices,
    const UnsignedInteger p) const
{
  if (vertices.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretizeRow, the given sample has a dimension=" << vertices.getDimension()
                                         << " different from the input dimension=" << inputDimension_;
  if (!(vertices.getSize() > 0))
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretizeRow, the given sample has a size 0";
  if (!(p < vertices.getSize()))
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretizeRow, the index p should be lower or equal to " << vertices.getSize() - 1
                                         << ", here, p=" << p;

  const UnsignedInteger size = vertices.getSize();
  Sample result(size * outputDimension_, outputDimension_);
  for(UnsignedInteger j = 0; j < outputDimension_; ++j)
    for(UnsignedInteger i = j; i < outputDimension_; ++i)
      result(p * outputDimension_ + i, j) = outputCovariance_(i, j);
  return result;
}

// discretize with use of HMatrix
HMatrix DiracCovarianceModel::discretizeHMatrix(const Sample & vertices,
    const HMatrixParameters & parameters) const
{
  return discretizeHMatrix(vertices, nuggetFactor_, parameters);
}

// discretize with use of HMatrix
HMatrix DiracCovarianceModel::discretizeHMatrix(const Sample & vertices,
    const Scalar nuggetFactor,
    const HMatrixParameters & parameters) const
{
#ifdef OPENTURNS_HAVE_HMAT
  HMatrixFactory hmatrixFactory;
  HMatrix covarianceHMatrix(hmatrixFactory.build(vertices, outputDimension_, true, parameters));
  // Update covariance matrix
  // Take into account nuggetFactor
  CovarianceMatrix oldCovariance(outputCovariance_);
  for(UnsignedInteger j = 0; j < outputDimension_; ++j) outputCovariance_(j, j) = amplitude_[j] * amplitude_[j] * (1.0 + nuggetFactor);
  // Compute the covariance
  DiracAssemblyFunction dirac(*this);
  covarianceHMatrix.assemble(dirac, 'L');
  // Restore old covariance
  outputCovariance_ = CovarianceMatrix(oldCovariance);
  return covarianceHMatrix;
#else
  (void)vertices;
  (void)nuggetFactor;
  (void)parameters;
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

/* Gradient */
Matrix DiracCovarianceModel::partialGradient(const Point & s,
    const Point & t) const
{
  if (s.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << inputDimension_;
  if (t.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << inputDimension_;
  // Gradient should be checked
  Matrix gradient(inputDimension_, outputDimension_ * outputDimension_);
  return gradient;
}

/* Parameters accessor */
void DiracCovarianceModel::setFullParameter(const Point & parameters)
{
  if (parameters.getDimension() != outputDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::setParameter, parameters should be of size " << outputDimension_ << ", here, parameters dimension = " << parameters.getDimension();
  setAmplitude(parameters);
}

Point DiracCovarianceModel::getFullParameter() const
{
  return getAmplitude();
}

Description DiracCovarianceModel::getFullParameterDescription() const
{
  Description description(0);
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    description.add(OSS() << "amplitude_" << j);
  return description;
}

void DiracCovarianceModel::setScale(const Point & scale)
{
  // Scale factor has no effect
  // No check of size or dimension
  scale_ = scale;
  LOGWARN(OSS() << "Scale parameter is not used.");
}

/** Amplitude accessor */
void DiracCovarianceModel::setAmplitude(const Point & amplitude)
{
  if (amplitude.getDimension() != outputDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::setAmplitude, amplitude vector should be of size " << outputDimension_
                                         << ", here, amplitude dimension = " << amplitude.getDimension();
  // Check positivity of amplitude
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
  {
    if (!(amplitude[i] > 0))
      throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::setAmplitude, amplitude should be stricly positive but the #" << i << " component equals " << amplitude[i];
  }
  amplitude_ = amplitude;
  computeCovariance();
}

void DiracCovarianceModel::setOutputCorrelation(const CorrelationMatrix & correlation)
{
  if (correlation.getDimension() != outputDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::setOutputCorrelation, correlation matrix should be of dimension " << outputDimension_
                                         << ", here, matrix's dimension = " << correlation.getDimension();
  outputCorrelation_ = correlation;
  computeCovariance();
}

/* String converter */
String DiracCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << DiracCovarianceModel::GetClassName()
      << ", amplitude=" << amplitude_
      << ", spatialCorrelation=" << outputCorrelation_;
  return oss;
}

/* String converter */
String DiracCovarianceModel::__str__(const String & ) const
{
  OSS oss;
  oss << DiracCovarianceModel::GetClassName()
      << "(t)=" << outputCovariance_.__str__()
      << " * t==" << Point(inputDimension_, 0.0).__str__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void DiracCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("covarianceFactor_", covarianceFactor_);
}

/* Method load() reloads the object from the StorageManager */
void DiracCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("covarianceFactor_", covarianceFactor_);
}

END_NAMESPACE_OPENTURNS
