//                                               -*- C++ -*-
/**
 *  @brief The MarginalUniformOrderStatistics distribution
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
#include "openturns/MarginalUniformOrderStatistics.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SimplicialCubature.hxx"
#include "openturns/CubaIntegration.hxx"
#include "openturns/GaussLegendre.hxx"
#include "openturns/IntervalMesher.hxx"
#include "openturns/UniformOrderStatistics.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MarginalUniformOrderStatistics)

static const Factory<MarginalUniformOrderStatistics> Factory_MarginalUniformOrderStatistics;


/* Default constructor */
MarginalUniformOrderStatistics::MarginalUniformOrderStatistics()
  : MarginalDistribution(UniformOrderStatistics(1), {0})
{
  setName("MarginalUniformOrderStatistics");
}

/* Parameters constructor */
MarginalUniformOrderStatistics::MarginalUniformOrderStatistics(const UnsignedInteger n,
                                                               const Indices & indices)
  : MarginalDistribution(UniformOrderStatistics(n), indices)
{
  setName("MarginalUniformOrderStatistics");
  // compute the log-PDF normalization factor
  // Waiting for a more accurate computation...
  const UnsignedInteger m = indices.getSize();
  if (m == 0) throw InvalidArgumentException(HERE) << "Error: cannot build a MarginalUniformOrderStatistics based on an empty Indice";
  logNormalizationFactor_ = SpecFunc::LogGamma(n + 1) - SpecFunc::LogGamma(indices[0] + 1);
  for (UnsignedInteger i = 1; i < m; ++i)
    logNormalizationFactor_ -= SpecFunc::LogGamma(indices[i] - indices[i - 1]);
  logNormalizationFactor_ -= SpecFunc::LogGamma(n - indices[indices.getSize() - 1]);
  simplex_ = IntervalMesher(Indices(m, 1)).build(Interval(m));
}

/* Comparison operator */
Bool MarginalUniformOrderStatistics::operator ==(const MarginalUniformOrderStatistics & other) const
{
  if (this == &other) return true;
  return (getN() == other.getN()) && (indices_ == other.indices_);
}

Bool MarginalUniformOrderStatistics::equals(const DistributionImplementation & other) const
{
  const MarginalUniformOrderStatistics* p_other = dynamic_cast<const MarginalUniformOrderStatistics*>(&other);
  if (p_other) return (*this == *p_other);
  return MarginalDistribution::equals(other);
}

/* Virtual constructor */
MarginalUniformOrderStatistics * MarginalUniformOrderStatistics::clone() const
{
  return new MarginalUniformOrderStatistics(*this);
}

/* Get the PDF of the distribution */
Scalar MarginalUniformOrderStatistics::computePDF(const Point & point) const
{
  const Scalar logPDF = computeLogPDF(point);
  if (logPDF == SpecFunc::LowestScalar) return 0.0;
  return std::exp(logPDF);
}

Scalar MarginalUniformOrderStatistics::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  if (!point.isIncreasing()) return SpecFunc::LowestScalar;
  // Now, we know that the components of point are in increasing order
  if ((point[0] <= 0.0) || (point[dimension_ - 1] >= 1.0)) return SpecFunc::LowestScalar;
  // First term, only u_{i0}
  Scalar logPDF = logNormalizationFactor_ + indices_[0] * std::log(point[0]);
  // Central terms, functions of (u_{i_j}-u_{i_{j-1}})
  for (UnsignedInteger j = 1; j < dimension_; ++j)
    logPDF += (indices_[j] - indices_[j - 1] - 1.0) * std::log(point[j] - point[j - 1]);
  // Last term, function of u_{i_{n-1}}
  logPDF += (getN() - indices_[dimension_ - 1] - 1.0) * std::log1p(-point[dimension_ - 1]);
  return logPDF;
}

/* Get the CDF of the distribution */
Scalar MarginalUniformOrderStatistics::computeCDF(const Point & point) const
{
  if (point.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << point.getDimension();
  // If the marginal distribution is a permutation of the underlying distribution
  if (getN() == dimension_) return MarginalDistribution::computeCDF(point);
  // 1D case is trivial
  if (dimension_ == 1)
    return DistFunc::pBeta(indices_[0] + 1.0, getN() - indices_[0], point[0]);
  // Large N case leads to a stack overflow
  if (getN() > ResourceMap::GetAsUnsignedInteger("MarginalUniformOrderStatistics-LargeCaseCDF"))
    {
      //      std::cerr << "In the large N case, N=" << getN() << std::endl;
      //SimplicialCubature algo;
      //algo.setMaximumCallsNumber(1000000);
      //const Mesh domain(simplex_.getVertices() * point, simplex_.getSimplices());
      //std::cerr << "domain=" << domain << std::endl;
      //return SpecFunc::Clip01(algo.integrate(Function(PDFWrapper(this)), domain)[0]);
      CubaIntegration algo("cuhre");
      return SpecFunc::Clip01(algo.integrate(Function(PDFWrapper(this)), Interval(Point(dimension_), point))[0]);
    }
  //  std::cerr << "In the small N case, N=" << getN() << std::endl;
  return MarginalDistribution::computeCDF(point);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution MarginalUniformOrderStatistics::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  if (dimension == 1) return clone();
  // Build the indices associated to the marginal of the marginal
  const UnsignedInteger outputDimension = indices.getSize();
  Indices marginalIndices(outputDimension);
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
    marginalIndices[i] = indices_[indices[i]];
  MarginalUniformOrderStatistics marginal(distribution_.getDimension(), marginalIndices);
  return marginal.clone();
} // getMarginal(Indices)

/* N accessor */
UnsignedInteger MarginalUniformOrderStatistics::getN() const
{
  return distribution_.getDimension();
}

/* Method save() stores the object through the StorageManager */
void MarginalUniformOrderStatistics::save(Advocate & adv) const
{
  MarginalDistribution::save(adv);
  adv.saveAttribute("logNormalizationFactor_", logNormalizationFactor_);
  adv.saveAttribute("simplex_", simplex_);
}

/* Method load() reloads the object from the StorageManager */
void MarginalUniformOrderStatistics::load(Advocate & adv)
{
  MarginalDistribution::load(adv);
  adv.loadAttribute("logNormalizationFactor_", logNormalizationFactor_);
  adv.loadAttribute("simplex_", simplex_);
  computeRange();
}


END_NAMESPACE_OPENTURNS
