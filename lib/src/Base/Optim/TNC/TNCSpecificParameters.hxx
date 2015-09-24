//                                               -*- C++ -*-
/**
 *  @brief TNC is an actual implementation for a bound-constrained optimization algorithm
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
#ifndef OPENTURNS_TNCSPECIFICPARAMETERS_HXX
#define OPENTURNS_TNCSPECIFICPARAMETERS_HXX

#include "OTprivate.hxx"
#include "PersistentObject.hxx"
#include "NumericalPoint.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class TNCSpecificParameters
 * Specific parameters of the TNC library
 */
class OT_API TNCSpecificParameters
  : public PersistentObject
{

  CLASSNAME;
public:


  /** Default constructor */
  TNCSpecificParameters();

  /** Parameters constructor */
  TNCSpecificParameters(const NumericalPoint & scale,
                        const NumericalPoint & offset,
                        const UnsignedInteger    maxCGit,
                        const NumericalScalar eta,
                        const NumericalScalar stepmx,
                        const NumericalScalar accuracy,
                        const NumericalScalar fmin,
                        const NumericalScalar rescale);

  /** Virtual constructor */
  virtual TNCSpecificParameters * clone() const;

  /** Scale accessor */
  NumericalPoint getScale() const;
  void setScale(const NumericalPoint & scale);

  /** Offset accessor */
  NumericalPoint getOffset() const;
  void setOffset(const NumericalPoint & offset);

  /** MaxCGit accessor */
  UnsignedInteger getMaxCGit() const;
  void setMaxCGit(const UnsignedInteger maxCGit);

  /** Eta accessor */
  NumericalScalar getEta() const;
  void setEta(const NumericalScalar eta);

  /** Stepmx accessor */
  NumericalScalar getStepmx() const;
  void setStepmx(const NumericalScalar stepmx);

  /** Accuracy accessor */
  NumericalScalar getAccuracy() const;
  void setAccuracy(const NumericalScalar accuracy);

  /** Fmin accessor */
  NumericalScalar getFmin() const;
  void setFmin(const NumericalScalar fmin);

  /** Rescale accessor */
  NumericalScalar getRescale() const;
  void setRescale(const NumericalScalar rescale);

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:
  NumericalPoint  scale_;
  NumericalPoint  offset_;
  UnsignedInteger    maxCGit_;
  NumericalScalar eta_;
  NumericalScalar stepmx_;
  NumericalScalar accuracy_;
  NumericalScalar fmin_;
  NumericalScalar rescale_;
}; /* class TNCSpecificParameters */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TNCSPECIFICPARAMETERS_HXX */
