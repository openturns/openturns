//                                               -*- C++ -*-
/**
 *  @brief NearestPointCheckerResult stores the test result
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_NEARESTPOINTCHECKERRESULT_HXX
#define OPENTURNS_NEARESTPOINTCHECKERRESULT_HXX

#include "OTprivate.hxx"
#include "NumericalSample.hxx"
#include "PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class NearestPointCheckerResult
 * NearestPointCheckerResult stores the test result
 */
class OT_API NearestPointCheckerResult:
  public PersistentObject
{
  CLASSNAME;
public:

  /** Default constructor */
  NearestPointCheckerResult();

  /** Standard constructor */
  NearestPointCheckerResult(const NumericalSample & verifyingConstraintPoints,
                            const NumericalSample & verifyingConstraintValues,
                            const NumericalSample & violatingConstraintPoints,
                            const NumericalSample & violatingConstraintValues);


  /** Virtual constructor */
  virtual NearestPointCheckerResult * clone() const;

  /** Assigment operator */

  /** verifyingConstraintPoints accessor */
  NumericalSample  getVerifyingConstraintPoints() const;
  /** verifyingConstraintValues accessor */
  NumericalSample  getVerifyingConstraintValues() const;
  /** violatingConstraintPoints accessor */
  NumericalSample  getViolatingConstraintPoints() const;
  /** violatingConstraintValues accessor */
  NumericalSample  getViolatingConstraintValues() const;
  /** verifyingConstraintPoints accessor */
  void setVerifyingConstraintPoints(const NumericalSample & verifyingConstraintPoints);

  /** verifyingConstraintValues accessor */
  void setVerifyingConstraintValues(const NumericalSample & verifyingConstraintValues);

  /** violatingConstraintPoints accessor */
  void setViolatingConstraintPoints(const NumericalSample & violatingConstraintPoints);

  /** violatingConstraintValuess accessor */
  void setViolatingConstraintValues(const NumericalSample & violatingConstraintValues);

  /** String converter */
  String __repr__() const;

private:


  NumericalSample verifyingConstraintPoints_;
  NumericalSample verifyingConstraintValues_;
  NumericalSample violatingConstraintPoints_;
  NumericalSample violatingConstraintValues_;

}; // class NearestPointCheckerResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NEARESTPOINTCHECKER_HXX */
