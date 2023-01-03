//                                               -*- C++ -*-
/**
 *  @brief This an abstract class for 1D function factories
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
#ifndef OPENTURNS_ORTHOGONALUNIVARIATEFUNCTIONFACTORY
#define OPENTURNS_ORTHOGONALUNIVARIATEFUNCTIONFACTORY

#include "openturns/UniVariateFunctionFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthogonalUniVariateFunctionFactory
 *
 * This an abstract class for 1D function factories
 */

class OT_API OrthogonalUniVariateFunctionFactory
  : public UniVariateFunctionFactory
{
  CLASSNAME

public:

  enum ParameterSet { ANALYSIS, PROBABILITY };

  /** Constructor */
  explicit OrthogonalUniVariateFunctionFactory(const Distribution & measure);

  /** Virtual constructor */
  OrthogonalUniVariateFunctionFactory * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Measure accessor */
  Distribution getMeasure() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  friend class Factory<OrthogonalUniVariateFunctionFactory>;

  /** Default constructor */
  OrthogonalUniVariateFunctionFactory();

  /** The distribution of the particular Orthonormal polynomial */
  Distribution measure_;

} ; /* class OrthogonalUniVariateFunctionFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHOGONALUNIVARIATEFUNCTIONFACTORY */
