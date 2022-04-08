//                                               -*- C++ -*-
/**
 *  @brief Cloud class for cloud plots
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
#include "openturns/Cloud.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Cloud)

static const Factory<Cloud> Factory_Cloud;


/* Default constructor */
Cloud::Cloud()
  : DrawableImplementation()
{}

/* Default constructor */
Cloud::Cloud(const Sample & data,
             const String & legend)
  : DrawableImplementation(data, legend)
{
  // Check data validity
  setData(data);
}

/* Constructor from complex numbers */
Cloud::Cloud(const ComplexCollection & data,
             const String & legend)
  : DrawableImplementation(Sample(0, 2), legend)
{
  // Convert the complex numbers into a Sample
  const UnsignedInteger size = data.getSize();
  Sample sample(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    sample(i, 0) = data[i].real();
    sample(i, 1) = data[i].imag();
  }
  // Check data validity
  setData(sample);
}

/* Constructor from 2 data sets */
Cloud::Cloud(const Sample & dataX,
             const Sample & dataY,
             const String & legend):
  DrawableImplementation(Sample(0, 2), legend)
{
  const UnsignedInteger size = dataX.getSize();
  if (dataY.getSize() != size) throw InvalidArgumentException(HERE) << "Error: cannot build a Cloud based on two numerical samples with different size.";
  if ((dataX.getDimension() != 1) || (dataY.getDimension() != 1)) throw InvalidArgumentException(HERE) << "Error: cannot build a Cloud based on two numerical samples of dimension greater than 1.";
  Sample dataFull(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    dataFull(i, 0) = dataX(i, 0);
    dataFull(i, 1) = dataY(i, 0);
  }
  // Check data validity
  setData(dataFull);
}

Cloud::Cloud(const Point & dataX,
             const Point & dataY,
             const String & legend):
  DrawableImplementation(Sample(0, 2), legend)
{
  const UnsignedInteger size = dataX.getDimension();
  if (dataY.getDimension() != size) throw InvalidArgumentException(HERE) << "Error: cannot build a Cloud based on two numerical points with different dimension.";
  Sample dataFull(size, 2);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    dataFull(i, 0) = dataX[i];
    dataFull(i, 1) = dataY[i];
  }
  // Check data validity
  setData(dataFull);
}

/* Constructor with parameters */
Cloud::Cloud(const Sample & data,
             const String & color,
             const String & pointStyle,
             const String & legend)
  : DrawableImplementation(data, legend)
{
  // Check data validity
  setData(data);
  setColor(color);
  setPointStyle(pointStyle);
}

/* String converter */
String Cloud::__repr__() const
{
  OSS oss;
  oss << "class=" << Cloud::GetClassName()
      << " name=" << getName()
      << " derived from " << DrawableImplementation::__repr__();
  return oss;
}

/* Draw method */
String Cloud::draw() const
{
  dataFileName_ = "";
  OSS oss;
  if (pointStyle_ != "none")
  {
    // Stores the data in a temporary file
    oss << DrawableImplementation::draw() << "\n";
    // The specific R command for drawing
    oss << "points(dataOT[,1], dataOT[,2]";
    const String code((OSS() << getPointCode(pointStyle_)));
    oss << ",pch=" << (pointStyle_ == "dot" ? "\".\"" : code)
        << ",col=\"" << color_
        << "\",lwd=" << lineWidth_
        << ")";
  }
  return oss;
}

/* Clone method */
Cloud * Cloud::clone() const
{
  return new Cloud(*this);
}

/* Check validity of data */
void Cloud::checkData(const Sample & data) const
{
  if (data.getDimension() != 2)
    throw InvalidDimensionException(HERE) << "Expected sample of dimension 2: got " << data.getDimension();
}

/* Method save() stores the object through the StorageManager */
void Cloud::save(Advocate & adv) const
{
  DrawableImplementation::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void Cloud::load(Advocate & adv)
{
  DrawableImplementation::load(adv);
}



END_NAMESPACE_OPENTURNS
