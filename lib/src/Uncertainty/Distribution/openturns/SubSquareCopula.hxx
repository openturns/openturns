//                                               -*- C++ -*-
/**
 *  @brief A class that implements a SubSquare copula
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/CopulaImplementation.hxx"

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
  explicit SubSquareCopula(const Function & phi);


  /** Comparison operator */
  Bool operator ==(const SubSquareCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual SubSquareCopula * clone() const;

  /** Get one realization of the SubSquareCopula distribution */
  Point getRealization() const;

  /** Get the PDF of the SubSquareCopula distribution */
  using CopulaImplementation::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the SubSquareCopula distribution */
  using CopulaImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x,
                                        const Point & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x,
                                        const Point & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q,
      const Point & y) const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Phi accessors */
  void setPhi(const Function & phi);
  Function getPhi() const;

  /** Mass accessor */
  Scalar getMass();

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:


private:

  // Compute the mass associated to phi
  void computeMass() const;

  // SubSquare copula parameter
  Function phi_;

  // Flag to tell if phi is zero
  Bool nullPhi_;

  // Mass
  Scalar mass_;

}; /* class SubSquareCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SUBSQUARECOPULA_HXX */
