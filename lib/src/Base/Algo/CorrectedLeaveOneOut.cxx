//                                               -*- C++ -*-
/**
 *  @brief Corrected implicit leave-one-out cross validation
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

#include "openturns/Function.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/CorrectedLeaveOneOut.hxx"
#include "openturns/SVDMethod.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CorrectedLeaveOneOut)

static const Factory<CorrectedLeaveOneOut> Factory_CorrectedLeaveOneOut;

/* Default constructor */
CorrectedLeaveOneOut::CorrectedLeaveOneOut()
  : FittingAlgorithmImplementation()
{
  // Nothing to do
}

/* Virtual constructor */
CorrectedLeaveOneOut * CorrectedLeaveOneOut::clone() const
{
  return new CorrectedLeaveOneOut( *this );
}

/* String converter */
String CorrectedLeaveOneOut::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}

/* Perform cross-validation */
Scalar CorrectedLeaveOneOut::run(const Sample & x,
                                 const Sample & y,
                                 const Point & weight,
                                 const FunctionCollection & basis,
                                 const Indices & indices) const
{
  return FittingAlgorithmImplementation::run(x, y, weight, basis, indices);
}


Scalar CorrectedLeaveOneOut::run(const Sample & y,
                                 const Point & weight,
                                 const Indices & indices,
                                 const DesignProxy & proxy) const
{
  return FittingAlgorithmImplementation::run(y, weight, indices, proxy);
}

Scalar CorrectedLeaveOneOut::run(LeastSquaresMethod & method,
                                 const Sample & y) const
{
  const Sample x(method.getInputSample());

  const UnsignedInteger sampleSize = y.getSize();

  if (y.getDimension() != 1) throw InvalidArgumentException(HERE) << "Output sample should be unidimensional (dim=" << y.getDimension() << ").";
  if (y.getSize() != sampleSize) throw InvalidArgumentException(HERE) << "Samples should be equally sized (in=" << sampleSize << " out=" << y.getSize() << ").";
  const Scalar variance = y.computeVariance()[0];

  const UnsignedInteger basisSize = method.getImplementation()->currentIndices_.getSize();
  if (!(sampleSize >= basisSize)) throw InvalidArgumentException(HERE) << "Not enough samples (" << sampleSize << ") required (" << basisSize << ")";

  // Build the design of experiments
  LOGINFO("Build the design matrix");

  const Matrix psiAk(method.computeWeightedDesign());

  // Solve the least squares problem argmin ||psiAk * coefficients - b||^2 using this decomposition
  LOGINFO("Solve the least squares problem");

  // Use the equivalence between SampleImplementation::data_ and Point
  const Point coefficients(method.solve(y.getImplementation()->getData()));

  // Compute the empirical error
  LOGINFO("Compute the empirical error");

  const Point yHat(psiAk * coefficients);

  const Point h(method.getHDiag());
  Scalar empiricalError = 0.0;
  for (UnsignedInteger i = 0; i < sampleSize; ++ i)
  {
    const Scalar ns = (y(i, 0) - yHat[i]) / (1.0 - h[i]);
    empiricalError += ns * ns / sampleSize;
  }
  LOGINFO(OSS() << "Empirical error=" << empiricalError);

  LOGINFO("Compute the correcting factor");
  const Scalar traceInverse = method.getGramInverseTrace();

  const Scalar correctingFactor = (1.0 * sampleSize) / (sampleSize - basisSize) * (1.0 + traceInverse);
  const Scalar relativeError = (!(variance > 0.0) ? 0.0 : correctingFactor * empiricalError / variance);
  LOGINFO(OSS() << "Relative error=" << relativeError);
  return relativeError;
}

/* Method save() stores the object through the StorageManager */
void CorrectedLeaveOneOut::save(Advocate & adv) const
{
  FittingAlgorithmImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void CorrectedLeaveOneOut::load(Advocate & adv)
{
  FittingAlgorithmImplementation::load(adv);
}

END_NAMESPACE_OPENTURNS
