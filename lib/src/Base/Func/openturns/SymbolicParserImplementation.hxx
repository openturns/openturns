//                                               -*- C++ -*-
/**
 *  @brief A math expression parser
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

#ifndef OPENTURNS_SYMBOLICPARSERIMPLEMENTATION_HXX
#define OPENTURNS_SYMBOLICPARSERIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"


BEGIN_NAMESPACE_OPENTURNS


class OT_API SymbolicParserImplementation
  : public PersistentObject
{

  CLASSNAME

public:
  /** Default constructor */
  SymbolicParserImplementation();

  /** Virtual copy constructor */
  SymbolicParserImplementation * clone() const override;

  /** Variables accessor */
  virtual Description getVariables() const;
  virtual void setVariables(const Description & inputVariablesNames);

  /** Formulas accessor */
  virtual Description getFormulas() const;
  virtual void setFormulas(const Description & formulas);

  virtual Point operator()(const Point & inP) const;
  virtual Sample operator()(const Sample & inS) const;

  /** Invalid values check accessor */
  virtual void setCheckOutput(const Bool checkOutput);
  virtual Bool getCheckOutput() const;

  /* Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /* Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  Description inputVariablesNames_;
  Description formulas_;
  Bool checkOutput_ = true;
}; /* class SymbolicParserImplementation */

END_NAMESPACE_OPENTURNS

#endif // OPENTURNS_SYMBOLICPARSERIMPLEMENTATION_HXX
