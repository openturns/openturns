//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/KroneckerCovarianceModel.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/GeneralizedExponential.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KroneckerCovarianceModel
 */

CLASSNAMEINIT(KroneckerCovarianceModel)

static const Factory<KroneckerCovarianceModel> Factory_KroneckerCovarianceModel;

/* Constructor from input dimension */
KroneckerCovarianceModel::KroneckerCovarianceModel(const UnsignedInteger inputDimension)
  : CovarianceModelImplementation(inputDimension)
  , rho_()
  , extraParameterNumber_(0)
{
  GeneralizedExponential rho(inputDimension);
  setCorrelationModel(rho);
}

/** Standard constructor with scale and amplitude parameters parameters */
KroneckerCovarianceModel::KroneckerCovarianceModel(const CovarianceModel &rho,
    const Point & amplitude)
  : CovarianceModelImplementation(rho.getInputDimension())
  , rho_()
  , extraParameterNumber_(0)
{
  outputDimension_ = amplitude.getDimension();
  amplitude_ = Point(outputDimension_, 1.0);
  setAmplitude(amplitude);
  setCorrelationModel(rho);
}

/** Standard constructor with scale and amplitude parameters parameters */
KroneckerCovarianceModel::KroneckerCovarianceModel(const CovarianceModel & rho,
    const Point & amplitude,
    const CorrelationMatrix & spatialCorrelation)
  : CovarianceModelImplementation(rho.getInputDimension())
  , rho_()
  , extraParameterNumber_(0)
{
  outputDimension_ = amplitude.getDimension();
  amplitude_ = Point(outputDimension_, 1.0);
  setAmplitude(amplitude);
  setCorrelationModel(rho);
  setOutputCorrelation(spatialCorrelation);
}

/** Standard constructor with scale, amplitude and spatial correlation parameters */
KroneckerCovarianceModel::KroneckerCovarianceModel(const CovarianceModel & rho,
    const CovarianceMatrix & sigma)
  : CovarianceModelImplementation(rho.getScale(), sigma)
  , rho_()
  , extraParameterNumber_(0)
{
  outputDimension_ = sigma.getDimension();
  outputCovariance_ = sigma;
  amplitude_ = Point(outputDimension_);
  isDiagonal_ = sigma.isDiagonal();
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
  {
    const Scalar amplitudeI = sqrt(sigma(i, i));
    if (!(amplitudeI > 0.0))
      throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::KroneckerCovarianceModel, the " << i << "-th diagonal component of the spatial covariance is non positive";
    amplitude_[i] = amplitudeI;
  }
  // Convert the spatial covariance into a spatial correlation
  if (!sigma.isDiagonal())
  {
    outputCorrelation_ = CorrelationMatrix(outputDimension_);
    for (UnsignedInteger j = 0; j < outputDimension_; ++j)
      for (UnsignedInteger i = j + 1; i < outputDimension_; ++i)
        outputCorrelation_(i, j) = sigma(i, j) / (amplitude_[i] * amplitude_[j]);
  } // !isDiagonal
  setCorrelationModel(rho);
}

/* Virtual constructor */
KroneckerCovarianceModel * KroneckerCovarianceModel::clone() const
{
  return new KroneckerCovarianceModel(*this);
}

void KroneckerCovarianceModel::setCorrelationModel(const CovarianceModel &rho)
{
  if (rho.getInputDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "Error in KroneckerCovarianceModel::KroneckerCovarianceModel : correlation model input dimension different from KroneckerCovarianceModel input dimension."
                                         << " Here, KroneckerCovarianceModel input dimension = " << inputDimension_ << ", whereas rho input dimension = " << rho.getInputDimension();

  if (rho.getOutputDimension() > 1)
    throw InvalidArgumentException(HERE) << "Error in KroneckerCovarianceModel::KroneckerCovarianceModel : correlation model should have output dimension equal to 1."
                                         << " Here, output dimension = " << rho.getOutputDimension();

  // Set the correlation model
  rho_ = rho;
  // Correlation model so amplitude is 1
  rho_.setAmplitude(Point(1, 1.0));

  // isStationary
  isStationary_ = rho_.getImplementation()->isStationary();

  // Initial copy of scale (for getScale purpose only)
  // scale is not necessary of size input dimension
  scale_ = rho_.getScale();

  // Active parameter
  // As scale_ is not necessary of size inputDimension_,
  // we should update the indices
  activeParameter_ = Indices(scale_.getSize() + outputDimension_);
  activeParameter_.fill();

  // Extra parameter of the correlation model
  extraParameterNumber_ = rho_.getFullParameter().getSize() - rho_.getScale().getSize() - rho_.getAmplitude().getSize();

  if (extraParameterNumber_)
  {
    const Indices rhoActiveParameter(rho_.getActiveParameter());
    const UnsignedInteger scaleAmplitudeRhoSize = scale_.getSize() + outputDimension_ * (outputDimension_ + 1) / 2;
    for (UnsignedInteger localIndex = 0; localIndex < extraParameterNumber_; ++localIndex)
    {
      const UnsignedInteger index = scale_.getSize() + 1 + localIndex;
      if (rhoActiveParameter.contains(index))
        activeParameter_.add(scaleAmplitudeRhoSize + index);
    }
  }
}

Matrix KroneckerCovarianceModel::partialGradient(const Point &s, const Point &t) const
{
  // Computation of the gradient
  if (s.getDimension() != getInputDimension())
    throw InvalidArgumentException(HERE) << "KroneckerCovarianceModel::partialGradient, the point s has dimension=" << s.getDimension() << ", expected dimension=" << getInputDimension();
  if (t.getDimension() != getInputDimension())
    throw InvalidArgumentException(HERE) << "KroneckerCovarianceModel::partialGradient, the point t has dimension=" << t.getDimension() << ", expected dimension=" << getInputDimension();

  // Covariance matrix is S * rho(tau), so gradient written as Sigma * grad(rho) where * is the Kronecker product,
  SquareMatrix covariance(outputCovariance_);
  covariance.getImplementation()->symmetrize();
  Point covariancePoint(*covariance.getImplementation());
  // Gradient using rho_ ==> inputDimension_ x 1 matrix
  const Matrix rhoGradient(rho_.partialGradient(s, t));

  Matrix gradient(getInputDimension(), covariancePoint.getDimension());
  // Final gradient is a kroneker product grad_{i,j} = gradRho_i * Cov_j
  // Cov seen as a vector of size outputDimension x outputDimension
  for (UnsignedInteger j = 0; j < covariancePoint.getDimension(); ++j)
    for (UnsignedInteger i = 0; i < getInputDimension(); ++i)
      gradient(i, j) = covariancePoint[j] * rhoGradient(i, 0);
  return gradient;
}

/* Is it safe to compute discretize in parallel? */
Bool KroneckerCovarianceModel::isParallel() const
{
  return rho_.getImplementation()->isParallel();
}

/* Computation of the covariance function, stationary interface
 * C(t, s)_{i,j} = amplitude_i * R_{i,j} * amplitude_j  * rho(t/scale, s/scale
 */
SquareMatrix KroneckerCovarianceModel::operator()(const Point &s,
    const Point &t) const
{
  SquareMatrix output(outputCovariance_);
  return output * rho_.computeAsScalar(s, t);
}

SquareMatrix KroneckerCovarianceModel::operator()(const Point &tau) const
{
  SquareMatrix output(outputCovariance_);
  return output * rho_.computeAsScalar(tau);
}

Scalar KroneckerCovarianceModel::computeAsScalar(const Point &s,
    const Point &t) const
{
  if (outputDimension_ > 1)
    throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::computeAsScalar: expected output dimension equal to 1. Here output dimension =" << getOutputDimension();
  if (s.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::computeAsScalar: expected a shift of dimension=" << getInputDimension() << ", got dimension=" << s.getDimension();
  if (t.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::computeAsScalar: expected a shift of dimension=" << getInputDimension() << ", got dimension=" << t.getDimension();
  // Compute correlation
  const Scalar rho = rho_.computeAsScalar(s, t);
  return amplitude_[0] * amplitude_[0] * rho;
}

Scalar KroneckerCovarianceModel::computeAsScalar(const Point &tau) const
{
  if (outputDimension_ > 1)
    throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::computeAsScalar: expected output dimension equal to 1. Here output dimension =" << getOutputDimension();
  if (tau.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::computeAsScalar: expected a shift of dimension=" << getInputDimension() << ", got dimension=" << tau.getDimension();
  const Scalar rho = rho_.computeAsScalar(tau);
  return amplitude_[0] * amplitude_[0] * rho;
}

Scalar KroneckerCovarianceModel::computeRhoOverSample(const Collection<Scalar>::const_iterator &s_begin,
    const Collection<Scalar>::const_iterator &t_begin) const
{
  // rho_.computeAsScalar here plays the role of correlation only
  // Purpose is to return the evaluation of rho_ model over a sample
  // similarly to the former computeStandardRepresentative
  return rho_.getImplementation()->computeAsScalar(s_begin, t_begin);
}

Scalar KroneckerCovarianceModel::computeAsScalar(const Collection<Scalar>::const_iterator &s_begin,
    const Collection<Scalar>::const_iterator &t_begin) const
{
  // Limiting outputDimension ==> Make sure there is no other usage
  if (outputDimension_ > 1)
    throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::computeAsScalar: expected output dimension equal to 1. Here output dimension =" << getOutputDimension();
  return amplitude_[0] * amplitude_[0] * rho_.getImplementation()->computeAsScalar(s_begin, t_begin);
}

struct KroneckerModelDiscretizePolicy
{
  const SampleImplementation &input_;
  // output_ is a CovarianceMatrix, but since we fill only its half part,
  // we can directly store results in the underlying MatrixImplementation.
  // This avoids comparing row and column numbers.
  MatrixImplementation & output_;
  const KroneckerCovarianceModel & model_;
  const UnsignedInteger inputDimension_;

  KroneckerModelDiscretizePolicy(const Sample &input,
                                 CovarianceMatrix & output,
                                 const KroneckerCovarianceModel & model)
    : input_(*input.getImplementation()), output_(*output.getImplementation()), model_(model), inputDimension_(input_.getDimension())
  {
  }

  inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> &r) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
    {
      const UnsignedInteger jLocal = static_cast<UnsignedInteger>(sqrt(2.0 * i + 0.25) - 0.5);
      const UnsignedInteger iLocal = i - (jLocal * (jLocal + 1)) / 2;
      // By construction, iLocal <= jLocal
      output_(jLocal, iLocal) = model_.computeRhoOverSample(input_.data_begin() + iLocal * inputDimension_, input_.data_begin() + jLocal * inputDimension_);
    }
  }
  static void genericKroneckerProduct(const SquareMatrix &leftMatrix, const SquareMatrix &rightMatrix, SquareMatrix &productMatrix)
  {
    const MatrixImplementation &left(*leftMatrix.getImplementation());
    const MatrixImplementation &right(*rightMatrix.getImplementation());
    MatrixImplementation &product(*productMatrix.getImplementation());
    const UnsignedInteger dimension = right.getDimension();
    for (UnsignedInteger i = 0; i < left.getDimension(); ++i)
    {
      for (UnsignedInteger j = 0; j <= i; ++j)
      {
        const Scalar left_ij = left(i, j);
        for (UnsignedInteger k = 0; k < dimension; ++k)
        {
          for (UnsignedInteger l = 0; l < dimension; ++l)
          {
            product(i * dimension + k, j * dimension + l) = left_ij * right(k, l);
          }
        }
      }
    }
  }
  CovarianceMatrix kroneckerProduct(const CovarianceMatrix &spatialCovariance) const
  {
    CovarianceMatrix covarianceMatrix(input_.getSize() * spatialCovariance.getDimension());
    genericKroneckerProduct(output_, spatialCovariance, covarianceMatrix);
    return covarianceMatrix;
  }
  TriangularMatrix kroneckerProductCholeskyFactor(const TriangularMatrix &spatialCovarianceCholeskyFactor) const
  {
    TriangularMatrix rhoCholeskyFactor(output_.computeCholesky());
    TriangularMatrix outputMatrix(rhoCholeskyFactor.getDimension() * spatialCovarianceCholeskyFactor.getDimension(), true);
    genericKroneckerProduct(rhoCholeskyFactor, spatialCovarianceCholeskyFactor, outputMatrix);
    return outputMatrix;
  }
}; /* end struct KroneckerModelDiscretizePolicy */

CovarianceMatrix KroneckerCovarianceModel::discretize(const Sample &vertices) const
{
  if (vertices.getDimension() != inputDimension_)
    throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::discretize: the given sample has a dimension=" << vertices.getDimension() << " different from the input dimension=" << inputDimension_;
  const UnsignedInteger size = vertices.getSize();
  CovarianceMatrix rhoMatrix(size);
  const KroneckerModelDiscretizePolicy policy(vertices, rhoMatrix, *this);
  // The loop is over the lower block-triangular part
  TBBImplementation::ParallelForIf(isParallel(), 0, size * (size + 1) / 2, policy);
  rhoMatrix.checkSymmetry();
  outputCovariance_.checkSymmetry();
  // Compute the Kronecker product of rhoMatrix by outputCovariance_
  return policy.kroneckerProduct(outputCovariance_);
}

TriangularMatrix KroneckerCovarianceModel::discretizeAndFactorize(const Sample &vertices) const
{
  const UnsignedInteger size = vertices.getSize();
  CovarianceMatrix rhoMatrix(size);
  const KroneckerModelDiscretizePolicy policy(vertices, rhoMatrix, *this);
  // The loop is over the lower block-triangular part
  TBBImplementation::ParallelForIf(isParallel(), 0, size * (size + 1) / 2, policy);
  // Compute the Cholesky factor of outputCovariance_
  if (outputCovarianceCholeskyFactor_.getDimension() == 0)
    outputCovarianceCholeskyFactor_ = outputCovariance_.computeCholesky();
  // Compute the Kronecker product of rhoCholeskyFactor by outputCovarianceCholeskyFactor_
  return policy.kroneckerProductCholeskyFactor(outputCovarianceCholeskyFactor_);
}

Point KroneckerCovarianceModel::getFullParameter() const
{
  // First generic parameter
  // Generic parameter returns scale, amplitude, correlation
  Point parameter(CovarianceModelImplementation::getFullParameter());
  if (extraParameterNumber_)
  {
    // Check if rho_ get extra parameters
    const UnsignedInteger rhoParameterSize = rho_.getFullParameter().getSize();
    const Point rhoParameter(rho_.getFullParameter());
    for (UnsignedInteger k = scale_.getSize() + 1; k < rhoParameterSize; ++k)
      parameter.add(rhoParameter[k]);
  }
  return parameter;
}

void KroneckerCovarianceModel::setFullParameter(const Point & parameter)
{
  // The true total size should also contains the additional parameter of rho_
  // Check if rho_ get extra parameters
  const UnsignedInteger rhoThetaSize = rho_.getScale().getSize();
  const UnsignedInteger totalSize = rhoThetaSize + outputDimension_ * (outputDimension_ + 1) / 2 + extraParameterNumber_;
  if (parameter.getSize() < totalSize)
    throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::setFullParameter, points have incompatible size. Point size = " << parameter.getSize()
                                         << " whereas expected size = " << totalSize;
  // set scale, amplitude, correlation
  // Can not rely on the parent method as scale_.getSize() is not
  // necessary input dimension
  UnsignedInteger index = 0;
  // Check the size
  for (UnsignedInteger i = 0; i < scale_.getDimension(); ++i)
  {
    if (!(parameter[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::setFullParameter, the component " << index << " of scale is non positive";
    scale_[i] = parameter[index];
    ++index;
  }
  // Second the amplitude parameter
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
  {
    if (!(parameter[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::setFullParameter, the component " << index << " of amplitude is non positive";
    amplitude_[i] = parameter[index];
    ++index;
  }
  CorrelationMatrix outputCorrelation(outputDimension_);
  // Third the output correlation parameter, only the lower triangle
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      outputCorrelation(i, j) = parameter[index];
      ++index;
    }
  setOutputCorrelation(outputCorrelation);
  // Set scale at rho level
  rho_.setScale(scale_);
  if (extraParameterNumber_ > 0)
  {
    // rho parameter
    // amplitude is unchanged (1)
    Point rhoParameter(rho_.getFullParameter());
    index = rhoThetaSize + outputDimension_ * (outputDimension_ + 1) / 2;
    for (UnsignedInteger localIndex = 0; localIndex < extraParameterNumber_; ++localIndex)
    {
      rhoParameter[rhoThetaSize + 1 + localIndex] = parameter[index];
      ++ index;
    }
    rho_.setFullParameter(rhoParameter);
  }
}

Description KroneckerCovarianceModel::getFullParameterDescription() const
{
  // First the scale parameter
  Description description(0);
  // First the scale parameter
  for (UnsignedInteger j = 0; j < scale_.getDimension(); ++j)
    description.add(OSS() << "scale_" << j);
  // Second the amplitude parameter
  for (UnsignedInteger j = 0; j < outputDimension_; ++j)
    description.add(OSS() << "amplitude_" << j);
  // Third the spatial correlation parameter, only the lower triangle
  for (UnsignedInteger i = 0; i < outputDimension_; ++i)
    for (UnsignedInteger j = 0; j < i; ++j)
      description.add(OSS() << "R_" << i << "_" << j);

  // Check if rho_ get extra parameters
  const UnsignedInteger rhoParameterSize = rho_.getFullParameter().getSize();
  if (rhoParameterSize > scale_.getSize() + 1)
  {
    const Description rhoParameterDescription(rho_.getFullParameterDescription());
    for (UnsignedInteger k = scale_.getSize() + 1; k < rhoParameterSize; ++k)
      description.add(rhoParameterDescription[k]);
  }
  return description;
}

void KroneckerCovarianceModel::setScale(const Point &scale)
{
  // Scale is usually of size inputDimension_
  // But if model is isotropic it should be of size 1
  if (scale.getDimension() != rho_.getScale().getSize())
    throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::setScale: the given scale has a dimension=" << scale.getDimension() << " different from the input dimension=" << inputDimension_;
  for (UnsignedInteger index = 0; index < scale.getSize(); ++index)
    if (!(scale[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In KroneckerCovarianceModel::setScale: the component " << index << " of scale is non positive";
  rho_.setScale(scale);
  // copy (at least for getScale)
  scale_ = scale;
}


/* String converter */
String KroneckerCovarianceModel::__repr__() const
{
  OSS oss(true);
  oss << "class=" << KroneckerCovarianceModel::GetClassName();
  oss << " rho=" << rho_
      << " amplitude=" << getAmplitude()
      << " spatial correlation=" << getOutputCorrelation()
      << " isDiagonal=" << isDiagonal();
  return oss;
}

/* String converter */
String KroneckerCovarianceModel::__str__(const String & offset) const
{
  OSS oss(false);
  oss << KroneckerCovarianceModel::GetClassName();
  oss << "(rho=" << rho_.__str__(offset)
      << ", amplitude=" << getAmplitude();
  if (!isDiagonal_)
    oss << ", spatial correlation=" << Os::GetEndOfLine() << offset << getOutputCorrelation().__str__(offset);
  else
    oss << ", no spatial correlation";
  oss << ")";
  return oss;
}

void KroneckerCovarianceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("rho_", rho_);
  adv.saveAttribute("extraParameterNumber_", extraParameterNumber_);
}

/* Method load() reloads the object from the StorageManager */
void KroneckerCovarianceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("rho_", rho_);
  adv.loadAttribute("extraParameterNumber_", extraParameterNumber_);
}

END_NAMESPACE_OPENTURNS
