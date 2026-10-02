//                                               -*- C++ -*-
/**
 *  @brief Implicit leave-one-out cross validation
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LeaveOneOut.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LeaveOneOut)

static const Factory<LeaveOneOut> Factory_LeaveOneOut;

/* Default constructor */
LeaveOneOut::LeaveOneOut()
  : FittingAlgorithmImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
LeaveOneOut * LeaveOneOut::clone() const
{
  return new LeaveOneOut( *this );
}

/* String converter */
String LeaveOneOut::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}

/* Perform cross-validation */
Scalar LeaveOneOut::run(const Sample & x,
                        const Sample & y,
                        const Point & weight,
                        const FunctionCollection & basis,
                        const Indices & indices) const
{
  return FittingAlgorithmImplementation::run(x, y, weight, basis, indices);
}


Scalar LeaveOneOut::run(const Sample & y,
                        const Point & weight,
                        const Indices & indices,
                        const DesignProxy & proxy) const
{
  return FittingAlgorithmImplementation::run(y, weight, indices, proxy);
}


Scalar LeaveOneOut::run(LeastSquaresMethod & method, const Sample & y) const
{
  const Sample x(method.getInputSample());
  const UnsignedInteger sampleSize = x.getSize();

  if (y.getDimension() != 1) throw InvalidArgumentException(HERE) << "Output sample should be unidimensional (dim=" << y.getDimension() << ").";
  if (y.getSize() != sampleSize) throw InvalidArgumentException(HERE) << "Samples should be equally sized (in=" << sampleSize << " out=" << y.getSize() << ").";
  // Weights of the least-squares method: a single value means uniform
  // weights, otherwise one value per sample point
  const Point methodWeights(method.getWeight());
  const Bool useUniformWeights = (methodWeights.getSize() == 1);
  if (!useUniformWeights && (methodWeights.getSize() != sampleSize)) throw InvalidArgumentException(HERE) << "Non-uniform weights size (" << methodWeights.getSize() << ") should match the sample size (" << sampleSize << ").";
  // Total weight mass, used to normalize the error and variance below so
  // that they converge to their continuous counterparts when the sample
  // size goes to infinity. For uniform weights it is w * n.
  Scalar weightSum = 0.0;
  if (useUniformWeights)
    weightSum = methodWeights[0] * sampleSize;
  else
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
      weightSum += methodWeights[i];
  if (!(weightSum > 0.0)) throw InvalidArgumentException(HERE) << "Weights must have a positive sum, here sum=" << weightSum;
  // Weighted output variance around the weighted mean
  Scalar weightedMean = 0.0;
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
    weightedMean += (useUniformWeights ? methodWeights[0] : methodWeights[i]) * y(i, 0);
  weightedMean /= weightSum;
  Scalar variance = 0.0;
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    const Scalar delta = y(i, 0) - weightedMean;
    variance += (useUniformWeights ? methodWeights[0] : methodWeights[i]) * delta * delta;
  }
  variance /= weightSum;

  const UnsignedInteger basisSize = method.getImplementation()->currentIndices_.getSize();
  if (!(sampleSize >= basisSize)) throw InvalidArgumentException(HERE) << "Not enough samples (" << sampleSize << ") required (" << basisSize << ")";

  // Build the design of experiments
  LOGINFO("Build the design matrix");

  const Matrix psiAk(method.computeDesign());

  // Solve the least squares problem argmin ||psiAk * coefficients - b||^2 using this decomposition
  LOGINFO("Solve the least squares problem");

  // Use the equivalence between SampleImplementation::data_ and Point
  const Point coefficients(method.solve(y.getImplementation()->getData()));

  // Compute the leave-one-out error through the PRESS residuals
  LOGINFO("Compute the leave-one-out error");

  const Point yHat(psiAk * coefficients);

  // Weighted leverages: dropping an observation is a rank-one downdate of
  // the weighted normal equations, so the leave-one-out residual is
  // r_i / (1 - h_i) for any weights
  const Point h(method.getHDiag());
  Scalar looError = 0.0;
  for (UnsignedInteger i = 0; i < sampleSize; ++ i)
  {
    const Scalar ns = (y(i, 0) - yHat[i]) / (1.0 - h[i]);
    looError += (useUniformWeights ? methodWeights[0] : methodWeights[i]) * ns * ns;
  }
  looError /= weightSum;
  LOGINFO(OSS() << "LOO error=" << looError);

  const Scalar relativeError = (!(variance > 0.0) ? 0.0 : looError / variance);
  LOGINFO(OSS() << "Relative error=" << relativeError);
  return relativeError;
}

/* Method save() stores the object through the StorageManager */
void LeaveOneOut::save(Advocate & adv) const
{
  FittingAlgorithmImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void LeaveOneOut::load(Advocate & adv)
{
  FittingAlgorithmImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
