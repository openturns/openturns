//                                               -*- C++ -*-
/**
 *  @brief AbdoRackwitz specific parameters.
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
#ifndef OPENTURNS_ABDORACKWITZSPECIFICPARAMETERS_HXX
#define OPENTURNS_ABDORACKWITZSPECIFICPARAMETERS_HXX

#include "OTprivate.hxx"
#include "NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class AbdoRackwitz
 * AbdoRackwitz is an actual implementation for
 * OptimizationSolverImplementation
 */
class OT_API AbdoRackwitzSpecificParameters
  : public PersistentObject
{

  CLASSNAME;
public:
  /** Default constructor */
  AbdoRackwitzSpecificParameters();

  AbdoRackwitzSpecificParameters(const NumericalScalar tau,
                                 const NumericalScalar omega,
                                 const NumericalScalar smooth);

  /** Virtual constructor */
  virtual AbdoRackwitzSpecificParameters * clone() const;

  /** Tau accessor */
  NumericalScalar getTau() const;
  void setTau(const NumericalScalar tau);

  /** Omega accessor */
  NumericalScalar getOmega() const;
  void setOmega(const NumericalScalar tau);

  /** Smooth accessor */
  NumericalScalar getSmooth() const;
  void setSmooth(const NumericalScalar tau);

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  /* Multiplicative decrease of linear step */
  NumericalScalar tau_;

  /* Armijo factor */
  NumericalScalar omega_;

  /* Growing factor in penalization term */
  NumericalScalar smooth_;

}; /* class AbdoRackwitzSpecificParameters */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ABDORACKWITZSPECIFICPARAMETERS_HXX */
