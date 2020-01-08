//                                               -*- C++ -*-
/**
 *  @brief Pairs class for contourchart plots
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cstdio>
#include <cstdlib>


#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/Os.hxx"
#include "openturns/Pairs.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Pairs)

static const Factory<Pairs> Factory_Pairs;

/* Default constructor */
Pairs::Pairs()
  : DrawableImplementation()
{}


/* Default constructor */
Pairs::Pairs(const Sample & data,
             const String & )
  : DrawableImplementation(data)
{
  checkData(data);
  labels_ = data_.getDescription();
  if(labels_.getSize() == 0) buildDefaultLabels();
}

/* Constructor with parameters*/
Pairs::Pairs(const Sample & data,
             const String & title,
             const Description & labels,
             const String & color,
             const String & pointStyle)
  : DrawableImplementation(data)
{
  checkData(data);
  setTitle(title);
  setColor(color);
  setLabels(labels);
  setPointStyle(pointStyle);
}


/* String converter */
String Pairs::__repr__() const
{
  OSS oss;
  oss << "class=" << Pairs::GetClassName()
      << " name=" << getName()
      << " data=" << data_
      << " derived from " << DrawableImplementation::__repr__();
  return oss;
}

/* Accessor for first coordinate */
Sample Pairs::getData() const
{
  return data_;
}

/* Accessor for labels */
Description Pairs::getLabels() const
{
  return labels_;
}

void Pairs::setLabels(const Description & labels)
{
  if (labels.getSize() != data_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the labels size must be equal equal to the data dimension";
  labels_ = labels;
}

/* Clean all the temporary data created by draw() method */
void Pairs::clean() const
{
  DrawableImplementation::clean();
  if (dataFileName_ != "") Os::Remove(dataFileName_);
}

/* Draw method */
String Pairs::draw() const
{
  dataFileName_ = "";
  OSS oss;
  if (pointStyle_ != "none")
  {
    // Stores the data in a temporary file
    // The specific R command for drawing
    oss << "dim_ <- " << data_.getDimension() << "\n" ;
    oss << "size_ <- " << data_.getSize() << "\n";
    oss << DrawableImplementation::draw() << "\n";
    oss << "description=c(";
    const UnsignedInteger length = data_.getDimension();
    for(UnsignedInteger i = 0; i < length - 1; ++i) oss << "\"" << labels_[i] << "\"" << ",";
    oss << "\"" << labels_[length - 1] << "\"" << ") \n";
    // DataFrame organisation
    const String code((OSS() << getPointCode(pointStyle_)));
    oss << "dataOT = data.frame(dataOT) \n";
    oss << "names(dataOT) <- description \n";
    oss << "points(pairs(dataOT "
        << ",pch=" << (pointStyle_ == "dot" ? "\".\"" : code)
        << ",col=\"" << color_ << "\""
        << ",main=\"" << getTitle() << "\"))";
  }
  return oss;
}

/* Clone method */
Pairs * Pairs::clone() const
{
  return new Pairs(*this);
}

String Pairs::getTitle() const
{
  return title_;
}
void Pairs::setTitle(const String & title)
{
  title_ = title;
}

/* Check for data validity */
void Pairs::checkData(const Sample & data) const
{
  if (data.getDimension() < 2)
  {
    throw InvalidDimensionException(HERE) << "Expected sample of dimension >=2: got " << data.getDimension();
  }
}

/* Build default labels by taking the level values */
void Pairs::buildDefaultLabels()
{
  const UnsignedInteger number = data_.getDimension();
  labels_ = Description(number);
  for (UnsignedInteger i = 0; i < number; ++i) labels_[i] = OSS() << "V" << i + 1;
}

/* Accessor for boundingbox */
Interval Pairs::getBoundingBox() const
{
  // data can be of any dimension
  return Interval(2);
}

/* Method save() stores the object through the StorageManager */
void Pairs::save(Advocate & adv) const
{
  DrawableImplementation::save(adv);
  adv.saveAttribute( "labels_", labels_ );
  adv.saveAttribute( "title_", title_ );
}

/* Method load() reloads the object from the StorageManager */
void Pairs::load(Advocate & adv)
{
  DrawableImplementation::load(adv);
  adv.loadAttribute( "labels_", labels_ );
  adv.loadAttribute( "title_", title_ );
}



END_NAMESPACE_OPENTURNS
