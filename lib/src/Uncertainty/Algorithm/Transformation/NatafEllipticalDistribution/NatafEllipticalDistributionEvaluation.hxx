//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformation evaluation for elliptical
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
#ifndef OPENTURNS_NATAFELLIPTICALDISTRIBUTIONEVALUATION_HXX
#define OPENTURNS_NATAFELLIPTICALDISTRIBUTIONEVALUATION_HXX

#include "OTprivate.hxx"
#include "LinearNumericalMathEvaluationImplementation.hxx"
#include "Matrix.hxx"
#include "NumericalPoint.hxx"
#include "StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NatafEllipticalDistributionEvaluation
 *
 * This class offers an interface for the Nataf function for elliptical distributions
 */
class OT_API NatafEllipticalDistributionEvaluation
  : public LinearNumericalMathEvaluationImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  NatafEllipticalDistributionEvaluation();

  /** Parameter constructor */
  NatafEllipticalDistributionEvaluation(const NumericalPoint & mean,
                                        const TriangularMatrix & inverseCholesky);

  /** Virtual constructor */
  virtual NatafEllipticalDistributionEvaluation * clone() const;

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const NumericalPoint & inP) const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:

}; /* NatafEllipticalDistributionEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NATAFELLIPTICALDISTRIBUTIONEVALUATION_HXX */
