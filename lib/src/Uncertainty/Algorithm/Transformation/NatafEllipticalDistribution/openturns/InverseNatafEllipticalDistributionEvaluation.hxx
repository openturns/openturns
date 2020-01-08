//                                               -*- C++ -*-
/**
 *  @brief Class for the InverseNataf transformation evaluation for elliptical
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_INVERSENATAFELLIPTICALDISTRIBUTIONEVALUATION_HXX
#define OPENTURNS_INVERSENATAFELLIPTICALDISTRIBUTIONEVALUATION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/LinearEvaluation.hxx"
#include "openturns/TriangularMatrix.hxx"
#include "openturns/Point.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class InverseNatafEllipticalDistributionEvaluation
 *
 * This class offers an interface for the InverseNataf function for elliptical distributions
 */
class OT_API InverseNatafEllipticalDistributionEvaluation
  : public LinearEvaluation
{
  CLASSNAME
public:


  /** Default constructor */
  InverseNatafEllipticalDistributionEvaluation();

  /** Parameter constructor */
  InverseNatafEllipticalDistributionEvaluation(const Point & mean,
      const TriangularMatrix & cholesky);

  /** Virtual constructor */
  virtual InverseNatafEllipticalDistributionEvaluation * clone() const;

  /** Gradient according to the marginal parameters */
  virtual Matrix parameterGradient(const Point & inP) const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:

}; /* InverseNatafEllipticalDistributionEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INVERSENATAFELLIPTICALDISTRIBUTIONEVALUATION_HXX */
