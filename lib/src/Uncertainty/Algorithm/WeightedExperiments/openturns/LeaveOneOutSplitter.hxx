//                                               -*- C++ -*-
/**
 *  @brief LeaveOneOutSplitter
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LEAVEONEOUTSPLITTER_HXX
#define OPENTURNS_LEAVEONEOUTSPLITTER_HXX

#include "openturns/SplitterImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LeaveOneOutSplitter
 *
 * LeaveOneOutSplitter
 */
class OT_API LeaveOneOutSplitter
  : public SplitterImplementation
{
  CLASSNAME

public:
  /** Default constructor */
  LeaveOneOutSplitter();

  /** Default constructor */
  explicit LeaveOneOutSplitter(const UnsignedInteger size);

  /** Virtual constructor method */
  LeaveOneOutSplitter * clone() const override;

  /** Generate next set of indices */
  Indices generate(Indices & indices2Out) const override;

  /** Number of indices pairs accessor */
  UnsignedInteger getSize() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

}; /* class LeaveOneOutSplitter */

} /* namespace OT */

#endif /* OPENTURNS_LEAVEONEOUTSPLITTER_HXX */
