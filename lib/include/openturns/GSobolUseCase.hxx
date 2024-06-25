//                                               -*- C++ -*-
/**
 *  @brief The header file that defines the GSobol' Use Case
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_OTGSOBOLUSECASE_HXX
#define OPENTURNS_OTGSOBOLUSECASE_HXX

#include "openturns/OTconfig.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

class GSobolUseCase
{
public:
  /* Default constructor */
  GSobolUseCase(const UnsignedInteger & dimension, const Point & a)
    : dimension_(dimension)
    , a_(a)
    , mean_(1.0)
  {
    // Reference analytical values
    ;
    variance_ = 1.0;
    // Create the gSobol function
    Description inputVariables(dimension);
    Description formula(1);
    formula[0] = "1.0";
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      variance_ *= 1.0 + 1.0 / (3.0 * pow(1.0 + a[i], 2.0));
      inputVariables[i] = (OSS() << "xi" << i);
      formula[0] = (OSS() << formula[0] << " * (abs(4.0 * xi" << i << " - 2.0) + " << a[i] << ") / (1.0 + " << a[i] << ")");
    }
    --variance_;

    model_ = SymbolicFunction(inputVariables, formula);

    // Create the input distribution
    Collection<Distribution> marginals(dimension, Uniform(0.0, 1.0));
    inputDistribution_ = JointDistribution(marginals);
  }

  Function getModel()
  {
    return model_;
  }

  UnsignedInteger getDimension()
  {
    return dimension_;
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

  // Get the first order Sobol’ index of a single input variable or the
  // interaction (high order) index of a group of variables.
  Scalar computeSobolIndex(const Indices & indices)
  {
    Scalar value = 1.0;
    for (UnsignedInteger i = 0; i < indices.getSize(); ++i)
    {
      value *= 1.0 / (3.0 * pow(1.0 + a_[indices[i]], 2.0));
    }
    return value / variance_;
  }

private:
  Function model_;
  JointDistribution inputDistribution_;
  UnsignedInteger dimension_;
  Point a_;
  Scalar mean_;
  Scalar variance_;

}; /* class GSobolUseCase */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_OTGSOBOLUSECASE_HXX */
