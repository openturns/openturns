//                                               -*- C++ -*-
/**
 *  @brief ADS simulation algorithm
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
#include "openturns/AdaptiveDirectionalStratification.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/QuadrantSampling.hxx"
#include "openturns/DirectionalSampling.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(AdaptiveDirectionalStratification)

/* Default constructor */
AdaptiveDirectionalStratification::AdaptiveDirectionalStratification()
  : EventSimulation()
  , partialStratification_(false)
  , maximumStratificationDimension_(ResourceMap::GetAsScalar("AdaptiveDirectionalStratification-DefaultMaximumStratificationDimension"))
{
  // Nothing to do
}

/* Constructor with parameters */
AdaptiveDirectionalStratification::AdaptiveDirectionalStratification(const RandomVector & event,
    const RootStrategy & rootStrategy,
    const SamplingStrategy & samplingStrategy)
  : EventSimulation(event)
  , standardEvent_(StandardEvent(event))
  , rootStrategy_(rootStrategy)
  , samplingStrategy_(samplingStrategy)
  , gamma_(ResourceMap::GetAsUnsignedInteger("AdaptiveDirectionalStratification-DefaultNumberOfSteps"), ResourceMap::GetAsScalar("AdaptiveDirectionalStratification-DefaultGamma"))
  , partialStratification_(false)
  , maximumStratificationDimension_(ResourceMap::GetAsUnsignedInteger("AdaptiveDirectionalStratification-DefaultMaximumStratificationDimension"))
{
  samplingStrategy_.setDimension(getEvent().getImplementation()->getAntecedent().getDimension());
}

/* Virtual constructor */
AdaptiveDirectionalStratification * AdaptiveDirectionalStratification::clone() const
{
  return new AdaptiveDirectionalStratification(*this);
}



void AdaptiveDirectionalStratification::run()
{

  // First, reset the convergence history
  convergenceStrategy_.setDimension(2);

  // input dimension
  const UnsignedInteger dimension = standardEvent_.getImplementation()->getFunction().getInputDimension();

  // current stratification dimension
  UnsignedInteger d = dimension;

  // stratified dimensions
  Indices strataIndices(d);
  strataIndices.fill();

  // current number of subdivisions: 2^d
  UnsignedInteger m = (1 << d);

  // number of steps
  const UnsignedInteger L = gamma_.getDimension();

  // initial uniform allocation
  Point w(m, 1.0 / m);

  // maximum directions budget
  const UnsignedInteger n0 = getMaximumOuterSampling();
  const UnsignedInteger blockSize = 1;

  // effective number of directions
  UnsignedInteger n = 0;

  // for each step
  for (UnsignedInteger l = 0; l < L; ++ l)
  {
    const Point w0(m, 1.0 / m);

    Scalar probabilityEstimate = 0.0;
    Scalar w0SigmaSum = 0.0;
    Point sigma(m, 0.0);

    Sample T0(d, m);
    Sample T1(d, m);

    // for each subdivision
    for (UnsignedInteger i = 0; i < m; ++ i)
    {
      // budget for this subdivision
      const UnsignedInteger ni = static_cast<UnsignedInteger>(gamma_[l] * n0 * w[i]); // (28)
      n += ni;

      QuadrantSampling quadrantSampling (samplingStrategy_, i);
      quadrantSampling.setQuadrantOrientation(quadrantOrientation_);
      quadrantSampling.setStrataIndices(strataIndices);
      DirectionalSampling directionalSampling (getEvent(), rootStrategy_, quadrantSampling);
      directionalSampling.setMaximumOuterSampling (ni);
      directionalSampling.setMaximumCoefficientOfVariation(getMaximumCoefficientOfVariation());
      directionalSampling.setBlockSize (blockSize);
      directionalSampling.run();
      const ProbabilitySimulationResult result(directionalSampling.getResult());
      const Scalar pf = result.getProbabilityEstimate();

      // concatenate the convergence history
      convergenceStrategy_.store(directionalSampling.getConvergenceStrategy().getSample());

      if (pf > 0.0)
      {
        probabilityEstimate += w0[i] * pf;
        sigma[i] = result.getStandardDeviation();
        w0SigmaSum += w0[i] * sigma[i];

        for (UnsignedInteger k = 0; k < d; ++ k)
        {
          if ((1 << k) & i)
            T0(k, i) = pf;
          else
            T1(k, i) = pf;
        }

      } // if pf > 0
      Log::Debug(OSS() << "AdaptiveDirectionalStratification::run n=" << n << " i=" << i << " ni=" << ni << " pf=" << pf << " sigma=" << sigma[i]);

    } // for i

    const Scalar varianceEstimate = w0SigmaSum * w0SigmaSum / (gamma_[l] * n); // (33)

    // update result
    setResult(ProbabilitySimulationResult(getEvent(), probabilityEstimate, varianceEstimate, n, blockSize));

    // update weights
    for (UnsignedInteger i = 0; i < m; ++ i)
    {
      w[i] = (w0SigmaSum > 0.0) ? w0[i] * sigma[i] / w0SigmaSum : 0.0; // (29)
    }

    if ((l == 0) && partialStratification_)
    {
      T_ = Point(dimension);
      for (UnsignedInteger k = 0; k < dimension; ++ k)
      {
        for (UnsignedInteger i = 0; i < m; ++ i)
          if ((1 << k) & i)
            T_[k] += std::abs(T0(k, i) - T1(k, i));

        Log::Debug(OSS() << "AdaptiveDirectionalStratification::run T[" << k << "]=" << T_[k]);
      }

      // sort variables according to T statistic
      Indices order(dimension);
      order.fill();
      for (SignedInteger i = dimension - 1; i >= 0; -- i)
        for (SignedInteger j = 0; j < i; ++ j)
          if (T_[order[j]] < T_[order[j + 1]])
          {
            const UnsignedInteger swap = order[j];
            order[j] = order[j + 1];
            order[j + 1] = swap;
          }
      for (UnsignedInteger k = 0; k < dimension; ++ k)
        Log::Debug(OSS() << "AdaptiveDirectionalStratification::run #" << k << " T[" << order[k] << "]=" << T_[order[k]]);

      // retrieve the p' variables contributing the most
      strataIndices = Indices();
      for (UnsignedInteger k = 0; k < dimension; ++ k)
      {
        if (k < maximumStratificationDimension_)
          strataIndices.add(order[k]);
      }

      // new stratification dimension d' < d
      const UnsignedInteger d2 = strataIndices.getSize();
      const UnsignedInteger m2 = 1 << d2;

      // compute new weights using existing simulations
      Point w2(m2, 0.0);
      for (UnsignedInteger i2 = 0; i2 < m2; ++ i2)
      {
        for (UnsignedInteger i = 0; i < m; ++ i)
        {
          UnsignedInteger sum = 0;
          for (UnsignedInteger k = 0; k < d2; ++ k)
          {
            const UnsignedInteger bit = 1 << strataIndices[k];
            if (bit & i)
              sum += 1 << k;
          }
          if (sum == i2)
          {
            w2[i2] += w[i];
          }
        }
      }

      // update some parameters
      m = m2;
      w = w2;
      d = d2;

    } // if partialStratification_
  } // for L
}

/* Root strategy accessor */
void AdaptiveDirectionalStratification::setRootStrategy(const RootStrategy & rootStrategy)
{
  rootStrategy_ = rootStrategy;
}

RootStrategy AdaptiveDirectionalStratification::getRootStrategy() const
{
  return rootStrategy_;
}

/* Sampling strategy */
void AdaptiveDirectionalStratification::setSamplingStrategy(const SamplingStrategy & samplingStrategy)
{
  const UnsignedInteger dimension = getEvent().getImplementation()->getAntecedent().getDistribution().getDimension();
  if (samplingStrategy.getDimension() != dimension)
    throw InvalidDimensionException(HERE) << "Error: the sampling strategy dimension (" << samplingStrategy.getDimension() << ") is not compatible with the antecedent dimension (" << dimension << ")";
  samplingStrategy_ = samplingStrategy;
}

SamplingStrategy AdaptiveDirectionalStratification::getSamplingStrategy() const
{
  return samplingStrategy_;
}

void AdaptiveDirectionalStratification::setGamma(const Point& gamma)
{
  const UnsignedInteger dimension = gamma.getDimension();
  if (dimension > 2) throw InvalidDimensionException(HERE) << "gamma dimension is " << dimension;
  Scalar sum = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    if (!(gamma[i] > 0.0)) throw InvalidArgumentException(HERE) << "gamma values should be positive";
    sum += gamma[i];
  }
  if (std::abs(sum - 1.0) > 1e-6) throw InvalidArgumentException(HERE) << "gamma components do not sum to 1";
  gamma_ = gamma;
}

Point AdaptiveDirectionalStratification::getGamma() const
{
  return gamma_;
}


void AdaptiveDirectionalStratification::setQuadrantOrientation(const OT::Point& quadrantOrientation)
{
  const UnsignedInteger dimension = getEvent().getImplementation()->getAntecedent().getDimension();
  if ((quadrantOrientation.getDimension() > 0) && (quadrantOrientation.getDimension() != dimension))
    throw InvalidDimensionException(HERE) << "Error: the quadrant orientation dimension (" << quadrantOrientation.getDimension() << ") is not compatible with the antecedent dimension (" << dimension << ")";
  quadrantOrientation_ = quadrantOrientation;
}

OT::Point AdaptiveDirectionalStratification::getQuadrantOrientation() const
{
  return quadrantOrientation_;
}


Sample AdaptiveDirectionalStratification::computeBlockSample()
{
  return Sample();
}

/* String converter */
String AdaptiveDirectionalStratification::__repr__() const
{
  OSS oss;
  oss << "class=" << AdaptiveDirectionalStratification::GetClassName();
  return oss;
}


void AdaptiveDirectionalStratification::setPartialStratification(Bool partialStratification)
{
  partialStratification_ = partialStratification;
}


OT::Bool AdaptiveDirectionalStratification::getPartialStratification() const
{
  return partialStratification_;
}


void AdaptiveDirectionalStratification::setMaximumStratificationDimension(OT::UnsignedInteger maximumStratificationDimension)
{
  maximumStratificationDimension_ = maximumStratificationDimension;
}

OT::UnsignedInteger AdaptiveDirectionalStratification::getMaximumStratificationDimension() const
{
  return maximumStratificationDimension_;
}


OT::Point AdaptiveDirectionalStratification::getTStatistic() const
{
  return T_;
}


END_NAMESPACE_OPENTURNS
