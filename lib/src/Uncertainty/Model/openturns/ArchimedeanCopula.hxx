//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for non elliptical distributions
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
#ifndef OPENTURNS_ARCHIMEDEANCOPULA_HXX
#define OPENTURNS_ARCHIMEDEANCOPULA_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/CopulaImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ArchimedeanCopula
 *
 * All traditionnal distribution inherits from this class.
 * Classes derived from ArchimedeanCopula are known by themselves,
 * without being reconstructed or built in any way.
 */
class OT_API ArchimedeanCopula
  : public CopulaImplementation
{
  CLASSNAME;
public:

  /** Default constructor */
  ArchimedeanCopula();


  /** Virtual constructor */
  virtual ArchimedeanCopula * clone() const;

  /** Comparison operator */
  Bool operator ==(const ArchimedeanCopula & other) const;

  /** String converter */
  String __repr__() const;

  /** Get the PDF of the archimedean copula */
  using CopulaImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the archimedean copula */
  using CopulaImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;
  using CopulaImplementation::computeComplementaryCDF;
  NumericalScalar computeComplementaryCDF(const NumericalPoint & point) const;

  /** Get the probability content of an interval */
  NumericalScalar computeProbability(const Interval & interval) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  NumericalScalar computeConditionalPDF(const NumericalScalar x,
                                        const NumericalPoint & y) const;

  /** Compute the archimedean generator of the archimedean copula, i.e.
   *  the function phi such that the CDF of the copula can
   *  be written as CDF(t) = phi^{-1}(phi(u)+phi(v))
   */
  virtual NumericalScalar computeArchimedeanGenerator(const NumericalScalar t) const;

  /** Compute the inverse of the archimedean generator */
  virtual NumericalScalar computeInverseArchimedeanGenerator(const NumericalScalar t) const;

  /** Compute the derivative of the archimedean generator */
  virtual NumericalScalar computeArchimedeanGeneratorDerivative(const NumericalScalar t) const;

  /** Compute the second derivative of the archimedean generator */
  virtual NumericalScalar computeArchimedeanGeneratorSecondDerivative(const NumericalScalar t) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using CopulaImplementation::getMarginal;
  Implementation getMarginal(const Indices & indices) const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

protected:


private:


}; /* class ArchimedeanCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ARCHIMEDEANCOPULA_HXX */
