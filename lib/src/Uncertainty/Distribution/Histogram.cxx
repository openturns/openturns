//                                               -*- C++ -*-
/**
 *  @brief The Histogram distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/Histogram.hxx"
#include "openturns/BarPlot.hxx"
#include "openturns/Curve.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Histogram);

static const Factory<Histogram> Factory_Histogram;

/* Default constructor */
Histogram::Histogram()
  : ContinuousDistribution()
  , first_(0.0)
  , width_(1)
  , height_(1, 1.0)
  , cumulatedWidth_(0)
{
  setName( "Histogram" );
  // This call set also the range.
  setData(NumericalPoint(1, 1.0), NumericalPoint(1, 1.0));
  setDimension( 1 );
}

/* Parameters constructor */
Histogram::Histogram(const NumericalScalar first,
                     const NumericalPoint & width,
                     const NumericalPoint & height)
  : ContinuousDistribution()
  , first_(first)
  , width_(0)
  , height_(0)
  , cumulatedWidth_(0)
{
  setName("Histogram");
  // This call set also the range.
  setData(width, height);
  setDimension(1);
}

/* Comparison operator */
Bool Histogram::operator ==(const Histogram & other) const
{
  if (this == &other) return true;
  return (first_ == other.first_) && (width_ == other.width_) && (height_ == other.height_);
}

Bool Histogram::equals(const DistributionImplementation & other) const
{
  const Histogram* p_other = dynamic_cast<const Histogram*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Histogram::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Histogram::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " first=" << first_
      << " width=" << width_
      << " height=" << height_;
  return oss;
}

String Histogram::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(origin = " << first_;
  for (UnsignedInteger i = 0; i < width_.getSize(); ++i) oss << ", {w" << i << " = " << width_[i] << ", h" << i << " = " << height_[i] << "}";
  oss << ")";
  return oss;
}

/* Virtual constructor */
Histogram * Histogram::clone() const
{
  return new Histogram(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Histogram::computeRange()
{
  const UnsignedInteger size = cumulatedWidth_.getSize();
  if (size == 0) return;
  setRange(Interval(first_, first_ + cumulatedWidth_[size - 1]));
}

/* Get one realization of the distribution */
NumericalPoint Histogram::getRealization() const
{
  return computeQuantile(RandomGenerator::Generate());
}


/* Get the DDF of the distribution */
NumericalPoint Histogram::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return NumericalPoint(1, 0.0);
}


/* Get the PDF of the distribution */
NumericalScalar Histogram::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  NumericalScalar x = point[0] - first_;
  const UnsignedInteger size = width_.getSize();
  if ((x < 0.0) || (x >= cumulatedWidth_[size - 1])) return 0.0;
  // Find the bin index by bisection
  UnsignedInteger iMin = 0;
  if (x < cumulatedWidth_[iMin]) return height_[iMin];
  UnsignedInteger iMax = size - 1;
  while (iMax > iMin + 1)
  {
    const UnsignedInteger i = (iMin + iMax) / 2;
    if (x < cumulatedWidth_[i]) iMax = i;
    else iMin = i;
  }
  return height_[iMax];
}


/* Get the CDF of the distribution */
NumericalScalar Histogram::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  NumericalScalar x = point[0] - first_;
  const UnsignedInteger size = width_.getSize();
  if (x <= 0.0) return 0.0;
  if (x >= cumulatedWidth_[size - 1]) return 1.0;
  // Find the bin index by bisection
  UnsignedInteger iMin = 0;
  if (x < cumulatedWidth_[iMin]) return x * height_[iMin];
  UnsignedInteger iMax = size - 1;
  while (iMax > iMin + 1)
  {
    const UnsignedInteger i = (iMin + iMax) / 2;
    if (x < cumulatedWidth_[i]) iMax = i;
    else iMin = i;
  }
  return cumulatedSurface_[iMax] + (x - cumulatedWidth_[iMax]) * height_[iMax];
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex Histogram::computeCharacteristicFunction(const NumericalScalar x) const
{
  if (x == 0.0) return 1.0;
  NumericalComplex result(0.0);
  const UnsignedInteger size = width_.getSize();
  if (std::abs(cumulatedWidth_[size - 1] * x) < 1e-10)
  {
    NumericalScalar term = height_[0] * cumulatedWidth_[0] * cumulatedWidth_[0];
    for (UnsignedInteger k = 1; k < size; ++k) term += height_[k] * (cumulatedWidth_[k - 1] + cumulatedWidth_[k]) * width_[k];
    result = NumericalComplex(1.0, 0.5 * x * term);
  }
  else
  {
    result = height_[0] * SpecFunc::Expm1(NumericalComplex(0.0, cumulatedWidth_[0] * x));
    for (UnsignedInteger k = 1; k < size; ++k) result += height_[k] * (std::exp(NumericalComplex(0.0, cumulatedWidth_[k] * x)) - std::exp(NumericalComplex(0.0, cumulatedWidth_[k - 1] * x)));
    result /= NumericalComplex(0.0, x);
  }
  result *= std::exp(NumericalComplex(0.0, first_ * x));
  return result;
}

/* Get the PDFGradient of the distribution */
NumericalPoint Histogram::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In Histogram::computePDFGradient(const NumericalPoint & point) const";
}

/* Get the CDFGradient of the distribution */
NumericalPoint Histogram::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  throw NotYetImplementedException(HERE) << "In Histogram::computeCDFGradient(const NumericalPoint & point) const";
}

/* Get the quantile of the distribution */
NumericalScalar Histogram::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  const NumericalScalar p = tail ? 1.0 - prob : prob;
  if (p <= 0.0) return first_;
  const UnsignedInteger size = width_.getSize();
  if (p >= 1.0) return first_ + cumulatedWidth_[size - 1];
  // Search of the bin
  UnsignedInteger index(p * size);
  NumericalScalar currentProba = cumulatedSurface_[index];
  UnsignedInteger currentIndex = index;
  // Basic search: upper bound. The loop must end because cumulatedSurface_[size - 1] = 1.0 and prob < 1.0
  while (p >= currentProba)
  {
    ++currentIndex;
    currentProba = cumulatedSurface_[currentIndex];
  }
  // At the end of the loop, we are sure that currentProba > p
  // If index < currentIndex, it means that p is associated with bin number currentIndex. Do a linear interpolation.
  if (index < currentIndex)
  {
    // currentIndex is now the number of the bin associated with prob
    return first_ + cumulatedWidth_[currentIndex] + (p - currentProba) / height_[currentIndex];
  }
  // Here we know that we have to go downstairs. We must check that currentIndex remains >= 0 in the loop.
  while ((p < currentProba) && (currentIndex > 0))
  {
    --currentIndex;
    currentProba = cumulatedSurface_[currentIndex];
  }
  // At the end of the loop, either p < cumulatedSurface_[0], which means that prob is associated with the first bin...
  if (p < currentProba) return first_ + width_[0] * p / currentProba;
  // ... or p >= cumulatedSurface_[currentIndex], which means that p is associated with the bin number currentIndex + 1. Do a linear interpolation.
  return first_ + cumulatedWidth_[currentIndex] + (p - currentProba) / height_[currentIndex + 1];
}

/* Compute the mean of the distribution */
void Histogram::computeMean() const
{
  NumericalScalar mean = first_;
  const UnsignedInteger size = width_.getSize();
  NumericalScalar lower = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar upper = lower + width_[i];
    mean += 0.5 * width_[i] * height_[i] * (lower + upper);
    lower = upper;
  }
  mean_ = NumericalPoint(1, mean);
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void Histogram::computeCovariance() const
{
  NumericalScalar value = 0.0;
  const UnsignedInteger size = width_.getSize();
  covariance_ = CovarianceMatrix(1);
  // Since variance is invariant by translation, we center the data for numerical stability
  NumericalScalar lower = first_ - getMean()[0];
  for (UnsignedInteger i = 0; i < size; i++)
  {
    const NumericalScalar upper = lower + width_[i];
    value += width_[i] * height_[i] * (lower * lower + lower * upper + upper * upper);
    lower = upper;
  }
  covariance_(0, 0) = value / 3.0;
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
NumericalPoint Histogram::getStandardMoment(const UnsignedInteger n) const
{
  if (n == 0) return NumericalPoint(1, 1.0);
  NumericalScalar value = 0.0;
  const UnsignedInteger size = width_.getSize();
  NumericalScalar xPrec = -1.0;
  const NumericalScalar factor = 2.0 / cumulatedWidth_[size - 1];
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar x = xPrec + width_[i] * factor;
    value += (std::pow(x, static_cast<int>(n + 1)) - std::pow(xPrec, static_cast<int>(n + 1))) * height_[i];
    xPrec = x;
  }
  value /= (n + 1) * factor;
  return NumericalPoint(1, value);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Histogram::Implementation Histogram::getStandardRepresentative() const
{
  const UnsignedInteger size = width_.getSize();
  // No need to transform an histogram if its range is already [-1.0, 1.0]
  if (first_ == -1.0 && std::abs(cumulatedWidth_[size - 1] - 2.0) <= ResourceMap::GetAsNumericalScalar("Distribution-DefaultQuantileEpsilon")) return clone();
  const NumericalScalar first = -1.0;
  const NumericalScalar factor = 2.0 / cumulatedWidth_[size - 1];
  return Histogram(first, factor * width_, height_ / factor).clone();
}

/* Parameters value and description accessor */

/* Parameters value accessor */
NumericalPoint Histogram::getParameter() const
{
  const UnsignedInteger size = width_.getSize();
  NumericalPoint parameter(1 + 2 * size);
  parameter[0] = first_;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    parameter[2 * i + 1] = width_[i];
    parameter[2 * i + 2] = height_[i];
  }
  return parameter;
}

void Histogram::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() % 2 == 0) throw InvalidArgumentException(HERE) << "Error: expected an odd number of values, got " << parameter.getSize();
  const NumericalScalar w = getWeight();
  const UnsignedInteger size = (parameter.getSize() - 1) / 2;
  first_ = parameter[0];
  width_ = NumericalPoint(size);
  height_ = NumericalPoint(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    width_[i] = parameter[2 * i + 1];
    height_[i] = parameter[2 * i + 2];
  }

  *this = Histogram(first_, width_, height_);
  setWeight(w);
}

/* Parameters description accessor */
Description Histogram::getParameterDescription() const
{
  const UnsignedInteger size = width_.getSize();
  Description description(1 + 2 * size);
  description[0] = "first";
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    {
      OSS oss;
      oss << "width_" << i;
      description[2 * i + 1] = oss;
    }
    {
      OSS oss;
      oss << "height_" << i;
      description[2 * i + 2] = oss;
    }
  }
  return description;
}


/* Interface specific to Histogram */

/* First point accessor */
void Histogram::setFirst(const NumericalScalar first)
{
  if (first != first_)
  {
    first_ = first;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on first
    computeRange();
  }
}

NumericalScalar Histogram::getFirst() const
{
  return first_;
}


void Histogram::setData(const NumericalPoint & l,
                        const NumericalPoint & h)
{
  NumericalScalar surface = 0.0;
  const UnsignedInteger size = l.getSize();
  if (size == 0) throw InvalidArgumentException(HERE) << "Error: the given width has a size of 0.";
  if (size != h.getSize()) throw InvalidArgumentException(HERE) << "Error: the width and the height must have the same size.";

  cumulatedWidth_ = NumericalPoint(size);
  cumulatedSurface_ = NumericalPoint(size);
  // first, check that all the heights and widths are >=0
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    NumericalScalar height = h[i];
    if (!(height >= 0.0)) throw InvalidArgumentException(HERE) << "Error: all the heights must be >= 0, here values=" << h;
    NumericalScalar width = l[i];
    if (!(width > 0.0)) throw InvalidArgumentException(HERE) << "Error: all the widths must be > 0, here value=" << l;
    surface += height * width;
    cumulatedWidth_[i] = width + (i == 0 ? 0.0 : cumulatedWidth_[i - 1]);
    cumulatedSurface_[i] = surface;
  }
  // Check if the surface is strictly positive
  if (surface < ResourceMap::GetAsNumericalScalar("Distribution-DefaultCDFEpsilon")) throw InvalidArgumentException(HERE) << "Error: the surface of the histogram is zero.";
  // Normalization
  NumericalScalar normalizationFactor = 1.0 / surface;
  width_ = NumericalPoint(size, 1.0);
  height_ = NumericalPoint(size, 1.0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    width_[i] = l[i];
    height_[i] = h[i] * normalizationFactor;
    cumulatedSurface_[i] *= normalizationFactor;
  }
  // Here the value could be slightly different from 1, fix it.
  cumulatedSurface_[size - 1] = 1.0;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}


NumericalPoint Histogram::getWidth() const
{
  return width_;
}


NumericalPoint Histogram::getHeight() const
{
  return height_;
}


/* Get the PDF singularities inside of the range - 1D only */
NumericalPoint Histogram::getSingularities() const
{
  const UnsignedInteger size = width_.getSize();
  // Here we know that size > 0
  NumericalScalar x = first_ + width_[0];
  NumericalPoint singularities(1, x);
  for (UnsignedInteger i = 1; i < size - 1; ++i)
  {
    x += width_[i];
    singularities.add(x);
  }
  return singularities;
}

/* Draw the PDF of the Histogram using a specific presentation */
Graph Histogram::drawPDF() const
{
  const UnsignedInteger lastIndex = cumulatedWidth_.getSize() - 1;
  // Must prefix explicitely by the class name in order to avoid conflict with the methods in the upper class
  return Histogram::drawPDF(first_ - 0.5 * width_[0], first_ + cumulatedWidth_[lastIndex] + 0.5 * width_[lastIndex]);
}

/* Draw the PDF of the Histogram using a specific presentation */
Graph Histogram::drawPDF(const NumericalScalar xMin,
                         const NumericalScalar xMax,
                         const UnsignedInteger pointNumber) const
{
  if (xMax <= xMin) throw InvalidArgumentException(HERE) << "Error: cannot draw a PDF with xMax >= xMin, here xmin=" << xMin << " and xmax=" << xMax;
  const String title(OSS() << getDescription()[0] << " PDF");
  const String xName(getDescription()[0]);
  Graph graphPDF(title, xName, "PDF", true, "topright");
  const UnsignedInteger size = width_.getSize();
  // Check for the border cases
  // If the histogram is completely at the right or at the left of the plot range,
  // just draw an horizontal line
  if ((xMax < first_) || (xMin > first_ + cumulatedWidth_[size - 1]))
  {
    Sample data(2, 2);
    data[0][0] = xMin;
    data[0][1] = 0.0;
    data[1][0] = xMax;
    data[1][1] = 0.0;
    graphPDF.add(Curve(data, "red", "solid", 2, title));
    return graphPDF;
  }
  // Find the index of the left bar to draw
  UnsignedInteger indexLeft = 0;
  while (first_ + cumulatedWidth_[indexLeft] < xMin) ++indexLeft;
  // Another special case: the plot range covers only partially a unique bar
  if (first_ + cumulatedWidth_[indexLeft] >= xMax)
  {
    Sample data(2, 2);
    data[0][0] = xMin;
    data[0][1] = height_[indexLeft];
    data[1][0] = xMax;
    data[1][1] = height_[indexLeft];
    graphPDF.add(Curve(data, "red", "solid", 2, title));
    return graphPDF;
  }
  // Find the index of the right bar to draw
  UnsignedInteger indexRight = indexLeft;
  UnsignedInteger shiftFull = 0;
  Sample dataFull(0, 2);
  while ((indexRight < size) && (first_ + cumulatedWidth_[indexRight] < xMax)) ++indexRight;
  // The graph is made of full bars for the class indices between indexLeft and indexRight
  // The graph can include completely the first class or not. It is checked using first_.
  // The graph can include completely the last class or not. It is checked usind indexRight == size.
  // !!! Only the first part of the graph has a label !!!
  // The first class is completely included
  if (xMin <= first_)
  {
    Sample data(2, 2);
    data[0][0] = xMin;
    data[0][1] = 0.0;
    data[1][0] = first_;
    data[1][1] = 0.0;
    dataFull.add(data);
  }
  // The first class that appears in the graph is only partially included
  else
  {
    Sample data(3, 2);
    data[0][0] = xMin;
    data[0][1] = height_[indexLeft];
    data[1][0] = first_ + cumulatedWidth_[indexLeft];
    data[1][1] = data[0][1];
    data[2][0] = data[1][0];
    data[2][1] = 0.0;
    dataFull.add(data);
    shiftFull = 1;
  }
  // Central part of the graph
  NumericalScalar startX = first_;
  if (indexLeft + shiftFull > 0) startX += cumulatedWidth_[indexLeft + shiftFull];
  for (UnsignedInteger i = indexLeft + shiftFull; i < indexRight; ++i)
  {
    Sample data(4, 2);
    data[0][0] = startX;
    data[0][1] = 0.0;
    data[1][0] = startX;
    data[1][1] = height_[i];
    startX += width_[i];
    data[2][0] = startX;
    data[2][1] = height_[i];
    data[3][0] = startX;
    data[3][1] = 0.0;
    dataFull.add(data);
  }
  // The last class is completely included
  if (indexRight == size)
  {
    Sample data(2, 2);
    data[0][0] = first_ + cumulatedWidth_[size - 1];
    data[0][1] = 0.0;
    data[1][0] = xMax;
    data[1][1] = 0.0;
    dataFull.add(data);
  }
  // The last class that appears in the graph is only partially included
  else
  {
    Sample data(3, 2);
    data[0][0] = first_ + cumulatedWidth_[indexRight - 1];
    data[0][1] = 0.0;
    data[1][0] = data[0][0];
    data[1][1] = height_[indexRight];
    data[2][0] = xMax;
    data[2][1] = height_[indexRight];
    dataFull.add(data);
  }
  Curve curve(dataFull, "red", "solid", 2, "");
  curve.setLegend(title);
  graphPDF.add(curve);
  NumericalPoint bb(graphPDF.getBoundingBox());
  bb[3] *= 1.1;
  graphPDF.setBoundingBox(bb);
  return graphPDF;
}

/* Method save() stores the object through the StorageManager */
void Histogram::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "first_", first_);
  adv.saveAttribute( "width_", width_);
  adv.saveAttribute( "height_", height_);
  adv.saveAttribute( "cumulatedWidth_", cumulatedWidth_);
  adv.saveAttribute( "cumulatedSurface_", cumulatedSurface_);
}

/* Method load() reloads the object from the StorageManager */
void Histogram::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "first_", first_);
  adv.loadAttribute( "width_", width_);
  adv.loadAttribute( "height_", height_);
  adv.loadAttribute( "cumulatedWidth_", cumulatedWidth_);
  adv.loadAttribute( "cumulatedSurface_", cumulatedSurface_);
  computeRange();
}

END_NAMESPACE_OPENTURNS

