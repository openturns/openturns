//                                               -*- C++ -*-
/**
 *  @brief A math expression parser
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

#ifndef OPENTURNS_ANALYTICALPARSER_HXX
#define OPENTURNS_ANALYTICALPARSER_HXX

#include "openturns/NumericalSample.hxx"


BEGIN_NAMESPACE_OPENTURNS


class MuParser;

class AnalyticalParser : public Object
{
public:
  /** Default constructor */
  AnalyticalParser();

  NumericalPoint operator()(const NumericalPoint & inP) const;

  void setVariablesFormulas(const Description & inputVariablesNames,
                            const Description & formulas);

  Description getFormulas() const;

protected:
  void initialize() const;

  Description inputVariablesNames_;
  Description formulas_;
  mutable Collection<Pointer<MuParser> > parsers_;
  mutable NumericalPoint inputStack_;

};

END_NAMESPACE_OPENTURNS

#endif // OPENTURNS_ANALYTICALPARSER_HXX
