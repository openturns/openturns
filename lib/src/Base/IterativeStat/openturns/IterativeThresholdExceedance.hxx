//                                               -*- C++ -*-
/**
 *  @brief IterativeThresholdExceedance
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
#ifndef OPENTURNS_ITERATIVETHRESHOLDEXCEEDANCE_HXX
#define OPENTURNS_ITERATIVETHRESHOLDEXCEEDANCE_HXX

#include "openturns/IterativeAlgorithmImplementation.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API IterativeThresholdExceedance
  : public IterativeAlgorithmImplementation
{
  CLASSNAME

public:

  /** Constructor */
  explicit IterativeThresholdExceedance(const UnsignedInteger dimension = 1, const Scalar thresholdValue = 0.0);

  /** Virtual constructor */
  IterativeThresholdExceedance * clone() const override;

  /** Increment method from a Point */
  virtual void increment(const Point & newData) override;

  /** Increment method from a Sample */
  virtual void increment(const Sample & newData) override;

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator ==(const IterativeThresholdExceedance & /*other*/) const
  {
    return true;
  }

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  inline
  Bool operator !=(const IterativeThresholdExceedance & other) const
  {
    return !operator==(other);
  }

  /** String converter */
  virtual String __repr__() const override;

  /** String converter */
  virtual String __str__(const String & offset = "") const override;

  /** Accessor to the threshold value */
  Scalar getThresholdValue() const;

  /** Accessor to the value exceeding the threshold value */
  Point getThresholdExceedance() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv) override;

private:
  Scalar thresholdValue_;
  Point data_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATIVETHRESHOLDEXCEEDANCE_HXX */
