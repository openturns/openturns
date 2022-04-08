//                                               -*- C++ -*-
/**
 *  @brief A math expression parser
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_SYMBOLICPARSERMUPARSER_HXX
#define OPENTURNS_SYMBOLICPARSERMUPARSER_HXX

#include "openturns/SymbolicParserImplementation.hxx"
#include "openturns/Pointer.hxx"

BEGIN_NAMESPACE_OPENTURNS

class MuParser;

class SymbolicParserMuParser
  : public SymbolicParserImplementation
{

  CLASSNAME
friend struct SymbolicParserMuParserPolicy;
public:
  /** Default constructor */
  SymbolicParserMuParser();

  /** Virtual copy constructor */
  SymbolicParserMuParser * clone() const override;

  Point operator()(const Point & inP) const override;
  Sample operator()(const Sample & inS) const override;

private:
  void initialize() const;

  Collection<Pointer<MuParser > > allocateExpressions(Point & stack) const;

  mutable Collection<Pointer<MuParser> > expressions_;
  mutable Point stack_;

  // one expression per thread for batch evaluation
  typedef Collection< Pointer< MuParser > > ExpressionCollection;
  mutable Collection<ExpressionCollection> threadExpressions_;
  mutable Collection<Point> threadStack_;

  UnsignedInteger smallSize_ = 0;
};

END_NAMESPACE_OPENTURNS

#endif // OPENTURNS_SYMBOLICPARSERMUPARSER_HXX
