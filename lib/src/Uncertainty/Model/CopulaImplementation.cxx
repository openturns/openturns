//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Continuous distributions
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

#include <cmath>
#include "openturns/CopulaImplementation.hxx"
#include "openturns/IndependentCopula.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Brent.hxx"
#include "openturns/Curve.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/Combinations.hxx"
#include "openturns/ComposedDistribution.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/IteratedQuadrature.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CopulaImplementation);

/* Default constructor */
CopulaImplementation::CopulaImplementation()
  : ContinuousDistribution()
{
  // Set the flag isCopula_ here to avoid warnings on the initialization order
  isCopula_ = true;
}

/* Virtual constructor */
CopulaImplementation * CopulaImplementation::clone() const
{
  return new CopulaImplementation(*this);
}

/* Comparison operator */
Bool CopulaImplementation::operator ==(const CopulaImplementation & other) const
{
  return (this != &other);
}

/* String converter */
String CopulaImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << CopulaImplementation::GetClassName();
  return oss;
}

NumericalScalar CopulaImplementation::computeSurvivalFunction(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (dimension == 1) return computeComplementaryCDF(point);
  Bool allOutside(true);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      if (point[i] >= 1.0) return 0.0;
      allOutside &= (point[i] <= 0.0);
    }
  if (allOutside) return 1.0;
  // Use Poincaré's formula
  NumericalScalar value(1.0 + (1 - 2 * (dimension % 2)) * computeCDF(point));
  // We know the explicit value of the 1D marginal distributions
  for (UnsignedInteger i = 0; i < dimension; ++i) value -= point[i];
  NumericalScalar sign(1.0);
  for (UnsignedInteger i = 2; i < dimension - 1; ++i)
    {
      NumericalScalar contribution(0.0);
      Combinations::IndicesCollection indices(Combinations(i, dimension).generate());
      NumericalPoint subPoint(i);
      for (UnsignedInteger j = 0; j < indices.getSize(); ++j)
        {
          for (UnsignedInteger k = 0; k < i; ++k) subPoint[k] = point[indices[j][k]];
          contribution += getMarginal(indices[j])->computeCDF(subPoint);
        }
      value += sign * contribution;
      sign = -sign;
    }
  return value;
}

/* Generic implementation of the quantile computation for copulas */
NumericalPoint CopulaImplementation::computeQuantile(const NumericalScalar prob,
                                                     const Bool tail) const
{
  const UnsignedInteger dimension(getDimension());
  // Special case for bording values
  const NumericalScalar q(tail ? 1.0 - prob : prob);
  if (q <= 0.0) return NumericalPoint(dimension, 0.0);
  if (q >= 1.0) return NumericalPoint(dimension, 1.0);
  // Special case for dimension 1
  if (dimension == 1) return NumericalPoint(1, q);
  QuantileWrapper wrapper(this);
  const NumericalMathFunction f(bindMethod<QuantileWrapper, NumericalPoint, NumericalPoint>(wrapper, &QuantileWrapper::computeDiagonal, 1, 1));
  NumericalScalar leftTau(q);
  const NumericalPoint leftPoint(1, leftTau);
  const NumericalPoint leftValue(f(leftPoint));
  NumericalScalar leftCDF(leftValue[0]);
  // Upper bound of the bracketing interval
  NumericalScalar rightTau(1.0 - (1.0 - q) / dimension);
  NumericalPoint rightPoint(1, rightTau);
  const NumericalPoint rightValue(f(rightPoint));
  NumericalScalar rightCDF(rightValue[0]);
  // Use Brent's method to compute the quantile efficiently
  Brent solver(cdfEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_);
  return NumericalPoint(dimension, solver.solve(f, q, leftTau, rightTau, leftCDF, rightCDF));
}


/* Get the mean of the copula */
NumericalPoint CopulaImplementation::getMean() const
{
  return NumericalPoint(getDimension(), 0.5);
}

/* Get the standard deviation of the copula */
NumericalPoint CopulaImplementation::getStandardDeviation() const
{
  // 0.2886751345948128822545744 = 1 / sqrt(12)
  return NumericalPoint(getDimension(), 0.2886751345948128822545744);
}

/* Get the Spearman correlation of the copula */
CorrelationMatrix CopulaImplementation::getSpearmanCorrelation() const
{
  return getLinearCorrelation();
}

struct CopulaImplementationKendallTauWrapper
{
  CopulaImplementationKendallTauWrapper(const DistributionImplementation::Implementation & distribution)
    : p_distribution_(distribution)
  {
    // Nothing to do
  }

  NumericalPoint kernel(const NumericalPoint & point) const
  {
    return NumericalPoint(1, p_distribution_->computeCDF(point) * p_distribution_->computePDF(point));
  }

  const DistributionImplementation::Implementation & p_distribution_;
};

/* Get the Kendall concordance of the copula */
CorrelationMatrix CopulaImplementation::getKendallTau() const
{
  const UnsignedInteger dimension(getDimension());
  CorrelationMatrix tau(dimension);
  if (hasIndependentCopula()) return tau;
  // Here we have a circular dependency between copulas and distributions
  // if (hasEllipticalCopula())
  //   {
  //     const CorrelationMatrix shape(getShapeMatrix());
  //     for (UnsignedInteger i = 0; i < dimension; ++i)
  //       for(UnsignedInteger j = 0; j < i; ++j)
  //         tau(i, j) = std::asin(shape(i, j)) * (2.0 / M_PI);
  //     return tau;
  //   }
  const IteratedQuadrature integrator = IteratedQuadrature(GaussKronrod());
  const Interval square(NumericalPoint(2, 0.0), NumericalPoint(2, 1.0));
  // Performs the integration in the strictly lower triangle of the tau matrix
  Indices indices(2);
  for(UnsignedInteger rowIndex = 0; rowIndex < dimension_; ++rowIndex)
    {
      indices[0] = rowIndex;
      for (UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension_; ++columnIndex)
        {
          indices[1] = columnIndex;
          const Implementation marginalDistribution(getMarginal(indices));
          if (!marginalDistribution->hasIndependentCopula())
            {
              // Build the integrand
              const CopulaImplementationKendallTauWrapper functionWrapper(marginalDistribution);
              const NumericalMathFunction function(bindMethod<CopulaImplementationKendallTauWrapper, NumericalPoint, NumericalPoint>(functionWrapper, &CopulaImplementationKendallTauWrapper::kernel, 2, 1));
              tau(rowIndex, columnIndex) = integrator.integrate(function, square)[0];
            }
        } // loop over column indices
    } // loop over row indices
  return tau;
}

/* Get the skewness of the copula */
NumericalPoint CopulaImplementation::getSkewness() const
{
  return NumericalPoint(getDimension(), 0.0);
}

/* Get the kurtosis of the copula */
NumericalPoint CopulaImplementation::getKurtosis() const
{
  // 1.8 = 9/5
  return NumericalPoint(getDimension(), 1.8);
}

struct CopulaImplementationCovarianceWrapper
{
  CopulaImplementationCovarianceWrapper(const Distribution & distribution)
    : distribution_(distribution)
  {
    // Nothing to do
  }

  NumericalPoint kernel(const NumericalPoint & point) const
  {
    return NumericalPoint(1, distribution_.computeCDF(point) - point[0] * point[1]);
  }

  const Distribution & distribution_;
};

/* Compute the covariance of the copula */
void CopulaImplementation::computeCovariance() const
{
  const UnsignedInteger dimension(getDimension());
  // We need this to initialize the covariance matrix in two cases:
  // + this is the first call to this routine (which could be checked by testing the dimension of the copula and the dimension of the matrix
  // + the copula has changed from a non-independent one to the independent copula
  covariance_ = CovarianceMatrix(dimension);
  // First the diagonal terms, which are the marginal covariances
  // Uniform marginals, the diagonal is 1/12
  for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      // 0.08333333333333333333333333 = 1 / 12
      covariance_(i, i) = 0.08333333333333333333333333;
    }
  // Off-diagonal terms if the copula is not the independent copula
  if (!hasIndependentCopula())
    {
      const IteratedQuadrature integrator;
      const Interval unitSquare(NumericalPoint(2, 0.0), NumericalPoint(2, 1.0));
      // Performs the integration for each covariance in the strictly lower triangle of the covariance matrix
      // We start with the loop over the coefficients because the most expensive task is to get the 2D marginal copulas
      Indices indices(2);
      for(UnsignedInteger rowIndex = 0; rowIndex < dimension; ++rowIndex)
        {
          indices[0] = rowIndex;
          for(UnsignedInteger columnIndex = rowIndex + 1; columnIndex < dimension; ++columnIndex)
            {
              indices[1] = columnIndex;
              // For the usual case of a bidimensional copula, no need to extract marginal distributions
              Distribution marginalDistribution(*this);
              if (dimension > 2) marginalDistribution = getMarginal(indices);
              if (!marginalDistribution.getImplementation()->hasIndependentCopula())
                {
                  // Build the integrand
                  CopulaImplementationCovarianceWrapper functionWrapper(marginalDistribution);
                  NumericalMathFunction function(bindMethod<CopulaImplementationCovarianceWrapper, NumericalPoint, NumericalPoint>(functionWrapper, &CopulaImplementationCovarianceWrapper::kernel, 2, 1));
                  // Compute the covariance element
                  covariance_(rowIndex, columnIndex) = integrator.integrate(function, unitSquare)[0];
                }
            } // loop over column indices
        } // loop over row indices
    } // if !hasIndependentCopula
  isAlreadyComputedCovariance_ = true;
} // computeCovariance

/* Get the i-th marginal distribution */
CopulaImplementation::Implementation CopulaImplementation::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  return new IndependentCopula(1);
}

/* Get the copula of a distribution */
CopulaImplementation::Implementation CopulaImplementation::getCopula() const
{
  return clone();
}

/* Compute the mathematical and numerical range of the copula.
   Its mathematical range is the smallest closed interval outside
   of which the PDF is zero, and the numerical range is the interval
   outside of which the PDF is rounded to zero in double precision */
void CopulaImplementation::computeRange()
{
  setRange(Interval(getDimension()));
}

END_NAMESPACE_OPENTURNS
