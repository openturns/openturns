//                                               -*- C++ -*-
/**
 *  @brief TestResult implements the result of a statistical test
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_TESTRESULT_HXX
#define OPENTURNS_TESTRESULT_HXX

#include "PersistentObject.hxx"
#include "StorageManager.hxx"
#include "Description.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class TestResult
 *
 * TestResult implements the result of a statistical test
 */

class OT_API TestResult :
  public PersistentObject
{
  CLASSNAME;

public:


  /** Default constructor */
  TestResult();

  /** Parameters constructor */
  TestResult(const String & type,
             const Bool binMeasure,
             const NumericalScalar pVal,
             const NumericalScalar pThreshold);


  /** Virtual constructor */
  virtual TestResult * clone() const;

  /** Description Accessor */
  void setDescription(const Description & description);
  Description getDescription() const;

  /** String converter */
  String __repr__() const;

  /** Elements accessors */
  Bool getBinaryQualityMeasure() const;
  NumericalScalar getPValue() const;
  NumericalScalar getThreshold() const;
  String getTestType() const;

  /** Comparison operator */
  Bool operator ==(const TestResult & other) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  String testType_;
  Bool binaryQualityMeasure_ ;
  NumericalScalar pValueThreshold_ ;
  NumericalScalar pValue_ ;
  Description description_;

}
; /* class TestResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TESTRESULT_HXX */
