//                                               -*- C++ -*-
/**
 *  @brief Fourier function factory
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
#include "openturns/FourierSeriesFactory.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/FourierSeries.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(FourierSeriesFactory)

static const Factory<FourierSeriesFactory> Factory_FourierSeriesFactory;


/* Default constructor */
FourierSeriesFactory::FourierSeriesFactory()
  : OrthogonalUniVariateFunctionFactory(Uniform(-M_PI, M_PI))
{
  initializeCache();
}

/* Virtual constructor */
FourierSeriesFactory * FourierSeriesFactory::clone() const
{
  return new FourierSeriesFactory(*this);
}


/* String converter */
String FourierSeriesFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " measure=" << measure_;
}

/* The method to get the function of any order */
UniVariateFunction FourierSeriesFactory::build(const UnsignedInteger order) const
{
  const Bool isCosine = (order % 2) == 0;
  const UnsignedInteger k = (order + 1) / 2;
  return new FourierSeries(isCosine, k);
}


/* Method save() stores the object through the StorageManager */
void FourierSeriesFactory::save(Advocate & adv) const
{
  OrthogonalUniVariateFunctionFactory::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void FourierSeriesFactory::load(Advocate & adv)
{
  OrthogonalUniVariateFunctionFactory::load(adv);
}

END_NAMESPACE_OPENTURNS
