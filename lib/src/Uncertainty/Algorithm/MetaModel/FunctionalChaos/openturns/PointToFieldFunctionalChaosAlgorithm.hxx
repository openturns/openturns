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
#ifndef OPENTURNS_POINTTOFIELDFUNCTIONALCHAOSALGORITHM_HXX
#define OPENTURNS_POINTTOFIELDFUNCTIONALCHAOSALGORITHM_HXX

#include "openturns/FieldFunctionalChaosResult.hxx"
#include "openturns/ProcessSample.hxx"

#include <limits>

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class PointToFieldFunctionalChaosAlgorithm
 *
 */

class OT_API PointToFieldFunctionalChaosAlgorithm
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  PointToFieldFunctionalChaosAlgorithm();

  /** Constructor with parameters */
  PointToFieldFunctionalChaosAlgorithm(const Sample & inputSample,
                                       const ProcessSample & outputProcessSample,
                                       const Distribution & distribution);

  /** Virtual constructor */
  PointToFieldFunctionalChaosAlgorithm * clone() const override;

  /** Sample accessors */
  Sample getInputSample() const;
  ProcessSample getOutputProcessSample() const;

  /** Block indices accessors */
  Collection<Indices> getBlockIndices() const;
  void setBlockIndices(const Collection<Indices> & blockIndices);

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

  /** Response surface computation */
  void run();

  /** Result accessor */
  FieldFunctionalChaosResult getResult() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  // the data
  Sample inputSample_;
  ProcessSample outputProcessSample_;
  Distribution distribution_;

  // independent component blocks
  PersistentCollection<Indices> blockIndices_;

  /** Number of eigenvalues management
   *  The number of modes computed by 'run' method can be defined:
   *  - by specifying the threshold of spectrum cut-off
   *  - by specifying the number of modes to compute */
  Scalar threshold_ = 0.0;
  UnsignedInteger nbModes_ = std::numeric_limits<UnsignedInteger>::max();

  // Whether to recompress KL modes
  Bool recompress_ = false;

  // result data
  FieldFunctionalChaosResult result_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINTTOFIELDFUNCTIONALCHAOSALGORITHM_HXX */
