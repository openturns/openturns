//                                               -*- C++ -*-
/**
 *  @brief A math expression parser
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
  friend struct SymbolicParserExprTkPolicy;
public:
  /** Default constructor */
  SymbolicParserExprTk();

  /** Constructor with parameter */
  explicit SymbolicParserExprTk(const Description & outputVariablesNames);

  /** Virtual copy constructor */
  SymbolicParserExprTk * clone() const override;

  Point operator()(const Point & inP) const override;
  Sample operator()(const Sample & inS) const override;

  /* Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /* Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  void initialize() const;

  typedef Collection< Pointer< exprtk::expression<Scalar> > > ExpressionCollection;
  ExpressionCollection allocateExpressions(Point & stack) const;

  // expression for single evaluation
  mutable Collection< Pointer< exprtk::expression<Scalar> > > expressions_;
  mutable Point stack_;

  // one expression per thread for batch evaluation
  mutable Collection<ExpressionCollection> threadExpressions_;
  mutable Collection<Point> threadStack_;

  Description outputVariablesNames_;

  UnsignedInteger smallSize_ = 0;
};

END_NAMESPACE_OPENTURNS

#endif // OPENTURNS_SYMBOLICPARSEREXPRTK_HXX
