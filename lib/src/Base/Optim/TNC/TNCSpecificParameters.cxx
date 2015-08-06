//                                               -*- C++ -*-
/**
 *  @brief TNCSpecificParameters is an encapsulation of the TNC specific
 *         parameters.
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
#include <cstdlib>

#include "TNCObsolete.hxx"
#include "ResourceMap.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(TNCSpecificParameters);

static Factory<TNCSpecificParameters> RegisteredFactory("TNCSpecificParameters");

/* Default with parameters */
TNCSpecificParameters::TNCSpecificParameters():
  PersistentObject(),
  scale_(0),
  offset_(0),
  maxCGit_(ResourceMap::GetAsUnsignedInteger( "TNC-DefaultMaxCGit" )),
  eta_(ResourceMap::GetAsNumericalScalar( "TNC-DefaultEta" )),
  stepmx_(ResourceMap::GetAsNumericalScalar( "TNC-DefaultStepmx" )),
  accuracy_(ResourceMap::GetAsNumericalScalar( "TNC-DefaultAccuracy" )),
  fmin_(ResourceMap::GetAsNumericalScalar( "TNC-DefaultFmin" )),
  rescale_(ResourceMap::GetAsNumericalScalar( "TNC-DefaultRescale" ))
{
  // Nothing to do
} /* SpecificParametersTNC */

/* Parameters with parameters */
TNCSpecificParameters::TNCSpecificParameters(const NumericalPoint & scale,
    const NumericalPoint & offset,
    const UnsignedInteger    maxCGit,
    const NumericalScalar eta,
    const NumericalScalar stepmx,
    const NumericalScalar accuracy,
    const NumericalScalar fmin,
    const NumericalScalar rescale):
  PersistentObject(),
  scale_(scale),
  offset_(offset),
  maxCGit_(maxCGit),
  eta_(eta),
  stepmx_(stepmx),
  accuracy_(accuracy),
  fmin_(fmin),
  rescale_(rescale)
{
  // Nothing to do
} /* SpecificParametersTNC */

/* Virtual constructor */
TNCSpecificParameters * TNCSpecificParameters::clone() const
{
  return new TNCSpecificParameters(*this);
}

/* Scale accessor */
NumericalPoint TNCSpecificParameters::getScale() const
{
  return scale_;
}

void TNCSpecificParameters::setScale(const NumericalPoint & scale)
{
  scale_ = scale;
}

/* Offset accessor */
NumericalPoint TNCSpecificParameters::getOffset() const
{
  return offset_;
}

void TNCSpecificParameters::setOffset(const NumericalPoint & offset)
{
  offset_ = offset;
}

/* MaxCGit accessor */
UnsignedInteger TNCSpecificParameters::getMaxCGit() const
{
  return maxCGit_;
}

void TNCSpecificParameters::setMaxCGit(const UnsignedInteger maxCGit)
{
  maxCGit_ = maxCGit;
}

/* Eta accessor */
NumericalScalar TNCSpecificParameters::getEta() const
{
  return eta_;
}

void TNCSpecificParameters::setEta(const NumericalScalar eta)
{
  eta_ = eta;
}

/* Stepmx accessor */
NumericalScalar TNCSpecificParameters::getStepmx() const
{
  return stepmx_;
}

void TNCSpecificParameters::setStepmx(const NumericalScalar stepmx)
{
  stepmx_ = stepmx;
}

/* Accuracy accessor */
NumericalScalar TNCSpecificParameters::getAccuracy() const
{
  return accuracy_;
}

void TNCSpecificParameters::setAccuracy(const NumericalScalar accuracy)
{
  accuracy_ = accuracy;
}

/* Fmin accessor */
NumericalScalar TNCSpecificParameters::getFmin() const
{
  return fmin_;
}

void TNCSpecificParameters::setFmin(const NumericalScalar fmin)
{
  fmin_ = fmin;
}

/* Rescale accessor */
NumericalScalar TNCSpecificParameters::getRescale() const
{
  return rescale_;
}

void TNCSpecificParameters::setRescale(const NumericalScalar rescale)
{
  rescale_ = rescale;
}

/* String converter */
String TNCSpecificParameters::__repr__() const
{
  OSS oss;
  oss << "class=" << TNCSpecificParameters::GetClassName()
      << " scale=" << scale_
      << " offset=" << offset_
      << " maxCGit=" << maxCGit_
      << " eta=" << eta_
      << " stepmx=" << stepmx_
      << " accuracy=" << accuracy_
      << " fmin=" << fmin_
      << " rescale=" << rescale_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void TNCSpecificParameters::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("scale_", scale_);
  adv.saveAttribute("offset_", offset_);
  adv.saveAttribute("maxCGit_", maxCGit_);
  adv.saveAttribute("eta_", eta_);
  adv.saveAttribute("stepmx_", stepmx_);
  adv.saveAttribute("accuracy_", accuracy_);
  adv.saveAttribute("fmin_", fmin_);
  adv.saveAttribute("rescale_", rescale_);
}

/* Method load() reloads the object from the StorageManager */
void TNCSpecificParameters::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("scale_", scale_);
  adv.loadAttribute("offset_", offset_);
  adv.loadAttribute("maxCGit_", maxCGit_);
  adv.loadAttribute("eta_", eta_);
  adv.loadAttribute("stepmx_", stepmx_);
  adv.loadAttribute("accuracy_", accuracy_);
  adv.loadAttribute("fmin_", fmin_);
  adv.loadAttribute("rescale_", rescale_);
}

END_NAMESPACE_OPENTURNS



