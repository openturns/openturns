//                                               -*- C++ -*-
/**
 *  @brief A class which implements the ARMA process
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

#include <limits>
#include "ARMA.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"
#include "Normal.hxx"
#include "SpecFunc.hxx"
#include "Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection< NumericalComplex >         NumericalComplexCollection;

CLASSNAMEINIT(ARMA);

static const Factory<ARMA> RegisteredFactory;

ARMA::ARMA()
  : ProcessImplementation()
  , ARCoefficients_(0, 1)
  , MACoefficients_(0, 1)
  , noiseDistribution_(Normal())
  , p_(0)
  , q_(0)
  , state_(NumericalSample(0, 1), NumericalSample(0, 1))
  , hasComputedNThermalization_(true)
  , nThermalization_(2)
{
  setDimension(1);
  thermalize();
  setDescription(noiseDistribution_.getDescription());
}

/* Standard constructor with coefficients and a White Noise */
ARMA::ARMA(const ARMACoefficients & ARCoefficients,
           const ARMACoefficients & MACoefficients,
           const WhiteNoise & whiteNoise)
  : ProcessImplementation()
  // Don't initialize these attributes before to check if they are valid
  // as they can be huge
  , ARCoefficients_()
  , MACoefficients_()
  , noiseDistribution_(whiteNoise.getDistribution())
  , p_(ARCoefficients.getSize())
  , q_(MACoefficients.getSize())
  , state_()
  , hasComputedNThermalization_(false)
  , nThermalization_(0)
{
  if (ARCoefficients_.getDimension() != MACoefficients_.getDimension())
    throw InvalidArgumentException(HERE) << "Error:  Incompatible dimensions for coefficients of AR part and MA part";
  ARCoefficients_ = ARCoefficients;
  MACoefficients_ = MACoefficients;

  setDimension(ARCoefficients_.getDimension());
  setDescription(noiseDistribution_.getDescription());
  // This call checks that the given WhiteNoise is based on a RegularGrid
  setTimeGrid(whiteNoise.getTimeGrid());
  // The default state is with null values and noises
  state_ = ARMAState(NumericalSample(p_, dimension_), NumericalSample(q_, dimension_));
  // Thermalize
  thermalize();
}

/* Standard constructor with coefficients, a White Noise and a state */
ARMA::ARMA(const ARMACoefficients & ARCoefficients,
           const ARMACoefficients & MACoefficients,
           const WhiteNoise & whiteNoise,
           const ARMAState & state)
  : ProcessImplementation()
  // Don't initialize these attributes before to check if they are valid
  // as they can be huge
  , ARCoefficients_()
  , MACoefficients_()
  , noiseDistribution_(whiteNoise.getDistribution())
  , p_(ARCoefficients.getSize())
  , q_(MACoefficients.getSize())
  , state_()
  , hasComputedNThermalization_(false)
  , nThermalization_(0)
{
  if (ARCoefficients_.getDimension() != MACoefficients_.getDimension())
    throw InvalidArgumentException(HERE) << "Error:  Incompatible dimensions for coefficients of AR part and MA part";
  ARCoefficients_ = ARCoefficients;
  MACoefficients_ = MACoefficients;

  setDimension(ARCoefficients_.getDimension());
  setDescription(noiseDistribution_.getDescription());
  // This call checks that the given WhiteNoise is based on a RegularGrid
  setTimeGrid(whiteNoise.getTimeGrid());
  setState(state);
}

/* Virtual constructor */
ARMA * ARMA::clone() const
{
  return new ARMA(*this);
}

/* String converter */
String ARMA::__repr__() const
{
  OSS oss;
  oss << "class= " << ARMA::GetClassName()
      << " timeGrid=" << RegularGrid(mesh_)
      << " coefficients AR=" << ARCoefficients_
      << " coefficients MA=" << MACoefficients_
      << " noiseDistribution= " << noiseDistribution_
      << " state= " << state_ ;
  return oss;
}

String ARMA::__str__(const String & offset) const
{
  OSS oss;
  oss << "ARMA(";
  //  ARMA process
  for (UnsignedInteger d = 0; d < dimension_ ; ++d)
  {
    if (d > 0) oss << "\n";
    // Writing d-th the marginal process
    oss << "X_{" << d << ",t}" ;
    // decomposition by number of elements
    for (UnsignedInteger i = 0; i < p_ ; i++)
    {
      for (UnsignedInteger dimensionComponent = 0; dimensionComponent < dimension_ ; ++dimensionComponent)
      {
        const NumericalScalar ai(ARCoefficients_[i](d, dimensionComponent));
        if (ai > 0) oss << " + " <<  ai << " X_{" << dimensionComponent << ",t-" << i + 1 << "}";
        if (ai < 0) oss << " - " << -ai << " X_{" << dimensionComponent << ",t-" << i + 1 << "}";
      }
    }
    // MA part
    // with convention \epsilon_{t} + \sum_{i=1}^{q} \beta_{k} * \epsilon_{t-k}
    oss  << " = "  << "E_{" << d << ",t}";
    // q - 1 first components
    for (UnsignedInteger i = 0; i < q_  ; ++i)
    {
      for (UnsignedInteger dimensionComponent = 0; dimensionComponent < dimension_ ; ++dimensionComponent)
      {
        const NumericalScalar ai(MACoefficients_[i](d, dimensionComponent));
        if (ai > 0) oss << " + " <<  ai << " E_{" << dimensionComponent << ",t-" << i + 1 << "}";
        if (ai < 0) oss << " - " << -ai << " E_{" << dimensionComponent << ",t-" << i + 1 << "}";
      }
    }
  }
  oss << ", E_t ~ " << noiseDistribution_.__str__(offset) << ")";

  return oss;
}

/* Is the underlying a Normal process ? */
Bool ARMA::isNormal() const
{
  return (noiseDistribution_.isElliptical() && noiseDistribution_.getStandardDistribution().hasIndependentCopula());
}

/* Is the underlying a stationary process ? */
Bool ARMA::isStationary() const
{
  // True since we consider stationary process only
  return true;
}


UnsignedInteger ARMA::computeNThermalization(const NumericalScalar epsilon) const
{
  if (epsilon <= 0.0) throw InvalidArgumentException(HERE) << "Error: epsilon must be positive, here epsilon=" << epsilon;
  // MA processes are always stationary. Just do q_ + 1 steps to forget
  // the initial noise values
  if (p_ == 0) return q_ + 1;
  // Companion matrix - Matrix is of size (dimension * p_)
  SquareMatrix matrix(dimension_ * p_);
  for (UnsignedInteger coefficientIndex = 0; coefficientIndex < p_ ; ++coefficientIndex)
  {
    for  (UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++ rowIndex)
    {
      for (UnsignedInteger columnIndex = 0; columnIndex < dimension_; ++ columnIndex)
      {
        matrix( dimension_ * (p_ - 1) +  rowIndex, coefficientIndex * dimension_ + columnIndex ) = -ARCoefficients_[p_ - 1 - coefficientIndex](rowIndex, columnIndex) ;
      }
    }
  }

  // Incorporation into the previous for loop
  for (UnsignedInteger index = 0; index < dimension_ * (p_ - 1); ++index)
  {
    matrix(index, dimension_ + index) = 1.0;
  }

  // Computation of EigenValues without keeping intact (matrix not used after)
  const NumericalComplexCollection eigenValues(matrix.computeEigenValues(false));

  // Find the largest eigenvalue module
  NumericalScalar s(std::abs(eigenValues[0]));
  for (UnsignedInteger i = 1; i < eigenValues.getSize() ; ++i) s = std::max(s, std::abs(eigenValues[i]));
  // If the largest eigenvalue is not in the interior of the unit circle, the ARMA process is not stable
  if (s >= 1.0) throw InvalidArgumentException(HERE) << "Error: the ARMA process is not stationary with the given coefficients. Here, AR coefficients=" << ARCoefficients_ << " and MA coefficients=" << MACoefficients_ << " with largest eigenvalue s=" << s;
  return static_cast<UnsignedInteger>(ceil( log(epsilon) / log(s) ) );
}

/* Nthermalization get accessor */
UnsignedInteger ARMA::getNThermalization() const
{
  if (!hasComputedNThermalization_)
  {
    // Not yet in SpecFunc
    nThermalization_ = computeNThermalization(std::numeric_limits<NumericalScalar>::epsilon());
    hasComputedNThermalization_ = true;
  }
  return nThermalization_;
}

/* Nthermalization set accessor */
void ARMA::setNThermalization(const UnsignedInteger size)
{
  nThermalization_ = size;
  hasComputedNThermalization_ = true;
}


// Compute the steps next values of the process starting from the current state.
// The result is the current state extended steps date further
ARMAState ARMA::computeReccurence(const UnsignedInteger stepNumber) const
{
  // We extend the state by stepNumber points
  NumericalSample result(state_.getX());
  NumericalSample epsilonValues(state_.getEpsilon());
  // Pre-allocate the room for the stepNumber next values
  result.add(NumericalSample(stepNumber, dimension_));
  epsilonValues.add(noiseDistribution_.getSample(stepNumber));

  // Consider : X_t = \sum_{i=0}^{p-1} A[i] * X_{t-i-1} + \sum_{i=0}^{q-1} B[i] * \epsilon_{t-i-1} + \epsilon_{t}
  // last observations organization: x[0] -->  X_{-(p-1)}, x[1]-> X_{2-p} ... x[p-1] -> X_{-1}
  // coefficients organization: coeff[0]-> coeff_{t-1}, coeff[1] -> coeff_{t-2}....
  for (UnsignedInteger t = 0; t < stepNumber ; ++t)
  {
    // Next value
    // initialization using the current noise value
    result[p_ + t] = epsilonValues[q_ + t];
    // AR part computation : \sum_{i=0}^{p-1} A[i] * X_{t-i-1}
    for(UnsignedInteger i = 0; i < p_ ; ++i) result[p_ + t] -= ARCoefficients_[i] * result[(p_ - 1 + t) - i];
    // MA part computation : \sum_{i=0}^{p-1} B[i] * \epsilon_{t-i-1}
    for(UnsignedInteger i = 0; i < q_ ; ++i) result[p_ + t] += MACoefficients_[i] * epsilonValues[(q_ - 1 + t) - i];
  }

  return ARMAState(result, epsilonValues);
}

/* Thermalize : burn-in  */
void ARMA::thermalize() const
{
  // Check if the number of iterations nThermalize is known or compute it
  const UnsignedInteger stepNumber(getNThermalization());
  // Go stepNumber steps further and use the result to update the current state
  setState(computeReccurence(stepNumber));
}

/* Realization accessor */
Field ARMA::getRealization() const
{
  // thermalize to "forget" the initial state --> change state
  thermalize();

  // Get the size of the realization
  const UnsignedInteger size(RegularGrid(mesh_).getN());

  // Go size steps further: newState contains (size + p_) X values and (q_ + size) epsilon values
  const ARMAState newState(computeReccurence(size));

  // Update the current state
  setState(newState);

  // Use the X part of the newState to build the realization
  NumericalSample values(newState.getX().split(p_));
  values.setDescription(getDescription());
  return Field(mesh_, values);
}

// Prediction of the futur of an ARMA process
// possible futur changes
TimeSeries ARMA::getFuture(const UnsignedInteger stepNumber) const
{
  if (stepNumber == 0) throw InvalidArgumentException(HERE) << "Error: the number of future steps must be positive.";
  /* TimeGrid associated with the possible future */
  const NumericalScalar timeStep(RegularGrid(mesh_).getStep());
  // The EndTime is not considered to be included in the TimeGrid
  const RegularGrid futurTimeGrid(RegularGrid(mesh_).getEnd(), timeStep, stepNumber);

  // Run the computation
  const ARMAState newState(computeReccurence(stepNumber));

  return TimeSeries(futurTimeGrid, newState.getX().split(p_));
}

/* Coefficients accessor : AR & MA */
ARMACoefficients ARMA::getARCoefficients() const
{
  return ARCoefficients_;
}

/* Coefficients accessor : AR & MA */
ARMACoefficients ARMA::getMACoefficients() const
{
  return MACoefficients_;
}

/* State accessor of the ARMA process */
ARMAState ARMA::getState() const
{
  return state_;
}

void ARMA::setState(const ARMAState & state) const
{
  NumericalSample x(state.getX());
  NumericalSample epsilon(state.getEpsilon());
  if (p_ > x.getSize()) throw InvalidArgumentException(HERE) << "Error:  Size of coefficients of AR part is greater than the size of the last observed values";
  if (q_ > epsilon.getSize()) throw InvalidArgumentException(HERE) << "Error:  Size of coefficients of MA part is greater than the size of the last observed noise";
  // Only the p_ last values of X and the q_ last values of epsilon are needed
  state_ = ARMAState(x.split(x.getSize() - p_), epsilon.split(epsilon.getSize() - q_));
}

/* WhiteNoise accessor of the ARMA process */
WhiteNoise ARMA::getWhiteNoise() const
{
  return WhiteNoise(noiseDistribution_, mesh_);
}

void ARMA::setWhiteNoise(const WhiteNoise & whiteNoise)
{
  noiseDistribution_ = whiteNoise.getDistribution();
}


/* Get the random vector corresponding to the i-th marginal component */
ARMA::Implementation ARMA::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "Error: the index must be less than the output dimension";
  if (getDimension() == 1) return clone();
  throw NotYetImplementedException(HERE) << "In ARMA::getMarginal(const UnsignedInteger i) const";
}

/* Get the marginal random vector corresponding to indices components */
ARMA::Implementation ARMA::getMarginal(const Indices & indices) const
{
  if (!indices.check(dimension_ - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal process must be in the range [0, dim-1] and  must be different";
  throw NotYetImplementedException(HERE) << "In ARMA::getMarginal(const Indices & indices) const";
}

/* Method save() stores the object through the StorageManager */
void ARMA::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute( "p_", p_);
  adv.saveAttribute( "q_", q_);
  adv.saveAttribute( "hasComputedNThermalization_", hasComputedNThermalization_ );
  adv.saveAttribute( "nThermalization_", nThermalization_ );
  adv.saveAttribute( "ARCoefficients_", ARCoefficients_);
  adv.saveAttribute( "MACoefficients_", MACoefficients_ );
  adv.saveAttribute( "state_", state_ );
  adv.saveAttribute( "noiseDistribution_", noiseDistribution_ );
}

/* Method load() reloads the object from the StorageManager */
void ARMA::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute( "p_", p_);
  adv.loadAttribute( "q_", q_);
  adv.loadAttribute( "hasComputedNThermalization_", hasComputedNThermalization_ );
  adv.loadAttribute( "nThermalization_", nThermalization_ );
  adv.loadAttribute( "ARCoefficients_", ARCoefficients_);
  adv.loadAttribute( "MACoefficients_", MACoefficients_ );
  adv.loadAttribute( "state_", state_ );
  adv.loadAttribute( "noiseDistribution_", noiseDistribution_ );
}
END_NAMESPACE_OPENTURNS
