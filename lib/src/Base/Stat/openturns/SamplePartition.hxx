//                                               -*- C++ -*-
/**
 *  @brief This class is enables to build a second order model
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
#ifndef OPENTURNS_SAMPLEPARTITION_HXX
#define OPENTURNS_SAMPLEPARTITION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SamplePartition
 */

class OT_API SamplePartition
  : public PersistentObject
{

  CLASSNAME

public:

  /** Default constructor */
  SamplePartition();

  /** Constructor from a list of [start-end[ pairs of indices */
  explicit SamplePartition(const Sample & sample,
                           const Collection<Indices> & indicesCollection = Collection<Indices>());

  /** Constructor from the plain list of indices */
  SamplePartition(const Sample & sample,
                  const Indices & indices);

  /** Virtual copy constructor */
  SamplePartition * clone() const override;

  /** Retrieve peaks/clusters */
  Sample getPeakOverThreshold(const Scalar threshold, const UnsignedInteger r, SamplePartition & clusters) const;

  Sample getSample() const;

  Collection<Indices> getIndicesCollection() const;

  Graph draw(const Scalar threshold) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  Sample sample_;
  PersistentCollection<Indices> indicesCollection_;
} ; /* class SamplePartition */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FILTERINWINDOWSIMPLEMENTATION_HXX */
