//                                               -*- C++ -*-
/**
 *  @brief MulticollinearityAnalysis implements computation of various
 *  statistics used in multicollinear contexts
 *
 *  Copyright 2005-2026 Airbus-EDF-ONERA-Phimeca
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

#include <bitset>
#include "openturns/MulticollinearityAnalysis.hxx"
#include "openturns/KPermutationsDistribution.hxx"
#include "openturns/LinearModelAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MulticollinearityAnalysis)

static const Factory<MulticollinearityAnalysis> Factory_MulticollinearityAnalysis;

/* Extract the top-left part of a symmetric matrix */
static void extract(const SymmetricMatrix & source, SymmetricMatrix & target)
{
  const UnsignedInteger size = target.getDimension();
  for (UnsignedInteger i = 0; i < size; i++)
  {
    for (UnsignedInteger j = 0; j <= i; j++)
      target(i, j) = source(i, j);
  }
  target.checkSymmetry();
}

class LmgPmvdAlgorithm;

/**
 * @class LmgPmvdTBBBody
 *
 * TBB body to compute the variances associated with combinations of input variables
 */
struct LmgPmvdTBBBody
{
  LmgPmvdTBBBody(LmgPmvdAlgorithm & algo)
    : algo_(algo)
  {
    // Nothing to do
  }

  void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r) const;
  void getIndices(const UnsignedInteger combination, const UnsignedInteger size, Indices & indices) const;
  void extract2D(const SquareMatrix & source, const Indices & indices, SquareMatrix & target) const;
  void extract1D(const Matrix & source, const Indices & indices, Matrix & target) const;

  LmgPmvdAlgorithm & algo_;
};

/**
 * @class LmgPmvdAlgorithm
 *
 * Class for computation of LMG and PMVD indices
 */
class LmgPmvdAlgorithm
{
public:

  /* Constructor */
  LmgPmvdAlgorithm(const CovarianceMatrix & covMatrix)
    : dimension_(covMatrix.getDimension() - 1)
    , numberOfCombinations_(1 << dimension_) // 2^dimension_
    , covMatrix_(covMatrix)
    , covXY_(dimension_, 1)
    , sizes_(numberOfCombinations_)
    , variances_(numberOfCombinations_)
    , lmg_(dimension_)
    , pmvd_(dimension_)
  {
    // Extract needed parts from the covariance matrix
    for (UnsignedInteger i = 0; i < dimension_; i++)
      covXY_(i, 0) = covMatrix(i, dimension_);
    varY_ = covMatrix(dimension_, dimension_);
  }

  void run()
  {
    // First step: compute the variance associated with every combination of input variables
    const LmgPmvdTBBBody body(*this);
    const UnsignedInteger threshold = ResourceMap::GetAsUnsignedInteger("MulticollinearityAnalysis-DimensionThresholdForLmgPmvdParallelization");
    TBBImplementation::ParallelForIf(dimension_ >= threshold, 1, numberOfCombinations_, body, 1024);

    // Compute LMG indices
    computeLmg();

    // Compute PMVD indices
    computePmvd();
  }

  Point getLmg() const
  {
    return lmg_;
  }

  Point getPmvd() const
  {
    return pmvd_;
  }

  void computeLmg()
  {
    for(UnsignedInteger i = 0; i < dimension_; i++)
    {
      Point var1(dimension_);
      Point var2(dimension_);
      for (UnsignedInteger c = 1; c < numberOfCombinations_; c++)
      {
        const UnsignedInteger size = sizes_[c];
        if (c & (1 << i))
        {
          // The combination contains the variable
          var2[size - 1] += variances_[c];
        }
        else
        {
          // The combination does not contain the variable
          var1[size] += variances_[c];
        }
      }

      Scalar lmg = 0.0;
      for (UnsignedInteger j = 0; j < dimension_; j++)
        lmg += SpecFunc::Factorial(j) * SpecFunc::Factorial(dimension_ - j - 1) * (var2[j] - var1[j]);
      lmg /= SpecFunc::Factorial(dimension_) * varY_;
      lmg_[i] = lmg;
    }
  }

  void computePmvd()
  {
    // Partition the combinations by their size
    Collection<std::vector<Scalar>> partition(dimension_ + 1);
    for(UnsignedInteger size = 0; size <= dimension_; size++)
      partition[size].reserve(SpecFunc::BinomialCoefficient(dimension_, size));
    for(UnsignedInteger c = 0; c < numberOfCombinations_; c++)
      partition[sizes_[c]].push_back(c);

    Point potentials(numberOfCombinations_);

    // Compute the potential for combinations of size 1 (a single variable)
    UnsignedInteger k = 0;
    UnsignedInteger mask = 0;
    for (UnsignedInteger i = 0; i < dimension_; i++)
    {
      const UnsignedInteger c = 1 << i;
      potentials[c] = computeWorth(c);
      if (potentials[c] == 0.0)
      {
        // The variable has no potential, it will be ignored in the rest of the computation
        k++;
        mask |= c;
      }
    }

    // Compute the potential for combinations of size 2, then 3, and so on
    for (UnsignedInteger size = 2; size <= dimension_ - k; size++)
    {
      for (UnsignedInteger c : partition[size])
      {
        if ((c & mask) == 0)
        {
          Scalar sum = 0.0;
          UnsignedInteger i = 0;
          UnsignedInteger j = 0;
          do
          {
            const UnsignedInteger d = c & ~(1 << i);
            if (d != c)
            {
              // We have a sub-combination
              sum += 1 / potentials[d];
              j++;
            }
            i++;
          }
          while(j < size);
          potentials[c] = computeWorth(c) / sum;
        }
      }
    }

    // Now that all potentials have been calculated, we can compute the PMVD indices
    const UnsignedInteger all = (numberOfCombinations_ - 1) & ~mask;
    for (UnsignedInteger i = 0; i < dimension_; i++)
    {
      const UnsignedInteger c = 1 << i;
      if ((c & mask) == 0)
      {
        const UnsignedInteger complement = all & ~c;
        pmvd_[i] = potentials[all] / (potentials[complement] * varY_);
      }
      else
        pmvd_[i] = 0.0;
    }
  }

  Scalar computeWorth(UnsignedInteger combination) const
  {
    const UnsignedInteger all = numberOfCombinations_ - 1;
    const UnsignedInteger complement = all & ~combination;
    return variances_[all] - variances_[complement];
  }

  const UnsignedInteger dimension_;
  const UnsignedInteger numberOfCombinations_;
  const CovarianceMatrix covMatrix_;
  Matrix covXY_;
  Scalar varY_;
  Collection<UnsignedInteger> sizes_;
  Point variances_;
  Point lmg_;
  Point pmvd_;
};


/* Compute variances associated with a range of combinations */
void LmgPmvdTBBBody::operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & r) const
{
  // Pre-allocate some matrices
  Collection<CovarianceMatrix> matrices2D;
  Collection<Matrix> matrices1D;
  for (UnsignedInteger size = 1; size <= algo_.dimension_; size++)
  {
    matrices2D.add(CovarianceMatrix(size));
    matrices1D.add(Matrix(size, 1));
  }

  // Object to store the contents of a combination
  Indices indices(algo_.dimension_);

  // Iterate over the range of combinations
  for (UnsignedInteger c = r.begin() ; c < r.end(); c++)
  {
    // The size of the combination is the number of bits set to 1
    const UnsignedInteger size = std::bitset<32>(c).count();
    getIndices(c, size, indices);
    CovarianceMatrix & mat2D = matrices2D[size - 1];
    Matrix & mat1D = matrices1D[size - 1];
    extract2D(algo_.covMatrix_, indices, mat2D);
    extract1D(algo_.covXY_, indices, mat1D);
    const MatrixImplementation solution(mat2D.getImplementation()->solveLinearSystemCovInPlace(*mat1D.getImplementation()));
    Scalar var = 0.0;
    for (UnsignedInteger i = 0; i < size; i++)
      var += mat1D(i, 0) * solution(i, 0);
    algo_.variances_[c] = var;
    algo_.sizes_[c] = size;
  }
}

/* Get the list of indices contained in a combination */
void LmgPmvdTBBBody::getIndices(const UnsignedInteger combination, const UnsignedInteger size, Indices & indices) const
{
  UnsignedInteger i = 0;
  UnsignedInteger j = 0;
  while(j < size)
  {
    if (combination & (1 << i))
    {
      // Bit found, store the index
      indices[j++] = i;
    }
    i++;
  }
}

/* Extract a subset of a square matrix */
void LmgPmvdTBBBody::extract2D(const SquareMatrix & source, const Indices & indices, SquareMatrix & target) const
{
  const MatrixImplementation & sourceImpl = *source.getImplementation();
  MatrixImplementation & targetImpl = *target.getImplementation();
  const UnsignedInteger size = targetImpl.getDimension();
  for (UnsignedInteger i = 0; i < size; i++)
  {
    for (UnsignedInteger j = 0; j <= i; j++)
      targetImpl(i, j) = sourceImpl(indices[i], indices[j]);
  }
}

/* Extract a subset of a column matrix */
void LmgPmvdTBBBody::extract1D(const Matrix & source, const Indices & indices, Matrix & target) const
{
  const MatrixImplementation & sourceImpl = *source.getImplementation();
  MatrixImplementation & targetImpl = *target.getImplementation();
  const UnsignedInteger size = targetImpl.getNbRows();
  for (UnsignedInteger i = 0; i < size; i++)
    targetImpl(i, 0) = sourceImpl(indices[i], 0);
}


/**
 * @class LmgPmvdMonteCarloAlgorithm
 *
 * Estimation of LMG and PMVD indices via a Monte Carlo method
 */
class LmgPmvdMonteCarloAlgorithm
{
public:

  /* Constructor */
  LmgPmvdMonteCarloAlgorithm(const CovarianceMatrix & covMatrix, const UnsignedInteger iterations)
    : dimension_(covMatrix.getDimension() - 1)
    , iterations_(iterations)
    , covXX_(dimension_)
    , covXXperm_(dimension_)
    , covXY_(dimension_, 1)
    , covXYperm_(dimension_, 1)
    , nbPermutations_(0)
    , totalWeight_(0.0)
    , lmg_(dimension_)
    , pmvd_(dimension_)
    , delta_(dimension_)
  {
    // Extract needed parts from the covariance matrix
    extract(covMatrix, covXX_);
    for (UnsignedInteger i = 0; i < dimension_; i++)
      covXY_(i, 0) = covMatrix(i, dimension_);
    varY_ = covMatrix(dimension_, dimension_);
    // Pre-allocate some matrices
    for (UnsignedInteger size = 1; size <= dimension_; size++)
    {
      matrices2D.add(SquareMatrix(size));
      matrices1D.add(Matrix(size, 1));
    }
  }

  void run()
  {
    fullVar = computeFullVariance();
    // Draw a random permutation for each iteration
    const KPermutationsDistribution distribution(dimension_, dimension_);
    Indices permutation(dimension_);
    for (UnsignedInteger i = 0; i < iterations_; i++)
    {
      const Point realization(distribution.getRealization());
      for (UnsignedInteger j = 0; j < dimension_; j++)
        permutation[j] = static_cast<UnsignedInteger>(realization[j]);
      addPermutation(permutation);
    }
    lmg_ /= nbPermutations_ * varY_;
    pmvd_ /= totalWeight_ * varY_;
  }

  Point getLmg() const
  {
    return lmg_;
  }

  Point getPmvd() const
  {
    return pmvd_;
  }

  void addPermutation(const Indices & permutation)
  {
    permutate2D(permutation, covXX_, covXXperm_);
    permutate1D(permutation, covXY_, covXYperm_);
    const TriangularMatrix L = covXXperm_.computeCholesky();
    const SquareMatrix Linv = L.inverse();
    Scalar weight = 1.0;
    Scalar var1 = 0.0;
    Scalar var2 = 0.0;
    for (UnsignedInteger i = 0; i < dimension_; i++)
    {
      if (i == 0)
        var1 = 0.0;
      else
        var1 = var2;
      SquareMatrix & mat2D = matrices2D[i];
      Matrix & mat1D = matrices1D[i];
      extract2D(Linv, mat2D);
      extract1D(covXYperm_, mat1D);
      const MatrixImplementation M1(mat1D.getImplementation()->genProd(*mat2D.getImplementation(), true, true));
      const MatrixImplementation M2(M1.genProd(*mat2D.getImplementation(), false, false));
      const MatrixImplementation M3(M2.genProd(*mat1D.getImplementation(), false, false));
      var2 = M3(0, 0);
      delta_[permutation[i]] = var2 - var1;
      if (i < dimension_ - 1 && fullVar - var2 > 0)
        weight /= fullVar - var2;
    }
    lmg_ += delta_;
    pmvd_ += weight * delta_;
    nbPermutations_++;
    totalWeight_ += weight;
  }

  /* Apply a permutation to a square matrix */
  void permutate2D(const Indices & permutation, const SquareMatrix & source, SquareMatrix & target) const
  {
    const MatrixImplementation & sourceImpl = *source.getImplementation();
    MatrixImplementation & targetImpl = *target.getImplementation();
    const UnsignedInteger size = target.getDimension();
    for (UnsignedInteger i = 0; i < size; i++)
    {
      for (UnsignedInteger j = 0; j < size; j++)
        targetImpl(i, j) = sourceImpl(permutation[i], permutation[j]);
    }
  }

  /* Apply a permutation to a column matrix */
  void permutate1D(const Indices & permutation, const Matrix & source, Matrix & target) const
  {
    const MatrixImplementation & sourceImpl = *source.getImplementation();
    MatrixImplementation & targetImpl = *target.getImplementation();
    const UnsignedInteger size = target.getNbRows();
    for (UnsignedInteger i = 0; i < size; i++)
      targetImpl(i, 0) = sourceImpl(permutation[i], 0);
  }

  /* Extract the top-left part of a square matrix */
  void extract2D(const SquareMatrix & source, SquareMatrix & target) const
  {
    const MatrixImplementation & sourceImpl = *source.getImplementation();
    MatrixImplementation & targetImpl = *target.getImplementation();
    const UnsignedInteger size = targetImpl.getDimension();
    for (UnsignedInteger i = 0; i < size; i++)
    {
      for (UnsignedInteger j = 0; j <= i; j++)
        targetImpl(i, j) = sourceImpl(i, j);
    }
  }

  /* Extract the top part of a vector matrix */
  void extract1D(const Matrix & source, Matrix & target) const
  {
    const MatrixImplementation & sourceImpl = *source.getImplementation();
    MatrixImplementation & targetImpl = *target.getImplementation();
    const UnsignedInteger size = targetImpl.getNbRows();
    for (UnsignedInteger i = 0; i < size; i++)
      targetImpl(i, 0) = sourceImpl(i, 0);
  }

  /* Compute the explained variance of the full model (i.e. with all input variables) */
  Scalar computeFullVariance() const
  {
    const Matrix solution(covXX_.solveLinearSystem(covXY_));
    const MatrixImplementation mat(covXY_.getImplementation()->genProd(*solution.getImplementation(), true, false));
    return mat(0, 0);
  }
 
  UnsignedInteger dimension_;
  UnsignedInteger iterations_;
  CovarianceMatrix covXX_;
  CovarianceMatrix covXXperm_;
  Matrix covXY_;
  Matrix covXYperm_;
  Scalar varY_;
  Scalar fullVar;
  UnsignedInteger nbPermutations_;
  Scalar totalWeight_;
  Point lmg_;
  Point pmvd_;
  Point delta_;
  Collection<SquareMatrix> matrices2D;
  Collection<Matrix> matrices1D;
};


/* Default constructor */
MulticollinearityAnalysis::MulticollinearityAnalysis()
  : PersistentObject()
{
  // Nothing to do
}

/* Constructor with input sample */
MulticollinearityAnalysis::MulticollinearityAnalysis(const Sample & firstSample)
  : PersistentObject()
  , firstSample_(firstSample)
{
  // Nothing to do
}

/* Constructor with input and output samples */
MulticollinearityAnalysis::MulticollinearityAnalysis(const Sample & firstSample,
    const Sample & secondSample)
  : PersistentObject()
  , firstSample_(firstSample)
  , secondSample_(secondSample)
{
  // Nothing to do
}

/* Virtual constructor */
MulticollinearityAnalysis * MulticollinearityAnalysis::clone() const
{
  return new MulticollinearityAnalysis(*this);
}

/* String converter */
String MulticollinearityAnalysis::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " firstSample=" << firstSample_
      << " secondSample=" << secondSample_;
  return oss;
}

/* Compute LMG and PMVD indices */
void MulticollinearityAnalysis::computeLmgPmvd(PointWithDescription & lmg, PointWithDescription & pmvd) const
{
  checkInputSample();
  checkOutputSample();
  UnsignedInteger maxDimension = ResourceMap::GetAsUnsignedInteger("MulticollinearityAnalysis-MaximumInputDimensionForLmgPmvd");
  maxDimension = std::min(maxDimension, (UnsignedInteger)31); // Make sure we don't overflow 32-bit integers
  if (!(firstSample_.getDimension() <= maxDimension)) throw InvalidDimensionException(HERE) << "Error: input sample dimension must be at most " << maxDimension;

  LmgPmvdAlgorithm algo(computeCovariance());
  algo.run();
  lmg = algo.getLmg();
  lmg.setDescription(firstSample_.getDescription());
  pmvd = algo.getPmvd();
  pmvd.setDescription(firstSample_.getDescription());
}

/* Estimate LMG and PMVD indices via a Monte Carlo method */
void MulticollinearityAnalysis::estimateLmgPmvdMonteCarlo(PointWithDescription & lmg, PointWithDescription & pmvd, const UnsignedInteger iterations) const
{
  checkInputSample();
  checkOutputSample();

  LmgPmvdMonteCarloAlgorithm algo(computeCovariance(), iterations);
  algo.run();
  lmg = algo.getLmg();
  lmg.setDescription(firstSample_.getDescription());
  pmvd = algo.getPmvd();
  pmvd.setDescription(firstSample_.getDescription());
}

/* Compute Johnson index */
PointWithDescription MulticollinearityAnalysis::computeJohnson() const
{
  checkInputSample();
  checkOutputSample();

  // Compute the correlation matrix of the full sample (input + output)
  Sample fullSample(firstSample_);
  fullSample.stack(secondSample_);
  const CorrelationMatrix corMatrix(fullSample.computeLinearCorrelation());

  // Extract needed parts of the correlation matrix
  const UnsignedInteger dimension = firstSample_.getDimension();
  CorrelationMatrix corXX(dimension);
  extract(corMatrix, corXX);
  Matrix corXY(dimension, 1);
  for (UnsignedInteger i = 0; i < dimension; i++)
    corXY(i, 0) = corMatrix(i, dimension);

  // Perform the eigenvalues decomposition of the corXX matrix
  SquareMatrix Phi;
  const Point eigenvalues(corXX.computeEV(Phi));

  SquareMatrix Delta(dimension);
  for (UnsignedInteger i = 0; i < dimension; i++)
    Delta(i, i) = std::sqrt(std::max(eigenvalues[i], 0.0));

  SquareMatrix W(Phi * Delta * Phi.transpose());
  Matrix alpha(W.solveLinearSystem(corXY));
  W.squareElements();
  alpha.squareElements();
  const Matrix johnson(W * alpha);

  PointWithDescription result(dimension);
  for (UnsignedInteger i = 0; i < dimension; i++)
    result[i] = johnson(i, 0);
  result.setDescription(firstSample_.getDescription());
  return result;
}

/* Compute VIF metric */
PointWithDescription MulticollinearityAnalysis::computeVif() const
{
  checkInputSample();

  // We create an arbitrary output sample (the algorithm needs one although the metric doesn't depend on it)
  const UnsignedInteger size = firstSample_.getSize();
  Sample outputSample(size, 1);
  for (UnsignedInteger i = 0; i < size; i += 2)
    outputSample(i, 0) = 1.0;

  // Perform a linear regression
  LinearModelAlgorithm algo(firstSample_, outputSample);
  const LinearModelResult linearModelResult(algo.getResult());
  const SymmetricMatrix gramInverse(linearModelResult.getLeastSquaresMethod().getGramInverse());
  const Scalar SSE = linearModelResult.getSampleResiduals().asPoint().normSquare(); // Sum of squared errors
  const Scalar df = linearModelResult.getDegreesOfFreedom();
  const SymmetricMatrix V(gramInverse * (SSE / df));
  const CorrelationMatrix R(covarianceToCorrelation(removeRowAndColumn(V, 0)));
  const Scalar detR = R.computeDeterminant();
  if (detR == 0.0) throw NotDefinedException(HERE) << "Error: the matrix is singular";

  const UnsignedInteger dimension = firstSample_.getDimension();
  PointWithDescription result(dimension);
  for(UnsignedInteger i = 0; i < dimension; i++)
  {
    const SymmetricMatrix Ri(removeRowAndColumn(R, i));
    result[i] = Ri.computeDeterminant() / detR;
  }
  result.setDescription(firstSample_.getDescription());
  return result;
}

void MulticollinearityAnalysis::checkInputSample() const
{
  if (firstSample_.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: no input sample has been provided";
  if (!(firstSample_.getDimension() >= 2)) throw InvalidDimensionException(HERE) << "Error: input sample must have at least 2 dimensions";
}

void MulticollinearityAnalysis::checkOutputSample() const
{
  if (secondSample_.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: no output sample has been provided";
  if (secondSample_.getDimension() != 1) throw InvalidDimensionException(HERE) << "Error: output sample must be 1D";
  if (firstSample_.getSize() != secondSample_.getSize()) throw InvalidArgumentException(HERE) << "Error: input and output samples must have the same size";
}

/* Compute the covariance matrix of the full sample (input + output) */
CovarianceMatrix MulticollinearityAnalysis::computeCovariance() const
{
  Sample fullSample(firstSample_);
  fullSample.stack(secondSample_);
  return fullSample.computeCovariance();
}

/* Remove a row and a column from a symmetric matrix */
SymmetricMatrix MulticollinearityAnalysis::removeRowAndColumn(const SymmetricMatrix & matrix, const UnsignedInteger rowCol) const
{
  const UnsignedInteger size = matrix.getDimension();
  SymmetricMatrix result(size - 1);
  UnsignedInteger i2 = 0;
  for (UnsignedInteger i = 0; i < size; i++)
  {
    if (i != rowCol)
    {
      UnsignedInteger j2 = 0;
      for (UnsignedInteger j = 0; j <= i; j++)
      {
        if (j != rowCol)
        {
          result(i2, j2) = matrix(i, j);
          j2++;
        }
      }
      i2++;
    }
  }
  return result;
}

/* Convert a covariance matrix to a correlation matrix */
CorrelationMatrix MulticollinearityAnalysis::covarianceToCorrelation(const SymmetricMatrix & matrix) const
{
  const UnsignedInteger size = matrix.getDimension();
  CorrelationMatrix result(size);
  for (UnsignedInteger i = 0; i < size; i++)
  {
    for (UnsignedInteger j = 0; j < i; j++)
    {
      Scalar pij = matrix(i, i) * matrix(j, j);
      if (!(pij > 0)) throw InvalidArgumentException(HERE) << "Error: can't compute the correlation matrix";
      result(i, j) = matrix(i, j) / std::sqrt(pij);
    }
  }
  return result;
}

/* Method save() stores the object through the StorageManager */
void MulticollinearityAnalysis::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("firstSample_", firstSample_);
  adv.saveAttribute("secondSample_", secondSample_);
}

/* Method load() reloads the object from the StorageManager */
void MulticollinearityAnalysis::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("firstSample_", firstSample_);
  adv.loadAttribute("secondSample_", secondSample_);
}

END_NAMESPACE_OPENTURNS
