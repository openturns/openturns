//                                               -*- C++ -*-
/**
 * @brief Class for unavailable evaluation implementations
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_NOEVALUATION_HXX
#define OPENTURNS_NOEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class NoEvaluation
 *
 * Class for unavailable evaluation implementations
 */
class OT_API NoEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:



  /** Default constructor */
  NoEvaluation();

  /** Virtual constructor */
  virtual NoEvaluation * clone() const;

  /** Comparison operator */
  Bool operator ==(const NoEvaluation & other) const;

  /** String converter */
  virtual String __repr__() const;


  /** Test for actual implementation */
  virtual Bool isActualImplementation() const;


  /* Here is the interface that all derived class must implement */

  /** Operator () */
  virtual Point operator() (const Point & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager
   *  The inherited method is sufficient as we do not have any
   *  attribute
   */

  /** Method load() reloads the object from the StorageManager
   *  The inherited method is sufficient as we do not have any
   *  attribute
   */

protected:


private:

}; /* class NoEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NONOEVALUATION_HXX */
