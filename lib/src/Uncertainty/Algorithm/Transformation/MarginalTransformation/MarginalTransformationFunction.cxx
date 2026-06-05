//                                               -*- C++ -*-
/**
 *  @brief Marginal transformation function
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/MarginalTransformationFunction.hxx"
#include "openturns/MarginalTransformationEvaluation.hxx"
#include "openturns/MarginalTransformationGradient.hxx"
#include "openturns/MarginalTransformationHessian.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MarginalTransformationFunction)

/* Default constructor */
MarginalTransformationFunction::MarginalTransformationFunction()
  : Function()
  , inputDistributionCollection_()
  , outputDistributionCollection_()
{
  // Nothing to do
}

/* Parameter constructor */
MarginalTransformationFunction::MarginalTransformationFunction(const DistributionCollection & inputDistributionCollection,
    const DistributionCollection & outputDistributionCollection)
  : Function(Build(inputDistributionCollection, outputDistributionCollection))
  , inputDistributionCollection_(inputDistributionCollection)
  , outputDistributionCollection_(outputDistributionCollection)
{
  // Nothing to do
}

/* Build the function */
Function MarginalTransformationFunction::Build(const DistributionCollection & inputDistributionCollection,
    const DistributionCollection & outputDistributionCollection)
{
  const MarginalTransformationEvaluation evaluation(inputDistributionCollection, outputDistributionCollection);
  const MarginalTransformationGradient gradient(evaluation);
  const MarginalTransformationHessian hessian(evaluation);
  return Function(evaluation.clone(), gradient.clone(), hessian.clone());
}

/* Comparison operator */
Bool MarginalTransformationFunction::operator ==(const MarginalTransformationFunction & other) const
{
  return (inputDistributionCollection_ == other.inputDistributionCollection_)
      && (outputDistributionCollection_ == other.outputDistributionCollection_);
}

/* Input distribution collection accessor */
MarginalTransformationFunction::DistributionCollection MarginalTransformationFunction::getInputDistributionCollection() const
{
  return inputDistributionCollection_;
}

/* Output distribution collection accessor */
MarginalTransformationFunction::DistributionCollection MarginalTransformationFunction::getOutputDistributionCollection() const
{
  return outputDistributionCollection_;
}

/* String converter */
String MarginalTransformationFunction::__repr__() const
{
  OSS oss;
  oss << "class=" << MarginalTransformationFunction::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

String MarginalTransformationFunction::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
