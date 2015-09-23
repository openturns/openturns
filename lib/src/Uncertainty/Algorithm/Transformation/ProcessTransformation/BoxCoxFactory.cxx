//                                               -*- C++ -*-
/**
 *  @brief
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
#include "BoxCoxFactory.hxx"
#include "Exception.hxx"
#include "PersistentObjectFactory.hxx"
#include "Field.hxx"
#include "BoxCoxEvaluationImplementation.hxx"
#include "SpecFunc.hxx"
#include "algocobyla.h"
#include "Cobyla.hxx"
#include "Log.hxx"
#include "BoxCoxTransform.hxx"
#include "InverseBoxCoxTransform.hxx"
#include "Curve.hxx"
#include "Cloud.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BoxCoxFactory
 */

CLASSNAMEINIT(BoxCoxFactory);

static const Factory<BoxCoxFactory> RegisteredFactory;

/* Constructor with parameters */
BoxCoxFactory::BoxCoxFactory()
  : PersistentObject()
  , sample_()
  , sumLog_(0.0)
{
  // Nothing to do
}

/* Virtual constructor */
BoxCoxFactory * BoxCoxFactory::clone() const
{
  return new BoxCoxFactory(*this);
}

/* Compute the log-likelihood of the Box Cox transformation
   (\lambda-1)\sum\log(X_i)-\frac{n}{2}\log(\frac{n-1}{n}Var(T_\lambda(X_i)))
 */
NumericalScalar BoxCoxFactory::computeLogLikelihood(const NumericalScalar & lambda) const
{
  const UnsignedInteger size(sample_.getSize());
  BoxCoxEvaluationImplementation myBoxFunction(NumericalPoint(1, lambda));
  // compute the mean of the sample using the Box-Cox function
  const NumericalSample outSample(myBoxFunction(sample_));
  const NumericalScalar ratio(1.0 - 1.0 / size);
  const NumericalScalar sigma2(outSample.computeVariance()[0]);
  NumericalScalar result(-0.5 * size * log(sigma2 * ratio));

  //result is translated
  result += (lambda - 1.0) * sumLog_;
  return result;
}


/* objective function */
int BoxCoxFactory::ComputeObjectiveAndConstraint(int n,
    int m,
    double *x,
    double *f,
    double *con,
    void *state)
{
  const NumericalScalar lambda(x[0]);

  const BoxCoxFactory * factory = static_cast<BoxCoxFactory *>(state);
  *f = -factory->computeLogLikelihood( lambda );

  return 0;
}

/* Build the factory from data */
BoxCoxTransform BoxCoxFactory::build(const Field & timeSeries) const
{
  return build(timeSeries.getSample());
}

BoxCoxTransform BoxCoxFactory::build(const Field & timeSeries,
                                     const NumericalPoint & shift) const
{
  return build(timeSeries.getSample(), shift);
}

BoxCoxTransform BoxCoxFactory::build(const Field & timeSeries,
                                     const NumericalPoint & shift,
                                     Graph & graph) const
{
  return build(timeSeries.getSample(), shift, graph);
}

BoxCoxTransform BoxCoxFactory::build(const NumericalSample & sample) const
{
  return build(sample, NumericalPoint(sample.getDimension()));
}

BoxCoxTransform BoxCoxFactory::build(const NumericalSample & sample,
                                     const NumericalPoint & shift) const
{
  Graph tmp;
  return build(sample, shift, tmp);
}

BoxCoxTransform BoxCoxFactory::build(const NumericalSample & sample,
                                     const NumericalPoint & shift,
                                     Graph & graph) const
{
  // Check the input size
  const UnsignedInteger size(sample.getSize());
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a Box-Cox factory from an empty time series";

  // Check the input dimensions
  const UnsignedInteger dimension(sample.getDimension());
  if (shift.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the shift has a dimension=" << shift.getDimension() << " different from the sample dimension=" << dimension;

  // Shape parameters of the transformation
  NumericalPoint lambda(dimension);
  // Sum of the log-data
  NumericalPoint sumLog(dimension);

  // Currently the optimization is done sequently
  // cobyla rhobeg ==>  a reasonable initial change to the variables
  // cobyla rhoend ==> the required accuracy for the variables
  // if the cobyla will be the default method, then we must add these variables in the ResourceMap
  // maxfun ==> on input, the maximum number of function evaluations on output, the number of function evaluations done
  NumericalScalar rhoBeg(ResourceMap::GetAsNumericalScalar("BoxCoxFactory-DefaultRhoBeg"));
  NumericalScalar rhoEnd(ResourceMap::GetAsNumericalScalar("BoxCoxFactory-DefaultRhoEnd"));
  int maxFun(static_cast<int>(ResourceMap::GetAsNumericalScalar("BoxCoxFactory-DefaultMaxFun")));
  // Verbosity level : no msg
  cobyla_message message( COBYLA_MSG_NONE );
  // Keep the shifted marginal samples
  Collection< NumericalSample > marginalSamples(dimension);
  for (UnsignedInteger d = 0; d < dimension; ++d)
  {
    // Extract the marginal sample
    sample_ = sample.getMarginal(d);
    // Apply the shift
    sample_ += NumericalPoint(1, shift[d]);
    marginalSamples[d] = sample_;
    // number of optimized variables
    int n(1);
    // number of constraints
    int m(0);
    // starting point
    NumericalPoint x(n, 1.0);
    // Compute the sum of the log-data
    for (UnsignedInteger k = 0; k < size; ++k) sumLog[d] += log(sample_[k][0]);
    // Use
    sumLog_ = sumLog[d];
    // call cobyla algo
    int returnCode(cobyla( n, m, &x[0], rhoBeg, rhoEnd, message, &maxFun, BoxCoxFactory::ComputeObjectiveAndConstraint, (void*) this ));
    if (returnCode != 0) LOGWARN(OSS() << "Error solving maximum likelihood problem by cobyla method, message=" << cobyla_rc_string[returnCode - COBYLA_MINRC]);
    // Store the result
    lambda[d] = x[0];
  }
  // Graphical inspection
  graph = Graph("Box-Cox likelihood", "lambda", "log-likelihood", true, "topright");
  const NumericalScalar lambdaMax(*std::max_element(lambda.begin(), lambda.end()));
  const NumericalScalar lambdaMin(*std::min_element(lambda.begin(), lambda.end()));
  const NumericalScalar xMin(std::min(0.0, 0.002 * round(1000.0 * lambdaMin)));
  const NumericalScalar xMax(std::max(0.0, 0.002 * round(1000.0 * lambdaMax)));
  const UnsignedInteger npts(ResourceMap::GetAsUnsignedInteger("BoxCoxFactory-DefaultPointNumber"));
  NumericalSample lambdaValues(npts, 1);
  for (UnsignedInteger i = 0; i < npts; ++i) lambdaValues[i][0] = xMin + i * (xMax - xMin) / (npts - 1.0);
  for (UnsignedInteger d = 0; d < dimension; ++d)
  {
    NumericalSample logLikelihoodValues(npts, 1);
    sumLog_ = sumLog[d];
    sample_ = marginalSamples[d];
    for (UnsignedInteger i = 0; i < npts; ++i) logLikelihoodValues[i][0] = computeLogLikelihood(lambdaValues[i][0]);
    Curve curve(lambdaValues, logLikelihoodValues);
    curve.setColor(Curve::ConvertFromHSV((360.0 * d) / dimension, 1.0, 1.0));
    graph.add(curve);
    NumericalPoint optimum(2);
    optimum[0] = lambda[d];
    optimum[1] = computeLogLikelihood(optimum[0]);
    Cloud cloud(NumericalSample(1, optimum));
    cloud.setColor(curve.getColor());
    cloud.setPointStyle("circle");
    cloud.setLegend(String(OSS() << "lambda=" << lambda[d]));
    graph.add(cloud);
    NumericalPoint bb(graph.getBoundingBox());
    bb[3] += 0.1 * (bb[3] - bb[2]);
    graph.setBoundingBox(bb);
  }
  // return result ==> Box Cox transform
  return BoxCoxTransform(lambda, shift);
}

/* String converter */
String BoxCoxFactory::__repr__() const
{
  OSS oss;
  oss << "class=" << BoxCoxFactory::GetClassName();
  return oss;
}

/* String converter */
String BoxCoxFactory::__str__(const String & offset) const
{
  OSS oss;
  oss << "class=" << BoxCoxFactory::GetClassName();
  return oss;
}

END_NAMESPACE_OPENTURNS
