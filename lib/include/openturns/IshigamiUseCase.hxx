//                                               -*- C++ -*-
/**
 *  @brief The header file that defines the Ishigami Use Case
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_OTISHIGAMIUSECASE_HXX
#define OPENTURNS_OTISHIGAMIUSECASE_HXX

#define _USE_MATH_DEFINES
#include <cmath>          // For M_PI

#include "openturns/OTconfig.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

class IshigamiUseCase
{
public:
  /* Default constructor */
  IshigamiUseCase()
  {
    // Create the Ishigami function
    Description inputVariables = {"xi1", "xi2", "xi3"};
    Description formula(1);
    formula[0] = (OSS() << "sin(xi1) + (" << a_ << ") * (sin(xi2)) ^ 2 + (" << b_ << ") * xi3^4 * sin(xi1)");
    model_ = SymbolicFunction(inputVariables, formula);

    // Create the input distribution
    Collection<Distribution> marginals(dimension_, Uniform(-M_PI, M_PI));
    inputDistribution_ = JointDistribution(marginals);

    // Mean, variance
    mean_ = a_ / 2;
    variance_ = pow(b_, 2.0) * pow(M_PI, 8.0) / 18.0 \
                + b_ * pow(M_PI, 4.0) / 5.0 + pow(a_, 2.0) / 8.0 + 1.0 / 2.0;
    // Sobol' indices
    // First order Sobol' index of X1
    s1_ = (b_ * pow(M_PI, 4.0) / 5.0 + pow(b_, 2.0) * pow(M_PI, 8.0) / 50.0 \
           + 1.0 / 2.0) /        variance_;
    s2_ = (pow(a_, 2.0) / 8.0) / variance_;  // First order of X2
    s3_ = 0.0;  // First order of X3
    s12_ = 0.0;  // Interaction of (X1, X2)
    // Interaction of (X1, X3)
    s13_ = pow(b_, 2.0) * pow(M_PI, 8.0) * (1.0 / 9.0 - 1.0 / 25.0) / 2.0 / variance_;
    s23_ = 0.0;  // Interaction of (X2, X3)
    s123_ = 0.0;  // Interaction of (X1, X2, X3)
    sT1_ = s1_ + s13_;  // Total of X1
    sT2_ = s2_;  // Total of X2
    sT3_ = s3_ + s13_;  // Total of X3
  }

  Function getModel()
  {
    return model_;
  }

  UnsignedInteger getDimension()
  {
    return dimension_;
  }

  Scalar getA()
  {
    return a_;
  }

  Scalar getB()
  {
    return b_;
  }

  JointDistribution getInputDistribution()
  {
    return inputDistribution_;
  }

  Scalar getMean()
  {
    return mean_;
  }

  Scalar getVariance()
  {
    return variance_;
  }

  Point getFirstOrderSobolIndices()
  {
    const Point firstOrderIndices({s1_, s2_, s3_});
    return firstOrderIndices;
  }

  Point getTotalSobolIndices()
  {
    const Point totalIndices({sT1_, sT2_, sT3_});
    return totalIndices;
  }

  Scalar getFirstOrderInteractionSobolIndex(const Indices indices)
  {
    if (!indices.isIncreasing())
      throw InvalidArgumentException(HERE) << "Provided quantiles are not increasing";
    Scalar value = -1.0;
    if (indices.getSize() == 2)
    {
      if (indices[0] == 0 && indices[1] == 1) value = s12_;
      else if (indices[0] == 0 && indices[1] == 2) value = s13_;
      else if (indices[0] == 1 && indices[1] == 2) value = s23_;
      else
        throw InvalidArgumentException(HERE) << "Inconsistent 2D indices = " << indices;
    }
    else if (indices.getSize() == 3)
      if (indices[0] == 0 && indices[1] == 1 && indices[2] == 2) value = s123_;
      else
        throw InvalidArgumentException(HERE) << "Inconsistent 3D indices = " << indices;
    else
      throw InvalidArgumentException(HERE) << "Inconsistent dimension of indices = " << indices.getSize();
    return value;
  }

  Scalar getTotalInteractionSobolIndex(const Indices indices)
  {
    if (!indices.isIncreasing())
      throw InvalidArgumentException(HERE) << "Provided quantiles are not increasing";
    Scalar value = -1.0;
    if (indices.getSize() == 2)
    {
      if (indices[0] == 0 && indices[1] == 1) value = s12_ + s123_;
      else if (indices[0] == 0 && indices[1] == 2) value = s13_ + s123_;
      else if (indices[0] == 1 && indices[1] == 2) value = s23_ + s123_;
      else
        throw InvalidArgumentException(HERE) << "Inconsistent 2D indices = " << indices;
    }
    else if (indices.getSize() == 3)
      if (indices[0] == 0 && indices[1] == 1 && indices[2] == 2) value = s123_;
      else
        throw InvalidArgumentException(HERE) << "Inconsistent 3D indices = " << indices;
    else
      throw InvalidArgumentException(HERE) << "Inconsistent dimension of indices = " << indices.getSize();
    return value;
  }

private:
  Function model_;
  JointDistribution inputDistribution_;
  UnsignedInteger dimension_ = 3;
  Scalar a_ = 7.0;
  Scalar b_ = 0.1;
  Scalar mean_;
  Scalar variance_;
  Scalar s1_;
  Scalar s2_;
  Scalar s3_;
  Scalar s12_;
  Scalar s13_;
  Scalar s23_;
  Scalar s123_;
  Scalar sT1_;
  Scalar sT2_;
  Scalar sT3_;

}; /* class IshigamiUseCase */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OTISHIGAMIUSECASE_HXX */
