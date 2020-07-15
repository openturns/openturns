//                                               -*- C++ -*-
/**
 *  @brief Grid layout of graphs
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
#ifndef OPENTURNS_GRIDLAYOUT_HXX
#define OPENTURNS_GRIDLAYOUT_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/GraphImplementation.hxx"
#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class GridLayout
 *
 * The class GridLayout
 */
class OT_API GridLayout : public PersistentObject
{
  CLASSNAME

public:
  /** Default constructor */
  explicit GridLayout(const UnsignedInteger nbRows = 0,
                      const UnsignedInteger nbColumns = 0);

  /** String converter */
  String __repr__() const;

  /** Clone method */
  virtual GridLayout * clone() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() stores the object through the StorageManager */
  void load(Advocate & adv);

  /** Grid accessors */
  UnsignedInteger getNbRows() const;
  UnsignedInteger getNbColumns() const;
  Graph getGraph(const UnsignedInteger i, const UnsignedInteger j) const;
  void setGraph(const UnsignedInteger i, const UnsignedInteger j, const Graph & elt);

  /** Hide or show x and y axes */
  virtual void setAxes(const Bool showAxes);

  /** Set the legend position */
  virtual void setLegendPosition(const String & position);

  /** Accessor for title */
  void setTitle(const String & title);
  String getTitle() const;
private:

  /** Number of row/column */
  UnsignedInteger nbRows_;
  UnsignedInteger nbColumns_;

  /** Flat collection of sub-graphs */
  PersistentCollection <Graph> graphCollection_;

  String title_;

}; /* class GridLayout */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GRIDLAYOUT_HXX */
