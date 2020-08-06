//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition and projection
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_KARHUNENLOEVEALGORITHMIMPLEMENTATION_HXX
#define OPENTURNS_KARHUNENLOEVEALGORITHMIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/KarhunenLoeveResult.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class KarhunenLoeveAlgorithmImplementation
 *
 * Compute the KL decomposition of a given covariance model
 */
class OT_API KarhunenLoeveAlgorithmImplementation
  : public PersistentObject
{
  CLASSNAME

public:

  /** Default constructor */
  KarhunenLoeveAlgorithmImplementation();

  /** Default constructor */
  explicit KarhunenLoeveAlgorithmImplementation(const CovarianceModel & covariance,
      const Scalar threshold = 0.0);

  /** Virtual constructor */
  KarhunenLoeveAlgorithmImplementation * clone() const override;

  /** Threshold accessors */
  Scalar getThreshold() const;
  void setThreshold(const Scalar threshold);

  /** Number of modes accessors */
  UnsignedInteger getNbModes() const;
  void setNbModes(const UnsignedInteger nbModes);

  /** Covariance model accessors */
  virtual CovarianceModel getCovarianceModel() const;
  virtual void setCovarianceModel(const CovarianceModel & covariance);

  /** Do the computation */
  virtual void run();

  /** Result accessor */
  KarhunenLoeveResult getResult() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Covariance model */
  CovarianceModel covariance_;

  /** Number of eigenvalues management
   *  The number of modes computed by 'run' method can be defined:
   *  - by specifing the threshold of spectrum cut-off
   *  - by specifying the number of modes to compute */
  Scalar threshold_;
  UnsignedInteger nbModes_;


  /** Result */
  KarhunenLoeveResult result_;
}; /* class KarhunenLoeveAlgorithmImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVEALGORITHMIMPLEMENTATION_HXX */
