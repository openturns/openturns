//                                               -*- C++ -*-
/**
 *  @brief Result for field functional chaos
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
#ifndef OPENTURNS_FIELDFUNCTIONALCHAOSRESULT_HXX
#define OPENTURNS_FIELDFUNCTIONALCHAOSRESULT_HXX

#include "openturns/KarhunenLoeveResult.hxx"
#include "openturns/FunctionalChaosResult.hxx"
#include "openturns/FieldToPointFunction.hxx"
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/FieldFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FieldFunctionalChaosResult
 *
 */

class OT_API FieldFunctionalChaosResult
  : public PersistentObject
{
  CLASSNAME
public:
  typedef Collection<KarhunenLoeveResult> KarhunenLoeveResultCollection;

  /** Default constructor */
  FieldFunctionalChaosResult();

  /** Constructor with parameters */
  FieldFunctionalChaosResult(const KarhunenLoeveResultCollection & inputKLResultCollection,
                             const FunctionalChaosResult & fceResult,
                             const KarhunenLoeveResultCollection & outputKLResultCollection);

  /** Virtual constructor */
  FieldFunctionalChaosResult * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** KL decompositions accessors */
  KarhunenLoeveResultCollection getInputKLResultCollection() const;
  KarhunenLoeveResultCollection getOutputKLResultCollection() const;

  /** Chaos result accessor */
  FunctionalChaosResult getFCEResult() const;

  /** Block indices accessors */
  Collection<Indices> getBlockIndices() const;
  void setBlockIndices(const Collection<Indices> & blockIndices);

  /** Metamodel accessor */
  FieldToPointFunction getFieldToPointMetamodel() const;
  void setMetamodel(const FieldToPointFunction & metamodel);
  PointToFieldFunction getPointToFieldMetamodel() const;
  void setMetamodel(const PointToFieldFunction & metamodel);
  FieldFunction getFieldMetamodel() const;
  void setMetamodel(const FieldFunction & metamodel);

  /** Sample accessors */
  void setInputProcessSample(const ProcessSample & inputProcessSample);
  ProcessSample getInputProcessSample() const;
  void setOutputProcessSample(const ProcessSample & outputProcessSample);
  ProcessSample getOutputProcessSample() const;
  void setInputSample(const Sample & inputSample);
  Sample getInputSample() const;
  void setOutputSample(const Sample & outputSample);
  Sample getOutputSample() const;
  void setModesSample(const Sample & modesSample);
  Sample getModesSample() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  // KL and FCE decompositions
  PersistentCollection<KarhunenLoeveResult> inputKLResultCollection_;
  FunctionalChaosResult fceResult_;
  PersistentCollection<KarhunenLoeveResult> outputKLResultCollection_;

  // independent component blocks
  PersistentCollection<Indices> blockIndices_;

  // metamodel
  FieldToPointFunction fieldToPointMetamodel_;
  PointToFieldFunction pointToFieldMetamodel_;
  FieldFunction fieldMetamodel_;

  // data
  ProcessSample inputProcessSample_;
  ProcessSample outputProcessSample_;
  Sample inputSample_;
  Sample outputSample_;
  Sample modesSample_;

}; /* class FieldFunctionalChaosResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FIELDFUNCTIONALCHAOSRESULT_HXX */
