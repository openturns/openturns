//                                               -*- C++ -*-
/**
 *  @brief Class for the Nataf transformationGradient evaluation for elliptical
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
#include "openturns/MarginalTransformationGradient.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MarginalTransformationGradient)

static const Factory<MarginalTransformationGradient> Factory_MarginalTransformationGradient;

/* ParameterDefault constructor */
MarginalTransformationGradient::MarginalTransformationGradient()
  : GradientImplementation()
  , evaluation_()
{
  // Nothing to do
}

/* Parameter constructor */
MarginalTransformationGradient::MarginalTransformationGradient(const MarginalTransformationEvaluation & evaluation)
  : GradientImplementation()
  , evaluation_(evaluation)
{
  // Nothing to do
}

/* Virtual constructor */
MarginalTransformationGradient * MarginalTransformationGradient::clone() const
{
  return new MarginalTransformationGradient(*this);
}

/* Gradient */
Matrix MarginalTransformationGradient::gradient(const Point & inP) const
{
  const UnsignedInteger dimension = getOutputDimension();
  Matrix result(dimension, dimension);
  // (G^{-1} o F)' = F' . G^{-1}' o F = F' / (G' o G^{-1} o F)
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (evaluation_.getSimplifications()[i] && evaluation_.getExpressions()[i].getGradient().getImplementation()->getClassName() == "SymbolicGradient") result(i, i) = evaluation_.getExpressions()[i].gradient(Point(1, inP[i]))(0, 0);
    else
    {
      const Scalar inputPDF = evaluation_.inputDistributionCollection_[i].computePDF(inP[i]);
      // Quick rejection step: if the input PDF is zero, the result will be zero, so continue only if the value is > 0
      if (inputPDF > 0.0)
      {
        Scalar inputCDF = evaluation_.inputDistributionCollection_[i].computeCDF(inP[i]);
        // For accuracy reason, check if we are in the upper tail of the distribution
        const Bool upperTail = inputCDF > 0.5;
        if (upperTail) inputCDF = evaluation_.inputDistributionCollection_[i].computeComplementaryCDF(inP[i]);
        // The upper tail CDF is defined by CDF(x, upper) = P(X>x)
        // The upper tail quantile is defined by Quantile(CDF(x, upper), upper) = x
        const Point  outputQuantile(evaluation_.outputDistributionCollection_[i].computeQuantile(inputCDF, upperTail));
        const Scalar outputPDF = evaluation_.outputDistributionCollection_[i].computePDF(outputQuantile);
        // The output PDF should never be zero here, be it can occure due to some strange rounding error
        if (outputPDF > 0.0) result(i, i) = inputPDF / outputPDF;
      } // PDF > 0
    } // No simplification
  }
  return result;
}

/* Accessor for input point dimension */
UnsignedInteger MarginalTransformationGradient::getInputDimension() const
{
  return evaluation_.inputDistributionCollection_.getSize();
}

/* Accessor for output point dimension */
UnsignedInteger MarginalTransformationGradient::getOutputDimension() const
{
  return evaluation_.outputDistributionCollection_.getSize();
}

/* String converter */
String MarginalTransformationGradient::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MarginalTransformationGradient::GetClassName()
      << " evaluation=" << evaluation_;
  return oss;
}

String MarginalTransformationGradient::__str__(const String & ) const
{
  return OSS(false) << "Gradient of " << evaluation_.getName();
}

/* Method save() stores the object through the StorageManager */
void MarginalTransformationGradient::save(Advocate & adv) const
{
  GradientImplementation::save(adv);
  adv.saveAttribute( "evaluation_", evaluation_ );
}

/* Method load() reloads the object from the StorageManager */
void MarginalTransformationGradient::load(Advocate & adv)
{
  GradientImplementation::load(adv);
  adv.loadAttribute( "evaluation_", evaluation_ );
}

END_NAMESPACE_OPENTURNS

