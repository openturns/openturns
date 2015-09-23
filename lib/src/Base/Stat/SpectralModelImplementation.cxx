//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "SpectralModelImplementation.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"
#include "Log.hxx"
#include "Curve.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SpectralModelImplementation);
static const Factory<SpectralModelImplementation> RegisteredFactory;

/* Constructor with parameters */
SpectralModelImplementation::SpectralModelImplementation()
  : PersistentObject(),
    dimension_(0),
    frequencyGrid_()
{
  // Nothing to do
}

/* Virtual constructor */
SpectralModelImplementation * SpectralModelImplementation::clone() const
{
  return new SpectralModelImplementation(*this);
}

/* Dimension accessor */
UnsignedInteger SpectralModelImplementation::getDimension() const
{
  return dimension_;
}

/* Dimension accessor */
void SpectralModelImplementation::setDimension(const UnsignedInteger dimension)
{
  dimension_ = dimension;
}

/* Frequency grid accessors */
RegularGrid SpectralModelImplementation::getFrequencyGrid() const
{
  return frequencyGrid_;
}

void SpectralModelImplementation::setFrequencyGrid(const RegularGrid & frequencyGrid)
{
  frequencyGrid_ = frequencyGrid;
}

/* Computation of the spectral density function */
HermitianMatrix SpectralModelImplementation::operator() (const NumericalScalar frequency) const
{
  throw NotYetImplementedException(HERE) << "In SpectralModelImplementation::operator() (const NumericalScalar frequency) const";
}


/* String converter */
String SpectralModelImplementation::__repr__() const
{
  OSS oss(true);
  oss << "class=" << SpectralModelImplementation::GetClassName();
  return oss;
}

/* String converter */
String SpectralModelImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << "class=" << SpectralModelImplementation::GetClassName();
  return oss;
}

/* Drawing method */
Graph SpectralModelImplementation::draw(const UnsignedInteger rowIndex,
                                        const UnsignedInteger columnIndex,
                                        const Bool module) const
{
  if (rowIndex >= dimension_) throw InvalidArgumentException(HERE) << "Error: the given row index must be less than " << dimension_ << ", here rowIndex=" << rowIndex;
  if (columnIndex >= dimension_) throw InvalidArgumentException(HERE) << "Error: the given column index must be less than " << dimension_ << ", here columnIndex=" << columnIndex;
  const UnsignedInteger n(frequencyGrid_.getN());
  const NumericalScalar fMin(frequencyGrid_.getStart());
  const NumericalScalar fStep(frequencyGrid_.getStep());
  NumericalSample data(n, 2);
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    const NumericalScalar f(fMin + i * fStep);
    const NumericalComplex value((*this)(f)(rowIndex, columnIndex));
    data[i][0] = f;
    if (module) data[i][1] = std::abs(value);
    else data[i][1] = std::arg(value);
  }
  Graph graph(getName(), "f", String("spectral density") + (module ? String("module") : String("phase")), true, "topright");
  Curve curve(data);
  curve.setLineWidth(2);
  if (module) curve.setColor("red");
  else curve.setColor("blue");
  graph.add(curve);
  return graph;
}

/* Method save() stores the object through the StorageManager */
void SpectralModelImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute("frequencyGrid_", frequencyGrid_);
}

/* Method load() reloads the object from the StorageManager */
void SpectralModelImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("dimension_", dimension_);
  adv.loadAttribute("frequencyGrid_", frequencyGrid_);
}

END_NAMESPACE_OPENTURNS
