//                                               -*- C++ -*-
/**
 *  @brief A class that implements a SubSquare copula
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
#ifndef OPENTURNS_SUBSQUARECOPULA_HXX
#define OPENTURNS_SUBSQUARECOPULA_HXX

#include "CopulaImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SubSquareCopula
 *
 * The class implements a subSquare copula
 */
class OT_API SubSquareCopula
  : public CopulaImplementation
{
  CLASSNAME;

public:

  /** Default constructor */
  SubSquareCopula();

  /** Default constructor */
  explicit SubSquareCopula(const NumericalMathFunction & phi);


  /** Comparison operator */
  Bool operator ==(const SubSquareCopula & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual SubSquareCopula * clone() const;

  /** Get one realization of the SubSquareCopula distribution */
  NumericalPoint getRealization() const;

  /** Get the PDF of the SubSquareCopula distribution */
  using CopulaImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the SubSquareCopula distribution */
  using CopulaImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  NumericalScalar computeConditionalPDF(const NumericalScalar x,
                                        const NumericalPoint & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  NumericalScalar computeConditionalCDF(const NumericalScalar x,
                                        const NumericalPoint & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  NumericalScalar computeConditionalQuantile(const NumericalScalar q,
      const NumericalPoint & y) const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Phi accessors */
  void setPhi(const NumericalMathFunction & phi);
  NumericalMathFunction getPhi() const;

  /** Mass accessor */
  NumericalScalar getMass();

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using CopulaImplementation::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & parametersCollection);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:


private:

  // Compute the mass associated to phi
  void computeMass() const;

  // SubSquare copula parameter
  NumericalMathFunction phi_;

  // Flag to tell if phi is zero
  Bool nullPhi_;

  // Mass
  NumericalScalar mass_;

}; /* class SubSquareCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SUBSQUARECOPULA_HXX */
