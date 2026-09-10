//                                               -*- C++ -*-
/**
 *  @file  HODLRMatrixImplementation.cxx
 *  @brief HODLR compressed matrix implementation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
 */
#include "openturns/HODLRMatrixImplementation.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/IsotropicCovarianceModel.hxx"
#include "openturns/MaternModel.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/** Evaluator that scales another evaluator by a constant factor. */
class HODLRScalingEvaluator
  : public HODLREntryEvaluator
{
public:
  HODLRScalingEvaluator(Pointer<const HODLREntryEvaluator> inner, Scalar alpha)
    : inner_(inner)
    , alpha_(alpha)
  {
  }

  HODLRScalingEvaluator* clone() const override
  {
    return new HODLRScalingEvaluator(inner_, alpha_);
  }

  Scalar operator()(UnsignedInteger i, UnsignedInteger j) const override
  {
    return alpha_ * (*inner_)(i, j);
  }

  UnsignedInteger getSize() const override
  {
    return inner_->getSize();
  }

private:
  Pointer<const HODLREntryEvaluator> inner_;
  Scalar alpha_;
};

/** Evaluator that permutes the indices of another evaluator. */
class HODLRPermutedEvaluator
  : public HODLREntryEvaluator
{
public:
  HODLRPermutedEvaluator(Pointer<const HODLREntryEvaluator> inner, const Indices & permutation)
    : inner_(inner)
    , permutation_(permutation)
  {
  }

  HODLRPermutedEvaluator* clone() const override
  {
    return new HODLRPermutedEvaluator(inner_, permutation_);
  }

  Scalar operator()(UnsignedInteger i, UnsignedInteger j) const override
  {
    return (*inner_)(permutation_[i], permutation_[j]);
  }

  UnsignedInteger getSize() const override
  {
    return inner_->getSize();
  }

private:
  Pointer<const HODLREntryEvaluator> inner_;
  Indices permutation_;
};

CLASSNAMEINIT(HODLRMatrixImplementation)

static const Factory<HODLRMatrixImplementation> Factory_HODLRMatrixImplementation;

HODLRMatrixImplementation::HODLRMatrixImplementation()
  : PersistentObject()
  , n_(0)
  , p_node_()
  , isFactorized_(false)
  , symmetric_(true)
  , logDet_(0.0)
  , shiftAccumulated_(0.0)
{
}

HODLRMatrixImplementation::HODLRMatrixImplementation(const HODLRMatrixImplementation& other)
  : PersistentObject(other)
  , n_(other.n_)
  , p_node_()
  , isFactorized_(false)
  , symmetric_(other.symmetric_)
  , logDet_(0.0)
  , parameters_(other.parameters_)
  , diagonal_(other.diagonal_)
  , shiftAccumulated_(other.shiftAccumulated_)
  , p_evaluator_(other.p_evaluator_)
  , permutation_(other.permutation_)
  , inversePermutation_(other.inversePermutation_)
{
}

HODLRMatrixImplementation* HODLRMatrixImplementation::clone() const
{
  return new HODLRMatrixImplementation(*this);
}

HODLRMatrixImplementation::~HODLRMatrixImplementation()
{
  // Nothing to do
}

HODLRMatrixImplementation& HODLRMatrixImplementation::operator=(const HODLRMatrixImplementation& other)
{
  if (this != &other)
  {
    PersistentObject::operator=(other);
    p_node_.reset();
    n_ = other.n_;
    isFactorized_ = false;
    symmetric_ = other.symmetric_;
    logDet_ = 0.0;
    parameters_ = other.parameters_;
    diagonal_ = other.diagonal_;
    shiftAccumulated_ = other.shiftAccumulated_;
    p_evaluator_ = other.p_evaluator_;
    permutation_ = other.permutation_;
    inversePermutation_ = other.inversePermutation_;
  }
  return *this;
}

UnsignedInteger HODLRMatrixImplementation::getNbRows() const
{
  return n_;
}

UnsignedInteger HODLRMatrixImplementation::getNbColumns() const
{
  return n_;
}

const HODLRMatrixParameters& HODLRMatrixImplementation::getParameters() const
{
  return parameters_;
}

void HODLRMatrixImplementation::setPermutation(const Indices& permutation)
{
  const UnsignedInteger size = permutation.getSize();
  if (size == 0)
  {
    permutation_.clear();
    inversePermutation_.clear();
  }
  else
  {
    if (size != n_)
      throw InvalidArgumentException(HERE) << "Expected a permutation of size " << n_ << ", got " << size;
    Indices seen(size, 0);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      if (permutation[i] >= size)
        throw InvalidArgumentException(HERE) << "Permutation entry " << permutation[i] << " out of range [0, " << size << ")";
      if (seen[permutation[i]] == 1)
        throw InvalidArgumentException(HERE) << "Permutation entry " << permutation[i] << " appears more than once";
      seen[permutation[i]] = 1;
    }
    permutation_ = permutation;
    inversePermutation_ = Indices(size);
    for (UnsignedInteger i = 0; i < size; ++i)
      inversePermutation_[permutation_[i]] = i;
  }
  // The permutation is used at assembly time, so invalidate the assembled structure
  p_node_.reset();
  p_evaluator_.reset();
  diagonal_.clear();
  isFactorized_ = false;
}

Indices HODLRMatrixImplementation::getPermutation() const
{
  return permutation_;
}

void HODLRMatrixImplementation::assemble(const HODLRRealAssemblyFunction& f, char symmetry)
{
  assemble(f, parameters_, symmetry);
}

void HODLRMatrixImplementation::assemble(const HODLRRealAssemblyFunction& f,
                                         const HODLRMatrixParameters& parameters,
                                         char symmetry)
{
  if (n_ == 0)
    throw InvalidArgumentException(HERE) << "Empty HODLRMatrix";

  // Validate the symmetry flag against the value given at build() time
  switch (symmetry)
  {
    case 'L':
    case 'l':
    case 'U':
    case 'u':
      if (!symmetric_)
        throw InvalidArgumentException(HERE) << "HODLRMatrix was built with symmetric=false but assembled with symmetry '" << symmetry << "'";
      break;
    case 'N':
    case 'n':
    case 'F':
    case 'f':
      if (symmetric_)
        throw InvalidArgumentException(HERE) << "HODLRMatrix was built with symmetric=true but assembled with symmetry '" << symmetry << "'";
      break;
    default:
      throw InvalidArgumentException(HERE) << "Invalid symmetry flag '" << symmetry << "', must be one of 'L', 'U', 'N' or 'F'";
  }

  parameters_ = parameters;

  p_evaluator_.reset(f.clone());
  if (permutation_.getSize() > 0)
    p_evaluator_ = new HODLRPermutedEvaluator(p_evaluator_, permutation_);

  // Build diagonal in the (possibly permuted) assembly order
  diagonal_ = Point(n_);
  for (UnsignedInteger i = 0; i < n_; ++i)
    diagonal_[i] = (*p_evaluator_)(i, i);

  // Build the HODLR tree
  p_node_.reset();
  isFactorized_ = false;

  p_node_ = new HODLRNode(p_evaluator_, &diagonal_[0], 0, n_,
                        parameters.getMinLeafSize(),
                        parameters.getMaxRank(),
                        parameters.getAssemblyEpsilon());

  LOGDEBUG(OSS() << "HODLRMatrixImplementation::assemble done, n=" << n_);
}

void HODLRMatrixImplementation::rebuild()
{
  p_node_.reset();
  isFactorized_ = false;
  p_node_ = new HODLRNode(p_evaluator_, &diagonal_[0], 0, n_,
                          parameters_.getMinLeafSize(),
                          parameters_.getMaxRank(),
                          parameters_.getAssemblyEpsilon());
}

void HODLRMatrixImplementation::factorize()
{
  if (n_ == 0)
    throw InvalidArgumentException(HERE) << "Empty HODLRMatrix";

  if (!p_node_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not assembled";

  // Regularization loop: try factorization with increasing shift on failure.
  // Follows the same pattern as HMatrixImplementation::factorize().
  const Scalar regularizationEpsilon = ResourceMap::GetAsScalar("HODLRMatrix-RegularizationEpsilon");
  const UnsignedInteger maxIterations = ResourceMap::GetAsUnsignedInteger("HODLRMatrix-FactorizationIterations");
  if (maxIterations == 0)
    throw InvalidArgumentException(HERE) << "HODLRMatrix-FactorizationIterations must be positive";

  Scalar regularizationShift = shiftAccumulated_;
  for (UnsignedInteger iteration = 0; iteration < maxIterations; ++iteration)
  {
    p_node_->setShift(regularizationShift);
    try
    {
      p_node_->computeCholesky();
      shiftAccumulated_ = regularizationShift;
      break;
    }
    catch (const InternalException&)
    {
      LOGDEBUG(OSS() << "HODLRMatrix::factorize attempt " << iteration
                << " failed, regularizationShift=" << regularizationShift);
      if (regularizationShift == 0.0)
        regularizationShift = regularizationEpsilon;
      else
        regularizationShift *= 2.0;
      if (iteration == maxIterations - 1)
        throw InternalException(HERE) << "HODLRMatrix::factorize failed after "
                                      << maxIterations << " regularization attempts, last shift=" << regularizationShift;
      rebuild();
    }
  }

  logDet_ = p_node_->getLogDeterminant();
  isFactorized_ = true;
  // Warn when the accumulated regularization shift is non-negligible relative
  // to the diagonal: the factorization then approximates A + shift * I rather
  // than A, so the solve() and logDeterminant() results are for the shifted
  // matrix and are silently degraded.
  if (shiftAccumulated_ > 0.0)
  {
    Scalar maxDiag = 0.0;
    for (UnsignedInteger i = 0; i < n_; ++i)
      if (diagonal_[i] > maxDiag) maxDiag = diagonal_[i];
    const Scalar warnThreshold = ResourceMap::GetAsScalar("HODLRMatrix-RegularizationWarnThreshold");
    if ((maxDiag > 0.0) && (shiftAccumulated_ / maxDiag >= warnThreshold))
      LOGWARN(OSS() << "HODLRMatrix: the Cholesky factorization required a regularization shift of "
             << shiftAccumulated_ << " (ratio " << shiftAccumulated_ / maxDiag
             << " of the diagonal, threshold " << warnThreshold
             << "). The factorized matrix is A + shift * I, so the solve() and logDeterminant() "
             << "results are for the regularized matrix and may be inaccurate. Add a nugget or "
             << "increase the HODLRMatrix-RegularizationEpsilon / -Nugget values.");
  }
  LOGDEBUG(OSS() << "HODLRMatrix::factorize() done, log_det=" << logDet_);
}

/** Get the regularization shift applied during the last factorization */
Scalar HODLRMatrixImplementation::getRegularizationShift() const
{
  return shiftAccumulated_;
}

void HODLRMatrixImplementation::scale(Scalar alpha)
{
  if (n_ == 0)
    throw InvalidArgumentException(HERE) << "Empty HODLRMatrix";

  if (!p_evaluator_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not assembled";

  for (UnsignedInteger i = 0; i < n_; ++i)
    diagonal_[i] *= alpha;

  shiftAccumulated_ *= alpha;

  p_evaluator_ = new HODLRScalingEvaluator(p_evaluator_, alpha);
  rebuild();
}

void HODLRMatrixImplementation::gemv(char trans, Scalar alpha, const Point& x, Scalar beta, Point& y) const
{
  if (x.getDimension() != n_)
    throw InvalidArgumentException(HERE) << "x dimension mismatch";
  if (y.getDimension() != n_)
    throw InvalidArgumentException(HERE) << "y dimension mismatch";
  if (trans != 'N' && trans != 'n')
    throw InvalidArgumentException(HERE) << "trans must be 'N' (no transpose)";

  if (!p_node_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not assembled";
  if (!isFactorized_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not factorized";

  Matrix xmat(n_, 1);
  Matrix ymat(n_, 1);
  Scalar* xmat_data = &(*xmat.getImplementation())[0];
  Scalar* ymat_data = &(*ymat.getImplementation())[0];
  for (UnsignedInteger i = 0; i < n_; ++i)
  {
    const UnsignedInteger iPerm = permutation_.getSize() > 0 ? permutation_[i] : i;
    xmat_data[i] = x[iPerm];
    ymat_data[i] = 0.0;
  }

  // A x = L (L^T x): after the Cholesky factorization the tree describes
  // the Schur complements (not the original matrix), so the full matrix
  // product is obtained by applying the factor and its transpose.
  Matrix zmat(n_, 1, 0.0);
  p_node_->applyFactorTranspose(zmat, xmat);
  p_node_->applyFactor(ymat, zmat);

  for (UnsignedInteger i = 0; i < n_; ++i)
  {
    const UnsignedInteger iPerm = permutation_.getSize() > 0 ? inversePermutation_[i] : i;
    y[i] = alpha * ymat_data[iPerm] + beta * y[i];
  }
}

void HODLRMatrixImplementation::applyFactor(Point& y, const Point& x) const
{
  if (x.getDimension() != n_)
    throw InvalidArgumentException(HERE) << "x dimension mismatch";
  if (y.getDimension() != n_)
    throw InvalidArgumentException(HERE) << "y dimension mismatch";

  if (!p_node_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not assembled";
  if (!isFactorized_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not factorized";

  Matrix xmat(n_, 1);
  Matrix ymat(n_, 1, 0.0);
  Scalar* xmat_data = &(*xmat.getImplementation())[0];
  Scalar* ymat_data = &(*ymat.getImplementation())[0];
  for (UnsignedInteger i = 0; i < n_; ++i)
  {
    const UnsignedInteger iPerm = permutation_.getSize() > 0 ? permutation_[i] : i;
    xmat_data[i] = x[iPerm];
  }

  // y = L x where L is the lower-triangular Cholesky factor of A
  p_node_->applyFactor(ymat, xmat);

  for (UnsignedInteger i = 0; i < n_; ++i)
  {
    const UnsignedInteger iPerm = permutation_.getSize() > 0 ? inversePermutation_[i] : i;
    y[i] = ymat_data[iPerm];
  }
}

void HODLRMatrixImplementation::addIdentity(Scalar alpha)
{
  shiftAccumulated_ += alpha;
  for (UnsignedInteger i = 0; i < n_; ++i)
    diagonal_[i] += alpha;
  if (p_node_)
    p_node_->setShift(shiftAccumulated_);
  isFactorized_ = false;
}

void HODLRMatrixImplementation::applyNugget()
{
  const Scalar nugget = ResourceMap::GetAsScalar("HODLRMatrix-Nugget");
  if (nugget <= 0.0)
    return;
  Scalar meanDiagonal = 0.0;
  for (UnsignedInteger i = 0; i < n_; ++i)
    meanDiagonal += diagonal_[i];
  meanDiagonal /= n_;
  addIdentity(nugget * meanDiagonal);
}

Scalar HODLRMatrixImplementation::norm() const
{
  throw NotYetImplementedException(HERE) << "HODLRMatrixImplementation::norm";
}

Point HODLRMatrixImplementation::getDiagonal() const
{
  // The diagonal is stored as data and survives copies (which drop the tree),
  // so only require the diagonal to be populated, not the tree itself.
  if (diagonal_.getSize() == 0)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not assembled";
  if (permutation_.getSize() == 0)
    return diagonal_;
  Point result(n_);
  for (UnsignedInteger i = 0; i < n_; ++i)
    result[i] = diagonal_[inversePermutation_[i]];
  return result;
}

Point HODLRMatrixImplementation::solve(const Point& b, Bool trans) const
{
  if (trans)
    throw NotYetImplementedException(HERE) << "transposed not yet supported in HODLRMatrixImplementation::solve";

  if (b.getDimension() != n_)
    throw InvalidArgumentException(HERE) << "x dimension mismatch";

  if (!p_node_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not assembled";

  if (!isFactorized_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not factorized";

  Matrix x(b.getDimension(), 1);
  MatrixImplementation& xImpl = *x.getImplementation();
  for (UnsignedInteger i = 0; i < b.getDimension(); ++i)
  {
    const UnsignedInteger iPerm = permutation_.getSize() > 0 ? permutation_[i] : i;
    xImpl[i] = b[iPerm];
  }

  p_node_->solve(x);

  Point result(b.getDimension());
  for (UnsignedInteger i = 0; i < b.getDimension(); ++i)
  {
    const UnsignedInteger iPerm = permutation_.getSize() > 0 ? inversePermutation_[i] : i;
    result[i] = xImpl[iPerm];
  }
  return result;
}

Matrix HODLRMatrixImplementation::solve(const Matrix& m, Bool trans) const
{
  if (trans)
    throw NotYetImplementedException(HERE) << "transposed not yet supported in HODLRMatrixImplementation::solve";

  if (m.getNbRows() != n_)
    throw InvalidArgumentException(HERE) << "x dimension mismatch";

  if (!p_node_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not assembled";

  if (!isFactorized_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not factorized";

  Matrix x(m.getNbRows(), m.getNbColumns());
  MatrixImplementation& xImpl = *x.getImplementation();
  const Scalar* m_data = &(*m.getImplementation())[0];
  const UnsignedInteger nRhs = m.getNbColumns();
  const UnsignedInteger nRows = m.getNbRows();
  for (UnsignedInteger j = 0; j < nRhs; ++j)
    for (UnsignedInteger i = 0; i < nRows; ++i)
    {
      const UnsignedInteger iPerm = permutation_.getSize() > 0 ? permutation_[i] : i;
      xImpl[i + j * nRows] = m_data[iPerm + j * nRows];
    }

  p_node_->solve(x);

  Matrix result(m.getNbRows(), m.getNbColumns());
  MatrixImplementation& resultImpl = *result.getImplementation();
  Scalar* x_data = &(*x.getImplementation())[0];
  for (UnsignedInteger j = 0; j < nRhs; ++j)
    for (UnsignedInteger i = 0; i < nRows; ++i)
    {
      const UnsignedInteger iPerm = permutation_.getSize() > 0 ? inversePermutation_[i] : i;
      resultImpl[i + j * nRows] = x_data[iPerm + j * nRows];
    }
  return result;
}

Point HODLRMatrixImplementation::solveLower(const Point& b, Bool trans) const
{
  if (b.getDimension() != n_)
    throw InvalidArgumentException(HERE) << "x dimension mismatch";

  if (!p_node_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not assembled";

  if (!isFactorized_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not factorized";

  Matrix x(b.getDimension(), 1);
  MatrixImplementation& xImpl = *x.getImplementation();
  for (UnsignedInteger i = 0; i < b.getDimension(); ++i)
  {
    const UnsignedInteger iPerm = permutation_.getSize() > 0 ? permutation_[i] : i;
    xImpl[i] = b[iPerm];
  }

  p_node_->solveLower(x, trans);

  Point result(b.getDimension());
  for (UnsignedInteger i = 0; i < b.getDimension(); ++i)
  {
    const UnsignedInteger iPerm = permutation_.getSize() > 0 ? inversePermutation_[i] : i;
    result[i] = xImpl[iPerm];
  }
  return result;
}

Matrix HODLRMatrixImplementation::solveLower(const Matrix& m, Bool trans) const
{
  if (m.getNbRows() != n_)
    throw InvalidArgumentException(HERE) << "x dimension mismatch";

  if (!p_node_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not assembled";

  if (!isFactorized_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not factorized";

  Matrix x(m.getNbRows(), m.getNbColumns());
  MatrixImplementation& xImpl = *x.getImplementation();
  const Scalar* m_data = &(*m.getImplementation())[0];
  const UnsignedInteger nRhs = m.getNbColumns();
  const UnsignedInteger nRows = m.getNbRows();
  for (UnsignedInteger j = 0; j < nRhs; ++j)
    for (UnsignedInteger i = 0; i < nRows; ++i)
    {
      const UnsignedInteger iPerm = permutation_.getSize() > 0 ? permutation_[i] : i;
      xImpl[i + j * nRows] = m_data[iPerm + j * nRows];
    }

  p_node_->solveLower(x, trans);

  Matrix result(m.getNbRows(), m.getNbColumns());
  MatrixImplementation& resultImpl = *result.getImplementation();
  Scalar* x_data = &(*x.getImplementation())[0];
  for (UnsignedInteger j = 0; j < nRhs; ++j)
    for (UnsignedInteger i = 0; i < nRows; ++i)
    {
      const UnsignedInteger iPerm = permutation_.getSize() > 0 ? inversePermutation_[i] : i;
      resultImpl[i + j * nRows] = x_data[iPerm + j * nRows];
    }
  return result;
}

Scalar HODLRMatrixImplementation::logDeterminant() const
{
  if (!p_node_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not assembled";
  if (!isFactorized_)
    throw InvalidArgumentException(HERE) << "HODLRMatrix not factorized";
  // Cholesky: logDet_ holds log|L| = 0.5*log|A| (sum of log diag over all leaves)
  return 2.0 * logDet_;
}

std::pair<size_t, size_t> HODLRMatrixImplementation::compressionRatio() const
{
  if (!p_node_)
    return std::make_pair(static_cast<size_t>(0), static_cast<size_t>(n_) * n_);
  const size_t compressed = p_node_->getNnz();
  const size_t uncompressed = static_cast<size_t>(n_) * n_;
  return std::make_pair(compressed, uncompressed);
}

std::pair<size_t, size_t> HODLRMatrixImplementation::fullrkRatio() const
{
  const size_t full = static_cast<size_t>(n_) * n_;
  const size_t lr = p_node_ ? p_node_->getTotalRank() : 0;
  return std::make_pair(full, lr);
}

void HODLRMatrixImplementation::dump(const String& name) const
{
  LOGDEBUG(OSS() << "HODLRMatrix::dump(" << name << ")");
}

String HODLRMatrixImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class= " << HODLRMatrixImplementation::GetClassName()
      << ", n= " << n_
      << ", factorized= " << isFactorized_;
  return oss;
}

String HODLRMatrixImplementation::__str__(const String&) const
{
  OSS oss(false);
  oss << "class= " << HODLRMatrixImplementation::GetClassName();
  return oss;
}

void HODLRMatrixImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("n_", n_);
  adv.saveAttribute("symmetric_", symmetric_);
  adv.saveAttribute("parameters_", parameters_);
  adv.saveAttribute("diagonal_", diagonal_);
  adv.saveAttribute("shiftAccumulated_", shiftAccumulated_);
  adv.saveAttribute("permutation_", permutation_);
  adv.saveAttribute("inversePermutation_", inversePermutation_);
}

void HODLRMatrixImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("n_", n_);
  adv.loadAttribute("symmetric_", symmetric_);
  adv.loadAttribute("parameters_", parameters_);
  adv.loadAttribute("diagonal_", diagonal_);
  adv.loadAttribute("shiftAccumulated_", shiftAccumulated_);
  adv.loadAttribute("permutation_", permutation_);
  adv.loadAttribute("inversePermutation_", inversePermutation_);
}

// --- HODLRCovarianceAssemblyFunction ---

HODLRCovarianceAssemblyFunction::HODLRCovarianceAssemblyFunction(
    const CovarianceModel& covarianceModel,
    const Sample& vertices)
  : HODLRRealAssemblyFunction()
  , covarianceModel_(covarianceModel)
  , implementation_(covarianceModel.getImplementation())
  , vertices_(vertices)
  , verticesBegin_(vertices.getImplementation()->data_begin())
  , inputDimension_(vertices.getDimension())
  , covarianceDimension_(covarianceModel.getOutputDimension())
  , size_(vertices.getSize())
  , fastSqrt2nuOverTheta_()
  , fastAmplitudeSquare_(1.0)
  , fastNuggetFactor_(0.0)
  , fastNu_(0.0)
  , useFastMatern_(false)
{
  // The scalar assembly path calls the iterator-based computeAsScalar(), which
  // does not validate the point dimension, so a dimension mismatch used to
  // silently assemble the kernel on the first coordinates only. Check it here
  // to fail loudly instead of returning a wrong matrix.
  const UnsignedInteger modelInputDimension = covarianceModel_.getInputDimension();
  if (inputDimension_ != modelInputDimension)
    throw InvalidArgumentException(HERE) << "In HODLRCovarianceAssemblyFunction: the vertices have dimension="
                                         << inputDimension_ << " while the covariance model has input dimension="
                                         << modelInputDimension;

  // Try to set up the fast Matern evaluation path for the common
  // IsotropicCovarianceModel(MaternModel) / MaternModel configurations.
  const MaternModel* matern = nullptr;
  bool isotropicWrapper = false;
  if (const IsotropicCovarianceModel* iso = dynamic_cast<const IsotropicCovarianceModel*>(implementation_.get()))
  {
    matern = dynamic_cast<const MaternModel*>(iso->getKernel().getImplementation().get());
    isotropicWrapper = true;
  }
  else if (const MaternModel* direct = dynamic_cast<const MaternModel*>(implementation_.get()))
  {
    matern = direct;
  }
  if (matern && covarianceDimension_ == 1)
  {
    const Scalar nu = matern->getNu();
    if (nu == 0.5 || nu == 1.5 || nu == 2.5)
    {
      const Point scale = matern->getScale();
      const Scalar sqrt2nu = std::sqrt(2.0 * nu);
      // An IsotropicCovarianceModel wraps a 1D kernel: the norm is applied
      // before the kernel, so the single scale applies to every coordinate.
      const Scalar scaledSqrt2nuOverTheta = sqrt2nu / scale[0];
      fastSqrt2nuOverTheta_.resize(inputDimension_);
      for (UnsignedInteger d = 0; d < inputDimension_; ++d)
        fastSqrt2nuOverTheta_[d] = isotropicWrapper ? scaledSqrt2nuOverTheta : sqrt2nu / scale[d];
      const Scalar amplitude = matern->getAmplitude()[0];
      fastAmplitudeSquare_ = amplitude * amplitude;
      fastNuggetFactor_ = matern->getNuggetFactor();
      fastNu_ = nu;
      useFastMatern_ = true;
    }
  }
}

Scalar HODLRCovarianceAssemblyFunction::operator()(UnsignedInteger i, UnsignedInteger j) const
{
  if (covarianceDimension_ == 1)
  {
    if (useFastMatern_)
    {
      const Scalar* s = &verticesBegin_[i * inputDimension_];
      const Scalar* t = &verticesBegin_[j * inputDimension_];
      Scalar scaledPointSquare = 0.0;
      for (UnsignedInteger d = 0; d < inputDimension_; ++d)
      {
        const Scalar dx = (s[d] - t[d]) * fastSqrt2nuOverTheta_[d];
        scaledPointSquare += dx * dx;
      }
      const Scalar scaledPoint = std::sqrt(scaledPointSquare);
      if (scaledPoint <= SpecFunc::ScalarEpsilon)
        return fastAmplitudeSquare_ * (1.0 + fastNuggetFactor_);
      if (fastNu_ == 0.5)
        return fastAmplitudeSquare_ * std::exp(-scaledPoint);
      if (fastNu_ == 1.5)
        return fastAmplitudeSquare_ * std::exp(-scaledPoint) * (1.0 + scaledPoint);
      // nu == 2.5: (1 + s + s^2 / 3) exp(-s)
      return fastAmplitudeSquare_ * std::exp(-scaledPoint) * (1.0 + scaledPoint * (1.0 + scaledPoint / 3.0));
    }
    return implementation_->computeAsScalar(
        verticesBegin_ + i * inputDimension_, verticesBegin_ + j * inputDimension_);
  }

  const UnsignedInteger rowIndex = i / covarianceDimension_;
  const UnsignedInteger columnIndex = j / covarianceDimension_;
  const SquareMatrix localCovarianceMatrix(covarianceModel_(vertices_[rowIndex], vertices_[columnIndex]));
  const UnsignedInteger rowIndexLocal = i % covarianceDimension_;
  const UnsignedInteger columnIndexLocal = j % covarianceDimension_;
  return localCovarianceMatrix(rowIndexLocal, columnIndexLocal);
}

UnsignedInteger HODLRCovarianceAssemblyFunction::getSize() const
{
  return covarianceDimension_ * size_;
}

END_NAMESPACE_OPENTURNS
