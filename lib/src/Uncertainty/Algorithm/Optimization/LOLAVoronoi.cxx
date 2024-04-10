//                                               -*- C++ -*-
/**
 *  @brief LOLA-Voronoi sequential design
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

#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/LOLAVoronoi.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/KDTree.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/Combinations.hxx"
#include "openturns/CholeskyMethod.hxx"
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
  , voronoiSamplingSize_(ResourceMap::GetAsUnsignedInteger("LOLAVoronoi-DefaultVoronoiSamplingSize"))
{
  // LOLA criterion needs at least m=2d neighbours to compute gradient approximations
  if (x.getSize() < 2 * x.getDimension() + 1)
    throw InvalidArgumentException(HERE) << "Input sample size (" << x.getSize() << ") should be >=2d+1 (" << (2 * x.getDimension() + 1) << ")";
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

struct LVNeighboorhoodPolicy
{
  const LOLAVoronoi & lola_;
  const KDTree & tree_;

  LVNeighboorhoodPolicy(const LOLAVoronoi & lola, const KDTree & tree)
    : lola_(lola)
    , tree_(tree)
  {}

  inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & rnge) const
  {
    const UnsignedInteger m = 2 * lola_.x_.getDimension();
    for (UnsignedInteger i = rnge.begin(); i != rnge.end(); ++ i)
    {
      const UnsignedInteger k = std::min(lola_.x_.getSize(), m + lola_.neighbourhoodCandidatesNumber_ + 1);
      Indices candidateIndices(tree_.queryK(lola_.x_[i], k));

      // exclude the reference point x_i
      const UnsignedInteger indexI = candidateIndices.find(i);
      candidateIndices.erase(candidateIndices.begin() + indexI, candidateIndices.begin() + indexI + 1);

      // explore all combinations of the neighbouring points
      const IndicesCollection candidateCombinations(Combinations(m, k - 1).generate());
      Scalar bestNeighbourhoodScore = 0.0;
      for (UnsignedInteger n = 0; n < candidateCombinations.getSize(); ++ n)
      {
        // the cohesion is defined as the average distance of all neighbours from the origin (3.3)
        Scalar cohesion = 0.0;
        for (UnsignedInteger j = 0; j < m; ++ j)
        {
          const Point xj(lola_.x_[candidateIndices[candidateCombinations(n, j)]]);
          // sadly KDTree does not give the distances
          cohesion += Point(lola_.x_[i] - xj).norm() / m;
        }

        // the adhesion is defined as the average distance of neighbours from each other (3.4)
        Scalar adhesion = 0.0;
        for (UnsignedInteger j1 = 0; j1 < m; ++ j1)
        {
          for (UnsignedInteger j2 = j1 + 1; j2 < m; ++ j2)
          {
            const Point xj1(lola_.x_[candidateIndices[candidateCombinations(n, j1)]]);
            const Point xj2(lola_.x_[candidateIndices[candidateCombinations(n, j2)]]);
            adhesion += Point(xj1 - xj2).norm() / (m * (m - 1));
          }
        }

        // the cross-polytope ratio aggregates the cohesion (to minimize) and the adhesion (to maximize) (3.5)
        const Scalar crossPolytopeRatio = adhesion / (std::sqrt(2.0) * cohesion);

        // the neighbourhood score to maximize (3.7)
        const Scalar neighbourhoodScore = crossPolytopeRatio / cohesion;
        if (neighbourhoodScore > bestNeighbourhoodScore)
        {
          Indices neighbourhood;
          for (UnsignedInteger j = 0; j < m; ++ j)
            neighbourhood.add(candidateIndices[candidateCombinations(n, j)]);
          lola_.neighbourhood_[i] = neighbourhood;
          bestNeighbourhoodScore = neighbourhoodScore;
        }

      } // candidate neighbourhood loop
    } // x_i loop
  }
};

void LOLAVoronoi::updateNeighbourhood() const
{
  // tree queries are thread-safe
  KDTree tree(x_);
  neighbourhood_.resize(x_.getSize());

  const LVNeighboorhoodPolicy policy(*this, tree);
  TBBImplementation::ParallelFor(0, x_.getSize(), policy);
}

Point LOLAVoronoi::computeVoronoiScore() const
{
  // estimate the Voronoi cell size by Monte Carlo (2.5.1.1, algorithm 2)
  KDTree tree(x_);
  const Sample voronoiSample(distribution_.getSample(voronoiSamplingSize_));
  const Indices indices = tree.query(voronoiSample);
  Point voronoiScore(x_.getSize());
  for (UnsignedInteger i = 0; i < voronoiSamplingSize_; ++ i)
    voronoiScore[indices[i]] += 1.0 / voronoiSamplingSize_;
  return voronoiScore;
}


struct LVLOLAScorePolicy
{
  const LOLAVoronoi & lola_;
  Point & nonLinearScore_;

  LVLOLAScorePolicy(const LOLAVoronoi & lola, Point & nonLinearScore)
    : lola_(lola)
    , nonLinearScore_(nonLinearScore)
  {}

  inline void operator()(const TBBImplementation::BlockedRange<UnsignedInteger> & rnge) const
  {
    const UnsignedInteger d = lola_.x_.getDimension();
    const UnsignedInteger m = 2 * d;
    for (UnsignedInteger i = rnge.begin(); i != rnge.end(); ++ i)
    {
      const Point prx(lola_.x_[i]);
      const Point pry(lola_.y_[i]);
      const Indices neighbourhood(lola_.neighbourhood_[i]);

      Matrix p(m, d);
      for (UnsignedInteger ti = 0; ti < m; ++ ti)
      {
        const Point prtx(lola_.x_[neighbourhood[ti]]);
        for (UnsignedInteger j = 0; j < d; ++ j)
          p(ti, j) = prtx[j] - prx[j];
      }

      Scalar eprMax = 0.0;
      for (UnsignedInteger k = 0; k < lola_.y_.getDimension(); ++ k)
      {
        Point f(m);
        for (UnsignedInteger ti = 0; ti < m; ++ ti)
        {
          const Point prty(lola_.y_[neighbourhood[ti]]);
          f[ti] = prty[k];
        }

        // g = \argmin ||pg-f|| cf 3.3.3 equation (3.8)
        const Point g(CholeskyMethod(p).solve(f));

        // local nonlinearity, 3.3.4 equation (3.9)
        Scalar epr = 0.0;
        for (UnsignedInteger ti = 0; ti < m; ++ ti)
        {
          const Point prtx(lola_.x_[neighbourhood[ti]]);
          const Point prty(lola_.y_[neighbourhood[ti]]);
          epr += std::abs(prty[k] - (pry[k] + g.dot(prtx - prx)));
        }

        // the non linearity score is the max across output components cf 3.6 equation (3.12)
        if (epr > eprMax)
          eprMax = epr;
      }
      nonLinearScore_[i] = eprMax;
    } // i loop
  }
};

Point LOLAVoronoi::computeLOLAScore() const
{
  // update neighbourhood if necessary
  if (neighbourhood_.getSize() != x_.getSize())
    updateNeighbourhood();

  Point nonLinearScore(x_.getSize());
  const LVLOLAScorePolicy policy(*this, nonLinearScore);
  TBBImplementation::ParallelFor(0, x_.getSize(), policy);
  return nonLinearScore;
}

Sample LOLAVoronoi::generate(const UnsignedInteger size) const
{
  if (size > x_.getSize())
    throw InvalidArgumentException(HERE) << "cannot ask more than " << x_.getSize() << " new samples (asked " << size << ")";

  // hybrid score, see 3.4 equation (3.10)
  Sample hybridScore(x_.getSize(), 1);
  LOGINFO("LOLAVoronoi updating voronoi score");
  const Point voronoiScore(computeVoronoiScore());
  LOGINFO("LOLAVoronoi updating LOLA score");
  const Point nonLinearScore(computeLOLAScore());
  const Scalar sumLS = std::accumulate(nonLinearScore.begin(), nonLinearScore.end(), 0.0);
  for (UnsignedInteger i = 0; i < x_.getSize(); ++ i)
    hybridScore(i, 0) = voronoiScore[i] + nonLinearScore[i] / sumLS;
  const Indices ranking(hybridScore.argsort(false));
  const KDTree tree(x_);
  const UnsignedInteger d = x_.getDimension();
  Sample result(0, d);
  const UnsignedInteger m = 2 * d;
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    LOGINFO(OSS() << "LOLAVoronoi generating point " << (i + 1) << "/" << size);
    const Point xi = x_[ranking[i]];

    // compute the maximum distance from x_i to other points
    const Indices neighbourhood(neighbourhood_[ranking[i]]);
    Scalar neighbourhoodMaximumDistance = 0.0;
    for (UnsignedInteger j = 0; j < m; ++ j)
    {
      const Point xj = x_[neighbourhood[j]];
      const Scalar distance = (xi - xj).norm();
      if (distance > neighbourhoodMaximumDistance)
        neighbourhoodMaximumDistance = distance;
    } // j loop

    // to avoid cells around new candidates
    Sample xiNew(1, xi);
    xiNew.add(result);
    const KDTree treeNew(xiNew);

    // sample in the box containing the neighbourhood of x_i
    const Point width(d, neighbourhoodMaximumDistance);
    const Interval bounds(xi - width, xi + width);
    const TruncatedDistribution truncated(distribution_, bounds);
    const Sample voronoiSample(truncated.getSample(voronoiSamplingSize_));
    Point newPoint;
    Scalar candidatesMaximumDistance = 0.0;
    for (UnsignedInteger k = 0; k < voronoiSamplingSize_; ++ k)
    {
      const Point vk(voronoiSample[k]);
      const UnsignedInteger indexK = tree.query(vk);
      const UnsignedInteger indexNew = treeNew.query(vk);

      // keep only the points inside the voronoi cell of x_i but not closer to new candidates
      if ((indexK == ranking[i]) && (indexNew == 0))
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
    if (!newPoint.getDimension())
      throw InternalException(HERE) << "No new point found i=" << i;
    result.add(newPoint);
  } // i loop
  return result;
}

/* Voronoi sampling size accessor */
void LOLAVoronoi::setVoronoiSamplingSize(const UnsignedInteger voronoiSamplingSize)
{
  voronoiSamplingSize_ = voronoiSamplingSize;
}

UnsignedInteger LOLAVoronoi::getVoronoiSamplingSize() const
{
  return voronoiSamplingSize_;
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
  adv.saveAttribute("voronoiSamplingSize_", voronoiSamplingSize_);
}


/* Method load() reloads the object from the StorageManager */
void LOLAVoronoi::load(Advocate & adv)
{
  SequentialSamplingAlgorithmImplementation::load(adv);
  adv.loadAttribute("neighbourhoodCandidatesNumber_", neighbourhoodCandidatesNumber_);
  adv.loadAttribute("voronoiSamplingSize_", voronoiSamplingSize_);
}

END_NAMESPACE_OPENTURNS
