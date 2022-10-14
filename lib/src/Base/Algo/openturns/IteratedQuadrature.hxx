//                                               -*- C++ -*-
/**
 *  @brief This class allows one to compute integrals of a function over a
 *         domain defined by functions
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ITERATEDQUADRATURE_HXX
#define OPENTURNS_ITERATEDQUADRATURE_HXX

#include "openturns/IntegrationAlgorithmImplementation.hxx"
#include "openturns/IntegrationAlgorithm.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/ParametricFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IteratedQuadrature
 */

class OT_API IteratedQuadrature
  : public IntegrationAlgorithmImplementation
{

  CLASSNAME

public:

  typedef Collection< Function > FunctionCollection;

  /** Default constructor without parameters */
  IteratedQuadrature();

  /** Parameter constructor */
  IteratedQuadrature(const IntegrationAlgorithm & algorithm);

  /** Virtual copy constructor */
  IteratedQuadrature * clone() const override;

  /** Compute an approximation of \int_a^b\int_{L_1(x_1)}^{U_1(x_1)}\int_{L_1(x_1,x_2)}^{U_2(x_1,x_2)}\dots\int_{L_1(x_1,\dots,x_{n-1})}^{U_2(x_1,\dots,x_{n-1})} f(x_1,\dots,x_n)dx_1\dotsdx_n, where [a,b] is an 1D interval, L_k and U_k are functions from R^k into R.
   */
  using IntegrationAlgorithmImplementation::integrate;
  Point integrate(const Function & function,
                  const Interval & interval) const override;

  // This method allows one to get the estimated integration error as a scalar
  Point integrate(const Function & function,
                  const Scalar a,
                  const Scalar b,
                  const FunctionCollection & lowerBounds,
                  const FunctionCollection & upperBounds,
                  const Bool check = true) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;
private:

  // Class to compute in a recursive way a multidimensional integral
  class PartialFunctionWrapper: public FunctionImplementation
  {
  public:
    /* Default constructor */
    PartialFunctionWrapper(const IteratedQuadrature & quadrature,
                           const Function & function,
                           const IteratedQuadrature::FunctionCollection & lowerBounds,
                           const IteratedQuadrature::FunctionCollection & upperBounds)
      : FunctionImplementation()
      , quadrature_(quadrature)
      , function_(function)
      , lowerBounds_(lowerBounds)
      , upperBounds_(upperBounds)
    {
      // Nothing to do
    }

    Point operator()(const Point & point) const override
    {
      // Create the arguments of the local integration problem
      const Indices index(1, 0);
      const ParametricFunction function(function_, index, point);
      const UnsignedInteger size = lowerBounds_.getSize() - 1;
      const Scalar a = lowerBounds_[0](point)[0];
      const Scalar b = upperBounds_[0](point)[0];
      IteratedQuadrature::FunctionCollection lowerBounds(size);
      IteratedQuadrature::FunctionCollection upperBounds(size);
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        lowerBounds[i] = ParametricFunction(lowerBounds_[i + 1], index, point);
        upperBounds[i] = ParametricFunction(upperBounds_[i + 1], index, point);
      }
      const Point value(quadrature_.integrate(function, a, b, lowerBounds, upperBounds, false));
      for (UnsignedInteger i = 0; i < value.getDimension(); ++i)
        if (!SpecFunc::IsNormal(value[i])) throw InternalException(HERE) << "Error: NaN or Inf produced for x=" << point << " while integrating " << function;
      return value;
    }

    Sample operator()(const Sample & sample) const override
    {
      const UnsignedInteger sampleSize = sample.getSize();
      const UnsignedInteger outputDimension = function_.getOutputDimension();
      const UnsignedInteger size = lowerBounds_.getSize() - 1;
      IteratedQuadrature::FunctionCollection lowerBounds(size);
      IteratedQuadrature::FunctionCollection upperBounds(size);
      Sample result(sampleSize, outputDimension);
      const Indices index(1, 0);
      const Sample sampleA(lowerBounds_[0](sample));
      const Sample sampleB(upperBounds_[0](sample));
      for (UnsignedInteger k = 0; k < sampleSize; ++k)
      {
        const Point x(sample[k]);
        // Create the arguments of the local integration problem
        const ParametricFunction function(function_, index, x);
        const Scalar a = sampleA(k, 0);
        const Scalar b = sampleB(k, 0);
        for (UnsignedInteger i = 0; i < size; ++i)
        {
          lowerBounds[i] = ParametricFunction(lowerBounds_[i + 1], index, x);
          upperBounds[i] = ParametricFunction(upperBounds_[i + 1], index, x);
        } // Loop over bound functions
        result[k] = quadrature_.integrate(function, a, b, lowerBounds, upperBounds, false);
        for (UnsignedInteger i = 0; i < outputDimension; ++i)
          if (!SpecFunc::IsNormal(result(k, i))) throw InternalException(HERE) << "Error: NaN or Inf produced for x=" << x << " while integrating " << function;
      } // Loop over sample points
      return result;
    }

    PartialFunctionWrapper * clone() const override
    {
      return new PartialFunctionWrapper(*this);
    }

    UnsignedInteger getInputDimension() const override
    {
      return 1;
    }

    UnsignedInteger getOutputDimension() const override
    {
      return function_.getOutputDimension();
    }

    Description getInputDescription() const override
    {
      return Description(1, "t");
    }

    Description getOutputDescription() const override
    {
      return function_.getOutputDescription();
    }

  private:
    const IteratedQuadrature & quadrature_;
    const Function & function_;
    const IteratedQuadrature::FunctionCollection & lowerBounds_;
    const IteratedQuadrature::FunctionCollection & upperBounds_;
  }; // class PartialFunctionWrapper

  /* Underlying integration algorithm */
  IntegrationAlgorithm algorithm_;

} ; /* class IteratedQuadrature */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ITERATEDQUADRATURE_HXX */
