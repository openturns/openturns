//                                               -*- C++ -*-
/**
 *  @brief StrongMaxTest implements an algorithm to check if a given design point
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
#include <cmath>
#include <cstdlib>

#include "openturns/StrongMaximumTest.hxx"
#include "openturns/Normal.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/NearestPointChecker.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(StrongMaximumTest)

static const Factory<StrongMaximumTest> Factory_StrongMaximumTest;
typedef DistributionImplementation::InverseIsoProbabilisticTransformation InverseIsoProbabilisticTransformation;


/* For save/load mechanism*/
StrongMaximumTest::StrongMaximumTest()
  : PersistentObject(),
    event_(),
    standardSpaceDesignPoint_(0),
    nearDesignPointVerifyingEventPoints_(0, 0),
    nearDesignPointVerifyingEventValues_(0, 0),
    farDesignPointVerifyingEventPoints_(0, 0),
    farDesignPointVerifyingEventValues_(0, 0),
    nearDesignPointViolatingEventPoints_(0, 0),
    nearDesignPointViolatingEventValues_(0, 0),
    farDesignPointViolatingEventPoints_(0, 0),
    farDesignPointViolatingEventValues_(0, 0)
{
  // Nothing to do
}

/*
 * @class StrongMaximumTest
 * StrongMaximumTest allows one to validate a design point
 */

/* Standard constructor */
StrongMaximumTest::StrongMaximumTest(const StandardEvent & event,
                                     const Point & standardSpaceDesignPoint,
                                     const Scalar importanceLevel,
                                     const Scalar accuracyLevel,
                                     const Scalar confidenceLevel)
  : PersistentObject(),
    event_(event),
    standardSpaceDesignPoint_(standardSpaceDesignPoint),
    nearDesignPointVerifyingEventPoints_(0, standardSpaceDesignPoint.getDimension()),
    nearDesignPointVerifyingEventValues_(0, 1),
    farDesignPointVerifyingEventPoints_(0, standardSpaceDesignPoint.getDimension()),
    farDesignPointVerifyingEventValues_(0, 1),
    nearDesignPointViolatingEventPoints_(0, standardSpaceDesignPoint.getDimension()),
    nearDesignPointViolatingEventValues_(0, 1),
    farDesignPointViolatingEventPoints_(0, standardSpaceDesignPoint.getDimension()),
    farDesignPointViolatingEventValues_(0, 1)
{
  setImportanceLevel(importanceLevel);
  setAccuracyLevel(accuracyLevel);
  setConfidenceLevel(confidenceLevel);
  initializeParametersGivenConfidenceLevel();
}

/* Standard constructor */
StrongMaximumTest::StrongMaximumTest(const StandardEvent & event,
                                     const Point & standardSpaceDesignPoint,
                                     const Scalar importanceLevel,
                                     const Scalar accuracyLevel,
                                     const UnsignedInteger pointNumber)
  : PersistentObject(),
    event_(event),
    standardSpaceDesignPoint_(standardSpaceDesignPoint),
    nearDesignPointVerifyingEventPoints_(0, standardSpaceDesignPoint.getDimension()),
    nearDesignPointVerifyingEventValues_(0, 1),
    farDesignPointVerifyingEventPoints_(0, standardSpaceDesignPoint.getDimension()),
    farDesignPointVerifyingEventValues_(0, 1),
    nearDesignPointViolatingEventPoints_(0, standardSpaceDesignPoint.getDimension()),
    nearDesignPointViolatingEventValues_(0, 1),
    farDesignPointViolatingEventPoints_(0, standardSpaceDesignPoint.getDimension()),
    farDesignPointViolatingEventValues_(0, 1)
{
  if (standardSpaceDesignPoint.norm() < ResourceMap::GetAsScalar( "StrongMaximumTest-Epsilon" )) throw InvalidArgumentException(HERE) << "Error: the given standard space design point is too close to the origin for the strong maximum test to work, norm=" << standardSpaceDesignPoint.norm();
  setImportanceLevel(importanceLevel);
  setAccuracyLevel(accuracyLevel);
  setPointNumber(pointNumber);
  initializeParametersGivenPointNumber();
}

/* Virtual constructor */
StrongMaximumTest * StrongMaximumTest::clone() const
{
  return new StrongMaximumTest(*this);
}

/* standardSpaceDesignPoint accessor */
void StrongMaximumTest::setStandardSpaceDesignPoint(const Point & standardSpaceDesignPoint)
{
  if(standardSpaceDesignPoint.norm() <= 0.0) throw InvalidRangeException(HERE) << "DesignPoint with norm <= 0.0";
  standardSpaceDesignPoint_ = standardSpaceDesignPoint;
}

/* standardSpaceDesignPoint accessor */
Point StrongMaximumTest::getStandardSpaceDesignPoint() const
{
  return standardSpaceDesignPoint_;
}

/* deltaEpsilon accessor */
Scalar StrongMaximumTest::getDeltaEpsilon() const
{
  return deltaEpsilon_;
}

/* Event accessor */
StandardEvent StrongMaximumTest::getEvent() const
{
  return event_;
}

/* ImportanceLevel  accessor */
Scalar StrongMaximumTest::getImportanceLevel() const
{
  return importanceLevel_;
}

/* ImportanceLevel accessor */
void StrongMaximumTest::setImportanceLevel(const Scalar importanceLevel)
{
  if((importanceLevel >= 1.0) || (importanceLevel <= 0.0)) throw InvalidRangeException(HERE) << "importanceLevel is not within 0.0 and 1.0";
  importanceLevel_ = importanceLevel ;
}

/* AccuracyLevel  accessor */
Scalar StrongMaximumTest::getAccuracyLevel() const
{
  return accuracyLevel_;
}

/* AccuracyLevel accessor */
void StrongMaximumTest::setAccuracyLevel(const Scalar accuracyLevel)
{
  if (!(accuracyLevel > 1.0)) throw InvalidRangeException(HERE) << "accuracyLevel is not > 1.0";
  accuracyLevel_ = accuracyLevel ;
}

/* ConfidenceLevel accessor */
Scalar StrongMaximumTest::getConfidenceLevel() const
{
  return confidenceLevel_;
}

/* ConfidenceLevel accessor */
void StrongMaximumTest::setConfidenceLevel(const Scalar confidenceLevel)
{
  if((confidenceLevel > 1.0) || (confidenceLevel < 0.0)) throw InvalidRangeException(HERE) << "confidenceLevel is not within 0.0 and 1.0";
  confidenceLevel_ = confidenceLevel ;
}

/* DesignPointVicinity accessor */
Scalar StrongMaximumTest::getDesignPointVicinity() const
{
  return designPointVicinity_;
}

/* StrongMaxTestDesignPointVicinity accessor */
void  StrongMaximumTest::setDesignPointVicinity(const Scalar designPointVicinity)
{
  if((designPointVicinity >= 1.0) || (designPointVicinity <= 0.0)) throw InvalidRangeException(HERE) << "designPointVicinity is not within 0.0 and 1.0";
  designPointVicinity_ =  designPointVicinity;
}

/* StrongMaxTestPointNumber  accessor */
UnsignedInteger  StrongMaximumTest::getPointNumber() const
{
  return pointNumber_;
}

/* StrongMaxTestPointNumber accessor */
void StrongMaximumTest::setPointNumber(const UnsignedInteger pointNumber)
{
  if(pointNumber == 0) throw InvalidRangeException(HERE) << "pointNumber is equal to 0";
  pointNumber_ = pointNumber ;
}

/* NearDesignPointVerifyingEventPoints accessor */
Sample StrongMaximumTest::getNearDesignPointVerifyingEventPoints() const
{
  return nearDesignPointVerifyingEventPoints_;
}

/* NearDesignPointVerifyingEventValues accessor */
Sample StrongMaximumTest::getNearDesignPointVerifyingEventValues() const
{
  return nearDesignPointVerifyingEventValues_;
}

/* FarDesignPointVerifyingEventPoints accessor */
Sample StrongMaximumTest::getFarDesignPointVerifyingEventPoints() const
{
  return farDesignPointVerifyingEventPoints_;
}

/* FarDesignPointVerifyingEventValues accessor */
Sample StrongMaximumTest::getFarDesignPointVerifyingEventValues() const
{
  return farDesignPointVerifyingEventValues_;
}

/* NearDesignPointViolatingEventPoints accessor */
Sample StrongMaximumTest::getNearDesignPointViolatingEventPoints() const
{
  return nearDesignPointViolatingEventPoints_;
}

/* NearDesignPointViolatingEventValues accessor */
Sample StrongMaximumTest::getNearDesignPointViolatingEventValues() const
{
  return nearDesignPointViolatingEventValues_;
}

/* FarDesignPointViolatingEventPoints accessor */
Sample StrongMaximumTest::getFarDesignPointViolatingEventPoints() const
{
  return farDesignPointViolatingEventPoints_;
}

/* FarDesignPointViolatingEventValues accessor */
Sample StrongMaximumTest::getFarDesignPointViolatingEventValues() const
{
  return farDesignPointViolatingEventValues_;
}


/* String converter */
String StrongMaximumTest::__repr__() const
{
  OSS oss;
  oss << "class=" << StrongMaximumTest::GetClassName()
      << " event=" << event_
      << " standardSpaceDesignPoint=" << standardSpaceDesignPoint_
      << " importanceLevel=" << importanceLevel_
      << " accuracyLevel=" << accuracyLevel_
      << " confidenceLevel=" << confidenceLevel_
      << " designPointVicinity=" << designPointVicinity_
      << " pointNumber=" << pointNumber_
      << " deltaEpsilon=" << deltaEpsilon_
      << " nearDesignPointVerifyingEventPoints=" << nearDesignPointVerifyingEventPoints_
      << " nearDesignPointVerifyingEventValues=" << nearDesignPointVerifyingEventValues_
      << " farDesignPointVerifyingEventPoints=" << farDesignPointVerifyingEventPoints_
      << " farDesignPointVerifyingEventValues=" << farDesignPointVerifyingEventValues_
      << " nearDesignPointViolatingEventPoints=" << nearDesignPointViolatingEventPoints_
      << " nearDesignPointViolatingEventValues=" << nearDesignPointViolatingEventValues_
      << " farDesignPointViolatingEventPoints=" << farDesignPointViolatingEventPoints_
      << " farDesignPointViolatingEventValues=" << farDesignPointViolatingEventValues_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void StrongMaximumTest::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "event_", event_ );
  adv.saveAttribute( "standardSpaceDesignPoint_", standardSpaceDesignPoint_ );
  adv.saveAttribute( "importanceLevel_", importanceLevel_ );
  adv.saveAttribute( "accuracyLevel_", accuracyLevel_ );
  adv.saveAttribute( "confidenceLevel_", confidenceLevel_ );
  adv.saveAttribute( "designPointVicinity_", designPointVicinity_ );
  adv.saveAttribute( "pointNumber_", pointNumber_ );
  adv.saveAttribute( "deltaEpsilon_", deltaEpsilon_ );
  adv.saveAttribute( "nearDesignPointVerifyingEventPoints_", nearDesignPointVerifyingEventPoints_ );
  adv.saveAttribute( "nearDesignPointVerifyingEventValues_", nearDesignPointVerifyingEventValues_ );
  adv.saveAttribute( "farDesignPointVerifyingEventPoints_", farDesignPointVerifyingEventPoints_ );
  adv.saveAttribute( "farDesignPointVerifyingEventValues_", farDesignPointVerifyingEventValues_ );
  adv.saveAttribute( "nearDesignPointViolatingEventPoints_", nearDesignPointViolatingEventPoints_ );
  adv.saveAttribute( "nearDesignPointViolatingEventValues_", nearDesignPointViolatingEventValues_ );
  adv.saveAttribute( "farDesignPointViolatingEventPoints_", farDesignPointViolatingEventPoints_ );
  adv.saveAttribute( "farDesignPointViolatingEventValues_", farDesignPointViolatingEventValues_ );
}

/* Method load() reloads the object from the StorageManager */
void StrongMaximumTest::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "event_", event_ );
  adv.loadAttribute( "standardSpaceDesignPoint_", standardSpaceDesignPoint_ );
  adv.loadAttribute( "importanceLevel_", importanceLevel_ );
  adv.loadAttribute( "accuracyLevel_", accuracyLevel_ );
  adv.loadAttribute( "confidenceLevel_", confidenceLevel_ );
  adv.loadAttribute( "designPointVicinity_", designPointVicinity_ );
  adv.loadAttribute( "pointNumber_", pointNumber_ );
  adv.loadAttribute( "deltaEpsilon_", deltaEpsilon_ );
  adv.loadAttribute( "nearDesignPointVerifyingEventPoints_", nearDesignPointVerifyingEventPoints_ );
  adv.loadAttribute( "nearDesignPointVerifyingEventValues_", nearDesignPointVerifyingEventValues_ );
  adv.loadAttribute( "farDesignPointVerifyingEventPoints_", farDesignPointVerifyingEventPoints_ );
  adv.loadAttribute( "farDesignPointVerifyingEventValues_", farDesignPointVerifyingEventValues_ );
  adv.loadAttribute( "nearDesignPointViolatingEventPoints_", nearDesignPointViolatingEventPoints_ );
  adv.loadAttribute( "nearDesignPointViolatingEventValues_", nearDesignPointViolatingEventValues_ );
  adv.loadAttribute( "farDesignPointViolatingEventPoints_", farDesignPointViolatingEventPoints_ );
  adv.loadAttribute( "farDesignPointViolatingEventValues_", farDesignPointViolatingEventValues_ );
}

/* Initialize Strong Max Test Parameters : method 1 */
void StrongMaximumTest::initializeParametersGivenConfidenceLevel()
{
  /* evaluate the intermediate parameter delta_epsilon (see documentation) */
  deltaEpsilon_ = computeDeltaEpsilon();

  /* evaluate the HyperSphereSurfaceRatio (see documentation) */
  Scalar p = computeHyperSphereSurfaceRatio();
  // put eps1 and eps2 instead of 1.0 and 0.0
  if((p >= 1.0) || (p <= 0.0)) throw InvalidRangeException(HERE) << "hyperSphereSurfaceRatio is not strictly within 0.0 and 1.0";

  /* evaluate and affect the pointNumber_ */
  setPointNumber(static_cast<UnsignedInteger>(round(log1p(-confidenceLevel_) / log1p(-p))));


  /* evaluate and affect the designPointVicinity_ */
  setDesignPointVicinity(1.0 / (1.0 + accuracyLevel_ * deltaEpsilon_));
}

/* Initialize Strong Max Test Parameters : method 2 */
void StrongMaximumTest::initializeParametersGivenPointNumber()
{
  /* evaluate the intermediate parameter delta_epsilon (see documentation) */
  deltaEpsilon_ = computeDeltaEpsilon();

  /* evaluate the HyperSphereSurfaceRatio (see documentation) */
  Scalar p = computeHyperSphereSurfaceRatio();

  // put eps1 and eps2 instead of 1.0 and 0.0
  if((p >= 1.0) || (p <= 0.0)) throw InvalidRangeException(HERE) << "hyperSphereSurfaceRatio is not strictly within 0.0 and 1.0";

  /* evaluate and affect the confidenceLevel */
  setConfidenceLevel(1.0 - pow(1.0 - p, static_cast<int>(pointNumber_)));

  /* evaluate and affect the designPointVicinity_ */
  setDesignPointVicinity(1.0 / (1.0 + accuracyLevel_ * deltaEpsilon_));
}

/*  the function that evaluates the HyperSphereSurfaceRatio (see documentation) */
Scalar StrongMaximumTest::computeHyperSphereSurfaceRatio()
{
  const UnsignedInteger dimension = standardSpaceDesignPoint_.getDimension();
  const Scalar a = acos((1.0 + deltaEpsilon_) / ( 1.0 + accuracyLevel_ * deltaEpsilon_) );
  const Scalar sinA = sin(a);
  const Scalar squareSinA = sinA * sinA;
  Scalar sum = 0.0;
  /* even dimension  */
  if (dimension % 2 == 0)
  {
    const UnsignedInteger indexMax = dimension / 2 - 1;
    Scalar u = sinA;
    for (UnsignedInteger index = 0; index < indexMax; ++index)
    {
      sum += u;
      u *= (1.0 - 1.0 / (2.0 * index + 3.0)) * squareSinA;
    }
    /* M_1_PI = 1/PI cf cmath */
    return M_1_PI * (a - cos(a) * sum);
  }
  else
    /* odd dimension  */
  {
    const UnsignedInteger indexMax = (dimension - 1) / 2;
    Scalar u = 1.0;
    for (UnsignedInteger index = 0; index < indexMax; ++index)
    {
      sum += u;
      u *= (1.0 - 1.0 / (2.0 * index + 2.0)) * squareSinA;
    }
    return 0.5 * (1.0 - cos(a) * sum);
  }
}

/*  the function that evaluates the  intermediate parameter delta_epsilon (see documentation) */
Scalar StrongMaximumTest::computeDeltaEpsilon()
{
  /* evaluate the reliability index */
  const Scalar betaSquare = standardSpaceDesignPoint_.normSquare();

  /* get the input distribution in the standard space */
  const Distribution inputStandardDistribution(event_.getImplementation()->getAntecedent().getDistribution().getImplementation());

  /* evaluate the generator at beta square */
  const Scalar pdfMin = importanceLevel_ * inputStandardDistribution.computeDensityGenerator(betaSquare);

  /* research the interval [deltaMin deltaMax] including the solution */
  Scalar deltaMax = 1.0;

  while ( inputStandardDistribution.computeDensityGenerator(betaSquare * pow(1.0 + deltaMax, 2)) > pdfMin ) ++deltaMax;
  Scalar deltaMin = deltaMax - 1.0;

  /* we proceed to the dichotomie on [deltaMin deltaMax] */
  Scalar deltaMiddle = 0.0;
  const Scalar deltaEpsilon = ResourceMap::GetAsScalar( "StrongMaximumTest-DefaultDeltaPrecision" );
  while ( (deltaMax - deltaMin) > deltaEpsilon )
  {
    /* we evaluate the middle of  [deltaMin deltaMax] */
    deltaMiddle = 0.5 * (deltaMax + deltaMin);
    if(  inputStandardDistribution.computeDensityGenerator(betaSquare * pow(1.0 + deltaMiddle, 2)) > pdfMin )
    {
      deltaMin = deltaMiddle;
    }
    else
    {
      deltaMax = deltaMiddle;
    }
  }
  return 0.5 * (deltaMax + deltaMin);
}

/* the function that evaluate if a point is in the vicinity of the design point */
Bool StrongMaximumTest::isInTheVicinityOfTheDesignPoint(const Point & point)
{
  return point.dot(standardSpaceDesignPoint_) > point.norm() * standardSpaceDesignPoint_.norm() * designPointVicinity_;
}

/* The function that runs the Strong Max Test */
void StrongMaximumTest::run()
{
  /* prepare test parameters */

  /* radius of the inner sphere */
  const Scalar beta = standardSpaceDesignPoint_.norm();
  /* radius of the sphere to be sampled */
  const Scalar radius = beta * (1.0 + accuracyLevel_ * deltaEpsilon_);
  /* sample of the sphere */
  const Sample sample(sampleSphere(radius, standardSpaceDesignPoint_.getDimension(), pointNumber_));
  /* create a nearestPointChecker, in charge of the evaluation of the level function over the sample and to classify the points according to the operator and the threshold */
  NearestPointChecker nearestPointChecker(event_.getImplementation()->getFunction(), event_.getOperator(), event_.getThreshold(), sample);
  /* access to the inverse isoprobabilistic transformation */
  InverseIsoProbabilisticTransformation inverseIsoProbabilisticTransformation(event_.getImplementation()->getAntecedent().getDistribution().getInverseIsoProbabilisticTransformation());
  /* run test */
  try
  {
    nearestPointChecker.run();
  }
  catch(InvalidArgumentException & ex)
  {
    throw InvalidArgumentException(HERE) << ex;
  }
  catch(InternalException & ex)
  {
    throw  InternalException(HERE) << ex;
  }

  /* get nearestPointChecker result */
  NearestPointCheckerResult nearestPointCheckerResult(nearestPointChecker.getResult());
  /* split the two samples according to the vicinity of the design point
   * everything is done in place, using the attributs of the class in order
   * to limit the memory usage */

  nearDesignPointVerifyingEventPoints_ = nearestPointCheckerResult.getVerifyingConstraintPoints();
  nearDesignPointVerifyingEventValues_ = nearestPointCheckerResult.getVerifyingConstraintValues();

  UnsignedInteger sampleSize = nearDesignPointVerifyingEventPoints_.getSize();
  /* If there is something to classify */
  if (sampleSize > 0)
  {
    UnsignedInteger leftCounter = 0;
    UnsignedInteger rightCounter = sampleSize - 1;

    /* we sort among the nearDesignPointVerifyingEventPoints_ (ie which realise the event) the ones which are in the vicinity of the design point */
    while (leftCounter < rightCounter)
    {
      if (isInTheVicinityOfTheDesignPoint(nearDesignPointVerifyingEventPoints_[leftCounter]))
      {
        /* we leave at the beginning of the sample all the points (and the corresponding values) in the vicinity of the design point */
        ++leftCounter;
      }
      else
      {
        /* we put at the end of the sample  all the points (and the corresponding values) not in the vicinity of the design point */
        const Point point(nearDesignPointVerifyingEventPoints_[leftCounter]);
        const Point value(nearDesignPointVerifyingEventValues_[leftCounter]);
        nearDesignPointVerifyingEventPoints_[leftCounter] = nearDesignPointVerifyingEventPoints_[rightCounter];
        nearDesignPointVerifyingEventValues_[leftCounter] = nearDesignPointVerifyingEventValues_[rightCounter];
        nearDesignPointVerifyingEventPoints_[rightCounter] = point;
        nearDesignPointVerifyingEventValues_[rightCounter] = value;
        --rightCounter;
      }
    }
    /* At the end, we still have to check the point at the position leftCounter but without updating rightCounter, which might be already equals to 0 and then might try to become < 0 */
    if (isInTheVicinityOfTheDesignPoint(nearDesignPointVerifyingEventPoints_[leftCounter])) ++leftCounter;
    /* substitute physical points to standard points */
    nearDesignPointVerifyingEventPoints_ = inverseIsoProbabilisticTransformation(nearDesignPointVerifyingEventPoints_);

    /* we build the final two Samples (points, values) for each group */
    /* we split the sortedVerifyingConstraintPoints and the sortedVerifyingConstraintValues in 2 Samples each : one with the left side (points in the vicinity of the design point) and the other with the right side (points not in the vicinity of the design point) */
    if (leftCounter < sampleSize)
    {
      farDesignPointVerifyingEventPoints_ = nearDesignPointVerifyingEventPoints_.split(leftCounter);
      farDesignPointVerifyingEventValues_ = nearDesignPointVerifyingEventValues_.split(leftCounter);
    }
  }
  /* we do the same thing for points which violate the constraints (ie which don't realise the event) */
  farDesignPointViolatingEventPoints_ = nearestPointCheckerResult.getViolatingConstraintPoints();
  farDesignPointViolatingEventValues_ = nearestPointCheckerResult.getViolatingConstraintValues();

  sampleSize = farDesignPointViolatingEventPoints_.getSize();
  /* If there is something to classify */
  if (sampleSize > 0)
  {
    UnsignedInteger leftCounter = 0;
    UnsignedInteger rightCounter = sampleSize - 1;

    /* we sort among the nearDesignPointViolatingEventPoints_ (ie which realise the event) the ones which are in the vicinity of the design point */
    while (leftCounter < rightCounter)
    {
      if (isInTheVicinityOfTheDesignPoint(farDesignPointViolatingEventPoints_[leftCounter]))
      {
        /* we put at the end of the sample  all the points (and the corresponding values) not in the vicinity of the design point */
        Point point(farDesignPointViolatingEventPoints_[leftCounter]);
        Point value(farDesignPointViolatingEventValues_[leftCounter]);
        farDesignPointViolatingEventPoints_[leftCounter] = farDesignPointViolatingEventPoints_[rightCounter];
        farDesignPointViolatingEventValues_[leftCounter] = farDesignPointViolatingEventValues_[rightCounter];
        farDesignPointViolatingEventPoints_[rightCounter] = point;
        farDesignPointViolatingEventValues_[rightCounter] = value;
        --rightCounter;
      }
      else
      {
        /* we leave at the beginning of the sample all the points (and the corresponding values) in the vicinity of the design point */
        ++leftCounter;
      }
    }
    /* At the end, we still have to check the point at the position leftCounter but without updating rightCounter, which should be already equals to 0 and then could try to become < 0 */
    if (!isInTheVicinityOfTheDesignPoint(farDesignPointViolatingEventPoints_[leftCounter])) ++leftCounter;
    /* substitute physical points to standard points */
    farDesignPointViolatingEventPoints_ = inverseIsoProbabilisticTransformation(farDesignPointViolatingEventPoints_);

    /* we build the final two Samples (points, values) for each group */
    /* we split the sortedViolatingConstraintPoints and the sortedViolatingConstraintValues in 2 Samples each : one with the left side (points in the vicinity of the design point) and the other with the right side (points not in the vicinity of the design point) */
    if (leftCounter < sampleSize)
    {
      nearDesignPointViolatingEventPoints_ = farDesignPointViolatingEventPoints_.split(leftCounter);
      nearDesignPointViolatingEventValues_ = farDesignPointViolatingEventValues_.split(leftCounter);
    }
  }
}

/* the function that samples the sphere (radius) with N points */
Sample StrongMaximumTest::sampleSphere(const Scalar radius,
                                       const UnsignedInteger dimension,
                                       const UnsignedInteger pointNumber) const
{
  // First, generate a sample of a standard normal distribution of the proper size and dimension
  const Normal standardNormal(dimension);
  Sample sample(standardNormal.getSample(pointNumber));
  // Then, normalize the points to have length radius
  for (UnsignedInteger i = 0; i < pointNumber; ++i)
  {
    Scalar norm = static_cast<Point>(sample[i]).norm();
    // If the point is the origin, we reject it
    while (norm == 0.0)
    {
      sample[i] = standardNormal.getRealization();
      norm = Point(sample[i]).norm();
    }
    sample[i] *= (radius / norm);
  }
  // The normalize sample follow the uniform distribution over the hypersphere
  return sample;
}

END_NAMESPACE_OPENTURNS
