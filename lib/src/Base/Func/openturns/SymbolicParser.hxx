//                                               -*- C++ -*-
/**
 *  @brief A math expression parser
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

#ifndef OPENTURNS_SYMBOLICPARSER_HXX
#define OPENTURNS_SYMBOLICPARSER_HXX

#include "openturns/SymbolicParserImplementation.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"


BEGIN_NAMESPACE_OPENTURNS


class OT_API SymbolicParser
  : public TypedInterfaceObject<SymbolicParserImplementation>
{
  CLASSNAME

public:
  typedef Pointer<SymbolicParserImplementation> Implementation;

  /** Default constructor */
  SymbolicParser();

  /** Constructor with parameter */
  explicit SymbolicParser(const Description & outputVariablesNames);

  /** Constructor with parameters */
  SymbolicParser(const SymbolicParserImplementation & implementation);

  /** Constructor with parameters */
  SymbolicParser(Implementation & p_implementation);

  /** Variables accessors */
  Description getVariables() const;
  void setVariables(const Description & inputVariablesNames);

  /** Formulas accessors */
  Description getFormulas() const;
  void setFormulas(const Description & formulas);

  Point operator()(const Point & inP) const;
  Sample operator()(const Sample & inS) const;

  /** Invalid values check accessor */
  void setCheckOutput(const Bool checkOutput);
  Bool getCheckOutput() const;
};

END_NAMESPACE_OPENTURNS

#endif // OPENTURNS_SYMBOLICPARSER_HXX
