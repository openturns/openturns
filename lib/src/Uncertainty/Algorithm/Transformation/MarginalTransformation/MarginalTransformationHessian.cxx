//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformationHessian evaluation for elliptical
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
#include "MarginalTransformationHessian.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MarginalTransformationHessian);

static Factory<MarginalTransformationHessian> RegisteredFactory("MarginalTransformationHessian");

/* Default constructor */
MarginalTransformationHessian::MarginalTransformationHessian():
  NumericalMathHessianImplementation(),
  evaluation_()
{
  // Nothing to do
}

/* Parameter constructor */
MarginalTransformationHessian::MarginalTransformationHessian(const MarginalTransformationEvaluation & evaluation):
  NumericalMathHessianImplementation(),
  evaluation_(evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
MarginalTransformationHessian * MarginalTransformationHessian::clone() const
{
  return new MarginalTransformationHessian(*this);
}

/* Hessian */
SymmetricTensor MarginalTransformationHessian::hessian(const NumericalPoint & inP) const
{
  const UnsignedInteger dimension(getOutputDimension());
  SymmetricTensor result(dimension, dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (evaluation_.getSimplifications()[i] && evaluation_.getExpressions()[i].getHessian()->getClassName() == "AnalyticalNumericalMathHessianImplementation") result(i, i, i) = evaluation_.getExpressions()[i].hessian(NumericalPoint(1, inP[i]))(0, 0, 0);
    else
    {
      // (`@`(-(`@`((D@@2)(G), 1/G))/(`@`(D(G), 1/G))^3, F))*D(F)^2+(`@`(1/(`@`(D(G), 1/G)), F))*(D@@2)(F)
      const NumericalScalar inputPDF(evaluation_.inputDistributionCollection_[i].computePDF(inP[i]));
      // Quick rejection step: if the input PDF is zero, the result will be zero, so continue only if the value is > 0
      if (inputPDF > 0.0)
      {
        NumericalScalar inputCDF(evaluation_.inputDistributionCollection_[i].computeCDF(inP[i]));
        // For accuracy reason, check if we are in the upper tail of the distribution
        const Bool upperTail(inputCDF > 0.5);
        if (upperTail) inputCDF = evaluation_.inputDistributionCollection_[i].computeComplementaryCDF(inP[i]);
        // The upper tail CDF is defined by CDF(x, upper) = P(X>x)
        // The upper tail quantile is defined by Quantile(CDF(x, upper), upper) = x
        const NumericalPoint  outputQuantile(evaluation_.outputDistributionCollection_[i].computeQuantile(inputCDF, upperTail));
        const NumericalScalar outputPDF(evaluation_.outputDistributionCollection_[i].computePDF(outputQuantile));
        if (outputPDF > 0.0)
        {
          const NumericalScalar inputDDF(evaluation_.inputDistributionCollection_[i].computeDDF(inP[i]));
          const NumericalScalar outputDDF(evaluation_.outputDistributionCollection_[i].computeDDF(outputQuantile[0]));
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
  return evaluation_.inputDistributionCollection_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger MarginalTransformationHessian::getOutputDimension() const
{
  return evaluation_.outputDistributionCollection_.getSize();
}

/* String converter */
String MarginalTransformationHessian::__repr__() const
{
  OSS oss;
  oss << "class=" << MarginalTransformationHessian::GetClassName()
      << " evaluation=" << evaluation_;
  return oss;
}

String MarginalTransformationHessian::__str__(const String & offset) const
{
  return OSS() << offset << "Hessian of " << evaluation_.getName();
}

/* Method save() stores the object through the StorageManager */
void MarginalTransformationHessian::save(Advocate & adv) const
{
  NumericalMathHessianImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void MarginalTransformationHessian::load(Advocate & adv)
{
  NumericalMathHessianImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}

END_NAMESPACE_OPENTURNS
