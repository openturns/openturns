
//                                               -*- C++ -*-
/**
 * @brief HSICEstimatorImplementation implements the HSIC sensivity indices.
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
#include "openturns/HSICEstimatorImplementation.hxx"
#include "openturns/HSICEstimator.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/HSICStat.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Pie.hxx"
#include "openturns/Text.hxx"

BEGIN_NAMESPACE_OPENTURNS
CLASSNAMEINIT(HSICEstimatorImplementation)

static const Factory<HSICEstimatorImplementation> Factory_HSICEstimatorImplementation;

/* Default constructor */
HSICEstimatorImplementation::HSICEstimatorImplementation()
  : PersistentObject()
  , covarianceList_()
  , inputSample_()
  , outputSample_()
  , estimatorType_()
  , n_(0)
  , inputDimension_(0)
  , HSIC_XY_()
  , HSIC_XX_()
  , HSIC_YY_()
  , R2HSICIndices_()
  , PValuesPermutation_()
  , permutationSize_(ResourceMap::GetAsUnsignedInteger("HSICEstimator-PermutationSize"))
{
 // Nothing
}


/* Constructor */
HSICEstimatorImplementation::HSICEstimatorImplementation(
  const CovarianceModelCollection & covarianceList
, const Sample & X
, const Sample & Y
, const HSICStat & estimatorType)
  : PersistentObject()
  , covarianceList_(covarianceList)
  , inputSample_(X)
  , outputSample_(Y)
  , estimatorType_(estimatorType)
  , n_(X.getSize())
  , inputDimension_(X.getDimension())
  , HSIC_XY_ ()
  , HSIC_XX_ ()
  , HSIC_YY_ ()
  , R2HSICIndices_ ()
  , permutationSize_(ResourceMap::GetAsUnsignedInteger("HSICEstimator-PermutationSize"))
{
  if(covarianceList_.getSize() != (inputSample_.getDimension() + outputSample_.getDimension())) throw InvalidDimensionException(HERE) << "The number of covariance momdels is the dimension of the input +1";
  if(outputSample_.getDimension() != 1) throw InvalidDimensionException(HERE) << "The dimension of the output is 1.";
  if(inputSample_.getSize() != outputSample_.getSize()) throw InvalidDimensionException(HERE) << "Input and output samples must have the same size";
}

/* Virtual constructor */
HSICEstimatorImplementation * HSICEstimatorImplementation::clone() const
{
  return new HSICEstimatorImplementation(*this);
}

/* Compute the weight matrix from the weight function */
SquareMatrix HSICEstimatorImplementation::computeWeightMatrix(const Sample&) const
{
    throw NotYetImplementedException(HERE) << "Use the class HSICEstimatorConditionalSensitivity in order to perform conditional sensitivity analysis";
}

/* Compute a HSIC index (one marginal) by using the underlying estimator (biased or not) */
Scalar HSICEstimatorImplementation::computeHSICIndex(const Sample & inSample,
                                                     const Sample & outSample,
                                                     const CovarianceModel & inCovariance,
                                                     const CovarianceModel & outCovariance,
                                                     const SquareMatrix & weightMatrix) const
{
  return estimatorType_.computeHSICIndex(inSample, outSample, inCovariance, outCovariance, weightMatrix);
}

/* Compute HSIC and R2-HSIC indices */
void HSICEstimatorImplementation::computeIndices() const
{
  /* Compute weights */
  const SquareMatrix W = computeWeightMatrix(outputSample_);

  /* Init */
  HSIC_XX_ = Point(inputDimension_);
  HSIC_XY_ = Point(inputDimension_);
  HSIC_YY_ = Point(1);

  /* Loop over marginals: HSIC indices */
  for(UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    const Sample xdim(inputSample_.getMarginal(dim));
    HSIC_XY_[dim] = computeHSICIndex(xdim, outputSample_, covarianceList_[dim], covarianceList_[inputDimension_], W);
    HSIC_XX_[dim] = computeHSICIndex(xdim, xdim, covarianceList_[dim], covarianceList_[dim], W);
  }
  HSIC_YY_[0] = computeHSICIndex(outputSample_, outputSample_, covarianceList_[inputDimension_], covarianceList_[inputDimension_], W);

  /* Compute R2-HSIC */
  R2HSICIndices_ = Point(inputDimension_);
  for(UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    R2HSICIndices_[dim] = HSIC_XY_[dim] / sqrt(HSIC_XX_[dim] * HSIC_YY_[0]);
  }

  isAlreadyComputedIndices_ = true ;
}

/* Set permutation size */
void HSICEstimatorImplementation::setPermutationSize(const UnsignedInteger B)
{
  permutationSize_ = B;
  PValuesPermutation_ = Point();
  isAlreadyComputedPValuesPermutation_ = false;
}

/* Get permutation size */
UnsignedInteger HSICEstimatorImplementation::getPermutationSize() const
{
  return permutationSize_;
}

/* Compute p-value with permutation */
void HSICEstimatorImplementation::computePValuesPermutation() const
{
  const SquareMatrix Wobs(computeWeightMatrix(outputSample_));
  PValuesPermutation_ = Point(inputDimension_);
  Sample ShuffledSample;
  Collection<Sample> shuffleCollection(permutationSize_);
  Collection<SquareMatrix> weightMatrixCollection(permutationSize_);

  for( UnsignedInteger b = 0; b < permutationSize_; ++b)
  {
    ShuffledSample = shuffledCopy(outputSample_);
    shuffleCollection[b] = ShuffledSample;
    weightMatrixCollection[b] = computeWeightMatrix(ShuffledSample);
  }

  for(UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {

    const Sample xdim(inputSample_.getMarginal(dim));
    const Scalar HSIC_obs = computeHSICIndex(xdim, outputSample_, covarianceList_[dim], covarianceList_[inputDimension_], Wobs);
    Scalar HSIC_loc;
    UnsignedInteger count = 0;

    for( UnsignedInteger b = 0; b < permutationSize_; ++b)
    {
      const Sample Yp(shuffleCollection[b]);
      const SquareMatrix W(weightMatrixCollection[b]);
      HSIC_loc = computeHSICIndex(xdim, Yp, covarianceList_[dim], covarianceList_[inputDimension_], W);
      if( HSIC_loc > HSIC_obs) count += 1;
    }

    /* p-value by permutation */
    PValuesPermutation_[dim] = count * 1.0 / (permutationSize_ + 1) ;
  }
  isAlreadyComputedPValuesPermutation_ = true;
}

/* Compute the asymptotic p-values */
void HSICEstimatorImplementation::computePValuesAsymptotic() const
{
  PValuesAsymptotic_ = Point(inputDimension_);

  SquareMatrix H(n_, Collection<Scalar>(n_ * n_, -1.0 / n_));
  for(UnsignedInteger j = 0; j < n_; ++j)
  {
    H(j, j) += 1.0;
  }

  const CovarianceMatrix Ky(covarianceList_[inputDimension_].discretize(outputSample_));
  const Scalar traceKy = Ky.computeTrace();
  const Scalar sumKy = Ky.computeSumElements();

  const Scalar Ey = (sumKy - traceKy) / n_ / (n_ - 1 );
  const Matrix By = H * Ky * H;
  const Point HSICobsPt(getHSICIndices());

  for(UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    const Sample Xi(inputSample_.getMarginal(dim));
    const CovarianceMatrix Kx(covarianceList_[dim].discretize(Xi));
    const Scalar traceKx = Kx.computeTrace();
    const Scalar sumKx = Kx.computeSumElements();
    const Scalar Ex = (sumKx - traceKx) / n_ / (n_ - 1);

    const Matrix Bx = H * Kx * H;

    /* Hadamard product then square all elements */
    SquareMatrix B(Bx.computeHadamardProduct(By).getImplementation());
    B.squareElements();

    const Point nullDiag(n_);
    B.setDiagonal(nullDiag, 0);

    const Scalar mHSIC = (1 + Ex * Ey - Ex - Ey) / n_;
    const Scalar factor = 2.0 * (n_ - 4) * (n_ - 5) / n_ / (n_ - 1) / (n_ - 2) / (n_ - 3) / n_ / (n_ - 1);
    const Scalar varHSIC = B.computeSumElements() * factor;

    const Scalar alpha = mHSIC * mHSIC / varHSIC;
    const Scalar beta = n_ * varHSIC / mHSIC;

    const Gamma distribution(alpha, 1.0 / beta);
    const Scalar p = estimatorType_.computePValue(distribution, n_, HSICobsPt[dim], mHSIC);
    PValuesAsymptotic_[dim] = p;
  }
  isAlreadyComputedPValuesAsymptotic_ = true ;
}

/* Get the HSIC indices */
Point HSICEstimatorImplementation::getHSICIndices() const
{
  if(!(isAlreadyComputedIndices_))
  {
    computeIndices();
    isAlreadyComputedIndices_ = true ; 
  }
  return HSIC_XY_;
}

/* Get the R2-HSIC indices */
Point HSICEstimatorImplementation::getR2HSICIndices() const
{
  if(!(isAlreadyComputedIndices_))
  {
    computeIndices();
    isAlreadyComputedIndices_ = true ;
  }
  return R2HSICIndices_;
}

/* Get the p-values by permutation */
Point HSICEstimatorImplementation::getPValuesPermutation() const
{
  if( !(isAlreadyComputedPValuesPermutation_))
  {
    computePValuesPermutation();
    isAlreadyComputedPValuesPermutation_ = true ;
  }
  return PValuesPermutation_; 
}

/* Draw the HSIC indices */
Graph HSICEstimatorImplementation::drawValues(const Point &values, const String &title) const
{

  if (values.getDimension() == 0) throw InvalidArgumentException(HERE) << "Error: cannot draw cloud based on empty data.";
  Graph graph(title, "Input marginal number", "", true, "");

  /* Define cloud */
  Sample data(values.getDimension(), 2);
  for (UnsignedInteger k = 0; k < values.getDimension(); ++k)
  {
    data(k, 0) = k + 1.0;
    data(k, 1) = values[k];
  }

  Cloud cloud(data, "red", "circle", "");
  graph.add(cloud);

  /* Min & max indices values */
  const Scalar minInd = data.getMin()[1];
  const Scalar maxInd = data.getMax()[1];


  /* Add text description */
  for (UnsignedInteger k = 0; k < values.getDimension(); ++k)
  {
    data(k, 0) = (k + 1.0) + 0.08;
    data(k, 1) = values[k];
  }

  Description names(values.getDimension());
  for(UnsignedInteger i = 0; i < values.getDimension(); ++i)
  {
    OSS oss;
    oss << "X" << i + 1;
    names[i] = String(oss);
  }

  Text text(data, names, "right");
  text.setColor("black");
  graph.add(text);

  // Set bounding box
  const Scalar step = maxInd - minInd;

  Point lowerBound(2);
  lowerBound[0] = 0.9;
  lowerBound[1] = minInd - 0.05*step;

  Point upperBound(2);
  upperBound[0] = values.getDimension() + 0.2 ;
  upperBound[1] = maxInd + 0.05*step;

  graph.setBoundingBox(Interval(lowerBound, upperBound));

  return graph;
}


/* Draw the HSIC indices */
Graph HSICEstimatorImplementation::drawHSICIndices() const
{
  return drawValues(getHSICIndices(), "HSIC indices");
}

/* Draw the R2-HSIC indices */
Graph HSICEstimatorImplementation::drawR2HSICIndices() const
{
  return drawValues(getR2HSICIndices(), "R2-HSIC indices");
}

/* Draw the p-values */
Graph HSICEstimatorImplementation::drawPValuesPermutation() const
{
  return drawValues(getPValuesPermutation(), "p-values by permutation");
}

/* Get the covariance list */
HSICEstimatorImplementation::CovarianceModelCollection HSICEstimatorImplementation::getCovarianceList() const
{
  return covarianceList_;
}

/* Set the covariance list: dimension is input dimension + 1 */
void HSICEstimatorImplementation::setCovarianceList(const CovarianceModelCollection & coll)
{
  covarianceList_ = coll ;
  resetIndices();
}

/* Get the input sample */
Sample HSICEstimatorImplementation::getInputSample() const
{
  return inputSample_;
}

/* Set the input sample */
void HSICEstimatorImplementation::setInputSample(const Sample & inputSample)
{
  inputSample_ = inputSample;
  resetIndices();
}

/* Get the output sample */
Sample HSICEstimatorImplementation::getOutputSample() const
{
  return outputSample_;
}

/* Set the output sample: must be of dimension one */
void HSICEstimatorImplementation::setOutputSample(const Sample & outputSample)
{
  if(outputSample.getDimension() != 1)
  {
    throw InvalidArgumentException(HERE) << "Dimension of output sample should be 1.";
  }

  outputSample_ = outputSample;
  resetIndices();
}


/* Reset all indices to void */
void HSICEstimatorImplementation::resetIndices()
{
  HSIC_XY_ = Point();
  HSIC_XX_ = Point();
  HSIC_YY_ = Point();
  R2HSICIndices_ = Point();
  PValuesPermutation_ = Point();
  isAlreadyComputedIndices_ = false;
  isAlreadyComputedPValuesPermutation_ = false;
}

/* Get the dimension of the indices: the number of marginals */
UnsignedInteger HSICEstimatorImplementation::getDimension() const
{
  return inputDimension_;
}

/* Get the size of the study sample */
UnsignedInteger HSICEstimatorImplementation::getSize() const
{
  return n_;
}

/* Get the underlying estimator: biased or unbiased*/
HSICStat HSICEstimatorImplementation::getEstimator() const
{
  return estimatorType_;
}

/* Return a shuffled copy of a sample */
Sample HSICEstimatorImplementation::shuffledCopy(const Sample & inSample) const
{
  /* Shuffle an array a of n elements (indices 0..n-1)
    see https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_modern_algorithm
  */
  Sample sampleOut(inSample);
  Point ptmp(sampleOut.getDimension());
  for(UnsignedInteger i = sampleOut.getSize() - 1; i > 0; --i)
  {
    const UnsignedInteger j = RandomGenerator::IntegerGenerate(i + 1);
    ptmp = sampleOut[i];
    sampleOut[i] = sampleOut[j];
    sampleOut[j] = ptmp;
  }
  return sampleOut;
}

/* Run all computations at once */
void HSICEstimatorImplementation::run() const
{
  /* Compute the HSIC and R2-HSIC indices */
  if(!(isAlreadyComputedIndices_))
  {
    computeIndices();
  }

  /* Compute the p-values by permutation */
  if(!(isAlreadyComputedPValuesPermutation_))
  {
    computePValuesPermutation();
  }
}

/* Method save() stores the object through the StorageManager */
void HSICEstimatorImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "covarianceList_", covarianceList_ );
  adv.saveAttribute( "inputSample_", inputSample_ );
  adv.saveAttribute( "outputSample_", outputSample_ );
  adv.saveAttribute( "estimatorType_", estimatorType_ );
  adv.saveAttribute( "weightFunction_", weightFunction_ );
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "inputDimension_", inputDimension_ );
  adv.saveAttribute( "HSIC_XY_", HSIC_XY_ );
  adv.saveAttribute( "HSIC_XX_", HSIC_XX_ );
  adv.saveAttribute( "HSIC_YY_", HSIC_YY_ );
  adv.saveAttribute( "R2HSICIndices_", R2HSICIndices_ );
  adv.saveAttribute( "PValuesPermutation_", PValuesPermutation_ );
  adv.saveAttribute( "permutationSize_", permutationSize_ );
  adv.saveAttribute( "isAlreadyComputedIndices_", isAlreadyComputedIndices_ );
  adv.saveAttribute( "isAlreadyComputedPValuesPermutation_", isAlreadyComputedPValuesPermutation_ );
}

/* Method load() reloads the object from the StorageManager */
void HSICEstimatorImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "covarianceList_", covarianceList_ );
  adv.loadAttribute( "inputSample_", inputSample_ );
  adv.loadAttribute( "outputSample_", outputSample_ );
  adv.loadAttribute( "estimatorType_", estimatorType_ );
  adv.loadAttribute( "weightFunction_", weightFunction_ );
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "inputDimension_", inputDimension_ );
  adv.loadAttribute( "HSIC_XY_", HSIC_XY_ );
  adv.loadAttribute( "HSIC_XX_", HSIC_XX_ );
  adv.loadAttribute( "HSIC_YY_", HSIC_YY_ );
  adv.loadAttribute( "R2HSICIndices_", R2HSICIndices_ );
  adv.loadAttribute( "PValuesPermutation_", PValuesPermutation_ );
  adv.loadAttribute( "permutationSize_", permutationSize_ );
  adv.loadAttribute( "isAlreadyComputedIndices_", isAlreadyComputedIndices_ );
  adv.loadAttribute( "isAlreadyComputedPValuesPermutation_", isAlreadyComputedPValuesPermutation_ );
}

END_NAMESPACE_OPENTURNS

