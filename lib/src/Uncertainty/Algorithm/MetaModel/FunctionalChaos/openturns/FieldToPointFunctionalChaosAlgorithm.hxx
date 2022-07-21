//                                               -*- C++ -*-
/**
 *  @brief Functional chaos on field data
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FIELDTOPOINTFUNCTIONALCHAOSALGORITHM_HXX
#define OPENTURNS_FIELDTOPOINTFUNCTIONALCHAOSALGORITHM_HXX

#include "openturns/FieldFunctionalChaosResult.hxx"
#include "openturns/ProcessSample.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FieldToPointFunctionalChaosAlgorithm
 *
 */

class OT_API FieldToPointFunctionalChaosAlgorithm
  : public PersistentObject
{
  CLASSNAME
public:

  /** Default constructor */
  FieldToPointFunctionalChaosAlgorithm();

  /** Constructor with parameters */
  FieldToPointFunctionalChaosAlgorithm(const ProcessSample & inputProcessSample, const Sample & outputSample);

  /** Virtual constructor */
  FieldToPointFunctionalChaosAlgorithm * clone() const override;

  /** Sample accessors */
  ProcessSample getInputProcessSample() const;
  Sample getOutputSample() const;

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

  /** Centered sample flag accessor */
  void setCenteredSample(const Bool centered);
  Bool getCenteredSample() const;

  /** String converter */
  String __repr__() const override;

  /** Response surface computation */
  void run();

  /** Result accessor */
  FieldFunctionalChaosResult getResult() const;

  /** Build distribution of KL modes */
  static Distribution BuildDistribution(const Sample & modesSample);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  // the data
  ProcessSample inputProcessSample_;
  Sample outputSample_;

  // whether the sample is centered
  Bool centeredSample_ = false;

  // independent component blocks
  PersistentCollection<Indices> blockIndices_;

  /** Number of eigenvalues management
   *  The number of modes computed by 'run' method can be defined:
   *  - by specifing the threshold of spectrum cut-off
   *  - by specifying the number of modes to compute */
  Scalar threshold_ = 0.0;
  UnsignedInteger nbModes_;

  // Whether to recompress KL modes
  Bool recompress_ = false;

  // result data
  FieldFunctionalChaosResult result_;
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDTOPOINTFUNCTIONALCHAOSALGORITHM_HXX */
