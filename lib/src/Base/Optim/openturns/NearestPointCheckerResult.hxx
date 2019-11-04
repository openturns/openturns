//                                               -*- C++ -*-
/**
 *  @brief NearestPointCheckerResult stores the test result
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
#ifndef OPENTURNS_NEARESTPOINTCHECKERRESULT_HXX
#define OPENTURNS_NEARESTPOINTCHECKERRESULT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Sample.hxx"
#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class NearestPointCheckerResult
 * NearestPointCheckerResult stores the test result
 */
class OT_API NearestPointCheckerResult:
  public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  NearestPointCheckerResult();

  /** Standard constructor */
  NearestPointCheckerResult(const Sample & verifyingConstraintPoints,
                            const Sample & verifyingConstraintValues,
                            const Sample & violatingConstraintPoints,
                            const Sample & violatingConstraintValues);


  /** Virtual constructor */
  virtual NearestPointCheckerResult * clone() const;

  /** Assigment operator */

  /** verifyingConstraintPoints accessor */
  Sample  getVerifyingConstraintPoints() const;
  /** verifyingConstraintValues accessor */
  Sample  getVerifyingConstraintValues() const;
  /** violatingConstraintPoints accessor */
  Sample  getViolatingConstraintPoints() const;
  /** violatingConstraintValues accessor */
  Sample  getViolatingConstraintValues() const;
  /** verifyingConstraintPoints accessor */
  void setVerifyingConstraintPoints(const Sample & verifyingConstraintPoints);

  /** verifyingConstraintValues accessor */
  void setVerifyingConstraintValues(const Sample & verifyingConstraintValues);

  /** violatingConstraintPoints accessor */
  void setViolatingConstraintPoints(const Sample & violatingConstraintPoints);

  /** violatingConstraintValuess accessor */
  void setViolatingConstraintValues(const Sample & violatingConstraintValues);

  /** String converter */
  String __repr__() const;

private:


  Sample verifyingConstraintPoints_;
  Sample verifyingConstraintValues_;
  Sample violatingConstraintPoints_;
  Sample violatingConstraintValues_;

}; // class NearestPointCheckerResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NEARESTPOINTCHECKER_HXX */
