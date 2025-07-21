//                                               -*- C++ -*-
/**
 *  @brief LOLA-Voronoi sequential design
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/LOLAVoronoi.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/Combinations.hxx"
#include "openturns/CombinationsDistribution.hxx"
#include "openturns/LeastSquaresMethod.hxx"
#include "openturns/TruncatedDistribution.hxx"

#include <numeric>

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(LOLAVoronoi)


static const Factory<LOLAVoronoi> Factory_LOLAVoronoi;

/* Default constructor */
LOLAVoronoi::LOLAVoronoi()
  : SequentialSamplingAlgorithmImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
LOLAVoronoi::LOLAVoronoi(const Sample & x,
                         const Sample & y,
                         const Distribution & distribution)
  : SequentialSamplingAlgorithmImplementation(x, y)
  , distribution_(distribution)
  , neighbourhoodCandidatesNumber_(ResourceMap::GetAsUnsignedInteger("LOLAVoronoi-DefaultNeighbourhoodCandidatesNumber"))
  , voronoiMinimumSamplingSize_(ResourceMap::GetAsUnsignedInteger("LOLAVoronoi-DefaultVoronoiMinimumSamplingSize"))
  , voronoiMeanSamplingSize_(ResourceMap::GetAsUnsignedInteger("LOLAVoronoi-DefaultVoronoiMeanSamplingSize"))
{
  // LOLA criterion needs at least m=2d neighbours to compute gradient approximations
  if (x.getSize() < 2 * x.getDimension() + 1)
    throw InvalidArgumentException(HERE) << "Input sample size (" << x.getSize() << ") should be >=2d+1 (" << (2 * x.getDimension() + 1) << ")";

  // Input components cannot be constant in order to build the neighbourhood
  const Point sigma(x.computeStandardDeviation());
  for (UnsignedInteger i = 0; i < x.getDimension(); ++ i)
    if (!(sigma[i] > 0.0))
      throw InvalidArgumentException(HERE) << "Input sample component #" << i << " must not be constant";
}


/* Virtual constructor */
LOLAVoronoi * LOLAVoronoi::clone() const
{
  return new LOLAVoronoi( *this );
}


/* String converter */
String LOLAVoronoi::__repr__() const
{
  return OSS() << "class=" << GetClassName();
}

void LOLAVoronoi::computeVoronoiScore() const
{
  // estimate the Voronoi cell size by Monte Carlo (2.5.1.1, algorithm 2)
  const UnsignedInteger voronoiSamplingSize = std::max(voronoiMinimumSamplingSize_, voronoiMeanSamplingSize_ * x_.getSize());
  const Sample voronoiSample(distribution_.getSample(voronoiSamplingSize));
  const Indices indices = tree_.query(voronoiSample);
  voronoiScore_ = Point(x_.getSize());
  for (UnsignedInteger i = 0; i < voronoiSamplingSize; ++ i)
    voronoiScore_[indices[i]] += 1.0 / voronoiSamplingSize;
}


struct LVLOLAScorePolicy
{
  const LOLAVoronoi & lola_;

  // to compute neighbourhood
  IndicesCollection candidateCombinations_;
  UnsignedInteger previousSize_ = 0;
  UnsignedInteger maximumCombinationsNumber_ = ResourceMap::GetAsUnsignedInteger("LOLAVoronoi-MaximumCombinationsNumber");
  UnsignedInteger k_ = 0;

  // to compute non linear score
  String methodName_ = ResourceMap::Get("LOLAVoronoi-DecompositionMethod");
  String aggregationMethod_ = ResourceMap::GetAsString("LOLAVoronoi-NonLinearityAggregationMethod");

  LVLOLAScorePolicy(const LOLAVoronoi & lola, const UnsignedInteger previousSize)
    : lola_(lola)
    , previousSize_(previousSize)

  {
    const UnsignedInteger m = 2 * lola_.x_.getDimension();
    const UnsignedInteger k = std::min(lola_.x_.getSize(), m + lola_.neighbourhoodCandidatesNumber_ + 1);
    if (SpecFunc::LogGamma(k) - SpecFunc::LogGamma(m + 1) - SpecFunc::LogGamma(k - m) > std::log(maximumCombinationsNumber_))
    {
      candidateCombinations_ = IndicesCollection(maximumCombinationsNumber_, m);
      const CombinationsDistribution distribution(m, k - 1);
      for (UnsignedInteger i = 0; i < maximumCombinationsNumber_; ++i)
      {
        const Point point(distribution.getRealization());
        for (UnsignedInteger j = 0; j < m; ++j)
          candidateCombinations_(i, j) = static_cast<UnsignedInteger>(point[j]);
      } // i
    } // Large combination number
    else
    {
      candidateCombinations_ = Combinations(m, k - 1).generate();
    }

    k_ = std::min(lola_.x_.getSize(), m + lola_.neighbourhoodCandidatesNumber_ + 1);
  }

  inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & rnge) const
  {
    const UnsignedInteger d = lola_.x_.getDimension();
    const UnsignedInteger m = 2 * d;
    for (UnsignedInteger i = rnge.begin(); i != rnge.end(); ++ i)
    {
      Indices candidateIndices(lola_.tree_.queryK(lola_.x_[i], k_));

      // similarly to the "too far" heuristic proposed in 3.5.2 we can avoid updating this neighbourhood if the newly added points are too far
      // only if it was already computed once and if the closest indices do no contain newly added points (at indices >= previousSize_)
      // (we probably do not want to explore another batch of combinations even in the case it is capped by DefaultMaximumCombinationsNumber)
      // note that we do not need the distance ratio constant from 3.5.2 as candidates are picked from the closest points
      if ((i < previousSize_) && (candidateIndices.normInf() < previousSize_))
        continue;

      // exclude the reference point x_i
      const UnsignedInteger indexI = candidateIndices.find(i);
      if (indexI < candidateIndices.getSize())
        candidateIndices.erase(candidateIndices.begin() + indexI, candidateIndices.begin() + indexI + 1);

      // explore all combinations of the neighbouring points
      Scalar bestNeighbourhoodScore = 0.0;
      Indices & neighbourhoodI = lola_.neighbourhood_[i];
      for (UnsignedInteger n = 0; n < candidateCombinations_.getSize(); ++ n)
      {
        // the cohesion is defined as the average distance of all neighbours from the origin (3.3)
        Scalar cohesion = 0.0;
        for (UnsignedInteger j = 0; j < m; ++ j)
        {
          // expand Point(x[i] - x[cc]).norm() to avoid creating many Point instances
          const UnsignedInteger cc = candidateIndices[candidateCombinations_(n, j)];
          Scalar xmxjNorm = 0.0;
          for (UnsignedInteger j2 = 0; j2 < d; ++ j2)
          {
            const Scalar xmxj = lola_.x_(i, j2) - lola_.x_(cc, j2);
            xmxjNorm += xmxj * xmxj;
          }

          // sadly KDTree does not give the distances
          cohesion += std::sqrt(xmxjNorm) / m;
        }

        // the adhesion is defined as the average minimum distance of neighbours from each other (3.4)
        Scalar adhesion = 0.0;
        for (UnsignedInteger j1 = 0; j1 < m; ++ j1)
        {
          Scalar minNorm2j1 = SpecFunc::MaxScalar;
          for (UnsignedInteger j2 = j1 + 1; j2 < m; ++ j2)
          {
            const UnsignedInteger cc1 = candidateIndices[candidateCombinations_(n, j1)];
            const UnsignedInteger cc2 = candidateIndices[candidateCombinations_(n, j2)];

            // expand Point(x[cc1] - x[cc2]).norm() to avoid creating many Point instances
            Scalar x1m2norm = 0.0;
            for (UnsignedInteger j = 0; j < d; ++ j)
            {
              const Scalar x1m2j = lola_.x_(cc1, j) - lola_.x_(cc2, j);
              x1m2norm += x1m2j * x1m2j;
            }
            minNorm2j1 = std::min(minNorm2j1, x1m2norm);
          }
          adhesion += std::sqrt(minNorm2j1) / m;
        }

        // the cross-polytope ratio aggregates the cohesion (to minimize) and the adhesion (to maximize) (3.5)
        const Scalar crossPolytopeRatio = adhesion / (std::sqrt(2.0) * cohesion);

        // the neighbourhood score to maximize (3.7)
        const Scalar neighbourhoodScore = crossPolytopeRatio / cohesion;
        if (neighbourhoodScore > bestNeighbourhoodScore)
        {
          neighbourhoodI.clear();
          for (UnsignedInteger j = 0; j < m; ++ j)
            neighbourhoodI.add(candidateIndices[candidateCombinations_(n, j)]);
          bestNeighbourhoodScore = neighbourhoodScore;
        }

      } // candidate neighbourhood loop

      // now compute the score
      const Point prx(lola_.x_[i]);
      const Point pry(lola_.y_[i]);

      Matrix p(m, d);
      for (UnsignedInteger ti = 0; ti < m; ++ ti)
      {
        const Point prtx(lola_.x_[neighbourhoodI[ti]]);
        for (UnsignedInteger j = 0; j < d; ++ j)
          p(ti, j) = prtx[j] - prx[j];
      }

      Scalar eprAgg = 0.0;
      for (UnsignedInteger k = 0; k < lola_.y_.getDimension(); ++ k)
      {
        Point f(m);
        for (UnsignedInteger ti = 0; ti < m; ++ ti)
        {
          const Point prty(lola_.y_[neighbourhoodI[ti]]);
          // second member is wrong in equation (3.8) should be: f(p_ri) - f(p_r)
          f[ti] = prty[k] - pry[k];
        }

        // g = \argmin ||pg-f|| cf 3.3.3 equation (3.8)
        const Point g(LeastSquaresMethod::Build(methodName_, p).solve(f));

        // local nonlinearity, 3.3.4 equation (3.9)
        Scalar epr = 0.0;
        for (UnsignedInteger ti = 0; ti < m; ++ ti)
        {
          const Point prtx(lola_.x_[neighbourhoodI[ti]]);
          const Point prty(lola_.y_[neighbourhoodI[ti]]);
          epr += std::abs(prty[k] - (pry[k] + g.dot(prtx - prx)));
        }

        // the non linearity score is the max across output components cf 3.6 equation (3.12)
        if (aggregationMethod_ == "Maximum")
          eprAgg = std::max(eprAgg, epr);
        else if (aggregationMethod_ == "Average")
          eprAgg += epr / lola_.y_.getDimension();
        else
          throw InvalidArgumentException(HERE) << "LOLAVoronoi-NonLinearityAggregationMethod must be either 'Maximum' or 'Average'";
      }
      lola_.lolaScore_[i] = eprAgg;
    } // i loop
  }
};

void LOLAVoronoi::computeLOLAScore() const
{
  const UnsignedInteger previousSize = neighbourhood_.getSize();
  neighbourhood_.resize(x_.getSize());

  lolaScore_.resize(x_.getSize());
  const LVLOLAScorePolicy policy(*this, previousSize);
  TBBImplementation::ParallelFor(0, x_.getSize(), policy);
}

Sample LOLAVoronoi::generate(const UnsignedInteger size) const
{
  if (size > x_.getSize())
    throw InvalidArgumentException(HERE) << "cannot ask more than " << x_.getSize() << " new samples (asked " << size << ")";

  tree_ = KDTree(x_);

  LOGINFO("LOLAVoronoi updating voronoi score");
  computeVoronoiScore();
  LOGINFO("LOLAVoronoi updating LOLA score");
  computeLOLAScore();
  // hybrid score, see 3.4 equation (3.10)
  const Scalar sumLS = std::accumulate(lolaScore_.begin(), lolaScore_.end(), 0.0);
  const Scalar lambda = ResourceMap::GetAsScalar("LOLAVoronoi-HybridScoreTradeoff");
  if (!(lambda >= 0.0) || !(lambda <= 1.0))
    throw InvalidArgumentException(HERE) << "The LOLAVoronoi-HybridScoreTradeoff entry must be in [0, 1]";
  hybridScore_ = voronoiScore_ * lambda;
  for (UnsignedInteger i = 0; i < x_.getSize(); ++ i)
    hybridScore_[i] += (1.0 - lambda) * lolaScore_[i] / sumLS;
  const Indices ranking(Sample::BuildFromPoint(hybridScore_).argsort(false));
  const UnsignedInteger d = x_.getDimension();
  Sample result(0, d);
  const UnsignedInteger m = 2 * d;
  UnsignedInteger i = 0;
  // stop only when enough points are generated
  while (result.getSize() < size)
  {
    if (i >= x_.getSize())
      throw InternalException(HERE) << "Exhausted the number of candidates to generate new points from";

    LOGINFO(OSS() << "LOLAVoronoi generating point from candidate #" << i << " (completed " << result.getSize() << "/" << size << ")");

    const UnsignedInteger rankingI = ranking[i];
    ++ i;
    const Point xi = x_[rankingI];

    // compute the maximum distance from x_i to other points
    const Indices neighbourhood(neighbourhood_[rankingI]);
    Scalar neighbourhoodMaximumDistance = 0.0;
    for (UnsignedInteger j = 0; j < m; ++ j)
    {
      const Point xj = x_[neighbourhood[j]];
      const Scalar distance = (xi - xj).norm();
      if (distance > neighbourhoodMaximumDistance)
        neighbourhoodMaximumDistance = distance;
    } // j loop

    // new points only, avoids rebuilding the whole index when adding a new candidate
    Sample xiNew(1, xi);
    xiNew.add(result);
    const KDTree treeNew(xiNew);

    // sample in the box containing the neighbourhood of x_i
    const Point width(d, neighbourhoodMaximumDistance);
    const Interval bounds(xi - width, xi + width);
    Sample voronoiSample;
    const UnsignedInteger voronoiSamplingSize = std::max(voronoiMinimumSamplingSize_, voronoiMeanSamplingSize_ * x_.getSize());

    if (ResourceMap::GetAsBool("LOLAVoronoi-UseTruncatedDistribution"))
    {
      const TruncatedDistribution truncated(distribution_, bounds);
      voronoiSample = truncated.getSample(voronoiSamplingSize);
    }
    else
    {
      voronoiSample = Sample(voronoiSamplingSize, distribution_.getDimension());
      UnsignedInteger index = 0;
      while (index < voronoiSamplingSize)
      {
        const Point point(distribution_.getRealization());
        if (bounds.contains(point))
        {
          for (UnsignedInteger j = 0; j < point.getDimension(); ++j)
            voronoiSample(index, j) = point[j];
          ++ index;
        }
      } // while
    } // else
    Point newPoint;
    Scalar candidatesMaximumDistance = 0.0;
    for (UnsignedInteger k = 0; k < voronoiSamplingSize; ++ k)
    {
      const Point vk(voronoiSample[k]);
      const UnsignedInteger indexK = tree_.query(vk);
      const UnsignedInteger indexNew = treeNew.query(vk);

      // consider only the points inside the voronoi cell of x_i taking into account the cells around accepted candidates
      if ((indexK == rankingI) && (indexNew == 0))
      {
        // select random point which is furthest away from x_i and its neighbors as new adaptive sample
        Scalar distance = (vk - xi).norm();
        for (UnsignedInteger j = 0; j < m; ++ j)
        {
          const Point xj(x_[neighbourhood[j]]);
          distance += (vk - xj).norm();
        }
        if (distance > candidatesMaximumDistance)
        {
          candidatesMaximumDistance = distance;
          newPoint = vk;
        }
      }
    }
    // if no candidates could be accepted for this cell retry from next ranked point
    if (newPoint.getDimension())
      result.add(newPoint);
  }
  return result;
}

/* LOLA/Voronoi scores accessors */
Sample LOLAVoronoi::getLOLAScore() const
{
  return Sample::BuildFromPoint(lolaScore_);
}

Sample LOLAVoronoi::getVoronoiScore() const
{
  return Sample::BuildFromPoint(voronoiScore_);
}

Sample LOLAVoronoi::getHybridScore() const
{
  return Sample::BuildFromPoint(hybridScore_);
}

/* Voronoi sampling size accessor */
void LOLAVoronoi::setVoronoiMinimumSamplingSize(const UnsignedInteger voronoiMinimumSamplingSize)
{
  voronoiMinimumSamplingSize_ = voronoiMinimumSamplingSize;
}

UnsignedInteger LOLAVoronoi::getVoronoiMinimumSamplingSize() const
{
  return voronoiMinimumSamplingSize_;
}

/* Voronoi sampling ratio accessor */
void LOLAVoronoi::setVoronoiMeanSamplingSize(const UnsignedInteger voronoiMeanSamplingSize)
{
  if (voronoiMeanSamplingSize <= 0)
    throw InvalidArgumentException(HERE) << "The Voronoi mean sampling size should be positive";
  voronoiMeanSamplingSize_ = voronoiMeanSamplingSize;
}

UnsignedInteger LOLAVoronoi::getVoronoiMeanSamplingSize() const
{
  return voronoiMeanSamplingSize_;
}
  
/* Neighbourhood candidates number accessor */
void LOLAVoronoi::setNeighbourhoodCandidatesNumber(const UnsignedInteger neighbourhoodCandidatesNumber)
{
  neighbourhoodCandidatesNumber_ = neighbourhoodCandidatesNumber;
}

UnsignedInteger LOLAVoronoi::getNeighbourhoodCandidatesNumber() const
{
  return neighbourhoodCandidatesNumber_;
}

/* Method save() stores the object through the StorageManager */
void LOLAVoronoi::save(Advocate & adv) const
{
  SequentialSamplingAlgorithmImplementation::save(adv);
  adv.saveAttribute("neighbourhoodCandidatesNumber_", neighbourhoodCandidatesNumber_);
  adv.saveAttribute("voronoiMinimumSamplingSize_", voronoiMinimumSamplingSize_);
  adv.saveAttribute("voronoiMeanSamplingSize_", voronoiMeanSamplingSize_);
}


/* Method load() reloads the object from the StorageManager */
void LOLAVoronoi::load(Advocate & adv)
{
  SequentialSamplingAlgorithmImplementation::load(adv);
  adv.loadAttribute("neighbourhoodCandidatesNumber_", neighbourhoodCandidatesNumber_);
  if (adv.hasAttribute("voronoiMinimumSamplingSize_")) // OT >=1.26
  {
    adv.loadAttribute("voronoiMinimumSamplingSize_", voronoiMinimumSamplingSize_);
    adv.loadAttribute("voronoiMeanSamplingSize_", voronoiMeanSamplingSize_);
  }
}

END_NAMESPACE_OPENTURNS
