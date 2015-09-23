//                                               -*- C++ -*-
/**
 *  @brief This is the orthogonal polynomial basis
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
#include "OrthogonalProductPolynomialFactory.hxx"
#include "OSS.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"
#include "Indices.hxx"
#include "ComposedDistribution.hxx"
#include "NumericalMathFunctionImplementation.hxx"
#include "ProductPolynomialEvaluationImplementation.hxx"
#include "ProductPolynomialGradientImplementation.hxx"
#include "ProductPolynomialHessianImplementation.hxx"
#include "Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS



TEMPLATE_CLASSNAMEINIT(PersistentCollection<OrthogonalUniVariatePolynomialFamily>);

static const Factory<PersistentCollection<OrthogonalUniVariatePolynomialFamily> > RegisteredFactory_PC_OUVPF;

CLASSNAMEINIT(OrthogonalProductPolynomialFactory);

static const Factory<OrthogonalProductPolynomialFactory> RegisteredFactory_OPPF;

typedef Collection<NumericalPoint> NumericalPointCollection;
typedef ProductPolynomialEvaluationImplementation::PolynomialCollection PolynomialCollection;

/* Default constructor */
OrthogonalProductPolynomialFactory::OrthogonalProductPolynomialFactory()
  : OrthogonalFunctionFactory()
  , coll_()
  , phi_()
{
  // Nothing to do
}


/* Constructor */
OrthogonalProductPolynomialFactory::OrthogonalProductPolynomialFactory(const PolynomialFamilyCollection & coll)
  : OrthogonalFunctionFactory()
  , coll_(coll)
  , phi_(coll.getSize())
{
  buildMeasure();
}


/* Constructor */
OrthogonalProductPolynomialFactory::OrthogonalProductPolynomialFactory(const PolynomialFamilyCollection & coll,
    const EnumerateFunction & phi)
  : OrthogonalFunctionFactory(),
    coll_(coll),
    phi_(phi)
{
  if (coll.getSize() != phi.getDimension()) throw InvalidArgumentException(HERE) << "Error: the enumerate function must have a dimension equal to the collection size";
  buildMeasure();
}


/* Virtual constructor */
OrthogonalProductPolynomialFactory * OrthogonalProductPolynomialFactory::clone() const
{
  return new OrthogonalProductPolynomialFactory(*this);
}


/* Return the enumerate function that translate unidimensional indices nto multidimensional indices */
EnumerateFunction OrthogonalProductPolynomialFactory::getEnumerateFunction() const
{
  return phi_;
}


/* Return the collection of univariate orthogonal polynomial families */
OrthogonalProductPolynomialFactory::PolynomialFamilyCollection OrthogonalProductPolynomialFactory::getPolynomialFamilyCollection() const
{
  return coll_;
}

/* Build the NumericalMathFunction of the given index */
NumericalMathFunction OrthogonalProductPolynomialFactory::build(const UnsignedInteger index) const
{
  // Compute the multi-indices using the EnumerateFunction
  Indices indices(phi_(index));
  const UnsignedInteger size(indices.getSize());
  // Then build the collection of polynomials using the collection of factories
  PolynomialCollection polynomials(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    polynomials[i] = coll_[i].build(indices[i]);
  }
  return NumericalMathFunctionImplementation(ProductPolynomialEvaluationImplementation(polynomials).clone(), ProductPolynomialGradientImplementation(polynomials).clone(), ProductPolynomialHessianImplementation(polynomials).clone());
}


/* String converter */
String OrthogonalProductPolynomialFactory::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " univariate polynomial collection=" << coll_
         << " measure=" << measure_;
}


/* Method save() stores the object through the StorageManager */
void OrthogonalProductPolynomialFactory::save(Advocate & adv) const
{
  OrthogonalFunctionFactory::save(adv);
  adv.saveAttribute( "coll_", coll_ );
  adv.saveAttribute( "phi_", phi_ );
}


/* Method load() reloads the object from the StorageManager */
void OrthogonalProductPolynomialFactory::load(Advocate & adv)
{
  OrthogonalFunctionFactory::load(adv);
  adv.loadAttribute( "coll_", coll_ );
  adv.loadAttribute( "phi_", phi_ );
}

/* Build the measure based on the one found in the family collection */
void OrthogonalProductPolynomialFactory::buildMeasure()
{
  const UnsignedInteger size(coll_.getSize());
  Collection<Distribution> distributions(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    distributions[i] = coll_[i].getMeasure();
  }
  measure_ = ComposedDistribution(distributions);
}

/* Nodes and weights of the multivariate polynomial associated with the marginal degrees indices[0], ...,indices[dimension] as the tensor product of the marginal orthogonal univariate polynomials, to build multivariate quadrature rules */
NumericalSample OrthogonalProductPolynomialFactory::getNodesAndWeights(const Indices & degrees,
    NumericalPoint & weights) const
{
  const UnsignedInteger degreesSize(degrees.getSize());
  if (degreesSize != coll_.getSize()) throw InvalidArgumentException(HERE) << "Error: the degrees size must match the size of the orthogonal univariate polynomials factories size.";
  Bool isConstant(true);
  for (UnsignedInteger i = 0; i < degreesSize; ++i) isConstant = isConstant && (degrees[i] == 0);
  if (isConstant) throw InvalidArgumentException(HERE) << "Error: cannot compute the roots and weights of a constant polynomial.";
  // First, get the nodes and weights of the marginal factories
  NumericalPointCollection marginalNodes;
  NumericalPointCollection marginalWeights;
  UnsignedInteger totalSize(1);
  for (UnsignedInteger i = 0; i < degreesSize; ++i)
  {
    const UnsignedInteger d(degrees[i]);
    totalSize *= d;
    NumericalPoint w;
    marginalNodes.add(coll_[i].getNodesAndWeights(d, w));
    marginalWeights.add(w);
  }
  // Perform the tensor product
  NumericalSample nodes(totalSize, degreesSize);
  weights = NumericalPoint(totalSize, 1.0);
  Indices indices(degreesSize, 0);
  for (UnsignedInteger i = 0; i < totalSize; ++i)
  {
    // Build the current node
    for (UnsignedInteger j = 0; j < degreesSize; ++j)
    {
      nodes[i][j] = marginalNodes[j][indices[j]];
      weights[i] *= marginalWeights[j][indices[j]];
    }
    /* Update the indices */
    ++indices[0];
    /* Propagate the remainders */
    for (UnsignedInteger j = 0; j < degreesSize - 1; ++j) indices[j + 1] += (indices[j] == degrees[j]);
    /* Correction of the indices. The last index cannot overflow. */
    for (UnsignedInteger j = 0; j < degreesSize - 1; ++j) indices[j] = indices[j] % degrees[j];
  }
  return nodes;
}

END_NAMESPACE_OPENTURNS
