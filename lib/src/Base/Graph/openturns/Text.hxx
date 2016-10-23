//                                               -*- C++ -*-
/**
 *  @brief Text class to add text on graphs
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
#ifndef OPENTURNS_TEXT_HXX
#define OPENTURNS_TEXT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DrawableImplementation.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Description.hxx"

#include <map>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Text
 *
 * This class adds text on a graph
 * Instance of Drawable
 */

class OT_API Text : public DrawableImplementation
{
  CLASSNAME;
public:

  typedef Collection<NumericalComplex>               NumericalComplexCollection;

  /** Default constructor */
  Text(const NumericalSample & data,
       const Description & texts,
       const String & textPosition = "top",
       const String & legend = "");

  /** Constructor from complex numbers */
  Text(const NumericalComplexCollection & data,
       const Description & texts,
       const String & textPosition = "top",
       const String & legend = "");

  /** Contructor from 2 data sets */
  Text(const NumericalSample & dataX,
       const NumericalSample & dataY,
       const Description & texts,
       const String & textPosition = "top",
       const String & legend = "");

  /** Contructor from 2 data sets */
  Text(const NumericalPoint & dataX,
       const NumericalPoint & dataY,
       const Description & texts,
       const String & textPosition = "top",
       const String & legend = "");

  static Bool IsValidTextPosition(String textPosition);

  /** String converter */
  String __repr__() const;

  /** Draw method */
  String draw() const;

  /** Clone method */
  virtual Text * clone() const;

  /** Accessors to text */
  Description getTextAnnotations() const;
  void setTextAnnotations(const Description & textAnnotations);

  /** Accessors to text position */
  Description getTextPositions() const;
  void setTextPositions(const Description & textPositions);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv);

protected:
  /** Check for data validity */
  virtual void checkData(const NumericalSample & data) const;

  /** Labels */
  Description textAnnotations_;

  /** Text position  */
  Description textPositions_;

private:

  Text() {};
  friend class Factory<Text>;

  /** A map matching text position with R codes */
  static std::map<String, UnsignedInteger> Position;

  /** Initialize positon map */
  static void InitializePositionMap();
  static Bool IsTextFirstInitialization;

}; /* class Text */

END_NAMESPACE_OPENTURNS
#endif /* OPENTURNS_TEXT_HXX */
