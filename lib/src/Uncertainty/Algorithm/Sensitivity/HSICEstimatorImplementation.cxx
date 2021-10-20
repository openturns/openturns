
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

#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Pie.hxx"
#include "openturns/Text.hxx"

BEGIN_NAMESPACE_OPENTURNS
CLASSNAMEINIT(HSICEstimatorImplementation)

static const Factory<HSICEstimatorImplementation> Factory_HSICEstimatorImplementation;

typedef Collection<CovarianceModel>   CovarianceModelCollection;

/** Default constructor */
HSICEstimatorImplementation::HSICEstimatorImplementation(): PersistentObject(),
  covarianceList_(CovarianceModelCollection()),
  inputSample_(Sample()),
  outputSample_(Sample()),
  estimatorType_(HSICStat()),
  weightFunction_(Function()),
  n_(0),
  inputDimension_(0),
  HSIC_XY_(Point()),
  HSIC_XX_(Point()),
  HSIC_YY_(Point()),
  R2HSICIndices_(Point()),
  PValuesPermutation_(Point()),
  permutationSize_(0)
{
  // Nothing
}


/** Default constructor */
HSICEstimatorImplementation::HSICEstimatorImplementation(const CovarianceModelCollection & covarianceList, const Sample & X, const Sample & Y,
    const HSICStat & estimatorType, const Function & weightFunction ): PersistentObject(),
  covarianceList_(covarianceList),
  inputSample_(X),
  outputSample_(Y),
  estimatorType_(estimatorType),
  weightFunction_(weightFunction),
  n_(X.getSize()),
  inputDimension_(X.getDimension()),
  HSIC_XY_(Point()),
  HSIC_XX_(Point()),
  HSIC_YY_(Point()),
  R2HSICIndices_(Point()),
  PValuesPermutation_(Point()),
  permutationSize_(100)
{
  if(covarianceList_.getSize() != (inputSample_.getDimension() + outputSample_.getDimension())) throw InvalidDimensionException(HERE) << "The number of covariance momdels is the dimension of the input +1";
  if(outputSample_.getDimension() != 1) throw InvalidDimensionException(HERE) << "The dimension of the output is 1.";
  if(inputSample_.getSize() != outputSample_.getSize()) throw InvalidDimensionException(HERE) << "Input and output samples must have the same size";

}

/** Default constructor */
HSICEstimatorImplementation::HSICEstimatorImplementation(const CovarianceModelCollection & covarianceList, const Sample & X, const Sample & Y,
    const HSICStat & estimatorType): PersistentObject(),
  covarianceList_(covarianceList),
  inputSample_(X),
  outputSample_(Y),
  estimatorType_(estimatorType),
  n_(X.getSize()),
  inputDimension_(X.getDimension()),
  HSIC_XY_ (Point()),
  HSIC_XX_ (Point()),
  HSIC_YY_ (Point()),
  R2HSICIndices_ (Point()),
  permutationSize_(100)
{
  if(covarianceList_.getSize() != (inputSample_.getDimension() + outputSample_.getDimension())) throw InvalidDimensionException(HERE) << "The number of covariance momdels is the dimension of the input +1";
  if(outputSample_.getDimension() != 1) throw InvalidDimensionException(HERE) << "The dimension of the output is 1.";
  if(inputSample_.getSize() != outputSample_.getSize()) throw InvalidDimensionException(HERE) << "Input and output samples must have the same size";

  Description input(1);
  input[0] = "x";
  SymbolicFunction one(input, Description(1, "1"));
  weightFunction_  = one ;
}

/** Virtual constructor */
HSICEstimatorImplementation * HSICEstimatorImplementation::clone() const
{
  return new HSICEstimatorImplementation(*this);
}

/** Compute the weight matrix from the weight function */
SquareMatrix HSICEstimatorImplementation::computeWeightMatrix(const Sample & Y) const
{
  const Sample wY = weightFunction_(Y);
  Scalar meanWY = wY.computeMean()[0];
  SquareMatrix mat(n_);
  for(UnsignedInteger j = 0; j < n_; ++j)
  {
    mat(j, j) = wY(j, 0) / meanWY;
  }
  return mat;
}

/** Compute a HSIC index (one marginal) by using the underlying estimator (biased or not) */
Scalar HSICEstimatorImplementation::computeHSICIndex( const Sample & inSample, const Sample & outSample, const CovarianceModel & inCovariance, const CovarianceModel & outCovariance, const SquareMatrix & weightMatrix) const
{
  return estimatorType_.computeHSICIndex(inSample, outSample, inCovariance, outCovariance, weightMatrix);
}

/** Compute HSIC and R2-HSIC indices */
void HSICEstimatorImplementation::computeIndices()
{
  /* Compute weigts */
  SquareMatrix W = computeWeightMatrix(outputSample_);

  /* Init */
  HSIC_XX_ = Point(inputDimension_);
  HSIC_XY_ = Point(inputDimension_);
  HSIC_YY_ = Point(1);

  /* Loop over marginals: HSIC indices */
  for(UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    Sample xdim(inputSample_.getMarginal(dim));
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

}

/** Set permutation size */
void HSICEstimatorImplementation::setPermutationSize(const UnsignedInteger B)
{
  permutationSize_ = B;
  PValuesPermutation_ = Point();
}

/** Get permutation size */
UnsignedInteger HSICEstimatorImplementation::getPermutationSize() const
{
  return permutationSize_;
}

/** Compute p-value with permutation */
void HSICEstimatorImplementation::computePValuesPermutation()
{
  SquareMatrix Wobs(computeWeightMatrix(outputSample_));
  PValuesPermutation_ = Point(inputDimension_);

  for(UnsignedInteger dim = 0; dim < inputDimension_; ++dim)
  {
    Sample xdim(inputSample_.getMarginal(dim));
    Scalar HSIC_obs = computeHSICIndex(xdim, outputSample_, covarianceList_[dim], covarianceList_[inputDimension_], Wobs);

    UnsignedInteger count = 0;
    for( UnsignedInteger b = 0; b < permutationSize_; ++b)
    {
      Sample Yp = shuffledCopy(outputSample_);
      SquareMatrix W(computeWeightMatrix(Yp));
      Scalar HSIC_loc = computeHSICIndex(xdim, Yp, covarianceList_[dim], covarianceList_[inputDimension_], W);
      if( HSIC_loc > HSIC_obs) count += 1;
    }

    /* p-value by permutation */
    PValuesPermutation_[dim] = count * 1.0 / (permutationSize_ + 1) ;
  }
}

/** Get the HSIC indices.
 *  This is not const as it triggers a computation of the indices
 *  if they are not computed yet.
 * */
Point HSICEstimatorImplementation::getHSICIndices()
{
  if( HSIC_XY_.getDimension() == 0)
  {
    computeIndices();
  }

  return HSIC_XY_;
}

/** Get the R2-HSIC indices.
 *  This is not const as it triggers a computation of the indices
 *  if they are not computed yet.
 * */
Point HSICEstimatorImplementation::getR2HSICIndices()
{
  if( R2HSICIndices_.getDimension() == 0)
  {
    computeIndices();
  }
  return R2HSICIndices_;
}

/** Get the p-values by permutation.
 *  This is not const as it triggers a computation of the values
 *  if they are not computed yet.
 * */
Point HSICEstimatorImplementation::getPValuesPermutation()
{
  if( PValuesPermutation_.getDimension() == 0)
  {
    computePValuesPermutation();
  }
  return PValuesPermutation_;
}

/** Draw the HSIC indices */
Graph HSICEstimatorImplementation::drawValues(const Point &values, String &title)
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
  Point lowerBound(2);
  lowerBound[0] = 0.0;
  if (minInd < 0)
    lowerBound[1] = 1.1 * minInd;
  else
    lowerBound[1] = 0.9 * minInd;

  Point upperBound(2);
  upperBound[0] = values.getDimension() + 1.0 ;

  if (maxInd > 0)
    upperBound[1] = 1.1 * maxInd;
  else
    upperBound[1] = 0.9 * maxInd;

  graph.setBoundingBox(Interval(lowerBound, upperBound));

  return graph;
}


/** Draw the HSIC indices */
Graph HSICEstimatorImplementation::drawHSICIndices()
{
  String title = "HSIC indices";
  Graph graph = drawValues(getHSICIndices(), title);
  return graph;
}

/** Draw the R2-HSIC indices */
Graph HSICEstimatorImplementation::drawR2HSICIndices()
{
  String title = "R2-HSIC indices";
  Graph graph = drawValues(getR2HSICIndices(), title);
  return graph;
}

/** Draw the p-values */
Graph HSICEstimatorImplementation::drawPValuesPermutation()
{
  String title = "p-values by permutation";
  Graph graph = drawValues(getPValuesPermutation(), title);
  return graph;
}

/** Get the covariance list */
CovarianceModelCollection HSICEstimatorImplementation::getCovarianceList() const
{
  return covarianceList_;
}

/** Set the covariance list: dimension is input dimension + 1 */
void HSICEstimatorImplementation::setCovarianceList(const CovarianceModelCollection & coll)
{
  covarianceList_ = coll ;
  resetIndices();
}

/** Get the input sample */
Sample HSICEstimatorImplementation::getInputSample() const
{
  return inputSample_;
}

/** Set the input sample */
void HSICEstimatorImplementation::setInputSample(const Sample & inputSample)
{
  inputSample_ = inputSample;
  resetIndices();
}

/** Get the output sample */
Sample HSICEstimatorImplementation::getOutputSample() const
{
  return outputSample_;
}

/**Set the output sample: must be of dimension one */
void HSICEstimatorImplementation::setOutputSample(const Sample & outputSample)
{
  if(outputSample.getDimension() != 1)
  {
    throw NotYetImplementedException(HERE) << "Dimension of output sample should be 1.";
  }

  outputSample_ = outputSample;
  resetIndices();
}


/** Reset all indices to void */
void HSICEstimatorImplementation::resetIndices()
{
  HSIC_XY_ = Point();
  HSIC_XX_ = Point();
  HSIC_YY_ = Point();
  R2HSICIndices_ = Point();
  PValuesPermutation_ = Point();
}

/** Get the dimension of the indices: the number of marginals */
UnsignedInteger HSICEstimatorImplementation::getDimension() const
{
  return inputDimension_;
}

/** Get the size of the study sample */
UnsignedInteger HSICEstimatorImplementation::getSize() const
{
  return n_;
}

/** Get the underlying estimator: biased or unbiased*/
HSICStat HSICEstimatorImplementation::getEstimator() const
{
  return estimatorType_;
}

Sample HSICEstimatorImplementation::shuffledCopy(const Sample & inSample) const
{
  /* Shuffle an array a of n elements (indices 0..n-1)
    see https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_modern_algorithm
  */
  Sample sampleOut(inSample);
  Point ptmp(sampleOut.getDimension());
  for(UnsignedInteger i = sampleOut.getSize() - 1; i > 0; --i)
  {
    UnsignedInteger j = RandomGenerator::IntegerGenerate(i + 1);
    ptmp = sampleOut[i];
    sampleOut[i] = sampleOut[j];
    sampleOut[j] = ptmp;
  }
  return sampleOut;
}

END_NAMESPACE_OPENTURNS
