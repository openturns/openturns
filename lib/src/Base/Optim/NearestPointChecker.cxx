//                                               -*- C++ -*-
/**
 *  @brief NearestPointChecker checks whether the nearest point found is really the nearestpoint
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/NearestPointChecker.hxx"
#include "openturns/Point.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




CLASSNAMEINIT(NearestPointChecker)
/*
 * @brief  Standard constructor: the problem is defined by a scalar valued function  (in fact, a 1-D vector valued function)
 *         and a level value
 */
/* Constructor with parameters */
NearestPointChecker::NearestPointChecker(const Function & levelFunction,
    const ComparisonOperator & comparisonOperator,
    const Scalar threshold,
    const Sample & sample):
  PersistentObject(),
  levelFunction_(levelFunction),
  comparisonOperator_(comparisonOperator),
  threshold_(threshold),
  sample_(sample)
{
  // Nothing to do
}

/* Virtual constructor */
NearestPointChecker * NearestPointChecker::clone() const
{
  return new NearestPointChecker(*this);
}

/* LevelFunction accessor */
Function NearestPointChecker::getLevelFunction() const
{
  return levelFunction_;
}

/* LevelFunction accessor */
void NearestPointChecker::setLevelFunction(const Function & levelFunction)
{
  levelFunction_ = levelFunction;
}

/* Result accessor */
NearestPointCheckerResult NearestPointChecker::getResult() const
{
  return result_;
}

/* Result accessor */
void NearestPointChecker::setResult(const NearestPointCheckerResult & result)
{
  result_ = result;
}

/* Comparison operator accessor */
void NearestPointChecker::setComparisonOperator(const ComparisonOperator & comparisonOperator)
{
  comparisonOperator_ = comparisonOperator;
}

/* Comparison operator accessor */
ComparisonOperator NearestPointChecker::getComparisonOperator() const
{
  return comparisonOperator_;
}

/* threshold accessor */
void NearestPointChecker::setThreshold(const Scalar threshold)
{
  threshold_ = threshold;
}

/* threshold accessor */
Scalar NearestPointChecker::getThreshold() const
{
  return threshold_;
}

/* sample accessor */
void NearestPointChecker::setSample(const Sample & sample)
{
  sample_ = sample;
}

/* sample accessor */
Sample NearestPointChecker::getSample() const
{
  return sample_;
}

/* Performs the actual test */
void  NearestPointChecker::run()
{
  /* Total number of points to be sampled */
  UnsignedInteger pointNumber = sample_.getSize();
  /* Compute the level function on the sample */
  Sample levelValuesSample(getLevelFunction()(sample_));
  Sample notVerifyingPoints(0, Point(sample_.getDimension()));
  Sample notVerifyingValues(0, Point(levelValuesSample.getDimension()));
  /* If there is something to classify */
  if (pointNumber > 0)
  {
    /* Sort the points according to the comparison operator and the threshold */
    UnsignedInteger toBeClassified = 0;
    UnsignedInteger notClassified = pointNumber - 1;
    /* While some points remain to be classified */
    while (toBeClassified < notClassified)
    {
      if (getComparisonOperator().compare(levelValuesSample(toBeClassified, 0), getThreshold())) // the point verifies the constraint
      {
        /* Leave it in place and go to the next point */
        toBeClassified++;
      }
      else // the point violates the constraint
      {
        /* Exchange the point with the last point not already classified */
        Point point(sample_[toBeClassified]);
        Point value(levelValuesSample[toBeClassified]);
        sample_[toBeClassified] = sample_[notClassified];
        sample_[notClassified] = point;
        levelValuesSample[toBeClassified] = levelValuesSample[notClassified];
        levelValuesSample[notClassified] = value;
        notClassified--;
      }
    }
    /* At the end, we still have to check the point at the position toBeClassified but without updating notClassified, which should be already equals to 0 and then could try to become < 0 */
    if (getComparisonOperator().compare(levelValuesSample(toBeClassified, 0), getThreshold())) toBeClassified++;

    /* we split the sample_ in 2 Sample : one with the left side (points verifying the constraint) and the other with the right side (points not verifying the constraint) */
    if (toBeClassified < pointNumber)
    {
      notVerifyingPoints = sample_.split(toBeClassified);

      /* we split the levelValuesSample in 2 Samples : one with the left side (points verifying the constraint) and the other with the right side (points not verifying the constraint) */
      notVerifyingValues = levelValuesSample.split(toBeClassified);
    }
  }
  /* we affect the 4 samples to result_ */
  setResult(NearestPointCheckerResult(sample_, levelValuesSample, notVerifyingPoints, notVerifyingValues));
}

/* String converter */
String NearestPointChecker::__repr__() const
{
  OSS oss;
  oss << "class=" << NearestPointChecker::GetClassName()
      << " LevelFunction=" << levelFunction_
      << " result=" << result_
      << " operator=" << comparisonOperator_
      << " threshold=" << threshold_;
  return oss;
}

END_NAMESPACE_OPENTURNS
