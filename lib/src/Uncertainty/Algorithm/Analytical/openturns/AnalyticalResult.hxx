//                                               -*- C++ -*-
/**
 *  @brief AnalyticalResult implements an algorithm to find the design point
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
#ifndef OPENTURNS_ANALYTICALRESULT_HXX
#define OPENTURNS_ANALYTICALRESULT_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/PointWithDescription.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/Graph.hxx"
#include "openturns/Description.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/OptimizationResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class AnalyticalResult
 * AnalyticalResult stores the Analytical result
 */
class OT_API AnalyticalResult
  : public PersistentObject
{
  CLASSNAME
public:

  typedef Collection<Graph>                         GraphCollection;
  typedef Collection<Point>                PointCollection;
  typedef Collection<PointWithDescription> PointWithDescriptionCollection;
  typedef PointWithDescriptionCollection   Sensitivity;

  typedef enum {ELLIPTICAL = 0, CLASSICAL, PHYSICAL} ImportanceFactorType;

  /** Standard constructor */
  AnalyticalResult(const Point & standardSpaceDesignPoint,
                   const RandomVector & limitStateVariable,
                   const Bool isStandardPointOriginInFailureSpace);

  /* Default constructor (required by SWIG :-<) */
  AnalyticalResult();

  /** Virtual constructor */
  AnalyticalResult * clone() const override;

  /** StandardSpaceDesignPoint accessor */
  Point getStandardSpaceDesignPoint() const;

  /** PhysicalSpaceDesignPoint accessor */
  void setStandardSpaceDesignPoint(const Point & standardSpaceDesignPoint);

  /** PhysicalSpaceDesignPoint accessor */
  Point getPhysicalSpaceDesignPoint() const;

  /** LimitStateVariable accessor */
  RandomVector getLimitStateVariable() const;

  /** IsStandardPointOriginInFailureSpace  accessor */
  Bool getIsStandardPointOriginInFailureSpace() const;

  /** IsStandardPointOriginInFailureSpace  accessor */
  void setIsStandardPointOriginInFailureSpace(const Bool isStandardPointOriginInFailureSpace);

  /** Mean point conditioned to the standard event realization accessor */
  Point getMeanPointInStandardEventDomain() const;
  void setMeanPointInStandardEventDomain(const Point & meanPointInStandardEventDomain);

  /** ImportanceFactors accessor */
  PointWithDescription getImportanceFactors(const ImportanceFactorType type = ELLIPTICAL) const;

  /** ImportanceFactors graph */
  Graph drawImportanceFactors(const ImportanceFactorType type = ELLIPTICAL) const;

  /**  HasoferReliabilityIndex accessor */
  Scalar getHasoferReliabilityIndex() const;

  /**  HasoferReliabilityIndexSensitivity accessor */
  Sensitivity getHasoferReliabilityIndexSensitivity() const;

  /** Optimization result accessors */
  OptimizationResult getOptimizationResult() const;
  void setOptimizationResult(const OptimizationResult & optimizationResult);

  /** HasoferReliabilityIndexSensitivitygraph */
  GraphCollection drawHasoferReliabilityIndexSensitivity(Scalar width = ResourceMap::GetAsScalar( "AnalyticalResult-DefaultWidth" )) const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


  /** Sensitivitygraph */
  Graph drawSensitivity(const Sensitivity & sensitivity,
                        const Scalar width) const;

private:

  /** PhysicalSpaceDesignPoint evaluation */
  void computePhysicalSpaceDesignPoint() const;

  /** HasoferReliabilityIndex evaluation */
  void computeHasoferReliabilityIndex() const;

  /** ImportanceFactors evaluation */
  void computeImportanceFactors() const;

  /** ImportanceFactors evaluation */
  void computeClassicalImportanceFactors() const;

  /** Physical importance factors evaluation */
  void computePhysicalImportanceFactors() const;

  /** HasoferReliabilityIndexSensitivity evaluation */
  void computeHasoferReliabilityIndexSensitivity() const;

  /** MeanPointInStandardEventDomain evaluation */
  void computeMeanPointInStandardEventDomain() const;

  /** PhysicalSpaceDesignPoint accessor */
  void setPhysicalSpaceDesignPoint(const Point & physicalSpaceDesignPoint);

  /** HasoferReliabilityIndex accessor */
  void setHasoferReliabilityIndex(const Scalar & hasoferReliabilityIndex);

  /** Returns the position of the given (value, name) into the Point or the dimension of the Point if failed */
  UnsignedInteger computePosition(const String & marginalName,
                                  const String & marginalParameterName,
                                  const Description & parameterSetNames) const;

  Point    standardSpaceDesignPoint_;
  mutable Point    physicalSpaceDesignPoint_;
  RandomVector limitStateVariable_;
  Bool isStandardPointOriginInFailureSpace_;
  mutable Scalar hasoferReliabilityIndex_;
  mutable PointWithDescription importanceFactors_;
  mutable PointWithDescription classicalImportanceFactors_;
  mutable PointWithDescription physicalImportanceFactors_;
  mutable Sensitivity hasoferReliabilityIndexSensitivity_;
  mutable Point meanPointInStandardEventDomain_;
  mutable Bool isAlreadyComputedImportanceFactors_;
  mutable Bool isAlreadyComputedClassicalImportanceFactors_;
  mutable Bool isAlreadyComputedPhysicalImportanceFactors_;
  mutable Bool isAlreadyComputedHasoferReliabilityIndexSensitivity_;
  OptimizationResult optimizationResult_;
}; // class AnalyticalResult
END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ANALYTICALRESULT_HXX */
