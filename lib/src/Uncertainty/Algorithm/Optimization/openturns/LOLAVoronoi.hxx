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
#ifndef OPENTURNS_LOLAVORONOI_HXX
#define OPENTURNS_LOLAVORONOI_HXX

#include "openturns/SequentialSamplingAlgorithmImplementation.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/KDTree.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LOLAVoronoi
 *
 * Sparse approximation LS solver
 */
class OT_API LOLAVoronoi
  : public SequentialSamplingAlgorithmImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  LOLAVoronoi();

  /** Parameters constructor */
  LOLAVoronoi(const Sample & x,
              const Sample & y,
              const Distribution & distribution);

  /** Virtual constructor */
  LOLAVoronoi * clone() const override;

  /** String converter */
  String __repr__() const override;

  // Generate a new sample
  Sample generate(const UnsignedInteger size) const override;

  /** Voronoi sampling size accessor */
  void setVoronoiSamplingSize(const UnsignedInteger voronoiSamplingSize);
  UnsignedInteger getVoronoiSamplingSize() const;

  /** Neighbourhood candidates number accessor */
  void setNeighbourhoodCandidatesNumber(const UnsignedInteger neighbourhoodCandidatesNumber);
  UnsignedInteger getNeighbourhoodCandidatesNumber() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** LOLA/Voronoi scores accessor */
  Sample getLOLAScore() const;
  Sample getVoronoiScore() const;
  Sample getHybridScore() const;

protected:
  mutable KDTree tree_;

  friend struct LVVoronoiScorePolicy;
  void computeVoronoiScore() const;
  mutable Point voronoiScore_;

  friend struct LVLOLAScorePolicy;
  void computeLOLAScore() const;
  mutable Point lolaScore_;

  mutable Point hybridScore_;

  Distribution distribution_;

  UnsignedInteger neighbourhoodCandidatesNumber_ = 0;

  // neighbourhood of each input point
  mutable Collection<Indices> neighbourhood_;

  UnsignedInteger voronoiSamplingSize_ = 0;

}; /* class LOLAVoronoi */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOLAVORONOI_HXX */
