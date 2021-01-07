//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition and projection
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KarhunenLoeveAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KarhunenLoeveAlgorithmImplementation)

static const Factory<KarhunenLoeveAlgorithmImplementation> Factory_KarhunenLoeveAlgorithmImplementation;

/* Default constructor */
KarhunenLoeveAlgorithmImplementation::KarhunenLoeveAlgorithmImplementation()
  : PersistentObject()
  , covariance_()
  , threshold_(0.0)
  , nbModes_(std::numeric_limits<UnsignedInteger>::max())
  , result_()
{
  // Nothing to do
}

/* Default constructor */
KarhunenLoeveAlgorithmImplementation::KarhunenLoeveAlgorithmImplementation(const CovarianceModel & covariance,
    const Scalar threshold)
  : PersistentObject()
  , covariance_(covariance)
  , threshold_(threshold)
  , nbModes_(std::numeric_limits<UnsignedInteger>::max())
  , result_()
{
  // Nothing to do
}

/* Virtual constructor */
KarhunenLoeveAlgorithmImplementation * KarhunenLoeveAlgorithmImplementation::clone() const
{
  return new KarhunenLoeveAlgorithmImplementation(*this);
}

/* Threshold accessors */
Scalar KarhunenLoeveAlgorithmImplementation::getThreshold() const
{
  return threshold_;
}

void KarhunenLoeveAlgorithmImplementation::setThreshold(const Scalar threshold)
{
  if (threshold > 1.0 || threshold < 0.0)
    LOGWARN(OSS() << "threshold must be between 0.0 and 1.0");

  threshold_ = threshold;
}

/** Number of modes accessors */
UnsignedInteger KarhunenLoeveAlgorithmImplementation::getNbModes() const
{
  return nbModes_;
}

void KarhunenLoeveAlgorithmImplementation::setNbModes(const UnsignedInteger nbModes)
{
  nbModes_ = nbModes;
}

/* Covariance model accessors */
CovarianceModel KarhunenLoeveAlgorithmImplementation::getCovarianceModel() const
{
  return covariance_;
}

void KarhunenLoeveAlgorithmImplementation::setCovarianceModel(const CovarianceModel & covariance)
{
  covariance_ = covariance;
}

/* Result accessor */
KarhunenLoeveResult KarhunenLoeveAlgorithmImplementation::getResult() const
{
  return result_;
}

/* Compute the decomposition */
void KarhunenLoeveAlgorithmImplementation::run()
{
  throw NotYetImplementedException(HERE) << "In KarhunenLoeveAlgorithmImplementation::run()";
}

/* String converter */
String KarhunenLoeveAlgorithmImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " covariance model=" << covariance_
         << " threshold=" << threshold_
         << " nbModes=" << nbModes_
         << " result=" << result_;
}

String KarhunenLoeveAlgorithmImplementation::__str__(const String & ) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void KarhunenLoeveAlgorithmImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("covariance_", covariance_);
  adv.saveAttribute("threshold_", threshold_);
  adv.saveAttribute("nbModes_", nbModes_);
  adv.saveAttribute("result_", result_);
}


/* Method load() reloads the object from the StorageManager */
void KarhunenLoeveAlgorithmImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("covariance_", covariance_);
  adv.loadAttribute("threshold_", threshold_);
  adv.loadAttribute("nbModes_", nbModes_);
  adv.loadAttribute("result_", result_);
}


END_NAMESPACE_OPENTURNS
