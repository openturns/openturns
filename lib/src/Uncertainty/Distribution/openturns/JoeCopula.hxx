//                                               -*- C++ -*-
/**
 *  @brief The JoeCopula distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_JOECOPULA_HXX
#define OPENTURNS_JOECOPULA_HXX

#include "openturns/ExtremeValueCopula.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class JoeCopula
 *
 * The JoeCopula distribution.
 */
class OT_API JoeCopula
  : public ExtremeValueCopula
{
  CLASSNAME
public:

  /** Default constructor */
  JoeCopula();

  /** Parameters constructor */
  explicit JoeCopula(const Scalar theta,
                     const Scalar psi1 = 0.5,
                     const Scalar psi2 = 0.5);

  /** Comparison operator */
  using ExtremeValueCopula::operator ==;
  Bool operator ==(const JoeCopula & other) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /* Interface inherited from Distribution */
  /** Virtual constructor */
  JoeCopula * clone() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Mutator of psi1 */
  void setTheta(const Scalar theta);

  /** Accessor to psi1 */
  Scalar getTheta() const;

  /** Mutator of psi1 */
  void setPsi1(const Scalar psi1);

  /** Accessor to psi1 */
  Scalar getPsi1() const;

  /** Mutator of psi2 */
  void setPsi2(const Scalar psi2);

  /** Accessor to psi2 */
  Scalar getPsi2() const;

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
  Scalar psi1_;
  Scalar psi2_;

}; /* class JoeCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_JOECOPULA_HXX */
