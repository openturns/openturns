//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformationHessian evaluation for elliptical
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/MarginalTransformationHessian.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MarginalTransformationHessian)

static const Factory<MarginalTransformationHessian> Factory_MarginalTransformationHessian;

/* Default constructor */
MarginalTransformationHessian::MarginalTransformationHessian():
  HessianImplementation(),
  p_evaluation_(new MarginalTransformationEvaluation)
{
  // Nothing to do
}

/* Parameter constructor */
MarginalTransformationHessian::MarginalTransformationHessian(const MarginalTransformationEvaluation & evaluation):
  HessianImplementation(),
  p_evaluation_(evaluation.clone())
{
  // Nothing to do
}

/* Virtual constructor */
MarginalTransformationHessian * MarginalTransformationHessian::clone() const
{
  return new MarginalTransformationHessian(*this);
}

/* Hessian */
SymmetricTensor MarginalTransformationHessian::hessian(const Point & inP) const
{
  const UnsignedInteger dimension = getOutputDimension();
  SymmetricTensor result(dimension, dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (p_evaluation_->getSimplifications()[i] && p_evaluation_->getExpressions()[i].getHessian().getImplementation()->getClassName() == "SymbolicHessian") result(i, i, i) = p_evaluation_->getExpressions()[i].hessian(Point(1, inP[i]))(0, 0, 0);
    else
    {
      // (`@`(-(`@`((D@@2)(G), 1/G))/(`@`(D(G), 1/G))^3, F))*D(F)^2+(`@`(1/(`@`(D(G), 1/G)), F))*(D@@2)(F)
      const Scalar inputPDF = p_evaluation_->inputDistributionCollection_[i].computePDF(inP[i]);
      // Quick rejection step: if the input PDF is zero, the result will be zero, so continue only if the value is > 0
      if (inputPDF > 0.0)
      {
        Scalar inputCDF = p_evaluation_->inputDistributionCollection_[i].computeCDF(inP[i]);
        // For accuracy reason, check if we are in the upper tail of the distribution
        const Bool upperTail = inputCDF > 0.5;
        if (upperTail) inputCDF = p_evaluation_->inputDistributionCollection_[i].computeComplementaryCDF(inP[i]);
        // The upper tail CDF is defined by CDF(x, upper) = P(X>x)
        // The upper tail quantile is defined by Quantile(CDF(x, upper), upper) = x
        const Point  outputQuantile(p_evaluation_->outputDistributionCollection_[i].computeQuantile(inputCDF, upperTail));
        const Scalar outputPDF = p_evaluation_->outputDistributionCollection_[i].computePDF(outputQuantile);
        if (outputPDF > 0.0)
        {
          const Scalar inputDDF = p_evaluation_->inputDistributionCollection_[i].computeDDF(inP[i]);
          const Scalar outputDDF = p_evaluation_->outputDistributionCollection_[i].computeDDF(outputQuantile[0]);
          result(i, i, i) = (inputDDF - outputDDF * pow(inputPDF / outputPDF, 2)) / outputPDF;
        } // output PDF > 0
      } // input PDF > 0
    } // No simplification
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger MarginalTransformationHessian::getInputDimension() const
{
  return p_evaluation_->inputDistributionCollection_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger MarginalTransformationHessian::getOutputDimension() const
{
  return p_evaluation_->outputDistributionCollection_.getSize();
}

/* String converter */
String MarginalTransformationHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << MarginalTransformationHessian::GetClassName()
      << " evaluation=" << *p_evaluation_;
  return oss;
}

String MarginalTransformationHessian::__str__(const String & ) const
{
  return OSS() << "Hessian of " << p_evaluation_->getName();
}

/* Method save() stores the object through the StorageManager */
void MarginalTransformationHessian::save(Advocate & adv) const
{
  HessianImplementation::save(adv);
  adv.saveAttribute( "evaluation_", *p_evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void MarginalTransformationHessian::load(Advocate & adv)
{
  HessianImplementation::load(adv);
  TypedInterfaceObject<MarginalTransformationEvaluation> evaluation;
  adv.loadAttribute( "evaluation_", evaluation );
  p_evaluation_ = evaluation.getImplementation();
}

END_NAMESPACE_OPENTURNS
