//                                               -*- C++ -*-
/**
 *  @brief A math expression parser
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

#ifndef OPENTURNS_SYMBOLICPARSEREXPRTK_HXX
#define OPENTURNS_SYMBOLICPARSEREXPRTK_HXX

#include "openturns/SymbolicParserImplementation.hxx"
#include "openturns/Pointer.hxx"

// Forward declaration
namespace exprtk
{
template<typename T> class expression;
}

BEGIN_NAMESPACE_OPENTURNS

class SymbolicParserExprTk
  : public SymbolicParserImplementation
{

  CLASSNAME

public:
  /** Default constructor */
  SymbolicParserExprTk();

  /** Constructor with parameter */
  explicit SymbolicParserExprTk(const Description & outputVariablesNames);

  /** Virtual copy constructor */
  virtual SymbolicParserExprTk * clone() const;

  Point operator()(const Point & inP) const;
  Sample operator()(const Sample & inS) const;

  /* Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /* Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  void initialize() const;

  mutable Collection< Pointer< exprtk::expression<Scalar> > > expressions_;
  mutable Point inputStack_;
  Description outputVariablesNames_;

};

END_NAMESPACE_OPENTURNS

#endif // OPENTURNS_SYMBOLICPARSEREXPRTK_HXX
