//                                               -*- C++ -*-
/**
 *  @brief StrongMaxTest implements an algorithm to check if a given design point
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
#ifndef OPENTURNS_STRONGMAXTEST_HXX
#define OPENTURNS_STRONGMAXTEST_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class StrongMaximumTest
 * StrongMaximumTest allows one to validate a design point
 */
class OT_API StrongMaximumTest :
  public PersistentObject
{

  CLASSNAME
public:

  /** Default constructor */
  StrongMaximumTest();

  /** Standard constructor */
  StrongMaximumTest(const StandardEvent & event,
                    const Point & standardSpaceDesignPoint,
                    const Scalar importanceLevel,
                    const Scalar accuracyLevel,
                    const Scalar confidenceLevel);

  /** Standard constructor */
  StrongMaximumTest(const StandardEvent & event,
                    const Point & standardSpaceDesignPoint,
                    const Scalar importanceLevel,
                    const Scalar accuracyLevel,
                    const UnsignedInteger pointNumber);


  /** Virtual constructor */
  StrongMaximumTest * clone() const override;

  /** standardSpaceDesignPoint accessor */
  Point getStandardSpaceDesignPoint() const;

  /** Event accessor */
  StandardEvent getEvent() const;

  /** ImportanceLevel accessor */
  Scalar getImportanceLevel() const;

  /** AccuracyLevel accessor */
  Scalar getAccuracyLevel() const;

  /** ConfidenceLevel accessor */
  Scalar getConfidenceLevel() const;

  /** DesignPointVicinity accessor */
  Scalar  getDesignPointVicinity() const;

  /** PointNumber accessor */
  UnsignedInteger getPointNumber() const;

  /** DeltaEpsilon accessor */
  Scalar getDeltaEpsilon() const;

  /** The function that runs the Strong Max Test */
  void run();

  /** NearDesignPointVerifyingEventPoints accessor */
  Sample getNearDesignPointVerifyingEventPoints() const;

  /** FarDesignPointVerifyingEventPoints accessor */
  Sample getFarDesignPointVerifyingEventPoints() const;

  /** NearDesignPointViolatingEventPoints accessor */
  Sample getNearDesignPointViolatingEventPoints() const;

  /** FarDesignPointViolatingEventPoints accessor */
  Sample getFarDesignPointViolatingEventPoints() const;

  /** NearDesignPointVerifyingEventValues accessor */
  Sample getNearDesignPointVerifyingEventValues() const;

  /** FarDesignPointVerifyingEventValues accessor */
  Sample getFarDesignPointVerifyingEventValues() const;

  /** NearDesignPointViolatingEventValues accessor */
  Sample getNearDesignPointViolatingEventValues() const;

  /** FarDesignPointViolatingEventValues accessor */
  Sample getFarDesignPointViolatingEventValues() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  /** AccuracyLevel accessor */
  void setAccuracyLevel(const Scalar accuracyLevel);

  /** ConfidenceLevel accessor */
  void setConfidenceLevel( const Scalar confidenceLevel);

  /** PointNumber accessor */
  void setPointNumber(const UnsignedInteger pointNumber);

  /** ImportanceLevel accessor */
  void setImportanceLevel(const Scalar importanceLevel);

  /** standardSpaceDesignPoint accessor */
  void setStandardSpaceDesignPoint(const Point & standardSpaceDesignPoint );

  /** DesignPointVicinity accessor */
  void setDesignPointVicinity(const Scalar designPointVicinity);

  /** Event accessor */
  void setEvent(const StandardEvent & event);

  /** Initialize Strong Max Test Parameters : method 1 */
  void initializeParametersGivenConfidenceLevel();

  /** Initialize Strong Max Test Parameters : method 2 */
  void initializeParametersGivenPointNumber();


  /** the function that samples the sphere (radius) with N points */
  Sample sampleSphere(const Scalar radius,
                      const UnsignedInteger dimension,
                      const UnsignedInteger pointNumber) const;

  /**  the function that evaluates the HyperSphereSurfaceRatio (see documentation) */
  Scalar computeHyperSphereSurfaceRatio();

  /**  the function that evaluates delta_epsilon (see documentation) */
  Scalar computeDeltaEpsilon();

  /** the function that evaluates if a point is in the vicinity of the design point */
  Bool isInTheVicinityOfTheDesignPoint(const Point & numericalPoint);

  StandardEvent event_;
  Point standardSpaceDesignPoint_;
  Scalar importanceLevel_;
  Scalar accuracyLevel_;
  Scalar confidenceLevel_;
  Scalar designPointVicinity_; // cosinus of the cone around the standard design point
  UnsignedInteger pointNumber_;
  Scalar deltaEpsilon_;
  Sample nearDesignPointVerifyingEventPoints_;
  Sample nearDesignPointVerifyingEventValues_;
  Sample farDesignPointVerifyingEventPoints_;
  Sample farDesignPointVerifyingEventValues_;
  Sample nearDesignPointViolatingEventPoints_;
  Sample nearDesignPointViolatingEventValues_;
  Sample farDesignPointViolatingEventPoints_;
  Sample farDesignPointViolatingEventValues_;

}; // class StrongMaximumTest

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STRONGMAXTEST_HXX */
