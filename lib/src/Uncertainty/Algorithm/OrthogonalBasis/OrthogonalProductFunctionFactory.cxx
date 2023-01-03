//                                               -*- C++ -*-
/**
 *  @brief This is the orthogonal function basis
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
#include "openturns/OrthogonalProductFunctionFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Indices.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/FunctionImplementation.hxx"
#include "openturns/ProductUniVariateFunctionEvaluation.hxx"
#include "openturns/ProductUniVariateFunctionGradient.hxx"
#include "openturns/ProductUniVariateFunctionHessian.hxx"
#include "openturns/LinearEnumerateFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



TEMPLATE_CLASSNAMEINIT(PersistentCollection<OrthogonalUniVariateFunctionFamily>)

static const Factory<PersistentCollection<OrthogonalUniVariateFunctionFamily> > Factory_PersistentCollection_OrthogonalUniVariateFunctionFamily;

CLASSNAMEINIT(OrthogonalProductFunctionFactory)

static const Factory<OrthogonalProductFunctionFactory> Factory_OrthogonalProductFunctionFactory;

typedef Collection<Point> PointCollection;
typedef ProductUniVariateFunctionEvaluation::UniVariateFunctionCollection UniVariateFunctionCollection;

/* Default constructor */
OrthogonalProductFunctionFactory::OrthogonalProductFunctionFactory()
  : OrthogonalFunctionFactory()
  , tensorizedFunctionFactory_()
{
  // Nothing to do
}


/* Constructor */
OrthogonalProductFunctionFactory::OrthogonalProductFunctionFactory(const FunctionFamilyCollection & coll)
  : OrthogonalFunctionFactory()
{
  buildTensorizedFunctionFactory(coll, LinearEnumerateFunction(coll.getSize()) );
  buildMeasure(coll);
}


/* Constructor */
OrthogonalProductFunctionFactory::OrthogonalProductFunctionFactory(const FunctionFamilyCollection & coll,
    const EnumerateFunction & phi)
  : OrthogonalFunctionFactory()
{
  if (coll.getSize() != phi.getDimension()) throw InvalidArgumentException(HERE) << "Error: the enumerate function must have a dimension equal to the collection size";
  buildTensorizedFunctionFactory(coll, phi);
  buildMeasure(coll);
}


/* Virtual constructor */
OrthogonalProductFunctionFactory * OrthogonalProductFunctionFactory::clone() const
{
  return new OrthogonalProductFunctionFactory(*this);
}


/* Return the enumerate function that translate unidimensional indices into multidimensional indices */
EnumerateFunction OrthogonalProductFunctionFactory::getEnumerateFunction() const
{
  return tensorizedFunctionFactory_.getEnumerateFunction();
}

/* Return the collection of univariate orthogonal polynomial families */
OrthogonalProductFunctionFactory::FunctionFamilyCollection OrthogonalProductFunctionFactory::getFunctionFamilyCollection() const
{
  TensorizedUniVariateFunctionFactory::FunctionFamilyCollection functionColl(tensorizedFunctionFactory_.getFunctionFamilyCollection());
  FunctionFamilyCollection coll;
  for (UnsignedInteger i = 0; i < functionColl.getSize(); ++ i)
  {
    coll.add(dynamic_cast<OrthogonalUniVariateFunctionFactory*>(functionColl[i].getImplementation().get())->clone());
  }
  return coll;
}


/* Build the Function of the given index */
Function OrthogonalProductFunctionFactory::build(const UnsignedInteger index) const
{
  return tensorizedFunctionFactory_.build(index);
}


/* String converter */
String OrthogonalProductFunctionFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " factory=" << tensorizedFunctionFactory_
         << " measure=" << measure_;
}


/* Method save() stores the object through the StorageManager */
void OrthogonalProductFunctionFactory::save(Advocate & adv) const
{
  OrthogonalFunctionFactory::save(adv);
  adv.saveAttribute("tensorizedFunctionFactory_", tensorizedFunctionFactory_);
}


/* Method load() reloads the object from the StorageManager */
void OrthogonalProductFunctionFactory::load(Advocate & adv)
{
  OrthogonalFunctionFactory::load(adv);
  adv.loadAttribute("tensorizedFunctionFactory_", tensorizedFunctionFactory_);
}

/*  Build product function factory */
void OrthogonalProductFunctionFactory::buildTensorizedFunctionFactory(const FunctionFamilyCollection & coll,
    const EnumerateFunction & phi)
{
  TensorizedUniVariateFunctionFactory::FunctionFamilyCollection functionColl;
  const UnsignedInteger size = coll.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    functionColl.add(UniVariateFunctionFamily(*coll[i].getImplementation()));
  }
  tensorizedFunctionFactory_.setFunctionFamilyCollection(functionColl);
  tensorizedFunctionFactory_.setEnumerateFunction(phi);
}

/* Build the measure based on the one found in the family collection */
void OrthogonalProductFunctionFactory::buildMeasure(const FunctionFamilyCollection & coll)
{
  const UnsignedInteger size = coll.getSize();
  Collection<Distribution> distributions(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    distributions[i] = coll[i].getMeasure();
  }
  measure_ = ComposedDistribution(distributions);
}


END_NAMESPACE_OPENTURNS
