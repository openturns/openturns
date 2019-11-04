//                                               -*- C++ -*-
/**
 *  @brief Isoprobabilistic transformation
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
#ifndef OPENTURNS_DISTRIBUTIONTRANSFORMATION_HXX
#define OPENTURNS_DISTRIBUTIONTRANSFORMATION_HXX

#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class DistributionTransformation
 *
 */
class OT_API DistributionTransformation
  : public Function
{
  CLASSNAME
public:

  /* Default constructor */
  DistributionTransformation();

  /** Parameter constructor */
  DistributionTransformation (const Distribution & left,
                              const Distribution & right);

  /** Comparison operator */
  Bool operator ==(const DistributionTransformation & other) const;

  DistributionTransformation inverse() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

private:
  static Function Build(const Distribution & left,
                        const Distribution & right);

  Distribution left_;
  Distribution right_;

}; /* class DistributionTransformation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTRIBUTIONTRANSFORMATION_HXX */
