//                                               -*- C++ -*-
/**
 *  @brief Functional chaos on field data
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FIELDFUNCTIONALCHAOSALGORITHM_HXX
#define OPENTURNS_FIELDFUNCTIONALCHAOSALGORITHM_HXX

#include "openturns/FieldFunctionalChaosResult.hxx"
#include "openturns/ProcessSample.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FieldFunctionalChaosAlgorithm
 *
 */

class OT_API FieldFunctionalChaosAlgorithm
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  FieldFunctionalChaosAlgorithm();

  /** Virtual constructor */
  FieldFunctionalChaosAlgorithm * clone() const override;

  /** Block indices accessors */
  Collection<Indices> getBlockIndices() const;

  /** Threshold accessors */
  Scalar getThreshold() const;
  void setThreshold(const Scalar threshold);

  /** Number of modes accessors */
  UnsignedInteger getNbModes() const;
  void setNbModes(const UnsignedInteger nbModes);

  /** Recompression flag accessors */
  Bool getRecompress() const;
  void setRecompress(const Bool recompress);

  /** String converter */
  String __repr__() const override;
  
  /** Result accessor */
  FieldFunctionalChaosResult getResult() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  void decompose(const ProcessSample & processSample,
                 Collection<KarhunenLoeveResult> & klResultCollection);
  
  // independent component blocks
  PersistentCollection<Indices> blockIndices_;

  /** Number of eigenvalues management
   *  The number of modes computed by 'run' method can be defined:
   *  - by specifying the threshold of spectrum cut-off
   *  - by specifying the number of modes to compute */
  Scalar threshold_ = 0.0;
  UnsignedInteger nbModes_ = 0;

  // Whether to recompress KL modes
  Bool recompress_ = false;

  // result data
  FieldFunctionalChaosResult result_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDFUNCTIONALCHAOSALGORITHM_HXX */
