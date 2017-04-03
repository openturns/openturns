//                                               -*- C++ -*-
/**
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/DiracCovarianceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/HMatrix.hxx"
#include "openturns/HMatrixFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DiracCovarianceModel);

static const Factory<DiracCovarianceModel> Factory_DiracCovarianceModel;

class DiracAssemblyFunction : public HMatrixTensorRealAssemblyFunction
{
private:
  const CovarianceMatrix covarianceMatrix_;

public:
  DiracAssemblyFunction(const DiracCovarianceModel & covarianceModel)
    : HMatrixTensorRealAssemblyFunction(covarianceModel.getDimension())
    , covarianceMatrix_(covarianceModel(NumericalPoint(covarianceModel.getSpatialDimension())))
  {
    // Nothing to do
  }

  void compute(UnsignedInteger i, UnsignedInteger j, Matrix* localValues) const
  {
    if (i == j)
    {
      memcpy( &localValues->getImplementation()->operator[](0), &covarianceMatrix_.getImplementation()->operator[](0), dimension_ * dimension_ * sizeof(NumericalScalar) );
    }
  }
};

/* Default constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger spatialDimension)
  : StationaryCovarianceModel(spatialDimension)
  , covarianceFactor_()
{
  // Remove the scale from the active parameter
  activeParameter_ = Indices(dimension_);
  activeParameter_.fill();
}

/* Parameters constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger spatialDimension,
    const NumericalPoint & amplitude)
  : StationaryCovarianceModel(spatialDimension)
  , covarianceFactor_()
{
  dimension_ = amplitude.getDimension();
  setAmplitude(amplitude);

  // Remove the scale from the active parameter
  activeParameter_ = Indices(dimension_);
  activeParameter_.fill();
}

/** Parameters constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger spatialDimension,
    const NumericalPoint & amplitude,
    const CorrelationMatrix & correlation)
  : StationaryCovarianceModel(NumericalPoint(spatialDimension, 1.0), NumericalPoint(amplitude.getDimension(), 1.0))
  , covarianceFactor_()
{
  dimension_ = amplitude.getDimension();
  // Set spatial correlation
  setSpatialCorrelation(correlation);
  // set amplitude & compute covariance
  setAmplitude(amplitude);

  // Remove the scale from the active parameter
  activeParameter_ = Indices(dimension_);
  activeParameter_.fill();
}

/** Parameters constructor */
DiracCovarianceModel::DiracCovarianceModel(const UnsignedInteger spatialDimension,
    const CovarianceMatrix & covariance)
  : StationaryCovarianceModel(spatialDimension)
{
  dimension_ = covariance.getDimension();
  amplitude_ = NumericalPoint(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    amplitude_[i] = sqrt(covariance(i, i));
  if (!covariance.isDiagonal())
  {
    CorrelationMatrix correlation(dimension_);
    for(UnsignedInteger j = 0; j < dimension_; ++j)
      for(UnsignedInteger i = j; i < dimension_; ++i)
        spatialCorrelation_(i, j) = covariance(i, j) / (amplitude_[i] * amplitude_[j]);
  }
  // Copy covariance
  spatialCovariance_ = covariance;

  // Remove the scale from the active parameter
  activeParameter_ = Indices(dimension_);
  activeParameter_.fill();
}

void DiracCovarianceModel::computeCovariance()
{
  // Method that helps to compute spatialCovariance_ attribut (for tau=0)
  // after setAmplitude, setSpatialCorrelation
  spatialCovariance_ = CovarianceMatrix(dimension_);
  for(UnsignedInteger j = 0; j < dimension_; ++j) spatialCovariance_(j, j) = amplitude_[j] * amplitude_[j] * (1.0 + nuggetFactor_);
  if (!spatialCorrelation_.isDiagonal())
  {
    for(UnsignedInteger j = 0; j < dimension_; ++j)
      for(UnsignedInteger i = j + 1; i < dimension_; ++i)
        spatialCovariance_(i, j) = spatialCorrelation_(i, j) * amplitude_[i] * amplitude_[j];
  }
  // Compute once the Cholesky factor
  covarianceFactor_ = spatialCovariance_.computeCholesky();
}

/* Virtual constructor */
DiracCovarianceModel * DiracCovarianceModel::clone() const
{
  return new DiracCovarianceModel(*this);
}

/* Computation of the covariance density function */
CovarianceMatrix DiracCovarianceModel::operator() (const NumericalPoint & tau) const
{
  if (tau.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::operator(), the point tau has dimension=" << tau.getDimension() << ", expected dimension=" << spatialDimension_;
  // If tau.norm1 is zero we compute the covariance matrix
  // Otherwise the returned value is 0
  if (tau.norm() == 0)
    return spatialCovariance_;
  else
    return CovarianceMatrix(SquareMatrix(dimension_).getImplementation());
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
    , dimension_(model.getDimension())
  {}

  inline void operator()(const TBB::BlockedRange<UnsignedInteger> & r) const
  {
    for (UnsignedInteger index = r.begin(); index != r.end(); ++index)
    {
      const UnsignedInteger indexBlock = index * dimension_;
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        for (UnsignedInteger i = 0; i < dimension_; ++i)
          output_(indexBlock + i, indexBlock + j) = model_.spatialCovariance_(i, j);
    }
  }
}; /* end struct DiracCovarianceModelDiscretizePolicy */

CovarianceMatrix DiracCovarianceModel::discretize(const Sample & vertices) const
{
  if (vertices.getDimension() != spatialDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a dimension=" << vertices.getDimension()
                                         << " different from the input spatial dimension=" << spatialDimension_;

  if (vertices.getSize() == 0)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a size 0";

  const UnsignedInteger size = vertices.getSize();
  const UnsignedInteger fullSize = size * dimension_;
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
    , dimension_(model.getDimension())
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
  if (vertices.getDimension() != spatialDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a dimension=" << vertices.getDimension()
                                         << " different from the input spatial dimension=" << spatialDimension_;

  if (vertices.getSize() == 0)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretize, the given sample has a size 0";

  const UnsignedInteger size = vertices.getSize();
  const UnsignedInteger fullSize = size * dimension_;
  TriangularMatrix covarianceFactor(fullSize);

  const DiracCovarianceModelDiscretizeAndFactorizePolicy policy( vertices, covarianceFactor, *this );
  // The loop is over the lower block-triangular part
  TBB::ParallelFor( 0, size, policy );

  return covarianceFactor;
}

Sample DiracCovarianceModel::discretizeRow(const Sample & vertices,
    const UnsignedInteger p) const
{
  if (vertices.getDimension() != spatialDimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretizeRow, the given sample has a dimension=" << vertices.getDimension()
                                         << " different from the input dimension=" << spatialDimension_;
  if (vertices.getSize() == 0)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretizeRow, the given sample has a size 0";
  if (p >= vertices.getSize())
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::discretizeRow, the index p should be lower or equal to " << vertices.getSize() - 1
                                         << ", here, p=" << p;

  const UnsignedInteger size = vertices.getSize();
  Sample result(size * dimension_, dimension_);
  for(UnsignedInteger j = 0; j < dimension_; ++j)
    for(UnsignedInteger i = j; i < dimension_; ++i)
      result[p * dimension_ + i][j] = spatialCovariance_(i, j);
  return result;
}

// discretize with use of HMatrix
HMatrix DiracCovarianceModel::discretizeHMatrix(const Sample & vertices,
    const NumericalScalar nuggetFactor,
    const HMatrixParameters & parameters) const
{
#ifdef OPENTURNS_HAVE_HMAT
  HMatrixFactory hmatrixFactory;
  HMatrix covarianceHMatrix(hmatrixFactory.build(vertices, dimension_, true, parameters));
  // Update covariance matrix
  // Take into account nuggetFactor
  CovarianceMatrix oldCovariance(spatialCovariance_);
  for(UnsignedInteger j = 0; j < dimension_; ++j) spatialCovariance_(j, j) = amplitude_[j] * amplitude_[j] * (1.0 + nuggetFactor);
  // Compute the covariance
  DiracAssemblyFunction dirac(*this);
  covarianceHMatrix.assemble(dirac, 'L');
  // Restore old covariance
  spatialCovariance_ = CovarianceMatrix(oldCovariance);
  return covarianceHMatrix;
#else
  throw NotYetImplementedException(HERE) << "OpenTURNS had been compiled without HMat support";
#endif
}

/* Gradient */
Matrix DiracCovarianceModel::partialGradient(const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  // Gradient should be checked
  Matrix gradient(spatialDimension_, dimension_ * dimension_);
  return gradient;
}

/* Parameters accessor */
void DiracCovarianceModel::setFullParameter(const NumericalPoint & parameters)
{
  if (parameters.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::setParameter, parameters should be of size " << dimension_ << ", here, parameters dimension = " << parameters.getDimension();
  setAmplitude(parameters);
}

NumericalPoint DiracCovarianceModel::getFullParameter() const
{
  return getAmplitude();
}

Description DiracCovarianceModel::getFullParameterDescription() const
{
  Description description(0);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
    description.add(OSS() << "amplitude_" << j);
  return description;
}

void DiracCovarianceModel::setScale(const NumericalPoint & scale)
{
  // Scale factor has no effect
  // No check of size or dimension
  scale_ = scale;
  LOGWARN(OSS() << "Scale parameter is not used.");
}

/** Amplitude accessor */
void DiracCovarianceModel::setAmplitude(const NumericalPoint & amplitude)
{
  if (amplitude.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::setAmplitude, amplitude vector should be of size " << dimension_
                                         << ", here, amplitude dimension = " << amplitude.getDimension();
  // Check positivity of amplitude
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    if (amplitude[i] <= 0)
      throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::setAmplitude, amplitude should be stricly positive but the #" << i << " component equals " << amplitude[i];
  }
  amplitude_ = amplitude;
  computeCovariance();
}

void DiracCovarianceModel::setSpatialCorrelation(const CorrelationMatrix & correlation)
{
  if (correlation.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "In DiracCovarianceModel::setSpatialCorrelation, correlation matrix should be of dimension " << dimension_
                                         << ", here, matrix's dimension = " << correlation.getDimension();
  spatialCorrelation_ = correlation;
  computeCovariance();
}

/* Is it a stationary model ? */
Bool DiracCovarianceModel::isStationary() const
{
  return true;
}

/* String converter */
String DiracCovarianceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << DiracCovarianceModel::GetClassName()
      << ", amplitude=" << amplitude_
      << ", spatialCorrelation=" << spatialCorrelation_;
  return oss;
}

/* String converter */
String DiracCovarianceModel::__str__(const String & offset) const
{
  OSS oss;
  oss << DiracCovarianceModel::GetClassName();
  oss << "(t)=" << spatialCovariance_.__str__()
      << " * t==" << NumericalPoint(spatialDimension_, 0.0).__str__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void DiracCovarianceModel::save(Advocate & adv) const
{
  StationaryCovarianceModel::save(adv);
  adv.saveAttribute("covarianceFactor_", covarianceFactor_);
}

/* Method load() reloads the object from the StorageManager */
void DiracCovarianceModel::load(Advocate & adv)
{
  StationaryCovarianceModel::load(adv);
  adv.loadAttribute("covarianceFactor_", covarianceFactor_);
}

END_NAMESPACE_OPENTURNS
