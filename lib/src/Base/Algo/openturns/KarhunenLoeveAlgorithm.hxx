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
#ifndef OPENTURNS_KARHUNENLOEVEALGORITHM_HXX
#define OPENTURNS_KARHUNENLOEVEALGORITHM_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/KarhunenLoeveAlgorithmImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveAlgorithm
 *
 * Compute the KL decomposition of a given covariance model
 */
class OT_API KarhunenLoeveAlgorithm
  : public TypedInterfaceObject<KarhunenLoeveAlgorithmImplementation>
{
  CLASSNAME

public:

  typedef TypedInterfaceObject<KarhunenLoeveAlgorithmImplementation>::Implementation Implementation;

  /** Default constructor */
  KarhunenLoeveAlgorithm();

  /** Constructor from implementation */
  KarhunenLoeveAlgorithm(const KarhunenLoeveAlgorithmImplementation & implementation);

  /** Constructor from implementation */
  KarhunenLoeveAlgorithm(const Implementation & p_implementation);

  /** Threshold accessors */
  Scalar getThreshold() const;
  void setThreshold(const Scalar threshold);

  /** Number of modes accessors */
  UnsignedInteger getNbModes() const;
  void setNbModes(const UnsignedInteger nbModes);

  /** Covariance model accessors */
  CovarianceModel getCovarianceModel() const;
  void setCovarianceModel(const CovarianceModel & covariance);

  /** Result accessor */
  KarhunenLoeveResult getResult() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Compute the decomposition */
  void run();

}; /* class KarhunenLoeveAlgorithm */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVEALGORITHM_HXX */
