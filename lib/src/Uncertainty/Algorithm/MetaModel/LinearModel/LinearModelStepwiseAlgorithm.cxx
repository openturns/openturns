//                                               -*- C++ -*-
/**
 *  @brief Stepwise algorithms for linear model
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearModelStepwiseAlgorithm.hxx"
#include "openturns/Function.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Combinations.hxx"
#include "openturns/ConstantBasisFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"
#include "openturns/MatrixImplementation.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/AggregatedFunction.hxx"

#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearModelStepwiseAlgorithm)
static const Factory<LinearModelStepwiseAlgorithm> Factory_LinearModelStepwiseAlgorithm;

/* Default constructor */
LinearModelStepwiseAlgorithm::LinearModelStepwiseAlgorithm()
  : PersistentObject()
  , direction_(FORWARD)
  , penalty_(-1.0)
  , maximumIterationNumber_(1000)
  , hasRun_(false)
{
  // Add intercept
  const ConstantBasisFactory factory(inputSample_.getDimension());
  const Function one(factory.build()[0]);
  basis_.add(one);
  condensedFormula_ =  one.__str__();
}

/* Parameters constructor FORWARD and BACKWARD */
LinearModelStepwiseAlgorithm::LinearModelStepwiseAlgorithm(const Sample & inputSample,
    const Basis & basis,
    const Sample & outputSample,
    const Indices & minimalIndices,
    const Bool isForward,
    const Scalar penalty,
    const UnsignedInteger maximumIterationNumber)
  : PersistentObject()
  , inputSample_(inputSample)
  , basis_(basis)
  , outputSample_(outputSample)
  , direction_(isForward ? FORWARD : BACKWARD)
  , penalty_(penalty)
  , maximumIterationNumber_(maximumIterationNumber)
  , minimalIndices_(minimalIndices)
  , condensedFormula_(basis.__str__())
  , hasRun_(false)
{
  if (outputSample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: cannot perform step method based on output sample of dimension different from 1.";
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "Error: the size of the output sample=" << outputSample_.getSize() << " is different from the size of the input sample=" << inputSample_.getSize();
}

/* Parameters constructor BOTH */
LinearModelStepwiseAlgorithm::LinearModelStepwiseAlgorithm(const Sample & inputSample,
    const Basis & basis,
    const Sample & outputSample,
    const Indices & minimalIndices,
    const Indices & startIndices,
    const Scalar penalty,
    const UnsignedInteger maximumIterationNumber)
  : PersistentObject()
  , inputSample_(inputSample)
  , basis_(basis)
  , outputSample_(outputSample)
  , direction_(BOTH)
  , penalty_(penalty)
  , maximumIterationNumber_(maximumIterationNumber)
  , minimalIndices_(minimalIndices)
  , startIndices_(startIndices)
  , condensedFormula_(basis.__str__())
  , hasRun_(false)
{
  if (outputSample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Error: cannot perform step method based on output sample of dimension different from 1.";
  if (inputSample.getSize() != outputSample.getSize())
    throw InvalidArgumentException(HERE) << "Error: the size of the output sample=" << outputSample_.getSize() << " is different from the size of the input sample=" << inputSample_.getSize();
}

/* Virtual constructor */
LinearModelStepwiseAlgorithm * LinearModelStepwiseAlgorithm::clone() const
{
  return new LinearModelStepwiseAlgorithm(*this);
}


/* String converter */
String LinearModelStepwiseAlgorithm::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName()
      << " direction=" << direction_
      << " penalty=" << penalty_
      << " maximumIterationNumber=" << maximumIterationNumber_
      << " condensedFormula=" << condensedFormula_
      << " basis=" << basis_;
  return oss;
}

/* String converter */
String LinearModelStepwiseAlgorithm::__str__(const String & /*offset*/) const
{
  OSS oss(false);
  oss << "class=" << getClassName()
      << " direction=" << direction_
      << " penalty=" << penalty_
      << " maximumIterationNumber=" << maximumIterationNumber_
      << " condensedFormula=" << condensedFormula_
      << " basis=" << basis_;
  return oss;
}

/* Sample accessors */
Sample LinearModelStepwiseAlgorithm::getInputSample() const
{
  return inputSample_;
}

Sample LinearModelStepwiseAlgorithm::getOutputSample() const
{
  return outputSample_;
}

/* Direction accessors */
LinearModelStepwiseAlgorithm::Direction LinearModelStepwiseAlgorithm::getDirection() const
{
  return direction_;
}

/* Penalty accessors */
Scalar LinearModelStepwiseAlgorithm::getPenalty() const
{
  return penalty_;
}

/* Maximum number of iterations accessors */
UnsignedInteger LinearModelStepwiseAlgorithm::getMaximumIterationNumber() const
{
  return maximumIterationNumber_;
}

/* Formula accessor */
String LinearModelStepwiseAlgorithm::getFormula() const
{
  return condensedFormula_;
}

/*
  logLikelihood(\hat\beta, \hat\sigma | Y) = -(n/2) ( log(2\pi) + log(\hat\sigma^2) + 1)
  where
     \hat\sigma^2 = (1/n) (Y - HY) (Y - HY)^T
                H = X (X^T X)^{-1} X^T

    Using QR decomposition of X :(X=QR) we get : A = (X^T X)^{-1} = R^{-1} (R^T)^{-1}
                                                 H = X A X^T = Q Q^T

  When (X^T X)^{-1} has been computed, there are update methods to compute
  the inverse when a column is added to or removed from X.

*/

/* TBB functor to speed-up forward insertion index computation

    If X is augmented by one column:
      X_+ = (X x_+)
                         ( A + (1/c) D D^T |  -(1/c) D )
      (X^T_+ X_+)^{-1} = (-----------------+-----------)
                         (      -(1/c) D^T |   (1/c)   )

                         ( A | 0 )   1 ( D D^T | -D )
                       = (---+---) + - (-------+----)
                         ( 0 | 0 )   c (  -D^T |  1 )

     with   D = A X^T x_+
            c = x_+^T x_+ - x_+^T X D

    By definition,
      H_+ = X_+ (X_+^T X_+)^{-1} X_+^T
    And thus
      H_+ Y = X A X^T Y + (1/(x_+^T x_+ - x_+^T X A X^T x_+))*(
                       X A X^T x_+ x_+^T X A X^T
                     - X A X^T x_+ x_+^T
                     - x_+ x_+^T X A X^T
                     + x_+ x_+^T)
    We compute
      residual = Y - Q Q^T Y
    and for each column j in Smax \ S*,
        d_j = Q Q^T x_j
        v_j = x_j - d_j
        Y - H_j Y = residual - (x_j^T residual /(x_j^T v_j)) v_j

*/
struct UpdateForwardFunctor
{
  const Basis & basis_;
  const Indices & indexSet_;
  const Matrix & Xmax_;
  const Matrix & residual_;
  const Matrix & Q_;
  Scalar criterion_;
  UnsignedInteger bestIndex_;

  UpdateForwardFunctor(const Basis & basis, const Indices & indexSet, const Matrix & Xmax, const Matrix & residual, const Matrix & Q)
    : basis_(basis), indexSet_(indexSet), Xmax_(Xmax), residual_(residual), Q_(Q)
    , criterion_(SpecFunc::MaxScalar), bestIndex_(Xmax.getNbColumns()) {}

  UpdateForwardFunctor(const UpdateForwardFunctor & other, TBB::Split)
    : basis_(other.basis_), indexSet_(other.indexSet_), Xmax_(other.Xmax_), residual_(other.residual_), Q_(other.Q_)
    , criterion_(other.criterion_), bestIndex_(other.bestIndex_) {}

  void operator() (const TBB::BlockedRange<UnsignedInteger> & r)
  {
    const UnsignedInteger size(Xmax_.getNbRows());
    Matrix xi(size, 1);
    Point xiNP(size);
    Point viNP(size);
    Point residualNP(size);
    std::copy(&residual_(0, 0), &residual_(0, 0) + size, residualNP.begin());

    for (UnsignedInteger index = r.begin(); index != r.end(); ++index)
    {
      const UnsignedInteger i = indexSet_[index];
      std::copy(&Xmax_(0, i), &Xmax_(0, i) + size, &xi(0, 0));
      const Matrix Qtxi(Q_.getImplementation()->genProd(*(xi.getImplementation()), true, false));
      const Matrix di(Q_ * Qtxi);
      const Matrix vi(xi - di);
      std::copy(&vi(0, 0), &vi(0, 0) + size, viNP.begin());
      std::copy(&xi(0, 0), &xi(0, 0) + size, xiNP.begin());
      const Scalar denominator = xiNP.dot(viNP);
      if (denominator == 0.0) continue;
      const Scalar alpha = xiNP.dot(residualNP) / denominator;
      const Point newResidual(residualNP - alpha * viNP);
      const Scalar newCriterion = newResidual.normSquare();
      LOGDEBUG(OSS() << "Squared residual norm when adding column " << i << "(" << basis_[i] << "): " << newCriterion);
      if (newCriterion < criterion_)
      {
        criterion_ = newCriterion;
        bestIndex_ = i;
      }
    }
  }

  void join(const UpdateForwardFunctor & other)
  {
    if (other.criterion_ < criterion_)
    {
      criterion_ = other.criterion_;
      bestIndex_ = other.bestIndex_;
    }
  }
}; /* end struct UpdateForwardFunctor */

/* TBB functor to speed-up backward insertion index computation

    If column i is removed from X:
      X_{-i} = X where column i is removed
      A_{-i,-i} = A where column and row i are removed
                = (a_{jk})_{j,k=1..p, j<>i, k<>i}
      A_{i,-i} = (a_{ij})_{j=1..p, j<>i}
    It can be shown that
      (X_{-i}^T X_{-i})^{-1} = A_{-i,-i} - (1/a_{ii}) A_{-i,i} A_{i,-i}
    And thus
      H_- = X_{-i} (X_{-i}^T X_{-i})^{-1} X_{-i}^T
      H_- Y = X_{-i} (X_{-i}^T X_{-i})^{-1} X_{-i}^T Y
            = X_{-i} A_{-i,-i} X_{-i}^T - (1/a_{ii})  X_{-i} A_{-i,i} A_{i,-i} X_{-i}^T Y

    In order to reduce data copies, we do not compute X_{-i} and A_{-i,-i},
    but emulate the same operations by putting zeros at the right places.
    We denote [Q]_{i=0} matrix Q where i-th row is replaced by zeros.
      X_{-i} A_{-i,-i} = ([X]_{i=0} A)_{-i}
    Instead of
      H_- Y = X_{-i} A_{-i,-i} X_{-i}^T - (1/a_{ii})  X_{-i} A_{-i,i} A_{i,-i} X_{-i}^T Y
    we write
      H_- Y = [X]_{i=0} A [X^T Y]_{i=0} - (1/a_{ii})  [X]_{i=0} A_{,i} A_{i,} [X^T Y]_{i=0}
    The final trick is to replace [X]_{i=0} by X in top-left multiplications, and throw i-th
    coefficient and replace it by 0.

    Note that we have : [X^T Y]_{i=0} = X^T Y - x_i^T Y e_i
    and : A_{i,} [X^T Y]_{i=0} = e_i^T A ( X^T Y - x_i^T Y e_i ) = e_i^T A X^T Y - x_i^T Y/a_{ii}

    Consequently :
      H_- Y = X A [X^T Y]_{i=0} - (1/a_{ii})  [X]_{i=0} A_{,i} A_{i,} [X^T Y]_{i=0}
            = X A X^T Y - x_i^T Y X A e_i -(1/a_{ii}) ( e_i^T A X^T Y - x_i^T Y/a_{ii} ) X A e_i
            = X A X^T Y - (e_i^T A X^T Y)/a_{ii} X A e_i
            = X A X^T Y - ((X A e_i)^T Y)/a_{ii} X A e_i

    Using QR decomposition of X we get : X A X^T = Q Q^T, X A e_i = Q (R^T)^{-1} e_i and a_{ii} = ((R^T)^{-1} e_i)^T (R^T)^{-1} e_i

    We compute
      residual = Y - Q Q^T Y
    and for each column j in S* \ Smin,
      b_j = (R^T)^{-1} e_i
      d_j = Q b_j
      Y - H_- Y = residual + (d_j^T Y /(b_j^T b_j)) d_j

    Note that j in S* \ Smin refers to columns in Xmax, we need an array to store
    positions of these columns in X.
 */
struct UpdateBackwardFunctor
{
  const Basis & basis_;
  const Indices & indexSet_;
  const Indices & columnMaxToCurrent_; // position of maxX_ columns in X_
  const Indices & columnCurrentToMax_; // position of X_ columns in maxX_
  const Matrix & Y_;
  const Matrix & residual_;
  const Matrix & Q_;
  const Matrix & invRt_;
  Scalar criterion_;
  UnsignedInteger bestIndex_;

  UpdateBackwardFunctor(const Basis & basis, const Indices & indexSet, const Indices & columnMaxToCurrent, const Indices & columnCurrentToMax,
                        const Matrix & Y, const Matrix & residual, const Matrix & Q, const Matrix & invRt)
    : basis_(basis), indexSet_(indexSet), columnMaxToCurrent_(columnMaxToCurrent), columnCurrentToMax_(columnCurrentToMax)
    , Y_(Y), residual_(residual), Q_(Q), invRt_(invRt)
    , criterion_(SpecFunc::MaxScalar), bestIndex_(invRt.getNbColumns()) {}

  UpdateBackwardFunctor(const UpdateBackwardFunctor & other, TBB::Split)
    : basis_(other.basis_), indexSet_(other.indexSet_), columnMaxToCurrent_(other.columnMaxToCurrent_), columnCurrentToMax_(other.columnCurrentToMax_)
    , Y_(other.Y_), residual_(other.residual_), Q_(other.Q_), invRt_(other.invRt_)
    , criterion_(other.criterion_), bestIndex_(other.bestIndex_) {}

  void operator() (const TBB::BlockedRange<UnsignedInteger> & r)
  {
    const UnsignedInteger size(Q_.getNbRows());
    const UnsignedInteger p(invRt_.getNbRows());
    Matrix bi(p, 1);
    Point biNP(p);
    Point diNP(size);
    Point yNP(size);
    std::copy(&Y_(0, 0), &Y_(0, 0) + size, yNP.begin());

    Point residualNP(size);
    std::copy(&residual_(0, 0), &residual_(0, 0) + size, residualNP.begin());

    for (UnsignedInteger index = r.begin(); index != r.end(); ++index)
    {
      const UnsignedInteger iMax = indexSet_[index];
      const UnsignedInteger i = columnMaxToCurrent_[iMax];
      std::copy(&invRt_(0, i), &invRt_(0, i) + p, biNP.begin());
      std::copy(biNP.begin(), biNP.begin() + p, &bi(0, 0));

      const Matrix di(Q_ * bi);
      std::copy(&di(0, 0), &di(0, 0) + size, diNP.begin());
      const Scalar alpha = diNP.dot(yNP) / biNP.dot(biNP);
      const Point newResidual(residualNP + alpha * diNP);
      const Scalar newCriterion(newResidual.normSquare());
      LOGDEBUG(OSS() << "Squared residual norm when removing column " << iMax << "(" << basis_[iMax] << "): " << newCriterion);
      if (newCriterion < criterion_)
      {
        criterion_ = newCriterion;
        bestIndex_ = iMax;
      }
    }
  } // operator

  void join(const UpdateBackwardFunctor & other)
  {
    if (other.criterion_ < criterion_)
    {
      criterion_ = other.criterion_;
      bestIndex_ = other.bestIndex_;
    }
  }
}; /* end struct UpdateBackwardFunctor */

/* Perform regression */
void LinearModelStepwiseAlgorithm::run()
{
  if (hasRun_) return;

  const UnsignedInteger size(inputSample_.getSize());
  Y_ = Matrix(size, 1, outputSample_.getImplementation()->getData());
  const AggregatedFunction f(basis_);
  const Sample fx(f(inputSample_));
  LOGDEBUG(OSS() << "Total number of columns=" << fx.getDimension());

  {
    // Reduce scope of Xt
    const Matrix Xt(fx.getDimension(), size, fx.getImplementation()->getData());
    maxX_ = Xt.transpose();
  }
  Indices columnMaxToCurrent(maxX_.getNbColumns(), maxX_.getNbColumns());

  if (startIndices_.getSize() == 0)
  {
    switch(direction_)
    {
      case BACKWARD:
        currentX_ = maxX_;
        currentIndices_ = Indices(maxX_.getNbColumns());
        currentIndices_.fill();
        columnMaxToCurrent = currentIndices_;
        break;
      case FORWARD:
        if (minimalIndices_.getSize() == 0)
          throw InvalidArgumentException(HERE) << "Error: minimal indices must be specified when direction is FORWARD";
        buildCurrentMatrixFromIndices(minimalIndices_);
        for (UnsignedInteger i = 0; i < minimalIndices_.getSize(); ++i)
          columnMaxToCurrent[minimalIndices_[i]] = i;
        break;
      case BOTH:
        throw InvalidArgumentException(HERE) << "Error: start indices must be specified when direction is BOTH";
        break;
    }
  }
  else
  {
    buildCurrentMatrixFromIndices(startIndices_);
    for (UnsignedInteger i = 0; i < startIndices_.getSize(); ++i)
      columnMaxToCurrent[startIndices_[i]] = i;
  }
  UnsignedInteger p = currentX_.getNbColumns();

  UnsignedInteger iterations = 0;
  Scalar Lstar = 0.0;
  while(iterations < maximumIterationNumber_)
  {
    ++iterations;
    // Update Q,(R^T)^{-1}, residual = Y - Q*Q^T*Y  (X=QR)
    Lstar = penalty_ * p + computeLogLikelihood();
    LOGDEBUG(OSS() << "Iteration " << iterations << ", current criterion=" << Lstar);

    Scalar LF = SpecFunc::MaxScalar;
    UnsignedInteger indexF = maxX_.getNbColumns();
    if (direction_ == FORWARD || direction_ == BOTH)
    {
      // indexSet = Imax - I*
      Indices indexSet;
      for (UnsignedInteger i = 0; i < maxX_.getNbColumns(); ++i)
      {
        if (!currentIndices_.contains(i))
          indexSet.add(i);
      }
      UpdateForwardFunctor updateFunctor(basis_, indexSet, maxX_, currentResidual_, currentQ_);
      TBB::ParallelReduce(0, indexSet.getSize(), updateFunctor);
      indexF = updateFunctor.bestIndex_;
      LF = penalty_ * (p + 1) + size * std::log(updateFunctor.criterion_ / size);
      LOGDEBUG(OSS() << "Best candidate in forward direction is " << indexF << "(" << basis_[indexF] << "), squared residual norm=" << updateFunctor.criterion_ << ", criterion=" << LF);
    }
    Scalar LB = SpecFunc::MaxScalar;
    UnsignedInteger indexB = maxX_.getNbColumns();
    if (direction_ == BACKWARD || direction_ == BOTH)
    {
      // indexSet = I* - Imin
      Indices indexSet;
      for (UnsignedInteger i = 0; i < currentIndices_.getSize(); ++i)
      {
        if (!minimalIndices_.contains(currentIndices_[i]))
          indexSet.add(currentIndices_[i]);
      }
      UpdateBackwardFunctor updateFunctor(basis_, indexSet, columnMaxToCurrent, currentIndices_, Y_, currentResidual_, currentQ_, currentInvRt_);
      TBB::ParallelReduce(0, indexSet.getSize(), updateFunctor);
      indexB = updateFunctor.bestIndex_;
      LB = penalty_ * (p - 1) + size * std::log(updateFunctor.criterion_ / size);
      LOGDEBUG(OSS() << "Best candidate in backward direction is " << indexB << "(" << basis_[indexB] << "), squared residual norm=" << updateFunctor.criterion_ << ", criterion=" << LB);
    }
    if (!(LF < Lstar || LB < Lstar))
      break;

    if (LF < LB)
    {
      LOGDEBUG(OSS() << "Add column " << indexF);
      // Add indexF to currentIndices_
      currentIndices_.add(indexF);
      // Add column indexF to currentX_
      columnMaxToCurrent[indexF] = p;
      Matrix newX(size, p + 1);
      std::copy(&currentX_(0, 0), &currentX_(0, 0) + size * p, &newX(0, 0));
      std::copy(&maxX_(0, indexF), &maxX_(0, indexF) + size, &newX(0, p));
      currentX_ = newX;
      ++p;
    }
    else
    {
      LOGDEBUG(OSS() << "Remove column " << indexB);
      // Remove column indexB from currentX_
      Matrix newX(size, p - 1);
      const UnsignedInteger pos(columnMaxToCurrent[indexB]);
      std::copy(&currentX_(0, 0), &currentX_(0, 0) + size * pos, &newX(0, 0));

      if (pos + 1 != p)
        std::copy(&currentX_(0, pos + 1), &currentX_(0, pos + 1) + size * (p - pos - 1), &newX(0, pos));
      currentX_ = newX;
      --p;
      // Update columnMaxToCurrent
      const UnsignedInteger indexBpos = columnMaxToCurrent[indexB];
      for (Indices::iterator it = columnMaxToCurrent.begin(); it != columnMaxToCurrent.end(); ++it)
      {
        if (*it > indexBpos && *it != maxX_.getNbColumns())
          --(*it);
      }
      columnMaxToCurrent[indexB] = maxX_.getNbColumns();
      // Remove indexB from currentIndices_
      Indices newIndices;
      for (Indices::const_iterator it = currentIndices_.begin(); it != currentIndices_.end(); ++it)
      {
        if (*it != indexB)
          newIndices.add(*it);
      }
      currentIndices_ = newIndices;
    }
    LOGDEBUG(OSS() << "Index set is now " << currentIndices_.__str__());
  } // end while
  // Update Q,(R^T)^{-1}, residual = Y - Q*Q^T*Y  (X=QR)
  const Scalar criterion(penalty_ * p + computeLogLikelihood());
  LOGDEBUG(OSS() << "Final indices are " << currentIndices_.__str__() << " and criterion is " << criterion);

  Point regression(p);
  const Matrix QtY(currentQ_.getImplementation()->genProd(*(Y_.getImplementation()), true, false));
  const Matrix invRQtY(currentInvRt_.getImplementation()->genProd(*(QtY.getImplementation()), true, false));
  std::copy(&invRQtY(0, 0), &invRQtY(0, 0) + p, regression.begin());


  LOGDEBUG(OSS() << "regression=" << regression);

  Point diagonalGramInverse(p);
  Point invRtiNP(p);
  for (UnsignedInteger i = 0; i < p; ++ i)
  {
    std::copy(&currentInvRt_(0, i), &currentInvRt_(0, i) + p, invRtiNP.begin());
    diagonalGramInverse[i] = invRtiNP.dot(invRtiNP);
  }
  Point leverages(size);
  Matrix Qt(currentQ_.transpose());
  Point QtiNP(p);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    std::copy(&Qt(0, i), &Qt(0, i) + p, QtiNP.begin());
    leverages[i] = QtiNP.dot(QtiNP);
  }
  Sample residualSample(size, 1);
  std::copy(&currentResidual_(0, 0), &currentResidual_(0, 0) + size, &residualSample(0, 0));


  const Point sigma2(residualSample.computeRawMoment(2));
  const Scalar factor = size * sigma2[0] / (size - p);
  Sample standardizedResiduals(size, 1);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    standardizedResiduals(i, 0) = residualSample(i, 0) / std::sqrt(factor * (1.0 - leverages[i]));
  }

  Point cookDistances(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    cookDistances[i] = (1.0 / p) * standardizedResiduals(i, 0) * standardizedResiduals(i, 0) * (leverages[i] / (1.0 - leverages[i]));
  }

  Description coefficientsNames(0);
  Collection<Function> currentFunctions;
  for (Indices::const_iterator it = currentIndices_.begin(); it != currentIndices_.end(); ++it)
  {
    coefficientsNames.add(basis_[*it].__str__());
    currentFunctions.add(basis_[*it]);
  }
  LinearCombinationFunction metaModel(currentFunctions, regression);

  result_ = LinearModelResult(inputSample_, Basis(currentFunctions), currentX_, outputSample_, metaModel,
                              regression, condensedFormula_, coefficientsNames, residualSample, standardizedResiduals,
                              diagonalGramInverse, leverages, cookDistances, sigma2[0]);
  hasRun_ = true;
}

/* Build currentX_ and currentIndices_ from given indices */
void LinearModelStepwiseAlgorithm::buildCurrentMatrixFromIndices(const Indices & columns)
{
  const UnsignedInteger size(inputSample_.getSize());
  currentX_ = Matrix(size, columns.getSize());
  currentIndices_ = columns;
  for (UnsignedInteger i = 0; i < columns.getSize(); ++ i)
    std::copy(&maxX_(0, columns[i]), &maxX_(0, columns[i]) + size, &currentX_(0, i));
}


/* Get linear model result */
LinearModelResult LinearModelStepwiseAlgorithm::getResult()
{
  if (!hasRun_) run();
  return result_;
}

/* Compute the likelihood function */
Scalar LinearModelStepwiseAlgorithm::computeLogLikelihood()
{
  const UnsignedInteger size = currentX_.getNbRows();
  const UnsignedInteger p = currentX_.getNbColumns();
  Matrix R;
  currentQ_ = currentX_.computeQR(R, size < p, true);
  const MatrixImplementation b(*IdentityMatrix(p).getImplementation());
  //                                                     keep intact, lower, transposed
  currentInvRt_ = R.getImplementation()->solveLinearSystemTri(b, false, false, true);

  // residual = Y - Q*Q^T*Y
  const Matrix QtY = currentQ_.getImplementation()->genProd(*(Y_.getImplementation()), true, false);
  const Matrix Yhat(currentQ_ * QtY);
  currentResidual_ = Y_ - Yhat;
  Point residualNP(size);
  std::copy(&currentResidual_(0, 0), &currentResidual_(0, 0) + size, residualNP.begin());

  const Scalar normSquared = residualNP.normSquare();
  const Scalar result = size * std::log(normSquared / size);
  LOGDEBUG(OSS() << "Residual squared norm=" << normSquared << ", loglikelihood=" << result);
  return result;
}

/* Method save() stores the object through the StorageManager */
void LinearModelStepwiseAlgorithm::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "basis_", basis_ );
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "direction_", static_cast<double>(direction_) );
  adv.saveAttribute( "penalty_", penalty_ );
  adv.saveAttribute( "maximumIterationNumber_", maximumIterationNumber_ );
  adv.saveAttribute( "minimalIndices_", minimalIndices_ );
  adv.saveAttribute( "startIndices_", startIndices_ );
  adv.saveAttribute( "condensedFormula_", condensedFormula_ );
  adv.saveAttribute( "Y_", Y_ );
  adv.saveAttribute( "maxX_", maxX_ );
  adv.saveAttribute( "currentX_", currentX_ );
  adv.saveAttribute( "currentQ_", currentQ_ );
  adv.saveAttribute( "currentInvRt_", currentInvRt_ );
  adv.saveAttribute( "currentResidual_", currentResidual_ );
  adv.saveAttribute( "currentIndices_", currentIndices_ );
  adv.saveAttribute( "result_", result_ );
  adv.saveAttribute( "hasRun_", hasRun_ );
}

/* Method load() reloads the object from the StorageManager */
void LinearModelStepwiseAlgorithm::load(Advocate & adv)
{
  PersistentObject::load(adv);
  double direction;
  adv.loadAttribute( "inputSample_", inputSample_ );
  adv.loadAttribute( "basis_", basis_ );
  adv.loadAttribute( "outputSample_", outputSample_ );
  adv.loadAttribute( "direction_", direction );
  if (direction < -0.5)
    direction_ = BACKWARD;
  else if (direction > 0.5)
    direction_ = FORWARD;
  else
    direction_ = BOTH;
  adv.loadAttribute( "penalty_", penalty_ );
  adv.loadAttribute( "maximumIterationNumber_", maximumIterationNumber_ );
  adv.loadAttribute( "minimalIndices_", minimalIndices_ );
  adv.loadAttribute( "startIndices_", startIndices_ );
  adv.loadAttribute( "condensedFormula_", condensedFormula_ );
  adv.loadAttribute( "Y_", Y_ );
  adv.loadAttribute( "maxX_", maxX_ );
  adv.loadAttribute( "currentX_", currentX_ );
  adv.loadAttribute( "currentQ_", currentQ_ );
  adv.loadAttribute( "currentInvRt_", currentInvRt_ );
  adv.loadAttribute( "currentResidual_", currentResidual_ );
  adv.loadAttribute( "currentIndices_", currentIndices_ );
  adv.loadAttribute( "result_", result_ );
  adv.loadAttribute( "hasRun_", hasRun_ );
}

END_NAMESPACE_OPENTURNS
