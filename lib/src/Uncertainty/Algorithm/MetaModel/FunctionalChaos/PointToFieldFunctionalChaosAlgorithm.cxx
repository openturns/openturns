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

#include "openturns/PointToFieldFunctionalChaosAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KarhunenLoeveProjection.hxx"
#include "openturns/KarhunenLoeveLifting.hxx"
#include "openturns/FieldToPointFunction.hxx"
#include "openturns/LeastSquaresExpansion.hxx"
#include "openturns/PointToFieldConnection.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(PointToFieldFunctionalChaosAlgorithm)

static const Factory<PointToFieldFunctionalChaosAlgorithm> Factory_PointToFieldFunctionalChaosAlgorithm;


/* Default constructor */
PointToFieldFunctionalChaosAlgorithm::PointToFieldFunctionalChaosAlgorithm()
  : FieldFunctionalChaosAlgorithm()
{
  // Nothing to do
}

/* Constructor with parameters */
PointToFieldFunctionalChaosAlgorithm::PointToFieldFunctionalChaosAlgorithm(const Sample & inputSample,
    const ProcessSample & outputProcessSample,
    const Distribution & distribution)
  : FieldFunctionalChaosAlgorithm()
  , inputSample_(inputSample)
  , outputProcessSample_(outputProcessSample)
  , distribution_(distribution)
{
  if (inputSample.getSize() != outputProcessSample.getSize())
    throw InvalidArgumentException(HERE) << "PointToFieldFunctionalChaosAlgorithm the input sample size (" << inputSample.getSize()
                                         << ") must match the output sample size (" << outputProcessSample.getSize() << ")";
  if (inputSample.getDimension() != distribution.getDimension())
    throw InvalidArgumentException(HERE) << "PointToFieldFunctionalChaosAlgorithm the input sample dimension (" << inputSample.getDimension()
                                         << ") must match the input distribution dimension (" << distribution.getDimension() << ")";

  // default to one block for all output components
  Indices full(outputProcessSample_.getDimension());
  full.fill();
  blockIndices_.add(full);
}

/* Virtual constructor */
PointToFieldFunctionalChaosAlgorithm * PointToFieldFunctionalChaosAlgorithm::clone() const
{
  return new PointToFieldFunctionalChaosAlgorithm(*this);
}

Sample PointToFieldFunctionalChaosAlgorithm::getInputSample() const
{
  return inputSample_;
}

ProcessSample PointToFieldFunctionalChaosAlgorithm::getOutputProcessSample() const
{
  return outputProcessSample_;
}

void PointToFieldFunctionalChaosAlgorithm::setBlockIndices(const Collection<Indices> & blockIndices)
{
  Indices flat;
  for (UnsignedInteger i = 0; i < blockIndices.getSize(); ++ i)
    flat.add(blockIndices[i]);
  if ((flat.getSize() != outputProcessSample_.getDimension()) || !flat.check(outputProcessSample_.getDimension()))
    throw InvalidArgumentException(HERE) << "Block indices does not match with output dimension";
  blockIndices_ = blockIndices;
}

/* String converter */
String PointToFieldFunctionalChaosAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}

// dedicated function to stack liftings
class StackedLiftingFunction : public PointToFieldFunctionImplementation
{
public:
  explicit StackedLiftingFunction(const Collection<PointToFieldFunction> & liftingCollection)
    : PointToFieldFunctionImplementation()
    , liftingCollection_(liftingCollection)
  {
    if (!liftingCollection.getSize())
      throw InvalidArgumentException(HERE) << "liftingCollection should not be empty";

    outputMesh_ = liftingCollection[0].getOutputMesh();
    Description inputDescription;
    Description outputDescription;
    for (UnsignedInteger i = 0; i < liftingCollection.getSize(); ++ i)
    {
      inputDescription.add(liftingCollection[i].getInputDescription());
      outputDescription.add(liftingCollection[i].getOutputDescription());
    }

    inputDimension_ = inputDescription.getSize();
    outputDimension_ = outputDescription.getSize();
    setInputDescription(inputDescription);
    setOutputDescription(outputDescription);
  }

  StackedLiftingFunction * clone() const override
  {
    return new StackedLiftingFunction(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return inputDimension_;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return outputDimension_;
  }

  Sample operator() (const Point & inP) const override
  {
    Sample Y(getOutputMesh().getVerticesNumber(), 0);
    UnsignedInteger shift = 0;
    for (UnsignedInteger i = 0; i < liftingCollection_.getSize(); ++ i)
    {
      Point inPI(liftingCollection_[i].getInputDimension()); // subset of modes coefficients inP for block I
      std::copy(inP.begin() + shift, inP.begin() + shift + inPI.getDimension(), inPI.begin());
      shift += inPI.getDimension();
      const Sample y(liftingCollection_[i](inPI));
      Y.stack(y);
    }
    return Y;
  }

private:
  UnsignedInteger inputDimension_ = 0;
  UnsignedInteger outputDimension_ = 0;
  Collection<PointToFieldFunction> liftingCollection_;
};

/* Response surface computation */
void PointToFieldFunctionalChaosAlgorithm::run()
{
  // decompose input process
  Collection<KarhunenLoeveResult> klResultCollection(blockIndices_.getSize());
  decompose(outputProcessSample_, klResultCollection);

  // the global output lift stacks lifting of each block of variables
  const UnsignedInteger size = outputProcessSample_.getSize();
  Sample modesSample(size, 0);
  Collection<PointToFieldFunction> liftingCollection(blockIndices_.getSize());
  for (UnsignedInteger i = 0; i < blockIndices_.getSize(); ++ i)
  {
    const KarhunenLoeveProjection projectionI(klResultCollection[i]);
    const KarhunenLoeveLifting liftingI(klResultCollection[i]);
    liftingCollection[i] = liftingI;
    const ProcessSample outputProcessSampleI(outputProcessSample_.getMarginal(blockIndices_[i]));
    const Sample modesSampleI(projectionI(outputProcessSampleI));
    modesSample.stack(modesSampleI);
  }

  // build PCE expansion of projected modes vs output sample
  FunctionalChaosResult fceResult;
  const String expansion = ResourceMap::GetAsString("PointToFieldFunctionalChaosAlgorithm-Expansion");
  if (expansion == "LeastSquaresExpansion")
  {
    LeastSquaresExpansion fce(inputSample_, modesSample, distribution_);
    fce.run();
    fceResult = fce.getResult();
  }
  else if (expansion == "FunctionalChaosAlgorithm")
  {
    FunctionalChaosAlgorithm fce(inputSample_, modesSample, distribution_);
    fce.run();
    fceResult = fce.getResult();
  }
  else
    throw InvalidArgumentException(HERE) << "Unknown expansion: " << expansion;

  // compose FCE interpolation + output lifting
  const StackedLiftingFunction stackedLifting(liftingCollection);
  const PointToFieldFunction lifting(stackedLifting);
  PointToFieldFunction metamodel(PointToFieldConnection(lifting, fceResult.getMetaModel()));

  // populate result
  result_ = FieldFunctionalChaosResult(Collection<KarhunenLoeveResult>(), fceResult, klResultCollection);
  result_.setMetaModel(metamodel);
  result_.setInputSample(inputSample_);
  result_.setOutputBlockIndices(blockIndices_);
  result_.setOutputProcessSample(outputProcessSample_);
  result_.setModesSample(modesSample);
}

/* Method save() stores the object through the StorageManager */
void PointToFieldFunctionalChaosAlgorithm::save(Advocate & adv) const
{
  FieldFunctionalChaosAlgorithm::save(adv);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputProcessSample_", outputProcessSample_);
}

/* Method load() reloads the object from the StorageManager */
void PointToFieldFunctionalChaosAlgorithm::load(Advocate & adv)
{
  FieldFunctionalChaosAlgorithm::load(adv);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputProcessSample_", outputProcessSample_);
}

END_NAMESPACE_OPENTURNS
