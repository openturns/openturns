//                                               -*- C++ -*-
/**
 *  @brief TestResult implements the result of a statistical test
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "TestResult.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(TestResult);

static const Factory< TestResult > RegisteredFactory;

/* Default constructor */
TestResult::TestResult():
  PersistentObject(),
  testType_(""),
  binaryQualityMeasure_(false),
  pValueThreshold_(0.0),
  pValue_(0.0),
  description_(0)
{
  // Nothing to do
}

/* Parameters constructor */
TestResult::TestResult(const String & type,
                       const Bool binMeasure,
                       const NumericalScalar pVal,
                       const NumericalScalar pThreshold):
  PersistentObject(),
  testType_(type),
  binaryQualityMeasure_(binMeasure),
  pValueThreshold_(pThreshold),
  pValue_(pVal),
  description_(0)
{
  // Nothing to do
}

/* Virtual constructor */
TestResult * TestResult::clone() const
{
  return new TestResult(*this);
}

/* Description Accessor */
void TestResult::setDescription(const Description & description)
{
  description_ = description;
}

Description TestResult::getDescription() const
{
  return description_;
}

/* String converter */
String TestResult::__repr__() const
{
  return OSS() << "class=" << TestResult::GetClassName()
         << " name=" << getName()
         << " type=" << testType_
         << " binaryQualityMeasure=" << binaryQualityMeasure_
         << " p-value threshold=" << pValueThreshold_
         << " p-value=" << pValue_
         << " description=" << description_;
}

/* Elements accessors */
Bool TestResult::getBinaryQualityMeasure() const
{
  return binaryQualityMeasure_;
}

NumericalScalar TestResult::getPValue() const
{
  return pValue_;
}

NumericalScalar TestResult::getThreshold() const
{
  return pValueThreshold_;
}

String TestResult::getTestType() const
{
  return testType_;
}

/* Comparison operator */
Bool TestResult::operator == (const TestResult & other) const
{
  if (this == &other) return true;
  return (binaryQualityMeasure_ == other.binaryQualityMeasure_) && (pValue_ == other.pValue_) && (pValueThreshold_ == other.pValueThreshold_) && (testType_ == other.testType_);
}


/** Method save() stores the object through the StorageManager */
void TestResult::save(Advocate & adv) const
{

  PersistentObject::save(adv);
  adv.saveAttribute("testType_", testType_);
  adv.saveAttribute("binaryQualityMeasure_", binaryQualityMeasure_);
  adv.saveAttribute("pValueThreshold_", pValueThreshold_);
  adv.saveAttribute("pValue_", pValue_);
  adv.saveAttribute("description_", description_);
}

/** Method load() reloads the object from the StorageManager */
void TestResult::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("testType_", testType_);
  adv.loadAttribute("binaryQualityMeasure_", binaryQualityMeasure_);
  adv.loadAttribute("pValueThreshold_", pValueThreshold_);
  adv.loadAttribute("pValue_", pValue_);
  adv.loadAttribute("description_", description_);
}

END_NAMESPACE_OPENTURNS
