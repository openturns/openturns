//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition and projection
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
  CLASSNAME;

public:


  /** Default constructor */
  KarhunenLoeveAlgorithmImplementation();

  /** Default constructor */
  explicit KarhunenLoeveAlgorithmImplementation(const CovarianceModel & covariance,
						const NumericalScalar threshold = 0.0);

  /** Virtual constructor */
  virtual KarhunenLoeveAlgorithmImplementation * clone() const;

  /** Threshold accessors */
  NumericalScalar getThreshold() const;
  void setThreshold(const NumericalScalar threshold);

  /** Covariance model accessors */
  virtual CovarianceModel getCovarianceModel() const;
  virtual void setCovarianceModel(const CovarianceModel & covariance);

  /** Do the computation */
  virtual void run();

  /** Result accessor */
  KarhunenLoeveResult getResult() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  /** Covariance model */
  CovarianceModel covariance_;

  /** Threshold */
  NumericalScalar threshold_;

  /** Result */
  KarhunenLoeveResult result_;
}; /* class KarhunenLoeveAlgorithmImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_KARHUNENLOEVEALGORITHMIMPLEMENTATION_HXX */
