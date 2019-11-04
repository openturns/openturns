//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition and projection
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/KarhunenLoeveAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(KarhunenLoeveAlgorithm)


/* Constructor from implementation */
KarhunenLoeveAlgorithm::KarhunenLoeveAlgorithm(const KarhunenLoeveAlgorithmImplementation & implementation)
  : TypedInterfaceObject<KarhunenLoeveAlgorithmImplementation>( implementation.clone() )
{
  // Nothing to do
}

/* Constructor from implementation */
KarhunenLoeveAlgorithm::KarhunenLoeveAlgorithm(const Implementation & p_implementation)
  : TypedInterfaceObject<KarhunenLoeveAlgorithmImplementation>( p_implementation )
{
  // Nothing to do
}

/* Threshold accessors */
Scalar KarhunenLoeveAlgorithm::getThreshold() const
{
  return getImplementation()->getThreshold();
}

void KarhunenLoeveAlgorithm::setThreshold(const Scalar threshold)
{
  copyOnWrite();
  return getImplementation()->setThreshold(threshold);
}

/* Covariance model accessor */
void KarhunenLoeveAlgorithm::setCovarianceModel(const CovarianceModel & covariance)
{
  copyOnWrite();
  getImplementation()->setCovarianceModel(covariance);
}

CovarianceModel KarhunenLoeveAlgorithm::getCovarianceModel() const
{
  return getImplementation()->getCovarianceModel();
}

/* String converter */
String KarhunenLoeveAlgorithm::__repr__() const
{
  return getImplementation()->__repr__();
}

String KarhunenLoeveAlgorithm::__str__(const String & offset) const
{
  return getImplementation()->__str__( offset );
}

/* Do the computation */
void KarhunenLoeveAlgorithm::run()
{
  getImplementation()->run();
}

/* Result accessor */
KarhunenLoeveResult KarhunenLoeveAlgorithm::getResult() const
{
  return getImplementation()->getResult();
}

END_NAMESPACE_OPENTURNS
