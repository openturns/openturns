//                                               -*- C++ -*-
/**
 *  @brief Line sampling simulation method
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
 *
 */
#include "openturns/LineSampling.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/LinearFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LineSampling)

static const Factory<LineSampling> Factory_LineSampling;


/* Default constructor */
LineSampling::LineSampling()
  : EventSimulation()
{
}


/* Constructor with parameters */
LineSampling::LineSampling(const RandomVector & event,
                           const Point & initialAlpha,
                           const RootStrategy & rootStrategy)
  : EventSimulation(event.getImplementation()->asComposedEvent())
  , initialAlpha_(initialAlpha)
  , rootStrategy_(rootStrategy)
  , searchOppositeDirection_(ResourceMap::GetAsBool("LineSampling-DefaultSearchOppositeDirection"))
  , adaptiveImportantDirection_(ResourceMap::GetAsBool("LineSampling-DefaultAdaptiveImportantDirection"))
{
  if (!(event.isEvent() && event.isComposite())) throw InvalidArgumentException(HERE) << "LineSampling requires a composite event";
  const UnsignedInteger outputDimension = getEvent().getFunction().getOutputDimension();
  if (outputDimension > 1)
    throw InvalidArgumentException(HERE) << "Output dimension for LineSampling cannot be greater than 1, here output dimension=" << outputDimension;
  setInitialAlpha(initialAlpha);
  standardEvent_ = StandardEvent(getEvent());
}


/* Virtual constructor */
LineSampling * LineSampling::clone() const
{
  return new LineSampling(*this);
}


/* Performs the actual computation. */
void LineSampling::run()
{
  // First, initialize some parameters
  const UnsignedInteger dimension = initialAlpha_.getDimension();
  alpha_ = initialAlpha_.normalize();
  uStar_ = Point(dimension, std::sqrt(SpecFunc::MaxScalar));

  // clear history
  alphaHistory_ = Sample(0, dimension);
  rootPointsHistory_.clear();
  rootValuesHistory_.clear();

  EventSimulation::run();
}


/* Compute the block sample */
Sample LineSampling::computeBlockSample()
{
  const UnsignedInteger dimension = initialAlpha_.getDimension();
  const UnsignedInteger blockSize = getBlockSize();
  const Distribution inputDistribution(standardEvent_.getAntecedent().getDistribution());
  Sample blockSample(blockSize, 1);
  for (UnsignedInteger blockIndex = 0; blockIndex < blockSize; ++ blockIndex)
  {
    // generate new point in u as origin of the line
    Point uPoint(inputDistribution.getRealization());
    const Scalar uDot = uPoint.dot(alpha_);
    uPoint -= alpha_ * uDot;
    Matrix linear(dimension, 1);
    for (UnsignedInteger i = 0; i < dimension; ++i)
      linear(i, 0) = alpha_[i];

    // store alpha before its update
    if (storeHistory_)
      alphaHistory_.add(alpha_);

    const LinearFunction ray(Point(1, 0.0), uPoint, linear);
    Point roots;
    Sample rootPoints(0, dimension);
    Scalar pf = computeLineContribution(ray, roots, rootPoints);
    if (searchOppositeDirection_)
    {
      const LinearFunction rayOpposite(Point(1, 0.0), uPoint, linear * -1.0);
      pf += computeLineContribution(rayOpposite, roots, rootPoints);
    }
    if (storeHistory_)
    {
      rootValuesHistory_.add(roots);
      rootPointsHistory_.add(rootPoints);
    }
    blockSample(blockIndex, 0) = pf;
  }
  return blockSample;
}


/* Compute the contribution of the performance function along the direction */
Scalar LineSampling::computeLineContribution(const Function & ray, Point & rootsAccumulate, Sample & rootPointsAccumulate)
{
  const ComposedFunction functionAlongRay(standardEvent_.getFunction(), ray);
  const Point roots(rootStrategy_.solve(functionAlongRay, standardEvent_.getThreshold()));
  rootsAccumulate.add(roots);
  for (UnsignedInteger i = 0; i < roots.getSize(); ++ i)
  {
    const Point rootPoint(ray(Point({roots[i]})));
    if (adaptiveImportantDirection_ && (rootPoint.norm() < uStar_.norm()))
    {
      uStar_ = rootPoint;
      alpha_ = uStar_.normalize();
    }
    rootPointsAccumulate.add(rootPoint);
  }
  return computeContribution(roots);
}


/* Compute the contribution of a direction to the probability given the roots of the performance function along the direction */
Scalar LineSampling::computeContribution(const Point & roots) const
{
  Scalar sign = 1.0;
  Scalar estimate = 0.0;
  const UnsignedInteger size = roots.getSize();
  for (UnsignedInteger indexRoot = 0; indexRoot < size; ++indexRoot)
  {
    const Scalar currentRoot = roots[indexRoot];
    estimate += sign * DistFunc::pNormal(-currentRoot);
    sign = -sign;
  }
  // Is the origin in the failure space?
  // Here, we know that the getOriginValue() method will not throw an exception, as we already called the solve() method
  // of the root strategy, which in turn initialized the computation of the origin value.
  const Scalar originValue = rootStrategy_.getOriginValue();
  const Bool inEvent = standardEvent_.getDomain().contains(Point({originValue}));
  // avoid counting origin twice if we search in both directions
  const Scalar one = searchOppositeDirection_ ? 0.5 : 1.0;
  if (inEvent)
    estimate = one - estimate;
  return estimate;
}


void LineSampling::setInitialAlpha(const Point & initialAlpha)
{
  if (initialAlpha.getDimension() != getEvent().getAntecedent().getDistribution().getDimension())
    throw InvalidArgumentException(HERE) << "Got a direction of dimension=" << initialAlpha.getDimension();
  const Scalar alphaNorm = initialAlpha.norm();
  if (!(alphaNorm > 0.0))
    throw InvalidArgumentException(HERE) << "LineSampling: the important direction must be non null";
  initialAlpha_ = initialAlpha;
}

Point LineSampling::getInitialAlpha() const
{
  return initialAlpha_;
}

/* Opposite direction flag accessor */
void LineSampling::setSearchOppositeDirection(const Bool searchOppositeDirection)
{
  searchOppositeDirection_ = searchOppositeDirection;
}

Bool LineSampling::getSearchOppositeDirection() const
{
  return searchOppositeDirection_;
}

/* Adaptive important direction flag accessor */
void LineSampling::setAdaptiveImportantDirection(const Bool adaptiveImportantDirection)
{
  adaptiveImportantDirection_ = adaptiveImportantDirection;
}

Bool LineSampling::getAdaptiveImportantDirection() const
{
  return adaptiveImportantDirection_;
}

/* History accessors */
Bool LineSampling::getStoreHistory() const
{
  return storeHistory_;
}

void LineSampling::setStoreHistory(const Bool storeHistory)
{
  storeHistory_ = storeHistory;
}

Sample LineSampling::getAlphaHistory() const
{
  return alphaHistory_;
}

Collection<Point> LineSampling::getRootValuesHistory() const
{
  return rootValuesHistory_;
}

Collection<Sample> LineSampling::getRootPointsHistory() const
{
  return rootPointsHistory_;
}

/* Root strategy accessor */
void LineSampling::setRootStrategy(const RootStrategy & rootStrategy)
{
  rootStrategy_ = rootStrategy;
}

RootStrategy LineSampling::getRootStrategy() const
{
  return rootStrategy_;
}

/* String converter */
String LineSampling::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " derived from " << EventSimulation::__repr__()
      << " initialAlpha=" << initialAlpha_
      << " rootStrategy=" << rootStrategy_
      << " searchOppositeDirection=" << searchOppositeDirection_
      << " adaptiveImportantDirection=" << adaptiveImportantDirection_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void LineSampling::save(Advocate & adv) const
{
  EventSimulation::save(adv);
  adv.saveAttribute("initialAlpha_", initialAlpha_);
  adv.saveAttribute("rootStrategy_", rootStrategy_);
  adv.saveAttribute("searchOppositeDirection_", searchOppositeDirection_);
  adv.saveAttribute("adaptiveImportantDirection_", adaptiveImportantDirection_);
}


/* Method load() reloads the object from the StorageManager */
void LineSampling::load(Advocate & adv)
{
  EventSimulation::load(adv);
  adv.loadAttribute("initialAlpha_", initialAlpha_);
  adv.loadAttribute("rootStrategy_", rootStrategy_);
  adv.loadAttribute("searchOppositeDirection_", searchOppositeDirection_);
  adv.loadAttribute("adaptiveImportantDirection_", adaptiveImportantDirection_);
}


END_NAMESPACE_OPENTURNS

