//                                               -*- C++ -*-
/**
 *  @brief This is the natural basis associated to a multivariate distribution
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
#include "openturns/SoizeGhanemFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/OrthogonalProductPolynomialFactory.hxx"
#include "openturns/StandardDistributionPolynomialFactory.hxx"
#include "openturns/AdaptiveStieltjesAlgorithm.hxx"
#include "openturns/Function.hxx"
#include "openturns/LinearEnumerateFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS




class SoizeGhanemFactorEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:
  typedef PersistentCollection<Distribution> DistributionPersistentCollection;

  /* Default constructor */
  SoizeGhanemFactorEvaluation() {};

  /* Default constructor */
  SoizeGhanemFactorEvaluation(const Distribution & measure,
                              const Collection<Distribution> & marginals,
                              const Bool useCopula)
    : EvaluationImplementation()
    , measure_(measure)
    , useCopula_(useCopula)
    , marginals_(marginals)
  {
    // Extract the copula if needed
    if (useCopula_)
      copula_ = measure.getCopula();
  }

  /* Virtual constructor */
  SoizeGhanemFactorEvaluation * clone() const override
  {
    return new SoizeGhanemFactorEvaluation(*this);
  }

  /* Comparison operator */
  using EvaluationImplementation::operator ==;
  Bool operator ==(const SoizeGhanemFactorEvaluation & other) const
  {
    if (this == &other) return true;
    return (measure_ == other.measure_) && (useCopula_ == other.useCopula_);
  }

  /* String converter */
  String __repr__() const override
  {
    OSS oss(true);
    oss << "class=" << GetClassName()
        << " measure=" << measure_
        << " marginals=" << marginals_
        << " useCopula=" << useCopula_;
    if (useCopula_) oss << " copula=" << copula_;
    return oss;
  }

  String __str__(const String & offset = "") const override
  {
    OSS oss(true);
    const Description input(getInputDescription());
    oss << offset << input << "->1/sqrt(" << (useCopula_ ? copula_.__str__() : measure_.__str__()) << ".computePDF(";
    String separator(", ");
    for (UnsignedInteger i = 0; i < input.getSize(); ++i)
    {
      if (i == input.getSize() - 1) separator = ")";
      if (useCopula_)
        oss << marginals_[i].__str__() << ".computeCDF(" << input[i] << ")" << separator;
      else
        oss << input[i] << separator;
    }
    return oss;
  }

  using EvaluationImplementation::operator();
  /* Operator ()
    * The correction factor K(x) is given by:
    * K(x)=\sqrt{\frac{\prod_{k=1}^d p_k(x_k)}{p(x_1,\dots,x_d)}}
    * where p_k is the PDF of the kth marginal distribution and p
    * its join PDF. Introducing the copula PDF c, one gets:
    * K(x)=\frac{1}{\sqrt{c(F_1(x_1),\dots,F_d(x_d))}}
    * because p(x_1,\dots,x_d)=c(F_1(x_1),\dots,F_d(x_d))\prod_{k=1}^d p_k(x_k)
    * where F_k is the CDF of the kth marginal distribution.
    * See: Christian Soize, R. Ghanem.
    * "Physical systems with random uncertainties: Chaos representations with
    *  arbitrary probability measure". SIAM Journal on Scientific Computing,
    *  Society for Industrial and Applied Mathematics, 2004, 26 (2), pp.395-410.
    */
  Point operator() (const Point & inP) const override
  {
    const UnsignedInteger inputDimension = getInputDimension();
    if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given point has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inP.getDimension();
    Point result(1);
    if (useCopula_)
    {
      Point u(inputDimension);
      for (UnsignedInteger i = 0; i < inputDimension; ++i)
        u[i] = marginals_[i].computeCDF(inP[i]);
      result[0] = 1.0 / std::sqrt(std::max(SpecFunc::MinScalar, copula_.computePDF(u)));
    }
    else
    {
      Scalar logFactor = 0.0;
      for (UnsignedInteger i = 0; i < inputDimension; ++i)
        logFactor += marginals_[i].computeLogPDF(inP[i]);
      // \sqrt{\frac{\prod_{k=1}^d p_k(x_k)}{p(x_1,\dots,x_d)}}
      result[0] = std::exp(0.5 * (logFactor - measure_.computeLogPDF(inP)));
    }
    callsNumber_.increment();
    return result;
  }

  Sample operator() (const Sample & inS) const override
  {
    const UnsignedInteger inputDimension = getInputDimension();
    if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: the given sample has an invalid dimension. Expect a dimension " << inputDimension << ", got " << inS.getDimension();
    const UnsignedInteger size = inS.getSize();
    if (size == 0) return Sample(0, 1);
    Sample result(size, 1);
    if (useCopula_)
    {
      Sample u(size, 0);
      for (UnsignedInteger i = 0; i < inputDimension; ++i)
        u.stack(marginals_[i].computeCDF(inS.getMarginal(i)));
      const Sample pdf(copula_.computePDF(u));
      for (UnsignedInteger i = 0; i < size; ++i)
        result(i, 0) = 1.0 / std::sqrt(std::max(SpecFunc::MinScalar, pdf(i, 0)));
    }
    else
    {
      Sample logFactor(size, 1);
      for (UnsignedInteger i = 0; i < inputDimension; ++i)
        logFactor += marginals_[i].computeLogPDF(inS.getMarginal(i));
      // \sqrt{\frac{\prod_{k=1}^d p_k(x_k)}{p(x_1,\dots,x_d)}}
      const Sample logResult(logFactor - measure_.computeLogPDF(inS));
      for (UnsignedInteger i = 0; i < size; ++i)
        result(i, 0) = std::exp(0.5 * logResult(i, 0));
    }
    callsNumber_.fetchAndAdd(size);
    return result;
  }

  /* Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override
  {
    return measure_.getDimension();
  }

  /* Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  /* Accessor for input point description */
  Description getInputDescription() const override
  {
    return measure_.getDescription();
  }

  /* Accessor for output point description */
  Description getOutputDescription() const override
  {
    return Description(1, "k");
  }

  /* Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override
  {
    EvaluationImplementation::save(adv);
    adv.saveAttribute( "measure_", measure_ );
    adv.saveAttribute( "marginals_", marginals_ );
    adv.saveAttribute( "useCopula_", useCopula_ );
    adv.saveAttribute( "copula_", copula_ );
  }

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override
  {
    EvaluationImplementation::load(adv);
    adv.loadAttribute( "measure_", measure_ );
    adv.loadAttribute( "marginals_", marginals_ );
    adv.loadAttribute( "useCopula_", useCopula_ );
    adv.loadAttribute( "copula_", copula_ );
  }

private:

  /** The underlying measure */
  Distribution measure_;

  /** Flag to tell if the evaluation is based on the copula PDF or on the joint PDF */
  Bool useCopula_ = false;

  /** The copula of the measure */
  Distribution copula_;

  /** The 1D marginal distributions of the measure */
  DistributionPersistentCollection marginals_;

};

CLASSNAMEINIT(SoizeGhanemFactorEvaluation)

static const Factory<SoizeGhanemFactorEvaluation> Factory_SoizeGhanemFactorEvaluation;


CLASSNAMEINIT(SoizeGhanemFactory)

static const Factory<SoizeGhanemFactory> Factory_SoizeGhanemFactory;

/* Default constructor */
SoizeGhanemFactory::SoizeGhanemFactory()
  : OrthogonalFunctionFactory()
  , productPolynomial_()
  , phi_(LinearEnumerateFunction(1))
  , hasIndependentCopula_(measure_.hasIndependentCopula())
{
  buildProductPolynomialAndAdaptation(false);
}


/* Constructor */
SoizeGhanemFactory::SoizeGhanemFactory(const Distribution & measure,
                                       const Bool useCopula)
  : OrthogonalFunctionFactory(measure)
  , productPolynomial_()
  , phi_(LinearEnumerateFunction(measure.getDimension()))
  , hasIndependentCopula_(measure.hasIndependentCopula())
{
  buildProductPolynomialAndAdaptation(useCopula);
}


/* Constructor */
SoizeGhanemFactory::SoizeGhanemFactory(const Distribution & measure,
                                       const EnumerateFunction & phi,
                                       const Bool useCopula)
  : OrthogonalFunctionFactory(measure)
  , productPolynomial_()
  , phi_(phi)
  , hasIndependentCopula_(measure.hasIndependentCopula())
{
  if (measure.getDimension() != phi.getDimension()) throw InvalidArgumentException(HERE) << "Error: the enumerate function must have a dimension equal to the dimension of the measure";
  buildProductPolynomialAndAdaptation(useCopula);
}


/* Virtual constructor */
SoizeGhanemFactory * SoizeGhanemFactory::clone() const
{
  return new SoizeGhanemFactory(*this);
}


/* Return the enumerate function that translate unidimensional indices into multidimensional indices */
EnumerateFunction SoizeGhanemFactory::getEnumerateFunction() const
{
  return phi_;
}


/* Build the Function of the given index orthonormal
 * with respect to the following inner product:
 * <f_i, f_j> = \int_R^n f_i(x) f_j(x) c(F_1(x_1),\dots,F_n(x_n))\prod_{k=1}^n p_k(x_k)dx
 * See: Christian Soize, R. Ghanem.
 * "Physical systems with random uncertainties: Chaos representations with
 *  arbitrary probability measure". SIAM Journal on Scientific Computing,
 *  Society for Industrial and Applied Mathematics, 2004, 26 (2), pp.395-410.
 */
Function SoizeGhanemFactory::build(const UnsignedInteger index) const
{
  const Function productPolynomial(productPolynomial_.build(index));
  // If the distribution has an independent copula the SoizeGhanem basis
  // is exactly the product polynomial factory
  if (hasIndependentCopula_) return productPolynomial;
  return productPolynomial * adaptationFactor_;
}

/* Build the multivariate polynomial factory associated with the marginal distributions */
void SoizeGhanemFactory::buildProductPolynomialAndAdaptation(const Bool useCopula)
{
  const UnsignedInteger dimension = measure_.getDimension();
  Collection<Distribution> marginals(dimension);
  OrthogonalProductPolynomialFactory::PolynomialFamilyCollection coll(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    marginals[i] = measure_.getMarginal(i);
    coll[i] = StandardDistributionPolynomialFactory(AdaptiveStieltjesAlgorithm(marginals[i]));
  }
  productPolynomial_ = OrthogonalProductPolynomialFactory(coll, phi_);
  // Build the adaptation factor only if needed
  if (!hasIndependentCopula_)
    adaptationFactor_ = Function(SoizeGhanemFactorEvaluation(measure_, marginals, useCopula));
}

/* String converter */
String SoizeGhanemFactory::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName()
      << " productPolynomial=" << productPolynomial_
      << " phi=" << phi_
      << " measure=" << measure_
      << " hasIndependentCopula=" << hasIndependentCopula_;
  if (!hasIndependentCopula_)
    oss << " adaptationFactor=" << adaptationFactor_;
  return oss;
}


/* Method save() stores the object through the StorageManager */
void SoizeGhanemFactory::save(Advocate & adv) const
{
  OrthogonalFunctionFactory::save(adv);
  adv.saveAttribute( "productPolynomial_", productPolynomial_ );
  adv.saveAttribute( "phi_", phi_ );
  adv.saveAttribute( "hasIndependentCopula_", hasIndependentCopula_ );
  adv.saveAttribute( "adaptationFactor_", adaptationFactor_ );
}


/* Method load() reloads the object from the StorageManager */
void SoizeGhanemFactory::load(Advocate & adv)
{
  OrthogonalFunctionFactory::load(adv);
  adv.loadAttribute( "productPolynomial_", productPolynomial_ );
  adv.loadAttribute( "phi_", phi_ );
  adv.loadAttribute( "hasIndependentCopula_", hasIndependentCopula_ );
  adv.loadAttribute( "adaptationFactor_", adaptationFactor_ );
}



END_NAMESPACE_OPENTURNS
