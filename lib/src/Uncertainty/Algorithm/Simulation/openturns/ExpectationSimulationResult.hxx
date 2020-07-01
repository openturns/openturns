//                                               -*- C++ -*-
/**
 *  @brief Result of an expectation simulation
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_EXPECTATIONSIMULATIONRESULT_HXX
#define OPENTURNS_EXPECTATIONSIMULATIONRESULT_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/SimulationResult.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExpectationSimulationResult
 */
class OT_API ExpectationSimulationResult
  : public SimulationResult
{

  CLASSNAME
public:


  /** Default constructor */
  ExpectationSimulationResult();

  /** Standard constructor */
  ExpectationSimulationResult(const RandomVector & randomVector,
                              const Point expectationEstimate,
                              const UnsignedInteger outerSampling,
                              const UnsignedInteger blockSize);

  /** Virtual constructor */
  virtual ExpectationSimulationResult * clone() const;

  /** Event accessor */
  RandomVector getRandomVector() const;
  void setRandomVector(const RandomVector & randomVector);

  /** Probability estimate accessor */
  Point getExpectationEstimate() const;
  void setExpectationEstimate(const Point expectationEstimate);

  /** Variance estimate accessor */
  Point getVarianceEstimate() const;
  void setVarianceEstimate(const Point varianceEstimate);

  /** Coefficient of variation estimate accessor */
  virtual Point getCoefficientOfVariation() const;

  /** Standard deviation estimate accessor */
  virtual Point getStandardDeviation() const;

  /** Distribution of the expectation */
  Distribution getExpectationDistribution() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

  /* The event we are interested in */
  RandomVector randomVector_;
  Point expectationEstimate_;
  Point varianceEstimate_;

}; // class ExpectationSimulationResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXPECTATIONSIMULATIONRESULT_HXX */
