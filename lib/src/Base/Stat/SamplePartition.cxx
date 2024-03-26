//                                               -*- C++ -*-
/**
 *  @brief
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
#include "openturns/SamplePartition.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Polygon.hxx"
#include "openturns/Text.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SamplePartition
 */

CLASSNAMEINIT(SamplePartition)

static const Factory<SamplePartition> Factory_SamplePartition;

/* Constructor without parameters */
SamplePartition::SamplePartition()
: PersistentObject()
{
  // Nothing to do
}

SamplePartition::SamplePartition(const Sample & sample,
                const Collection<Indices> & indicesCollection)
: PersistentObject()
, sample_(sample)
, indicesCollection_(indicesCollection)
{
  if (!indicesCollection.getSize())
    indicesCollection_.add(Indices({0, sample.getSize()}));
}

SamplePartition::SamplePartition(const Sample & sample,
                const Indices & indices)
: PersistentObject()
, sample_(sample)
{
  const UnsignedInteger partitionSize = indices.getSize();
  Indices pair;
  UnsignedInteger previous = sample.getSize();
  if (partitionSize > 0)
  {
    previous = indices[0];
    pair.add(previous);
  }
  for (UnsignedInteger i = 1; i < partitionSize; ++ i)
  {
    const UnsignedInteger current = indices[i];
    if (current != previous + 1)
    {
      pair.add(previous + 1);
      indicesCollection_.add(pair);
      pair = Indices({current});
    }
    previous = current;
  }
  if (pair.getSize())
  {
    pair.add(previous + 1);
    indicesCollection_.add(pair);
  }
}


/* Virtual constructor */
SamplePartition * SamplePartition::clone() const
{
  return new SamplePartition(*this);
}


Sample SamplePartition::getPeakOverThreshold(const Scalar threshold, const UnsignedInteger r,
                                                         SamplePartition & clusters) const
{
  if (r == 0)
    throw InvalidArgumentException(HERE) << "SamplePartition minimum cluster gap should be > 0";

  Sample peaks(0, 1);
  Collection<Indices> indicesCollection;

  // loop over partitions
  for (UnsignedInteger i = 0; i < indicesCollection_.getSize(); ++ i)
  {
    const UnsignedInteger partitionStart = indicesCollection_[i][0];
    const UnsignedInteger partitionEnd = indicesCollection_[i][1];

    UnsignedInteger streak = 0; // number of consecutive values below threshold
    UnsignedInteger clusterStart = partitionStart;

    // loop over partition indices
    for (UnsignedInteger j = partitionStart; j < partitionEnd; ++ j)
    {
      if (sample_(j, 0) > threshold)
        streak = 0;
      else
        ++ streak;

      // check if a cluster ends
      if ((streak == r) || (j + 1 == partitionEnd))
      {
        const UnsignedInteger clusterEnd = std::max(clusterStart + 1, j + 1 - streak);
        Indices selection(clusterEnd - clusterStart);
        selection.fill(clusterStart);
        const Point peak = sample_.select(selection).getMax();
        // keep only clusters with values above threshold
        if (peak[0] > threshold)
        {
          peaks.add(peak);
          indicesCollection.add(Indices{clusterStart, clusterEnd});
        }
        clusterStart = j + 1; // reset cluster
        streak = 0;
      }
    }
  }
  clusters = SamplePartition(sample_, indicesCollection);
  return peaks;
}

Graph SamplePartition::draw(const Scalar threshold) const
{
  Graph graph(sample_.getDescription()[0] + " clusters", "Index", sample_.getDescription()[0], true, "");
  const UnsignedInteger size = sample_.getSize();
  Sample indexSample(size, 1);
  for (UnsignedInteger i = 0; i < size; ++ i)
    indexSample(i, 0) = i;

  Scalar yMin = sample_.getMin()[0];
  Scalar yMax = sample_.getMax()[0];
  const Scalar delta = yMax - yMin;
  yMin -= 0.05 * delta;
  yMax += 0.05 * delta;

  // loop over partitions
  Sample peaks(0, 1);
  Sample peakIndices(0, 1);
  for (UnsignedInteger i = 0; i < indicesCollection_.getSize(); ++ i)
  {
    const UnsignedInteger partitionStart = indicesCollection_[i][0];
    const UnsignedInteger partitionEnd = indicesCollection_[i][1];
    Scalar peak = threshold;
    Scalar peakIndex = 0;
    for (UnsignedInteger j = partitionStart; j < partitionEnd; ++ j)
    {
      if (sample_(j, 0) > peak)
      {
        peak = sample_(j, 0);
        peakIndex = j;
      }
    }
    peaks.add(Point({peak}));
    peakIndices.add(Point({peakIndex}));

    // cluster begin vertical lines
    Point dataX = {partitionStart - 0.5, partitionStart - 0.5};
    const Point dataY = {yMin, yMax};
    Curve vLine = Curve(dataX, dataY);
    vLine.setColor("gray");
    vLine.setLineStyle("dashed");
    graph.add(vLine);
    dataX = {partitionEnd + 0.5, partitionEnd + 0.5};
    vLine = Curve(dataX, dataY);
    vLine.setColor("gray");
    vLine.setLineStyle("dashed");
    graph.add(vLine);

    // cluster shade
    const Point rectX = {partitionStart - 0.5, partitionEnd + 0.5, partitionEnd + 0.5, partitionStart - 0.5};
    const Point rectY = {yMin, yMin, yMax, yMax};
    Polygon rect(rectX, rectY);
    rect.setColor("lightgray");
    graph.add(rect);
  }

  // add threshold horizontal line
  const Point dataX = {-0.5, static_cast<Scalar>(size) + 0.5};
  const Point dataY(2, threshold);
  Curve hLine(dataX, dataY);
  hLine.setColor("black");
  hLine.setLineStyle("dashed");
  graph.add(hLine);
  Text elt(Point({1.0}), Point({threshold}), {"threshold"}, "top");
  elt.setColor("black");
  graph.add(elt);

  // add cloud
  Cloud allCloud(indexSample, sample_);
  allCloud.setPointStyle(".");
  allCloud.setColor("black");
  graph.add(allCloud);

  // add peaks cloud
  Cloud peakCloud(peakIndices, peaks);
  peakCloud.setPointStyle(".");
  peakCloud.setColor("red");
  graph.add(peakCloud);

  return graph;
}

Sample SamplePartition::getSample() const
{
  return sample_;
}

Collection<Indices> SamplePartition::getIndicesCollection() const
{
  return indicesCollection_;
}

/* String converter */
String SamplePartition::__repr__() const
{
  OSS oss(true);
  oss << "class= " << SamplePartition::GetClassName();
  return oss;
}

/* String converter */
String SamplePartition::__str__(const String & ) const
{
  OSS oss(false);
  oss << "class= " << SamplePartition::GetClassName();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SamplePartition::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("sample_", sample_);
  adv.saveAttribute("indicesCollection_", indicesCollection_);
}

/* Method load() reloads the object from the StorageManager */
void SamplePartition::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("sample_", sample_);
  adv.loadAttribute("indicesCollection_", indicesCollection_);
}

END_NAMESPACE_OPENTURNS
