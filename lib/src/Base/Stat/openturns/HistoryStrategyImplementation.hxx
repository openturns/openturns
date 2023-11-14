//                                               -*- C++ -*-
/**
 *  @brief This class is a top-level class for the history mechanism of
 *  simulation algorithms. It delegates to its children the effective
 *  history strategy.
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
#ifndef OPENTURNS_HISTORYSTRATEGYIMPLEMENTATION_HXX
#define OPENTURNS_HISTORYSTRATEGYIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class HistoryStrategyImplementation
 */

class OT_API HistoryStrategyImplementation
  : public PersistentObject
{

  CLASSNAME

public:


  /** Constructor with parameters */
  HistoryStrategyImplementation();

  /** Virtual constructor */
  HistoryStrategyImplementation * clone() const override;

  /** Store the point according to the strategy */
  virtual void store(const Point & point);
  virtual void store(const Sample & sample);

  /** Declare dimension of Point stored */
  virtual void setDimension(const UnsignedInteger dimension);

  /** Clear the history storage */
  virtual void clear();

  /** History sample accessor */
  virtual Sample getSample() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Data container */
  Sample sample_;

  /** Initialization flag */
  Bool isInitialized_;
} ; /* class HistoryStrategyImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HISTORYSTRATEGYIMPLEMENTATION_HXX */
