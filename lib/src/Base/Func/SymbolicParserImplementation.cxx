
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

#include "openturns/SymbolicParserImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

#include <regex>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SymbolicParserImplementation);

static const Factory<SymbolicParserImplementation> Factory_SymbolicParserImplementation;

/* Default constructor */
SymbolicParserImplementation::SymbolicParserImplementation()
  : PersistentObject()
  , inputVariablesNames_()
  , formulas_()
  , checkResult_(ResourceMap::GetAsBool("SymbolicParser-CheckResult"))
{
  // Nothing to do
}

/* Virtual constructor */
SymbolicParserImplementation * SymbolicParserImplementation::clone() const
{
  return new SymbolicParserImplementation(*this);
}

/* Variables accessor */
Description SymbolicParserImplementation::getVariables() const
{
  return inputVariablesNames_;
}

void SymbolicParserImplementation::setVariables(const Description & inputVariablesNames)
{
  const UnsignedInteger size = inputVariablesNames.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
    if (!std::regex_match(inputVariablesNames[i], std::regex("[a-zA-Z][0-9a-zA-Z_]*")))
      throw InvalidArgumentException(HERE) << "Invalid input variable: " << inputVariablesNames[i];
  inputVariablesNames_ = inputVariablesNames;
}

/* Formulas accessor */
Description SymbolicParserImplementation::getFormulas() const
{
  return formulas_;
}

void SymbolicParserImplementation::setFormulas(const Description & formulas)
{
  formulas_ = formulas;
}

/* Evaluation operator */
Point SymbolicParserImplementation::operator()(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In SymbolicParserImplementation::operator()(const Point & inP) const";
}

/* Evaluation operator */
Sample SymbolicParserImplementation::operator()(const Sample & inS) const
{
  const UnsignedInteger size(inS.getSize());
  Sample result(size, formulas_.getSize());
  for (UnsignedInteger i = 0; i < size; ++i)
    result[i] = (*this)(inS[i]);
  return result;
}

/* Method save() stores the object through the StorageManager */
void SymbolicParserImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "inputVariablesNames_", inputVariablesNames_ );
  adv.saveAttribute( "formulas_", formulas_ );
  adv.saveAttribute( "checkResult_", checkResult_ );
}

/* Method load() reloads the object from the StorageManager */
void SymbolicParserImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "inputVariablesNames_", inputVariablesNames_ );
  adv.loadAttribute( "formulas_", formulas_ );
  adv.loadAttribute( "checkResult_", checkResult_ );
}


END_NAMESPACE_OPENTURNS
