//                                               -*- C++ -*-
/**
 *  @brief Factory for LogNormal distribution
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
#ifndef OPENTURNS_LOGNORMALFACTORY_HXX
#define OPENTURNS_LOGNORMALFACTORY_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/LogNormal.hxx"
#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LogNormalFactory
 */
class OT_API LogNormalFactory
  : public DistributionFactoryImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  LogNormalFactory();

  /** Virtual constructor */
  LogNormalFactory * clone() const override;

  /* Here is the interface that all derived class must implement */
  using DistributionFactoryImplementation::build;

  Distribution build(const Sample & sample) const override;
  Distribution build(const Sample & sample,
                     const UnsignedInteger method) const;
  Distribution build(const Point & parameters) const override;
  Distribution build() const override;
  LogNormal buildAsLogNormal(const Sample & sample) const;
  LogNormal buildAsLogNormal(const Sample & sample,
                             const UnsignedInteger method) const;
  LogNormal buildAsLogNormal(const Point & parameters) const;
  LogNormal buildAsLogNormal() const;

  /** Algorithm associated with the method of moments */
  LogNormal buildMethodOfMoments(const Sample & sample) const;

  /** Algoritm associated with the method of local likelihood maximization */
  LogNormal buildMethodOfLocalLikelihoodMaximization(const Sample & sample) const;

  /** Algorithm associated with the method of modified moments */
  LogNormal buildMethodOfModifiedMoments(const Sample & sample) const;

  /** Algorithm associated with the method of least-squares */
  LogNormal buildMethodOfLeastSquares(const Sample & sample) const;

private:
  // Helper class to maximize the local likelihood function
  class LogNormalFactoryLMLEParameterConstraint : public EvaluationImplementation
  {
  public:
    // Constructor from a GLM algorithm
    LogNormalFactoryLMLEParameterConstraint(const Sample &sample)
      : EvaluationImplementation(), sample_(sample), size_(sample.getSize())
    {
      // Nothing to do
    }

    LogNormalFactoryLMLEParameterConstraint *clone() const override
    {
      return new LogNormalFactoryLMLEParameterConstraint(*this);
    }

    // It is a simple call to the computeReducedLogLikelihood() of the algo
    Point operator()(const Point &point) const override
    {
      const Scalar gamma = point[0];
      const Point sums(computeMaximumLikelihoodSums(gamma));
      return Point(1, sums[0] * (sums[2] - sums[1] * (1.0 + sums[1] / size_)) + size_ * sums[3]);
    }

    /*
        S_0 = \sum_i (X_i - \gamma)^{-1}
        S_1 = \sum_i \log(X_i - \gamma)
        S_2 = \sum_i \log^2(X_i - \gamma)
        S_3 = \sum_i \log(X_i - \gamma) / (X_i - \gamma)
      */
    Point computeMaximumLikelihoodSums(const Scalar gamma) const
    {
      Point sums(4, 0.0);
      for (UnsignedInteger i = 0; i < size_; ++i)
      {
        const Scalar delta = sample_(i, 0) - gamma;
        if (!(delta > 0.0))
          throw InvalidArgumentException(HERE) << "Error: cannot estimate a LogNormal distribution based on the given sample using the method of local maximum likelihood, probably because the sample is constant.";
        const Scalar logDelta = std::log(delta);
        const Scalar inverseDelta = 1.0 / delta;
        sums[0] += inverseDelta;
        sums[1] += logDelta;
        sums[2] += logDelta * logDelta;
        sums[3] += logDelta * inverseDelta;
      }
      return sums;
    }

    UnsignedInteger getInputDimension() const override
    {
      return 1;
    }

    UnsignedInteger getOutputDimension() const override
    {
      return 1;
    }

    Description getInputDescription() const override
    {
      return Description(1, "gamma");
    }

    Description getOutputDescription() const override
    {
      return Description(1, "LocalMaximumLikelhood");
    }

    Description getDescription() const override
    {
      Description description(getInputDescription());
      description.add(getOutputDescription());
      return description;
    }

    String __repr__() const override
    {
      OSS oss;
      // Don't print algorithm_ here as it will result in an infinite loop!
      oss << "LogNormalFactoryLMLEParameterConstraint";
      return oss;
    }

    String __str__(const String &offset = "") const override
    {
      // Don't print algorithm_ here as it will result in an infinite loop!
      return OSS() << offset << __repr__();
    }

  private:
    const Sample &sample_;
    const UnsignedInteger size_;
  }; // LogNormalFactoryLMLEParameterConstraint

  LogNormal buildMethodOfLeastSquares(const Sample & sample,
                                      const Scalar gamma) const;

}; /* class LogNormalFactory */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOGNORMALFACTORY_HXX */
