
//                                               -*- C++ -*-
/**
 *  @brief A math expression parser
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/SymbolicParserImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SymbolicParserImplementation);

static const Factory<SymbolicParserImplementation> Factory_SymbolicParserImplementation;

/* Default constructor */
SymbolicParserImplementation::SymbolicParserImplementation()
  : PersistentObject()
  , inputVariablesNames_()
  , outputVariablesNames_()
  , formulas_()
{
  // Nothing to do
}

/** Constructor with parameter */
SymbolicParserImplementation::SymbolicParserImplementation(const Description & outputVariablesNames)
  : PersistentObject()
  , inputVariablesNames_()
  , outputVariablesNames_(outputVariablesNames)
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
  inputVariablesNames_ = inputVariablesNames;
}

void SymbolicParserImplementation::setImplicitOutputVariables(const Bool implicitOutputVariables)
{
  implicitOutputVariables_ = implicitOutputVariables;
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
Point SymbolicParserImplementation::operator()(const Point & inP) const
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
  adv.saveAttribute( "outputVariablesNames", outputVariablesNames_ );
  adv.saveAttribute( "implicitOutputVariables_", implicitOutputVariables_ );
  adv.saveAttribute( "formulas_", formulas_ );
}

/* Method load() reloads the object from the StorageManager */
void SymbolicParserImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "inputVariablesNames_", inputVariablesNames_ );
  adv.loadAttribute( "outputVariablesNames", outputVariablesNames_ );
  adv.loadAttribute( "implicitOutputVariables_", implicitOutputVariables_ );
  adv.loadAttribute( "formulas_", formulas_ );
}


END_NAMESPACE_OPENTURNS
