//                                               -*- C++ -*-
/**
 *  @brief Result of a Meta-model algorithm
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_METAMODELRESULT_HXX
#define OPENTURNS_METAMODELRESULT_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Function.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class MetaModelResult
 * Implementation of SimulationResult
 */
class OT_API MetaModelResult
  : public PersistentObject
{

  CLASSNAME
public:


  /** Default constructor */
  MetaModelResult();

  /** Standard constructor */
  MetaModelResult(const Sample & inputSample,
                  const Sample & outputSample,
                  const Function & metaModel,
                  const Point & residuals,
                  const Point & relativeErrors);

  /** Virtual constructor */
  MetaModelResult * clone() const override;

  /** Sample accessor */
  virtual void setInputSample(const Sample & sampleX);
  virtual Sample getInputSample() const;

  /** Sample accessor */
  virtual void setOutputSample(const Sample & sampleY);
  virtual Sample getOutputSample() const;

  /** MetaModel accessor */
  virtual void setMetaModel(const Function & metaModel);
  virtual Function getMetaModel() const;

  /** Marginal residuals accessor */
  virtual void setResiduals(const Point & residuals);
  virtual Point getResiduals() const;

  /** Marginal relative errors accessor */
  virtual void setRelativeErrors(const Point & relativeErrors);
  virtual Point getRelativeErrors() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  Sample inputSample_;
  Sample outputSample_;

  // The corresponding meta-model
  Function metaModel_;

  // The marginal residuals
  Point residuals_;

  // The marginal relative errors
  Point relativeErrors_;

}; // class MetaModelResult

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_METAMODELRESULT_HXX */
