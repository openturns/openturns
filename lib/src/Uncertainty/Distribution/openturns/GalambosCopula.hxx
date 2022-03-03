//                                               -*- C++ -*-
/**
 *  @brief The GalambosCopula distribution
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_GalambosCopula_HXX
#define OPENTURNS_GalambosCopula_HXX

#include "openturns/ExtremeValueCopula.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class GalambosCopula
 *
 * The GalambosCopula distribution.
 */
class OT_API GalambosCopula
  : public ExtremeValueCopula
{
  CLASSNAME
public:

  /** Default constructor */
  GalambosCopula();

  /** Parameters constructor */
  explicit GalambosCopula(const Scalar theta);

  /** Comparison operator */
  Bool operator ==(const GalambosCopula & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */
  /** Virtual constructor */
  GalambosCopula * clone() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Mutator of theta */
  void setTheta(const Scalar theta);

  /** Accessor to theta */
  Scalar getTheta() const;
  
  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:
  // update function when parameters are modified
  void updatePickandFunction();

  /** Joe Copula parameters */
  Scalar theta_;

}; /* class GalambosCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_GalambosCopula_HXX */
