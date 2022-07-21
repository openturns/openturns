//                                               -*- C++ -*-
/**
 *  @brief PointWithDescription extends the classical mathematical point with a description of the coordinates
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
#include "openturns/PointWithDescription.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PointWithDescription)
TEMPLATE_CLASSNAMEINIT(PersistentCollection<PointWithDescription>)

static const Factory<PointWithDescription> Factory_PointWithDescription;
static const Factory<PersistentCollection<PointWithDescription> > Factory_PersistentCollection_PointWithDescription;

/* Default constructor */
PointWithDescription::PointWithDescription()
  : Point()
  , description_()
{
  // Nothing to do
}

/* Constructor with size */
PointWithDescription::PointWithDescription(const UnsignedInteger size,
    const Scalar value)
  : Point(size, value)
  , description_(size)
{
  // Nothing to do
}



/* Constructor from a collection */
PointWithDescription::PointWithDescription(const Collection<Scalar> & coll)
  : Point(coll)
  , description_(coll.getSize())
{
  // Nothing to do
}

/* Constructor from a base class */
PointWithDescription::PointWithDescription(const Pointer<Point> & p_base)
  : Point(*p_base)
  , description_(p_base->getSize())
{
  // Nothing to do
}


/* Constructor from a collection */
PointWithDescription::PointWithDescription(std::initializer_list<Scalar> initList)
  : Point(initList)
  , description_(initList.size())
{
  // Nothing to do
}


/* Virtual constructor */
PointWithDescription * PointWithDescription::clone() const
{
  return new PointWithDescription(*this);
}

/* String converter */
String PointWithDescription::__repr__() const
{
  return OSS() << "class=" << PointWithDescription::GetClassName()
         << " name=" << getName()
         << " dimension=" << getDimension()
         << " description=" << getDescription()
         << " values=" << PersistentCollection<Scalar>::__repr__();
}

class KeyValuePair : public Object
{
  String key_;
  Scalar value_;
public:
  KeyValuePair(const String & key = "", Scalar value = 0.): key_(key), value_(value) {}
  String __repr__() const
  {
    return OSS() << key_ << " : " << value_;
  }
  String __str__(const String & ) const
  {
    return OSS() << key_ << " : " << value_;
  }
};

String PointWithDescription::__str__(const String & ) const
{
  const UnsignedInteger size = getSize();
  const Description desc = getDescription();
  std::vector<KeyValuePair> assoc(size);
  for (UnsignedInteger i = 0; i < size; ++i) assoc[i] = KeyValuePair(i < desc.getSize() ? desc[i] : "", (*this)[i]);

  OSS oss(false);
  oss << "[";
  std::copy(assoc.begin(), assoc.end(), OSS_iterator<KeyValuePair>(oss, ", "));
  oss << "]";
  return oss;
}




/* Description Accessor */
void PointWithDescription::setDescription(const Description & description)
{
  if (description.getSize() != getDimension())
    throw InvalidArgumentException(HERE) << "Description has incorrect dimension (" << description.getSize()
                                         << "). Expected " << getDimension();
  description_ = description;
}


/* Description Accessor */
Description PointWithDescription::getDescription() const
{
  return description_;
}




/* Method save() stores the object through the StorageManager */
void PointWithDescription::save(Advocate & adv) const
{
  Point::save(adv);
  adv.saveAttribute( "description_", description_ );
}


/* Method load() reloads the object from the StorageManager */
void PointWithDescription::load(Advocate & adv)
{
  Point::load(adv);
  adv.loadAttribute( "description_", description_ );
}


END_NAMESPACE_OPENTURNS
