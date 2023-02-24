//                                               -*- C++ -*-
/**
 *  @brief IterativeExtrema
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
#ifndef OPENTURNS_ITERATIVEEXTREMA_HXX
#define OPENTURNS_ITERATIVEEXTREMA_HXX

#include "openturns/IterativeAlgorithmImplementation.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API IterativeExtrema
  : public IterativeAlgorithmImplementation
{
  CLASSNAME

public:
  /** Constructors */
  explicit IterativeExtrema(const UnsignedInteger dimension = 1);

  /**Virtual constructor */
  IterativeExtrema * clone() const override;

  /** Increment method from a Point */
  void increment(const Point & newData) override;

  /** Increment method from a Sample */
  void increment(const Sample & newData) override;

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  using IterativeAlgorithmImplementation::operator ==;
  inline
  Bool operator ==(const IterativeExtrema & other) const
  {
    return (dimension_ == other.dimension_) && (minData_ == other.minData_) && (maxData_ == other.maxData_);
  }

  /**
   * Comparison operator
   *
   * This method compares objects based on their content.
   */
  using IterativeAlgorithmImplementation::operator !=;
  inline
  Bool operator !=(const IterativeExtrema & other) const
  {
    return !operator==(other);
  }

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String &) const override;

  /** Accessor to the min */
  Point getMin() const;

  /** Accessor to the max */
  Point getMax() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Point minData_;
  Point maxData_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATIVEEXTREMA_HXX */
