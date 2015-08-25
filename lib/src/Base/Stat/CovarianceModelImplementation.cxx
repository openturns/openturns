//                                               -*- C++ -*-
/**
 *  @brief
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
#include "CovarianceModelImplementation.hxx"
#include "Exception.hxx"
#include "Log.hxx"
#include "SpecFunc.hxx"
#include "PersistentObjectFactory.hxx"
#include "MatrixImplementation.hxx"
#include "HMatrix.hxx"
#include "HMatrixFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CovarianceModelImplementation
 */

CLASSNAMEINIT(CovarianceModelImplementation);

static const Factory<CovarianceModelImplementation> RegisteredFactory;

/* Dimension-based constructor */
CovarianceModelImplementation::CovarianceModelImplementation(const UnsignedInteger spatialDimension)
  : PersistentObject()
  , spatialDimension_(spatialDimension)
  , dimension_(1)
  , amplitude_(1, 1.0)
  , scale_(1, 1.0)
  , spatialCorrelation_(0)
  , isDiagonal_(true)
  , nuggetFactor_(ResourceMap::GetAsNumericalScalar("CovarianceModelImplementation-DefaultNuggetFactor"))
{
  // Nothing to do
}

/* Constructor with parameters */
CovarianceModelImplementation::CovarianceModelImplementation(const UnsignedInteger spatialDimension,
    const NumericalPoint & amplitude,
    const NumericalPoint & scale)
  : PersistentObject()
  , spatialDimension_(spatialDimension)
  , dimension_(amplitude.getDimension())
  , amplitude_(amplitude)
  , scale_(scale)
  , spatialCorrelation_(0)
  , isDiagonal_(true)
  , nuggetFactor_(ResourceMap::GetAsNumericalScalar("CovarianceModelImplementation-DefaultNuggetFactor"))
{
  if (scale.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Incompatible dimensions between amplitude and scale values";
  setAmplitude(amplitude);
  setScale(scale);
}

CovarianceModelImplementation::CovarianceModelImplementation(const UnsignedInteger spatialDimension,
    const NumericalPoint & amplitude,
    const NumericalPoint & scale,
    const CorrelationMatrix & spatialCorrelation)
  : PersistentObject()
  , spatialDimension_(spatialDimension)
  , dimension_(amplitude.getDimension())
  , amplitude_(amplitude)
  , scale_(scale)
  , spatialCorrelation_(0)
  , isDiagonal_(true)
  , nuggetFactor_(ResourceMap::GetAsNumericalScalar("CovarianceModelImplementation-DefaultNuggetFactor"))
{
  if (scale.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Incompatible dimensions between amplitude and scale values";
  if (spatialCorrelation.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given spatial correlation has a dimension different from the scales and amplitudes.";
  setAmplitude(amplitude);
  setScale(scale);
  setSpatialCorrelation(spatialCorrelation);
}

CovarianceModelImplementation::CovarianceModelImplementation(const UnsignedInteger spatialDimension,
    const NumericalPoint & scale,
    const CovarianceMatrix & spatialCovariance)
  : PersistentObject()
  , spatialDimension_(spatialDimension)
  , dimension_(scale.getDimension())
  , amplitude_(scale.getDimension())
  , scale_(scale)
  , spatialCorrelation_(0)
  , isDiagonal_(true)
  , nuggetFactor_(ResourceMap::GetAsNumericalScalar("CovarianceModelImplementation-DefaultNuggetFactor"))
{
  if (spatialCovariance.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: the given spatial covariance has a dimension different from the scales and amplitudes.";
  setScale(scale);
  NumericalPoint amplitude(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    amplitude[i] = sqrt(spatialCovariance(i, i));
  // Check that the amplitudes are valid
  setAmplitude(amplitude);
  // Convert the spatial covariance into a spatial correlation
  if (!spatialCovariance.isDiagonal())
  {
    spatialCorrelation_ = CorrelationMatrix(dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      for (UnsignedInteger j = 0; j < i; ++j)
        spatialCorrelation_(i, j) = spatialCovariance(i, j) / (amplitude[i] * amplitude[j]);
  } // !isDiagonal
}

/* Virtual constructor */
CovarianceModelImplementation * CovarianceModelImplementation::clone() const
{
  return new CovarianceModelImplementation(*this);
}

/* Dimension accessors */
UnsignedInteger CovarianceModelImplementation::getDimension() const
{
  return dimension_;
}

UnsignedInteger CovarianceModelImplementation::getSpatialDimension() const
{
  return spatialDimension_;
}

CovarianceMatrix CovarianceModelImplementation::operator() (const NumericalScalar s,
    const NumericalScalar t) const
{
  return operator() (NumericalPoint(1, s), NumericalPoint(1, t));
}

CovarianceMatrix CovarianceModelImplementation::operator() (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::operator() (const NumericalPoint & s, const NumericalPoint & t) const";
}

NumericalScalar CovarianceModelImplementation::computeAsScalar (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (dimension_ != 1) throw NotDefinedException(HERE) << "Error: the covariance model is of dimension=" << dimension_ << ", expected dimension=1.";
  return (*this)(s, t)(0, 0);
}

NumericalScalar CovarianceModelImplementation::computeAsScalar(const NumericalScalar s,
    const NumericalScalar t) const
{
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::computeAsScalar(const NumericalScalar s, const NumericalScalar t) const";
}

/* Computation of the covariance function */
CovarianceMatrix CovarianceModelImplementation::operator() (const NumericalScalar tau) const
{
  return operator() (NumericalPoint(1, tau));
}

CovarianceMatrix CovarianceModelImplementation::operator() (const NumericalPoint & tau) const
{
  return operator() (NumericalPoint(tau.getDimension()), tau);
}

NumericalScalar CovarianceModelImplementation::computeAsScalar (const NumericalPoint & tau) const
{
  if (dimension_ != 1) throw NotDefinedException(HERE) << "Error: the covariance model is of dimension=" << dimension_ << ", expected dimension=1.";
  return (*this)(tau)(0, 0);
}

NumericalScalar CovarianceModelImplementation::computeAsScalar(const NumericalScalar tau) const
{
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::computeAsScalar(const NumericalScalar tau) const";
}

/* Gradient */
Matrix CovarianceModelImplementation::partialGradient (const NumericalPoint & s,
    const NumericalPoint & t) const
{
  if (s.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point s has dimension=" << s.getDimension() << ", expected dimension=" << spatialDimension_;
  if (t.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the point t has dimension=" << t.getDimension() << ", expected dimension=" << spatialDimension_;
  Matrix gradient(spatialDimension_, dimension_ * dimension_);
  CovarianceMatrix covarianceST(operator()(s, t));
  // Convert result into MatrixImplementation to symmetrize & get the collection
  MatrixImplementation covarianceSTImplementation(*covarianceST.getImplementation());
  covarianceSTImplementation.symmetrize();
  const NumericalPoint centralValue(covarianceSTImplementation);
  const NumericalScalar epsilon(std::sqrt(SpecFunc::NumericalScalarEpsilon));
  // Loop over the shifted points
  for (UnsignedInteger i = 0; i < spatialDimension_; ++i)
  {
    NumericalPoint currentPoint(s);
    currentPoint[i] += epsilon;
    CovarianceMatrix localCovariance(operator()(currentPoint, t));
    MatrixImplementation localCovarianceImplementation(*localCovariance.getImplementation());
    localCovarianceImplementation.symmetrize();
    const NumericalPoint currentValue(localCovarianceImplementation);
    for (UnsignedInteger j = 0; j < centralValue.getDimension(); ++j)
      gradient(i, j) = (currentValue[j] - centralValue[j]) / epsilon;
  }
  return gradient;
}

/* Discretize the covariance function on a given TimeGrid/Mesh */
CovarianceMatrix CovarianceModelImplementation::discretize(const RegularGrid & timeGrid) const
{
  return discretize(timeGrid.getVertices());
}

struct CovarianceModelDiscretizePolicy
{
  const NumericalSample & input_;
  CovarianceMatrix & output_;
  const CovarianceModelImplementation & model_;
  const UnsignedInteger dimension_;

  CovarianceModelDiscretizePolicy(const NumericalSample & input,
                                  CovarianceMatrix & output,
                                  const CovarianceModelImplementation & model)
    : input_(input)
    , output_(output)
    , model_(model)
    , dimension_(model.getDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger jLocal(static_cast< UnsignedInteger >(sqrt(2.0 * i + 0.25) - 0.5));
      const UnsignedInteger jBase(jLocal * dimension_);
      const UnsignedInteger iLocal(i - (jLocal * (jLocal + 1)) / 2);
      const UnsignedInteger iBase(iLocal * dimension_);
      const CovarianceMatrix localCovariance(model_(input_[iLocal], input_[jLocal]));
      for (UnsignedInteger ii = 0; ii < dimension_; ++ii)
        for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
          output_(iBase + ii, jBase + jj) = localCovariance(ii, jj);
    }
  }

}; /* end struct CovarianceModelDiscretizePolicy */


CovarianceMatrix CovarianceModelImplementation::discretize(const NumericalSample & vertices) const
{
  if (vertices.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has a dimension=" << vertices.getDimension() << " different from the input dimension=" << spatialDimension_;
  const UnsignedInteger size(vertices.getSize());
  const UnsignedInteger fullSize(size * dimension_);
  CovarianceMatrix covarianceMatrix(fullSize);
  const CovarianceModelDiscretizePolicy policy( vertices, covarianceMatrix, *this );
  // The loop is over the lower block-triangular part
  TBB::ParallelFor( 0, size * (size + 1) / 2, policy );
  return covarianceMatrix;
}

CovarianceMatrix CovarianceModelImplementation::discretize(const Mesh & mesh) const
{
  return discretize(mesh.getVertices());
}

struct CovarianceModelScalarDiscretizeRowPolicy
{
  const NumericalSample & input_;
  const NumericalPoint p_;
  NumericalSample & output_;
  const CovarianceModelImplementation & model_;

  CovarianceModelScalarDiscretizeRowPolicy(const NumericalSample & input,
      const UnsignedInteger p,
      NumericalSample & output,
      const CovarianceModelImplementation & model)
    : input_(input)
    , p_(input[p])
    , output_(output)
    , model_(model)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i) output_[i][0] = model_.computeAsScalar(p_, input_[i]);
  }

}; /* end struct CovarianceModelScalarDiscretizeRowPolicy */

struct CovarianceModelDiscretizeRowPolicy
{
  const NumericalSample & input_;
  const NumericalPoint p_;
  NumericalSample & output_;
  const CovarianceModelImplementation & model_;
  const UnsignedInteger dimension_;

  CovarianceModelDiscretizeRowPolicy(const NumericalSample & input,
                                     const UnsignedInteger p,
                                     NumericalSample & output,
                                     const CovarianceModelImplementation & model)
    : input_(input)
    , p_(input[p])
    , output_(output)
    , model_(model)
    , dimension_(model.getDimension())
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const MatrixImplementation localCovariance(*(model_(p_, input_[i]).getImplementation()));
      const UnsignedInteger base(i * dimension_);
      for (UnsignedInteger ii = 0; ii < dimension_; ++ii)
        for (UnsignedInteger jj = 0; jj < dimension_; ++jj)
          output_[base + ii][jj] = localCovariance(ii, jj);
    }
  }

}; /* end struct CovarianceModelDiscretizeRowPolicy */

NumericalSample CovarianceModelImplementation::discretizeRow(const NumericalSample & vertices,
    const UnsignedInteger p) const
{
  if (vertices.getDimension() != spatialDimension_) throw InvalidArgumentException(HERE) << "Error: the given sample has a dimension=" << vertices.getDimension() << " different from the input dimension=" << spatialDimension_;
  const UnsignedInteger size(vertices.getSize());
  NumericalSample result(size * dimension_, dimension_);
  if (dimension_ == 1)
  {
    const CovarianceModelScalarDiscretizeRowPolicy policy( vertices, p, result, *this );
    TBB::ParallelFor( 0, size, policy );
  }
  else
  {
    const CovarianceModelDiscretizeRowPolicy policy( vertices, p, result, *this );
    TBB::ParallelFor( 0, size, policy );
  }
  return result;
}

  /** Discretize the covariance function on a given TimeGrid/Mesh using HMatrix */

HMatrix CovarianceModelImplementation::discretizeHMatrix(const RegularGrid & timeGrid,
                                                         const NumericalScalar nuggetFactor,
                                                         const HMatrixParameters & parameters) const
{
  return discretizeHMatrix(timeGrid.getVertices(), nuggetFactor, parameters);
}

HMatrix CovarianceModelImplementation::discretizeHMatrix(const Mesh & mesh,
                                                         const NumericalScalar nuggetFactor,
                                                         const HMatrixParameters & parameters) const
{
  return discretizeHMatrix(mesh.getVertices(), nuggetFactor, parameters);
}

HMatrix CovarianceModelImplementation::discretizeHMatrix(const NumericalSample & vertices,
                                                         const NumericalScalar nuggetFactor,
                                                         const HMatrixParameters & parameters) const
{
#ifdef OPENTURNS_HAVE_HMAT
  const UnsignedInteger size(vertices.getSize());
  HMatrixFactory hmatrixFactory;
  const NumericalScalar assemblyEpsilon = parameters.getAssemblyEpsilon();
  const NumericalScalar recompressionEpsilon = parameters.getRecompressionEpsilon();

  HMatrix covarianceHMatrix = hmatrixFactory.build(vertices, dimension_, true);
  // Set assembly & recompression epsilon
  covarianceHMatrix.getImplementation()->setKey("assembly-epsilon", OSS() << assemblyEpsilon);
  covarianceHMatrix.getImplementation()->setKey("recompression-epsilon", OSS() << recompressionEpsilon);
  if (dimension_ == 1)
  {
    CovarianceAssemblyFunction simple(*this, vertices, nuggetFactor);
    covarianceHMatrix.assemble(simple, 'L');
  }
  else
  {
    CovarianceBlockAssemblyFunction block(*this, vertices, nuggetFactor);
    covarianceHMatrix.assemble(block, 'L');
  }
  return covarianceHMatrix;
#else
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::discretizeHMatrix, OpenTURNS had been compiled without HMat support";
#endif
}


/* Amplitude accessor */
NumericalPoint CovarianceModelImplementation::getAmplitude() const
{
  return amplitude_;
}

void CovarianceModelImplementation::setAmplitude(const NumericalPoint & amplitude)
{
  for (UnsignedInteger index = 0; index < dimension_; ++index)
    if (amplitude[index] <= 0)
      throw InvalidArgumentException(HERE) << "Error - The component " << index << " of amplitude is non positive" ;
  amplitude_ = amplitude;
}

/* Scale accessor */
NumericalPoint CovarianceModelImplementation::getScale() const
{
  return scale_;
}

void CovarianceModelImplementation::setScale(const NumericalPoint & scale)
{
  for (UnsignedInteger index = 0; index < dimension_; ++index)
    if (scale[index] <= 0)
      throw InvalidArgumentException(HERE) << "Error - The component " << index << " of scale is non positive" ;
  scale_ = scale;
}

/* Spatial correlation accessor */
CorrelationMatrix CovarianceModelImplementation::getSpatialCorrelation() const
{
  if (!isDiagonal_) return spatialCorrelation_;
  return CorrelationMatrix(dimension_);
}

void CovarianceModelImplementation::setSpatialCorrelation(const CorrelationMatrix & spatialCorrelation)
{
  isDiagonal_ = spatialCorrelation.isDiagonal();
  if (!isDiagonal_) spatialCorrelation_ = spatialCorrelation;
}

/* Nugget factor accessor */
NumericalScalar CovarianceModelImplementation::getNuggetFactor() const
{
  return nuggetFactor_;
}

void CovarianceModelImplementation::setNuggetFactor(const NumericalScalar nuggetFactor)
{
  if (nuggetFactor < 0.0) throw InvalidArgumentException(HERE) << "Error: the nugget factor=" << nuggetFactor << " is negative";
  nuggetFactor_ = nuggetFactor;
}

/* Parameters accessor */
void CovarianceModelImplementation::setParameters(const NumericalPoint & parameters)
{
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::setParameters(const NumericalPoint & parameters)";
}

NumericalPointWithDescription CovarianceModelImplementation::getParameters() const
{
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::getParameters() const";
}

/* Is it a stationary model ? */
Bool CovarianceModelImplementation::isStationary() const
{
  return false;
}

/* Is it a diagonal model ? */
Bool CovarianceModelImplementation::isDiagonal() const
{
  return isDiagonal_;
}

/* Marginal accessor */
CovarianceModelImplementation::Implementation CovarianceModelImplementation::getMarginal(const UnsignedInteger index) const
{
  if (index >= dimension_) throw InvalidArgumentException(HERE) << "Error: index=" << index << " must be less than output dimension=" << dimension_;
  if (dimension_ != 1) throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::getMarginal(const UnsignedInteger index) const";
  return clone();
}

/* Drawing method for 1D models */
Graph CovarianceModelImplementation::draw(const UnsignedInteger rowIndex,
    const UnsignedInteger columnIndex) const
{
  if (rowIndex >= dimension_) throw InvalidArgumentException(HERE) << "Error: the given row index must be less than " << dimension_ << ", here rowIndex=" << rowIndex;
  if (columnIndex >= dimension_) throw InvalidArgumentException(HERE) << "Error: the given column index must be less than " << dimension_ << ", here columnIndex=" << columnIndex;
  throw NotYetImplementedException(HERE) << "In CovarianceModelImplementation::draw()";
}

/* String converter */
String CovarianceModelImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class= " << CovarianceModelImplementation::GetClassName();
  return oss;
}

/* String converter */
String CovarianceModelImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class= " << CovarianceModelImplementation::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CovarianceModelImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);

  adv.saveAttribute("spatialDimension_", spatialDimension_);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute("amplitude_", amplitude_);
  adv.saveAttribute("scale_", scale_);
  adv.saveAttribute("spatialCorrelation_", spatialCorrelation_);
  adv.saveAttribute("isDiagonal_", isDiagonal_);
  adv.saveAttribute("nuggetFactor_", nuggetFactor_);
}

/* Method load() reloads the object from the StorageManager */
void CovarianceModelImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);

  adv.loadAttribute("spatialDimension_", spatialDimension_);
  adv.loadAttribute("dimension_", dimension_);
  adv.loadAttribute("amplitude_", amplitude_);
  adv.loadAttribute("scale_", scale_);
  adv.loadAttribute("spatialCorrelation_", spatialCorrelation_);
  adv.loadAttribute("isDiagonal_", isDiagonal_);
  adv.loadAttribute("nuggetFactor_", nuggetFactor_);
}

END_NAMESPACE_OPENTURNS
