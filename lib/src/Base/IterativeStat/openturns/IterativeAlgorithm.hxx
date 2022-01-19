//                                               -*- C++ -*-
/**
 *  @brief IterativeAlgorithm
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
#ifndef OPENTURNS_ITERATIVEALGORITHM_HXX
#define OPENTURNS_ITERATIVEALGORITHM_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/IterativeAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

class IterativeAlgorithmImplementation;

class OT_API IterativeAlgorithm
  : public TypedInterfaceObject<IterativeAlgorithmImplementation>
{
  CLASSNAME

public:

  typedef TypedInterfaceObject<IterativeAlgorithmImplementation>::Implementation Implementation;

  /**
   * Default constructor
   *
   * The constructor sets a new Id to the object,
   * so it can be later referenced by a Study object.
   * It is also declared visible if member of a study.
   *
   * The object has the default name but it does not
   * use storage for it.
   */
  IterativeAlgorithm();

  IterativeAlgorithm(const UnsignedInteger dimension);

  IterativeAlgorithm(const IterativeAlgorithmImplementation & implementation);

  IterativeAlgorithm(const Implementation & p_implementation);

#ifndef SWIG
  IterativeAlgorithm(IterativeAlgorithmImplementation * p_implementation);
#endif

  /**
   * Increment methods
   *
   * @warning These methods MUST be overloaded in derived classes.
   */

  virtual void increment(const Point & newData);

  virtual void increment(const Sample & newData);

  /**
   * Iteration accessor
   *
   * This method returns the current iteration number of the algorithm.
   */
  UnsignedInteger getIteration() const;

  /**
   * Dimension accessor
   *
   * This method returns the dimension of the object.
   */
  UnsignedInteger getDimension() const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

}; /* class IterativeAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATIVEALGORITHM_HXX */
