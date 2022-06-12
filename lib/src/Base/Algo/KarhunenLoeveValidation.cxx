//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition validation services
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

#include "openturns/KarhunenLoeveValidation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KarhunenLoeveReduction.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(KarhunenLoeveValidation)

static const Factory<KarhunenLoeveValidation> Factory_KarhunenLoeveValidation;

/* Default constructor */
KarhunenLoeveValidation::KarhunenLoeveValidation()
  : PersistentObject()
{
  // Nothing to do
}


/* Constructor with parameters */
KarhunenLoeveValidation::KarhunenLoeveValidation(const ProcessSample & sample,
    const KarhunenLoeveResult & result)
  : PersistentObject()
  , sample_(sample)
  , result_(result)
{
  if (sample.getDimension() != result.getModesAsProcessSample().getDimension())
    throw InvalidArgumentException(HERE) << "Sample dimension = " << sample.getDimension() << " does not match result dimension = " << result.getModesAsProcessSample().getDimension();
}

/* Constructor with parameters */
KarhunenLoeveValidation::KarhunenLoeveValidation(const ProcessSample & sample,
    const KarhunenLoeveResult & result,
    const TrendTransform & trend)
  : KarhunenLoeveValidation(sample, result)
{
  trend_ = trend;
}

KarhunenLoeveValidation * KarhunenLoeveValidation::clone() const
{
  return new KarhunenLoeveValidation(*this);
}


ProcessSample KarhunenLoeveValidation::computeResidual() const
{
  const UnsignedInteger outputDimension = sample_.getDimension();
  const UnsignedInteger size = sample_.getSize();
  ProcessSample result(result_.getMesh(), size, outputDimension);
  KarhunenLoeveReduction reduction(result_);
  if (trend_.getInputDimension() > 0)
    reduction.setTrend(trend_);
  for (UnsignedInteger i = 0; i < size; ++ i)
    result[i] = sample_[i] - reduction(sample_[i]);
  result.setName("KL residual");
  return result;
}


Field KarhunenLoeveValidation::computeResidualMean() const
{
  Field result(computeResidual().computeMean());
  result.setName("KL residual mean");
  return result;
}


Field KarhunenLoeveValidation::computeResidualStandardDeviation() const
{
  Field result(computeResidual().computeStandardDeviation());
  result.setName("KL residual standard deviation");
  return result;
}


GridLayout KarhunenLoeveValidation::drawValidation() const
{
  KarhunenLoeveReduction reduction(result_);
  if (trend_.getInputDimension() > 0)
    reduction.setTrend(trend_);
  const UnsignedInteger outputDimension = sample_.getDimension();

  const UnsignedInteger size = sample_.getSize();

  const Description description(sample_[0].getDescription());
  Sample reducedFieldSample(0, outputDimension);
  Sample fieldSample(0, outputDimension);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    fieldSample.add(sample_[i]);
    reducedFieldSample.add(reduction(sample_[i]));
  }

  GridLayout grid(1, outputDimension);
  const Point minS(fieldSample.getMin());
  const Point maxS(fieldSample.getMax());
  for (UnsignedInteger j = 0; j < outputDimension; ++ j)
  {
    Graph graph("", OSS() << "Observed - " << description[j], OSS() << "Reduced - " << description[j], true, "topright");

    // diagonal
    Sample diagonalPoints(2, 2);
    diagonalPoints(0, 0) = minS[j];
    diagonalPoints(0, 1) = diagonalPoints(0, 0);
    diagonalPoints(1, 0) = maxS[j];
    diagonalPoints(1, 1) = diagonalPoints(1, 0);
    Curve diagonal(diagonalPoints);
    diagonal.setColor("red");
    graph.add(diagonal);

    const Cloud cloud(fieldSample.getMarginal(j), reducedFieldSample.getMarginal(j));
    graph.add(cloud);

    grid.setGraph(0, j, graph);
  }
  grid.setTitle("Karhunen-Loeve Validation");
  return grid;
}


Graph KarhunenLoeveValidation::drawObservationWeight(const UnsignedInteger k) const
{
  const UnsignedInteger K = result_.getEigenvalues().getSize();
  if (!(k < K))
    throw InvalidArgumentException(HERE) << "k (" << k << ") should be in [0; " << K - 1 << "]";
  const Sample xi(result_.project(sample_));
  const UnsignedInteger size = xi.getSize();
  Point v(size);
  Point x(size);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    v[i] = xi(i, k) * xi(i, k) / size;
    x[i] = i;
  }
  const Curve curve(x, v);
  Graph result(OSS() << "Field weight axis " << k, "Field index", "w", true);
  result.add(curve);
  return result;
}


Graph KarhunenLoeveValidation::drawObservationQuality() const
{
  KarhunenLoeveReduction reduction(result_);
  if (trend_.getInputDimension() > 0)
    reduction.setTrend(trend_);
  const ProcessSample reduced(reduction(sample_));
  const UnsignedInteger size = sample_.getSize();
  Point x(size);
  Point q(size);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    x[i] = i;
    const Scalar qK = reduced.getField(i).norm();
    const Scalar qT = sample_.getField(i).norm();
    const Scalar ratio = qK / qT;
    q[i] = ratio * ratio;
  }
  const Curve curve(x, q);
  Graph result(OSS() << "Field quality", "Field index", "q", true);
  result.add(curve);
  return result;
}


/* String converter */
String KarhunenLoeveValidation::__repr__() const
{
  return OSS(true) << "class=" << getClassName() << " result=" << result_;
}


String KarhunenLoeveValidation::__str__(const String & offset) const
{
  return offset + __repr__();
}


/* Method save() stores the object through the StorageManager */
void KarhunenLoeveValidation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("sample_", sample_);
  adv.saveAttribute("result_", result_);
  adv.saveAttribute("trend_", trend_);
}


/* Method load() reloads the object from the StorageManager */
void KarhunenLoeveValidation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("sample_", sample_);
  adv.loadAttribute("result_", result_);
  adv.loadAttribute("trend_", trend_);
}

END_NAMESPACE_OPENTURNS
