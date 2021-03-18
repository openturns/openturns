//                                               -*- C++ -*-
/**
 *  @brief Associate to a given point its class index
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

#include "openturns/MinimumVolumeClassifier.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/Contour.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MinimumVolumeClassifier)

static const Factory<MinimumVolumeClassifier> Factory_MinimumVolumeClassifier;

/* Default constructor */
MinimumVolumeClassifier::MinimumVolumeClassifier()
  : ClassifierImplementation()
{
  // Nothing to do
}

/* Default constructor */
MinimumVolumeClassifier::MinimumVolumeClassifier(const Distribution & distribution,
    const Point & alpha)
  : ClassifierImplementation()
  , distribution_(distribution)
  , alpha_(alpha)
{
  if (!distribution.isContinuous())
    throw InvalidArgumentException(HERE) << "Distribution must be continuous";
  std::sort(alpha_.begin(), alpha_.end());
  std::unique(alpha_.begin(), alpha_.end());
  if (alpha != alpha_)
    throw InvalidArgumentException(HERE) << "Alpha must be sorted and unique";
  const UnsignedInteger size = alpha_.getSize();
  if (!size)
    throw InvalidArgumentException(HERE) << "Alpha must be non empty";
  for (UnsignedInteger i = 0; i < size; ++ i)
    if (!(alpha_[i] >= 0.0) || !(alpha_[i] <= 1.0))
      throw InvalidArgumentException(HERE) << "Confidence level must be in [0, 1]";
}

/* Virtual constructor */
MinimumVolumeClassifier * MinimumVolumeClassifier::clone() const
{
  return new MinimumVolumeClassifier( *this );
}

/* String converter */
String MinimumVolumeClassifier::__repr__() const
{
  return OSS() << "class=" << GetClassName()
         << " distribution=" << distribution_
         << " alpha=" << alpha_;
}

UnsignedInteger MinimumVolumeClassifier::getNumberOfClasses() const
{
  return alpha_.getSize() + 1;
}


/* Associate a set of points to a class */
UnsignedInteger MinimumVolumeClassifier::classify(const Point& inP) const
{
  if (inP.getDimension() != distribution_.getDimension())
    throw InvalidArgumentException(HERE) << "Error: the point to classify has dimension=" << inP.getDimension() << " but the classifier expects dimension=" << distribution_.getDimension();
  (void) getLevelSet();
  const Scalar pdf = distribution_.computeLogPDF(inP);
  UnsignedInteger k = 0;
  for (; k < threshold_.getSize(); ++ k)
    if (pdf > std::log(threshold_[k]))
      break;
  return k;
}

Indices MinimumVolumeClassifier::classify(const Sample & inS) const
{
  // overload here because getLevelSet is not thread-safe
  (void) getLevelSet();

  return ClassifierImplementation::classify(inS);
}

LevelSet MinimumVolumeClassifier::getLevelSet(const UnsignedInteger j) const
{
  if (!isLevelSetComputed_)
  {
    levelSetCollection_.resize(alpha_.getSize());
    threshold_.resize(alpha_.getSize());
    for (UnsignedInteger i = 0; i < alpha_.getSize(); ++ i)
      levelSetCollection_[i] = distribution_.computeMinimumVolumeLevelSetWithThreshold(alpha_[i], threshold_[i]);
    isLevelSetComputed_ = true;
  }
  if (j >= alpha_.getSize())
    throw InvalidDimensionException(HERE) << "j";
  return levelSetCollection_[j];
}

Scalar MinimumVolumeClassifier::grade(const Point& inP,
                                      const UnsignedInteger outC) const
{
  if (outC >= getNumberOfClasses())
    throw InvalidDimensionException(HERE) << "Class number (=" << outC << ") must be lower than number of classes (" << getNumberOfClasses() << ")";
  (void) getLevelSet();
  const Scalar pdf = distribution_.computeLogPDF(inP);
  UnsignedInteger k = 0;
  for (; k < threshold_.getSize(); ++ k)
    if (pdf > std::log(threshold_[k]))
      break;
  const Scalar sign = ((k == outC) ? 1.0 : -1.0);
  return pdf * sign;
}

/* Dimension accessor */
UnsignedInteger MinimumVolumeClassifier::getDimension() const
{
  return distribution_.getDimension();
}

/** Distribution accessors */
Distribution MinimumVolumeClassifier::getDistribution() const
{
  return distribution_;
}

Point MinimumVolumeClassifier::getThreshold() const
{
  (void) getLevelSet();
  return threshold_;
}

/* Method save() stores the object through the StorageManager */
void MinimumVolumeClassifier::save(Advocate & adv) const
{
  ClassifierImplementation::save(adv);
  adv.saveAttribute( "distribution_", distribution_ );
  adv.saveAttribute( "alpha_", alpha_ );
}

/* Method load() reloads the object from the StorageManager */
void MinimumVolumeClassifier::load(Advocate & adv)
{
  ClassifierImplementation::load(adv);
  adv.loadAttribute( "distribution_", distribution_ );
  adv.loadAttribute( "alpha_", alpha_ );
}

GridLayout MinimumVolumeClassifier::drawContour(const Point & contourAlpha) const
{
  const UnsignedInteger dimension = distribution_.getDimension();
  GridLayout grid(dimension, dimension);

  // compute contour thresholds from confidence levels
  Point sortedAlpha(contourAlpha);
  std::sort(sortedAlpha.begin(), sortedAlpha.end(), std::greater<Scalar>());
  Point thresholds(contourAlpha.getSize());
  Description labels(contourAlpha.getSize());
  for (UnsignedInteger i = 0; i < contourAlpha.getSize(); ++ i)
  {
    (void)distribution_.computeMinimumVolumeLevelSetWithThreshold(sortedAlpha[i], thresholds[i]);
    labels[i] = (OSS().setPrecision(3) << thresholds[i]);
  }

  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    Graph pdfGraph(distribution_.getMarginal(i).drawPDF());
    pdfGraph.setLegends(Description(1));
    pdfGraph.setYTitle(i == 0 ? distribution_.getDescription()[i] : "");
    pdfGraph.setXTitle(i == dimension - 1 ? distribution_.getDescription()[i] : "");
    grid.setGraph(i, i, pdfGraph);
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      const Indices indices = {j, i};
      Graph contour(distribution_.getMarginal(indices).drawPDF());
      // by default we get several contours with each its color, keep only first contour to use a custom number of levels
      contour.setDrawables(Graph::DrawableCollection(1, contour.getDrawable(0)));
      contour.setColors(Description(1, "black"));
      contour.setYTitle(j == 0 ? distribution_.getDescription()[i] : "");
      contour.setXTitle(i == dimension - 1 ? distribution_.getDescription()[j] : "");
      contour.setLegends(Description(contour.getLegends().getSize()));
      dynamic_cast<Contour*>(contour.getDrawable(0).getImplementation().get())->setLevels(thresholds);
      dynamic_cast<Contour*>(contour.getDrawable(0).getImplementation().get())->setLabels(labels);
      dynamic_cast<Contour*>(contour.getDrawable(0).getImplementation().get())->setDrawLabels(true);
      grid.setGraph(i, j, contour);
    }
  }
  return grid;
}

GridLayout MinimumVolumeClassifier::drawSample(const Sample & sample, const Indices & classes) const
{
  const UnsignedInteger dimension = distribution_.getDimension();
  if (sample.getDimension() != distribution_.getDimension())
    throw InvalidArgumentException(HERE) << "Error: the sample has dimension=" << sample.getDimension() << " but the classifier expects dimension=" << dimension;
  if (classes.isEmpty() || !classes.check(getNumberOfClasses()))
    throw InvalidArgumentException(HERE) << "Classes must be in [0,"<<getNumberOfClasses()<<"[";
  GridLayout grid(dimension - 1, dimension - 1);
  const Description description(distribution_.getDescription());

  // split sample per classes
  Indices classifiedSample(classify(sample));
  Collection<Sample> separatedSamples(classes.getSize(), Sample(0, dimension));
  for (UnsignedInteger i = 0; i < sample.getSize(); ++ i)
  {
    for (UnsignedInteger k = 0; k < classes.getSize(); ++ k)
    {
      if (classifiedSample[i] == classes[k])
        separatedSamples[k].add(sample[i]);
    }
  }
  Description colors = DrawableImplementation::BuildDefaultPalette(getNumberOfClasses());
  if (dimension == 1)
  {
    grid = GridLayout(1, 1);
    Graph graph("", description[0], "", true, "topright");
    for (UnsignedInteger k = 0; k < classes.getSize(); ++ k)
    {
      Sample sampleXC(separatedSamples[k]);
      sampleXC.stack(Sample(sampleXC.getSize(), 1));
      const Cloud cloud(sampleXC, colors[classes[k]], "fsquare", "");
      graph.add(cloud);
    }
    grid.setGraph(0, 0, graph);
  }
  else
  {
    for (UnsignedInteger i = 0; i < dimension; ++ i)
    {
      for (UnsignedInteger j = 0; j < i; ++ j)
      {
        const Indices indices = {j, i};
        Graph graph("", i == dimension - 1 ? description[j] : "", j == 0 ? description[i] : "", true, "topright");
        for (UnsignedInteger k = 0; k < classes.getSize(); ++ k)
        {
          const Cloud cloud(separatedSamples[k].getMarginal(indices), colors[classes[k]], "fsquare", "");
          graph.add(cloud);
        }
        grid.setGraph(i - 1, j, graph);
      }
    }
  }
  return grid;
}


GridLayout MinimumVolumeClassifier::drawContourAndSample(const Point & alpha, const Sample & sample, const Indices & classes) const
{
  const UnsignedInteger dimension = distribution_.getDimension();
  GridLayout grid(drawContour(alpha));
  GridLayout gridSamples(drawSample(sample, classes));
  if (dimension == 1)
    grid.getGraph(0, 0).getImplementation()->add(*gridSamples.getGraph(0, 0).getImplementation());
  else
  {
    for (UnsignedInteger i = 0; i < dimension; ++ i)
      for (UnsignedInteger j = 0; j < i; ++ j)
        grid.getGraph(i, j).getImplementation()->add(*gridSamples.getGraph(i - 1, j).getImplementation());
  }
  return grid;
}


END_NAMESPACE_OPENTURNS
