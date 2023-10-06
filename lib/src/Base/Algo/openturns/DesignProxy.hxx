//                                               -*- C++ -*-
/**
 *  @brief Design matrix cached evaluation
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DESIGNPROXY_HXX
#define OPENTURNS_DESIGNPROXY_HXX

#include "openturns/Sample.hxx"
#include "openturns/Basis.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/Function.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class DesignProxy
 *
 * Design matrix cached evaluation
 */
class OT_API DesignProxy
  : public PersistentObject
{
  CLASSNAME
public:
  typedef Collection<Function> FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  /** Default constructor */
  DesignProxy();

  /** Parameters constructor */
  DesignProxy(const Sample & x,
              const FunctionCollection & basis);

  /** Parameters constructor */
  explicit DesignProxy(const Matrix & matrix);

  /** Virtual constructor */
  DesignProxy * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Accessors */
  Sample getInputSample() const;
  FunctionCollection getBasis() const;
  FunctionCollection getBasis(const Indices & indices) const;

  /** Compute the design matrix with the provided basis terms indices */
  Matrix computeDesign(const Indices & indices) const;

  /** Row filter accessor */
  void setRowFilter(const Indices & rowFilter);
  Indices getRowFilter() const;

  /** Row filter flag accessor */
  Bool hasRowFilter() const;

  /** Effective size of sample */
  UnsignedInteger getSampleSize() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  void initialize() const;

  /** Input sample */
  Sample x_;

  /** Basis */
  FunctionPersistentCollection basis_;

  /** Design matrix cache */
  mutable Matrix designCache_;

  /** Flags to tell if the column has already been computed */
  mutable Indices alreadyComputed_;

  /** Filter on the rows of the design matrix */
  Indices rowFilter_;

}; /* class DesignProxy */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DESIGNPROXY_HXX */
