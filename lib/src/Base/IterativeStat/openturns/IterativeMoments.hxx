//                                               -*- C++ -*-
/**
 *  @brief IterativeMoments
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
#ifndef OPENTURNS_ITERATIVEMOMENTS_HXX
#define OPENTURNS_ITERATIVEMOMENTS_HXX

#include "openturns/IterativeAlgorithmImplementation.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API IterativeMoments
  : public IterativeAlgorithmImplementation
{
  CLASSNAME

public:

  /** Constructor */
  explicit IterativeMoments(const UnsignedInteger orderMax = 2, const UnsignedInteger dimension = 1);

  /** Virtual constructor */
  IterativeMoments * clone() const override;

  /** Increment method from a Point */
  void increment(const Point & newData) override;

  /** Increment method from a Sample */
  void increment(const Sample & newData) override;

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator ==(const IterativeMoments & /*other*/) const
  {
    return true;
  }

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator !=(const IterativeMoments & other) const
  {
    return !operator==(other);
  }

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Accessor to the mean */
  Point getMean() const;

  /** Accessor to the variance */
  Point getVariance() const;

  /** Accessor to the skewness */
  Point getSkewness() const;

  /** Accessor to the kurtosis */
  Point getKurtosis() const;

  /** Accessor to the centered moments */
  Sample getCenteredMoments() const;

  /** Get the actual order */
  UnsignedInteger getOrder() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Accessor to the standard error of the mean */
  Point getStandardErrorOfTheMean() const;

  /** Accessor to the coefficient of variation */
  Point getCoefficientOfVariation() const;

  /** Accessor to the standard deviation */
  Point getStandardDeviation() const;


private:
  void updateHigherMoments(UnsignedInteger orderMax, const Point & delta, const Point & delta_over_n );


private:
  UnsignedInteger orderMax_;
  Sample centeredMoments_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATIVEKURTOSIS_HXX */
