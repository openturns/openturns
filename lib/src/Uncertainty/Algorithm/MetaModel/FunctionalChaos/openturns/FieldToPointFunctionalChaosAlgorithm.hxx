//                                               -*- C++ -*-
/**
 *  @brief Functional chaos on field data
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/FieldFunctionalChaosAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FieldToPointFunctionalChaosAlgorithm
 *
 */

class OT_API FieldToPointFunctionalChaosAlgorithm
  : public FieldFunctionalChaosAlgorithm
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

  /** Centered sample flag accessor */
  void setCenteredSample(const Bool centered);
  Bool getCenteredSample() const;

  /** Block indices accessors */
  void setBlockIndices(const Collection<Indices> & blockIndices);

  /** String converter */
  String __repr__() const override;

  /** Response surface computation */
  void run();

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
};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDTOPOINTFUNCTIONALCHAOSALGORITHM_HXX */
