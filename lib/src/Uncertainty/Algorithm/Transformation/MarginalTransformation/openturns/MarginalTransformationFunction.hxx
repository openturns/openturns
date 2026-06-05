//                                               -*- C++ -*-
/**
 *  @brief Marginal transformation function
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MARGINALTRANSFORMATIONFUNCTION_HXX
#define OPENTURNS_MARGINALTRANSFORMATIONFUNCTION_HXX

#include "openturns/Function.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MarginalTransformationFunction
 *
 */
class OT_API MarginalTransformationFunction
  : public Function
{
  CLASSNAME
public:

  typedef Collection<Distribution>              DistributionCollection;

  /** Default constructor */
  MarginalTransformationFunction();

  /** Parameter constructor */
  MarginalTransformationFunction(const DistributionCollection & inputDistributionCollection,
                                 const DistributionCollection & outputDistributionCollection);

  /** Comparison operator */
  using Function::operator ==;
  Bool operator ==(const MarginalTransformationFunction & other) const;

  /** Input distribution collection accessor */
  DistributionCollection getInputDistributionCollection() const;

  /** Output distribution collection accessor */
  DistributionCollection getOutputDistributionCollection() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

private:

  /** Build the function */
  static Function Build(const DistributionCollection & inputDistributionCollection,
                        const DistributionCollection & outputDistributionCollection);

  // Marginal distributions of the input
  DistributionCollection inputDistributionCollection_;

  // Marginal distributions of the output
  DistributionCollection outputDistributionCollection_;

}; /* class MarginalTransformationFunction */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MARGINALTRANSFORMATIONFUNCTION_HXX */
