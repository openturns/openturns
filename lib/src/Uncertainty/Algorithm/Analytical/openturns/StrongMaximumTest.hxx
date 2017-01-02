//                                               -*- C++ -*-
/**
 *  @brief StrongMaxTest implements an algorithm to check if a given design point
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_STRONGMAXTEST_HXX
#define OPENTURNS_STRONGMAXTEST_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/NumericalSample.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class StrongMaximumTest
 * StrongMaximumTest allows to validate a design point
 */
class OT_API StrongMaximumTest :
  public PersistentObject
{

  CLASSNAME;
public:

  /** Default constructor */
  StrongMaximumTest();

  /** Standard constructor */
  StrongMaximumTest(const StandardEvent & event,
                    const NumericalPoint & standardSpaceDesignPoint,
                    const NumericalScalar importanceLevel,
                    const NumericalScalar accuracyLevel,
                    const NumericalScalar confidenceLevel);

  /** Standard constructor */
  StrongMaximumTest(const StandardEvent & event,
                    const NumericalPoint & standardSpaceDesignPoint,
                    const NumericalScalar importanceLevel,
                    const NumericalScalar accuracyLevel,
                    const UnsignedInteger pointNumber);


  /** Virtual constructor */
  virtual StrongMaximumTest * clone() const;

  /** standardSpaceDesignPoint accessor */
  NumericalPoint getStandardSpaceDesignPoint() const;

  /** Event accessor */
  StandardEvent getEvent() const;

  /** ImportanceLevel accessor */
  NumericalScalar getImportanceLevel() const;

  /** AccuracyLevel accessor */
  NumericalScalar getAccuracyLevel() const;

  /** ConfidenceLevel accessor */
  NumericalScalar getConfidenceLevel() const;

  /** DesignPointVicinity accessor */
  NumericalScalar  getDesignPointVicinity() const;

  /** PointNumber accessor */
  UnsignedInteger getPointNumber() const;

  /** DeltaEpsilon accessor */
  NumericalScalar getDeltaEpsilon() const;

  /** The function that runs the Strong Max Test */
  void run();

  /** NearDesignPointVerifyingEventPoints accessor */
  NumericalSample getNearDesignPointVerifyingEventPoints() const;

  /** FarDesignPointVerifyingEventPoints accessor */
  NumericalSample getFarDesignPointVerifyingEventPoints() const;

  /** NearDesignPointViolatingEventPoints accessor */
  NumericalSample getNearDesignPointViolatingEventPoints() const;

  /** FarDesignPointViolatingEventPoints accessor */
  NumericalSample getFarDesignPointViolatingEventPoints() const;

  /** NearDesignPointVerifyingEventValues accessor */
  NumericalSample getNearDesignPointVerifyingEventValues() const;

  /** FarDesignPointVerifyingEventValues accessor */
  NumericalSample getFarDesignPointVerifyingEventValues() const;

  /** NearDesignPointViolatingEventValues accessor */
  NumericalSample getNearDesignPointViolatingEventValues() const;

  /** FarDesignPointViolatingEventValues accessor */
  NumericalSample getFarDesignPointViolatingEventValues() const;

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:

  /** AccuracyLevel accessor */
  void setAccuracyLevel(const NumericalScalar accuracyLevel);

  /** ConfidenceLevel accessor */
  void setConfidenceLevel( const NumericalScalar confidenceLevel);

  /** PointNumber accessor */
  void setPointNumber(const UnsignedInteger pointNumber);

  /** ImportanceLevel accessor */
  void setImportanceLevel(const NumericalScalar importanceLevel);

  /** standardSpaceDesignPoint accessor */
  void setStandardSpaceDesignPoint(const NumericalPoint & standardSpaceDesignPoint );

  /** DesignPointVicinity accessor */
  void setDesignPointVicinity(const NumericalScalar designPointVicinity);

  /** Event accessor */
  void setEvent(const StandardEvent & event);

  /** Initialize Strong Max Test Parameters : method 1 */
  void initializeParametersGivenConfidenceLevel();

  /** Initialize Strong Max Test Parameters : method 2 */
  void initializeParametersGivenPointNumber();


  /** the function that samples the sphere (radius) with N points */
  NumericalSample sampleSphere(const NumericalScalar radius,
                               const UnsignedInteger dimension,
                               const UnsignedInteger pointNumber) const;

  /**  the function that evaluates the HyperSphereSurfaceRatio (see documentation) */
  NumericalScalar computeHyperSphereSurfaceRatio();

  /**  the function that evaluates delta_epsilon (see documentation) */
  NumericalScalar computeDeltaEpsilon();

  /** the function that evaluates if a point is in the vicinity of the design point */
  Bool isInTheVicinityOfTheDesignPoint(const NumericalPoint & numericalPoint);

  StandardEvent event_;
  NumericalPoint standardSpaceDesignPoint_;
  NumericalScalar importanceLevel_;
  NumericalScalar accuracyLevel_;
  NumericalScalar confidenceLevel_;
  NumericalScalar designPointVicinity_; // cosinus of the cone around the standard design point
  UnsignedInteger pointNumber_;
  NumericalScalar deltaEpsilon_;
  NumericalSample nearDesignPointVerifyingEventPoints_;
  NumericalSample nearDesignPointVerifyingEventValues_;
  NumericalSample farDesignPointVerifyingEventPoints_;
  NumericalSample farDesignPointVerifyingEventValues_;
  NumericalSample nearDesignPointViolatingEventPoints_;
  NumericalSample nearDesignPointViolatingEventValues_;
  NumericalSample farDesignPointViolatingEventPoints_;
  NumericalSample farDesignPointViolatingEventValues_;

}; // class StrongMaximumTest

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STRONGMAXTEST_HXX */
