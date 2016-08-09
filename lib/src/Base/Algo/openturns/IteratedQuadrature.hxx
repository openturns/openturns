//                                               -*- C++ -*-
/**
 *  @brief This class allows to compute integrals of a function over a
 *         domain defined by functions
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_ITERATEDQUADRATURE_HXX
#define OPENTURNS_ITERATEDQUADRATURE_HXX

#include "openturns/IntegrationAlgorithmImplementation.hxx"
#include "openturns/IntegrationAlgorithm.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IteratedQuadrature
 */

class OT_API IteratedQuadrature
  : public IntegrationAlgorithmImplementation
{

  CLASSNAME;

public:

  typedef Collection< NumericalMathFunction > NumericalMathFunctionCollection;

  /** Default constructor without parameters */
  IteratedQuadrature();

  /** Parameter constructor */
  IteratedQuadrature(const IntegrationAlgorithm & algorithm);

  /** Virtual copy constructor */
  virtual IteratedQuadrature * clone() const;

  /** Compute an approximation of \int_a^b\int_{L_1(x_1)}^{U_1(x_1)}\int_{L_1(x_1,x_2)}^{U_2(x_1,x_2)}\dots\int_{L_1(x_1,\dots,x_{n-1})}^{U_2(x_1,\dots,x_{n-1})} f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an 1D interval, L_k and U_k are functions from R^k into R.
   */
  using IntegrationAlgorithmImplementation::integrate;
  NumericalPoint integrate(const NumericalMathFunction & function,
                           const Interval & interval) const;

  // This method allows to get the estimated integration error as a scalar
  NumericalPoint integrate(const NumericalMathFunction & function,
                           const NumericalScalar a,
                           const NumericalScalar b,
                           const NumericalMathFunctionCollection & lowerBounds,
                           const NumericalMathFunctionCollection & upperBounds,
                           const Bool check = true) const;

  /** String converter */
  virtual String __repr__() const;

  /** String converter */
  virtual String __str__(const String & offset = "") const;

private:

  // Class to compute in a recursive way a multidimensional integral
  class PartialFunctionWrapper: public NumericalMathFunctionImplementation
  {
  public:
    /* Default constructor */
    PartialFunctionWrapper(const IteratedQuadrature & quadrature,
                           const NumericalMathFunction & function,
                           const IteratedQuadrature::NumericalMathFunctionCollection & lowerBounds,
                           const IteratedQuadrature::NumericalMathFunctionCollection & upperBounds)
      : NumericalMathFunctionImplementation()
      , quadrature_(quadrature)
      , function_(function)
      , lowerBounds_(lowerBounds)
      , upperBounds_(upperBounds)
    {
      // Nothing to do
    }

    NumericalPoint operator()(const NumericalPoint & point) const
    {
      // Create the arguments of the local integration problem
      const Indices index(1, 0);
      const NumericalMathFunction function(function_, index, point);
      const UnsignedInteger size = lowerBounds_.getSize() - 1;
      const NumericalScalar a = lowerBounds_[0](point)[0];
      const NumericalScalar b = upperBounds_[0](point)[0];
      IteratedQuadrature::NumericalMathFunctionCollection lowerBounds(size);
      IteratedQuadrature::NumericalMathFunctionCollection upperBounds(size);
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        lowerBounds[i] = NumericalMathFunction(lowerBounds_[i + 1], index, point);
        upperBounds[i] = NumericalMathFunction(upperBounds_[i + 1], index, point);
      }
      const NumericalPoint value(quadrature_.integrate(function, a, b, lowerBounds, upperBounds, false));
      for (UnsignedInteger i = 0; i < value.getDimension(); ++i)
        if (!SpecFunc::IsNormal(value[i])) throw InternalException(HERE) << "Error: NaN or Inf produced for x=" << point << " while integrating " << function;
      return value;
    }

    NumericalSample operator()(const NumericalSample & sample) const
    {
      const UnsignedInteger sampleSize = sample.getSize();
      const UnsignedInteger outputDimension = function_.getOutputDimension();
      const UnsignedInteger size = lowerBounds_.getSize() - 1;
      IteratedQuadrature::NumericalMathFunctionCollection lowerBounds(size);
      IteratedQuadrature::NumericalMathFunctionCollection upperBounds(size);
      NumericalSample result(sampleSize, outputDimension);
      const Indices index(1, 0);
      const NumericalSample sampleA(lowerBounds_[0](sample));
      const NumericalSample sampleB(upperBounds_[0](sample));
      for (UnsignedInteger k = 0; k < sampleSize; ++k)
      {
        const NumericalPoint x(sample[k]);
        // Create the arguments of the local integration problem
        const NumericalMathFunction function(function_, index, x);
        const NumericalScalar a = sampleA[k][0];
        const NumericalScalar b = sampleB[k][0];
        for (UnsignedInteger i = 0; i < size; ++i)
        {
          lowerBounds[i] = NumericalMathFunction(lowerBounds_[i + 1], index, x);
          upperBounds[i] = NumericalMathFunction(upperBounds_[i + 1], index, x);
        } // Loop over bound functions
        result[k] = quadrature_.integrate(function, a, b, lowerBounds, upperBounds, false);
        for (UnsignedInteger i = 0; i < outputDimension; ++i)
          if (!SpecFunc::IsNormal(result[k][i])) throw InternalException(HERE) << "Error: NaN or Inf produced for x=" << x << " while integrating " << function;
      } // Loop over sample points
      return result;
    }

    PartialFunctionWrapper * clone() const
    {
      return new PartialFunctionWrapper(*this);
    }

    UnsignedInteger getInputDimension() const
    {
      return 1;
    }

    UnsignedInteger getOutputDimension() const
    {
      return function_.getOutputDimension();
    }

    Description getInputDescription() const
    {
      return Description(1, "t");
    }

    Description getOutputDescription() const
    {
      return function_.getOutputDescription();
    }

  private:
    const IteratedQuadrature & quadrature_;
    const NumericalMathFunction & function_;
    const IteratedQuadrature::NumericalMathFunctionCollection & lowerBounds_;
    const IteratedQuadrature::NumericalMathFunctionCollection & upperBounds_;
  }; // class PartialFunctionWrapper

  /* Underlying integration algorithm */
  IntegrationAlgorithm algorithm_;

} ; /* class IteratedQuadrature */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATEDQUADRATURE_HXX */
