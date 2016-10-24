//                                               -*- C++ -*-
/**
 *  @brief Design matrix cached evaluation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_DESIGNPROXY_HXX
#define OPENTURNS_DESIGNPROXY_HXX

#include "openturns/NumericalSample.hxx"
#include "openturns/Basis.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Basis.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class DesignProxy
 *
 * Design matrix cached evaluation
 */
class OT_API DesignProxy
  : public Object
{
  CLASSNAME;
public:

  /** Default constructor */
  DesignProxy();

  /** Parameters constructor */
  DesignProxy(const NumericalSample & x,
              const Basis & basis);

  /** Virtual constructor */
  virtual DesignProxy * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Accessors */
  NumericalSample getInputSample() const;
  Basis getBasis() const;

  /** Compute the design matrix with the provided basis terms indices */
  MatrixImplementation computeDesign(const Indices & indices) const;

  /** Row filter accessor */
  void setRowFilter(const Indices & rowFilter);
  Indices getRowFilter() const;

  /** Row filter flag accessor */
  Bool hasRowFilter() const;

  /** Weight accessor */
  void setWeight(const NumericalPoint & weight);
  NumericalPoint getWeight() const;

  /** Weight flag accessor */
  Bool hasWeight() const;

protected:
  void initialize();

  /** Input sample */
  NumericalSample x_;

  /** Basis */
  Basis basis_;

  /** Design matrix cache */
  mutable MatrixImplementation designCache_;

  /** Flags to tell if the column has already been computed */
  mutable Indices alreadyComputed_;

  /** Filter on the rows of the design matrix */
  Indices rowFilter_;

  /** Weight on each basis function */
  NumericalPoint weight_;

}; /* class DesignProxy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DESIGNPROXY_HXX */
