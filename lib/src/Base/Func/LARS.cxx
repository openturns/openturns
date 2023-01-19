//                                               -*- C++ -*-
/**
 *  @brief Least Angle Regression Algorithm
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/BasisSequenceFactoryImplementation.hxx"
#include "openturns/LARS.hxx"
#include "openturns/PenalizedLeastSquaresAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LARS)

typedef Collection<Scalar>    ScalarCollection;

static const Factory<LARS> Factory_LARS;

/* Default constructor */
LARS::LARS(const Bool verbose)
  : BasisSequenceFactoryImplementation(verbose)
  , relativeConvergence_(1.0)
  , oldCoefficientsL1Norm_(0.0)
  , coefficientsL1Norm_(0.0)
{
  // Nothing to do
}

/* Virtual constructor */
LARS * LARS::clone() const
{
  return new LARS( *this );
}

/* Method to create new BasisSequence objects */
BasisSequence LARS::build (const Sample & x,
                           const Sample & y,
                           const FunctionCollection & basis,
                           const Indices & indices)
{
  return BasisSequenceFactoryImplementation::build(x, y, basis, indices);
}

void LARS::initialize()
{
  BasisSequenceFactoryImplementation::initialize();
  oldCoefficientsL1Norm_ = 0.0;
  coefficientsL1Norm_ = 0.0;
  coefficients_.clear();
  mu_.clear();
  relativeConvergence_ = 1.0;
  predictors_.clear();
  inPredictors_.clear();
  mPsiX_ = Matrix();
}

/* Method to create new BasisSequence objects */
void LARS::updateBasis(LeastSquaresMethod & method,
                       const Sample & y)
{
  Sample x(method.getInputSample());

  const UnsignedInteger sampleSize = x.getSize();

  if (!(sampleSize > 0)) throw InvalidArgumentException( HERE ) << "Output sample cannot be empty.";
  if (y.getDimension() != 1) throw InvalidArgumentException( HERE ) << "Output sample should be unidimensional (dim=" << y.getDimension() << ").";
  if (y.getSize() != sampleSize) throw InvalidArgumentException( HERE ) << "Samples should be equally sized (in=" << sampleSize << " out=" << y.getSize() << ").";
  //   if (x.getDimension() != psi.getDimension()) throw InvalidArgumentException( HERE ) << "Sample dimension (" << x.getDimension() << ") does not match basis dimension (" << psi.getDimension() << ").";

  // get y as as point
  const Point mY(y.getImplementation()->getData());

  // precompute the design matrix on the whole basis
  if (mPsiX_.getNbRows() == 0)
  {
    mPsiX_ = method.computeWeightedDesign(true);
  }
  const UnsignedInteger basisSize = mPsiX_.getNbColumns();

  // regression coefficients
  if (coefficients_.getDimension() == 0) coefficients_ = Point(basisSize);

  // current least-square state
  if (mu_.getDimension() == 0) mu_ = Point(sampleSize);

  conservedPsi_k_ranks_ = currentIndices_;
  addedPsi_k_ranks_.clear();

  const UnsignedInteger maximumNumberOfIterations = std::min( basisSize, sampleSize - 1 );
  const UnsignedInteger iterations = predictors_.getSize();
  if (iterations == 0) inPredictors_ = Indices(basisSize, 0);
  if ((iterations < maximumNumberOfIterations) && (relativeConvergence_ > maximumRelativeConvergence_))
  {
    UnsignedInteger candidatePredictor = 0;
    // find the predictor most correlated with the current residual only after
    // the constant function has been introduced
    const Point cC(mPsiX_.getImplementation()->genVectProd(mY - mu_, true));
    Scalar cMax = -1.0;
    if (iterations == 0)
    {
      cMax = std::abs(cC[0]);
    }
    else
    {
      for (UnsignedInteger j = 0; j < basisSize; ++ j)
        if (!inPredictors_[j])
        {
          const Scalar cAbs = std::abs(cC[j]);
          if (cAbs > cMax)
          {
            cMax = cAbs;
            candidatePredictor = j;
          }
        } // if
    }
    if (getVerbose()) LOGINFO(OSS() << "predictor=" << candidatePredictor << " residual=" << cMax);

    // add the predictor index
    predictors_.add(candidatePredictor);
    inPredictors_[candidatePredictor] = 1;
    const UnsignedInteger globalPredictor = method.getInitialIndices()[candidatePredictor];
    addedPsi_k_ranks_.add(globalPredictor);
    currentIndices_.add(globalPredictor);
    method.update(addedPsi_k_ranks_, conservedPsi_k_ranks_, removedPsi_k_ranks_);

    // Starting from here, predictors_ has a size at least equal to 1
    // store the sign of the correlation
    UnsignedInteger predictorsSize = predictors_.getSize();
    Point sC(predictorsSize);
    for (UnsignedInteger j = 0; j < predictorsSize; ++ j) sC[j] = (cC[predictors_[j]] < 0.0 ? -1.0 : 1.0);
    // store correlations of the inactive set
    Point cI;
    for (UnsignedInteger j = 0; j < basisSize; ++ j)
      if (!inPredictors_[j]) cI.add(cC[j]);

    if (getVerbose()) LOGINFO(OSS() << "matrix of elements of the inactive set built.");

    Matrix mPsiAk(method.computeWeightedDesign());

    if (getVerbose()) LOGINFO(OSS() << "matrix of elements of the active set built.");

    Point ga1(method.solveNormal(sC));
    if (getVerbose()) LOGINFO( OSS() << "Solved normal equation.");

    // normalization coefficient
    Scalar cNorm = 1.0 / sqrt(sC.dot(ga1));

    // descent direction
    const Point descentDirectionAk(cNorm * ga1);
    const Point u(mPsiAk * descentDirectionAk);
    const Point d2(mPsiX_.getImplementation()->genVectProd(u, true));
    Point d;
    for (UnsignedInteger j = 0; j < basisSize; ++ j)
      if (!inPredictors_[j]) d.add(d2[j]);

    // compute step
    Scalar step = cMax / cNorm;
    for (UnsignedInteger j = 0; j < basisSize - predictorsSize; ++ j)
    {
      Scalar lhs = (cMax - cI[j]) / (cNorm - d[j]);
      Scalar rhs = (cMax + cI[j]) / (cNorm + d[j]);
      if (lhs > 0.0)
        step = std::min(step, lhs);
      if (rhs > 0.0)
        step = std::min(step, rhs);
    }

    // update mu
    mu_ += step * u;

    // update coefficients
    oldCoefficientsL1Norm_ = coefficientsL1Norm_;
    coefficientsL1Norm_ = 0.0;
    for (UnsignedInteger j = 0; j < predictorsSize; ++ j)
    {
      coefficients_[predictors_[j]] += step * descentDirectionAk[j];
      coefficientsL1Norm_ += std::abs(coefficients_[predictors_[j]]);
    }

    if (coefficientsL1Norm_ > 0.0) relativeConvergence_ = std::abs(1.0 - oldCoefficientsL1Norm_ / coefficientsL1Norm_);
    else relativeConvergence_ = -1.0;

    if (getVerbose()) LOGINFO( OSS() << "End of iteration " << iterations << " over " << maximumNumberOfIterations - 1 << " iteration(s)" << ", relative convergence=" << relativeConvergence_ << " for a target=" << maximumRelativeConvergence_);
  }
}

/* String converter */
String LARS::__repr__() const
{
  return OSS(true) << "class=" << getClassName();
}

String LARS::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Method save() stores the object through the StorageManager */
void LARS::save(Advocate & adv) const
{
  BasisSequenceFactoryImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void LARS::load(Advocate & adv)
{
  BasisSequenceFactoryImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
