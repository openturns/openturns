//                                               -*- C++ -*-
/**
 *  @brief Isoprobabilistic transformation
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DistributionTransformation.hxx"
#include "openturns/RosenblattEvaluation.hxx"
#include "openturns/InverseRosenblattEvaluation.hxx"
#include "openturns/MarginalTransformationEvaluation.hxx"
#include "openturns/MarginalTransformationGradient.hxx"
#include "openturns/MarginalTransformationHessian.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/ComposedFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<Distribution> DistributionCollection;

CLASSNAMEINIT(DistributionTransformation)

/* Default constructor */
DistributionTransformation::DistributionTransformation ()
  : Function()
{
  // Nothing to do
}

/* Parameter constructor */
DistributionTransformation::DistributionTransformation (const Distribution & distribution,
    const Distribution & measure)
  : Function(Build(distribution, measure))
  , left_(distribution)
  , right_(measure)
{
  // Nothing to do
}

/* Parameter constructor */
Function DistributionTransformation::Build (const Distribution & distribution,
    const Distribution & measure)
{

  Function transformation;
  LOGINFO("Build the iso-probabilistic transformation");
  const Bool noTransformation = (measure == distribution);
  const UnsignedInteger dimension = distribution.getDimension();
  if (noTransformation)
  {
    // The first case is when distribution == measure, in which case there is
    // no isoprobabilistic transformation to introduce.
    LOGINFO(OSS(false) << "Same distribution for input vector=" << distribution << " and basis=" << measure);
    transformation = IdentityFunction(dimension);
  }
  else // No transformation
  {
    // The second case is when both distributions share the same copula
    // in which case the transformation is made of maginal transformations
    if (distribution.getCopula() == measure.getCopula())
    {
      // We use empty collections to avoid the construction of default distributions
      LOGINFO("Same copula for input vector and basis");
      DistributionCollection marginalX(0);
      DistributionCollection marginalZ(0);
      for (UnsignedInteger i = 0; i < dimension; ++i)
      {
        marginalX.add(distribution.getMarginal(i));
        marginalZ.add(measure.getMarginal(i));
      }
      // The distributions have an independent copula, they can be converted one into another by marginal transformation. T is such that T(X) = Z
      const MarginalTransformationEvaluation evaluationT(MarginalTransformationEvaluation(marginalX, marginalZ));
      const MarginalTransformationGradient gradientT(evaluationT);
      const MarginalTransformationHessian hessianT(evaluationT);
      transformation = Function(evaluationT.clone(), gradientT.clone(), hessianT.clone());
    }
    // The third case is when both distributions share the same standard
    // distribution in which case the transformation is made of
    // isoprobabilistic transformations
    else
    {
      if (distribution.getStandardDistribution() == measure.getStandardDistribution())
      {
        LOGINFO("Same standard space for input vector and basis");
        // The distributions share the same standard space, it is thus possible to transform one into the other by composition between their isoprobabilistic transformations. T = T^{-1}_Z o T_X and T^{-1} = T^{-1}_X o T_Z
        const Function TX(distribution.getIsoProbabilisticTransformation());
        const Function TinvZ(measure.getInverseIsoProbabilisticTransformation());
        transformation = ComposedFunction(TinvZ, TX);
      }
      // The fourth and last case is when the standard spaces are different
      // We use the Rosenblatt transformation for each distribution with non-normal
      // standard space
      else
      {
        LOGINFO("Different standard space for input vector and basis");
        Function TX;
        Function invTX;
        if (distribution.getStandardDistribution().hasIndependentCopula())
        {
          LOGINFO("Normal standard space for input vector");
          TX = distribution.getIsoProbabilisticTransformation();
        }
        else
        {
          LOGINFO("Non-normal standard space for input vector");
          TX = Function(FunctionImplementation(RosenblattEvaluation(distribution.getImplementation()).clone()));
        }
        Function TZ;
        Function invTZ;
        if (measure.getStandardDistribution().hasIndependentCopula())
        {
          LOGINFO("Normal standard space for basis");
          invTZ = measure.getInverseIsoProbabilisticTransformation();
        }
        else
        {
          LOGINFO("Non-normal standard space for basis");
          invTZ = Function(FunctionImplementation(InverseRosenblattEvaluation(measure.getImplementation()).clone()));
        }
        transformation = ComposedFunction(invTZ, TX);
      } // Different standard spaces
    } // Non-independent input copula
  } // Transformation
  return transformation;
}


/* Parameter constructor */
DistributionTransformation DistributionTransformation::inverse() const
{
  return DistributionTransformation(right_, left_);
}

/* Comparison operator */
Bool DistributionTransformation::operator ==(const DistributionTransformation & ) const
{
  return true;
}

/* String converter */
String DistributionTransformation::__repr__() const
{
  OSS oss;
  oss << "class=" << DistributionTransformation::GetClassName()
      << " name=" << getName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

String DistributionTransformation::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
