//                                               -*- C++ -*-
/**
 *  @brief NearestPointChecker checks wether the nearest point found is really the nearestpoint
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
#ifndef OPENTURNS_NEARESTPOINTCHECKER_HXX
#define OPENTURNS_NEARESTPOINTCHECKER_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/ComparisonOperator.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/NearestPointCheckerResult.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class NearestPointChecker
 *  checks wether the nearest point found is really the nearestpoint
 *
 */

class OT_API NearestPointChecker
  : public PersistentObject
{
  CLASSNAME;
public:


  /** Constructor with parameters */
  NearestPointChecker(const NumericalMathFunction & levelFunction,
                      const ComparisonOperator & comparisonOperator,
                      const NumericalScalar threshold,
                      const NumericalSample & sample);


  /** Virtual constructor */
  virtual NearestPointChecker * clone() const;

  /** levelFunction  accessor */
  void setLevelFunction(const NumericalMathFunction & levelFunction);

  /** levelFunction accessor */
  NumericalMathFunction getLevelFunction() const;

  /** Result accessor */
  void setResult(const NearestPointCheckerResult & result );

  /** Result accessor */
  NearestPointCheckerResult getResult() const;

  /** Comparison operator accessor */
  void setComparisonOperator(const ComparisonOperator & comparisonOperator );

  /** Comparison operator accessor */
  ComparisonOperator getComparisonOperator() const;

  /** threshold accessor */
  void setThreshold(const NumericalScalar threshold);

  /** threshold accessor */
  NumericalScalar getThreshold() const;

  /** sample accessor */
  void setSample(const NumericalSample & sample);

  /** sample accessor */
  const NumericalSample & getSample() const;

  /** Performs the actual test */
  void  run();

  /** String converter */
  String __repr__() const;

private:

  NumericalMathFunction levelFunction_;
  NearestPointCheckerResult result_;
  ComparisonOperator comparisonOperator_;
  NumericalScalar threshold_;
  NumericalSample sample_;
} ; /* class NearestPointChecker */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NEARESTPOINTCHECKER_HXX */
