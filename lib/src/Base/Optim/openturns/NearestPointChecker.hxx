//                                               -*- C++ -*-
/**
 *  @brief NearestPointChecker checks whether the nearest point found is really the nearestpoint
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
#ifndef OPENTURNS_NEARESTPOINTCHECKER_HXX
#define OPENTURNS_NEARESTPOINTCHECKER_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Function.hxx"
#include "openturns/Sample.hxx"
#include "openturns/ComparisonOperator.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/NearestPointCheckerResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class NearestPointChecker
 *  checks whether the nearest point found is really the nearestpoint
 *
 */

class OT_API NearestPointChecker
  : public PersistentObject
{
  CLASSNAME
public:


  /** Constructor with parameters */
  NearestPointChecker(const Function & levelFunction,
                      const ComparisonOperator & comparisonOperator,
                      const Scalar threshold,
                      const Sample & sample);


  /** Virtual constructor */
  NearestPointChecker * clone() const override;

  /** levelFunction  accessor */
  void setLevelFunction(const Function & levelFunction);

  /** levelFunction accessor */
  Function getLevelFunction() const;

  /** Result accessor */
  void setResult(const NearestPointCheckerResult & result );

  /** Result accessor */
  NearestPointCheckerResult getResult() const;

  /** Comparison operator accessor */
  void setComparisonOperator(const ComparisonOperator & comparisonOperator );

  /** Comparison operator accessor */
  ComparisonOperator getComparisonOperator() const;

  /** threshold accessor */
  void setThreshold(const Scalar threshold);

  /** threshold accessor */
  Scalar getThreshold() const;

  /** sample accessor */
  void setSample(const Sample & sample);

  /** sample accessor */
  Sample getSample() const;

  /** Performs the actual test */
  void run();

  /** String converter */
  String __repr__() const override;

private:

  Function levelFunction_;
  NearestPointCheckerResult result_;
  ComparisonOperator comparisonOperator_;
  Scalar threshold_;
  Sample sample_;
} ; /* class NearestPointChecker */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NEARESTPOINTCHECKER_HXX */
