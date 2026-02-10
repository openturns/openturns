//                                               -*- C++ -*-
/**
 *  @brief The linear model validation
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
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearModelValidation.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/LeaveOneOutSplitter.hxx"
#include "openturns/KFoldSplitter.hxx"
#include "openturns/MetaModelValidation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LinearModelValidation)

static const Factory<LinearModelValidation> Factory_LinearModelValidation;

/* Default constructor */
LinearModelValidation::LinearModelValidation()
  : MetaModelValidation()
{
  // Nothing to do
}

/* Parameter constructor */
LinearModelValidation::LinearModelValidation(const LinearModelResult & linearModelResult)
  : LinearModelValidation(linearModelResult, LeaveOneOutSplitter(linearModelResult.getSampleResiduals().getSize()))
{
  // Nothing to do
}

/* Parameter constructor */
LinearModelValidation::LinearModelValidation(const LinearModelResult & linearModelResult,
    const LeaveOneOutSplitter & splitter)
  : MetaModelValidation(linearModelResult.getOutputSample(),
                        ComputeMetamodelLeaveOneOutPredictions(linearModelResult, splitter))
  , linearModelResult_(linearModelResult)
  , splitter_(splitter)
{
  const UnsignedInteger sampleSize = linearModelResult_.getSampleResiduals().getSize();
  if ((splitter_.getN() != sampleSize))
    throw InvalidArgumentException(HERE) << "The parameter N in the splitter is " << splitter_.getN()
                                         << " but the sample size is " << sampleSize;
  if (!ResourceMap::GetAsBool("LinearModelValidation-ModelSelection") && \
      linearModelResult.involvesModelSelection())
    throw InvalidArgumentException(HERE) << "Cannot perform fast cross-validation "
                                         << "with a linear model involving model selection";
}

/* Parameter constructor */
LinearModelValidation::LinearModelValidation(const LinearModelResult & linearModelResult,
    const KFoldSplitter & splitter)
  : MetaModelValidation(linearModelResult.getOutputSample(),
                        ComputeMetamodelKFoldPredictions(linearModelResult, splitter))
  , linearModelResult_(linearModelResult)
  , splitter_(splitter)
{
  const UnsignedInteger sampleSize = linearModelResult_.getSampleResiduals().getSize();
  if ((splitter_.getN() != sampleSize))
    throw InvalidArgumentException(HERE) << "The parameter N in the splitter is " << splitter_.getN()
                                         << " but the sample size is " << sampleSize;
  if (!ResourceMap::GetAsBool("LinearModelValidation-ModelSelection") && \
      linearModelResult.involvesModelSelection())
    throw InvalidArgumentException(HERE) << "Cannot perform fast cross-validation "
                                         << "with a linear model involving model selection";
}

/* Virtual constructor */
LinearModelValidation * LinearModelValidation::clone() const
{
  return new LinearModelValidation(*this);
}


/* String converter */
String LinearModelValidation::__repr__() const
{
  return OSS(true)
         << "class=" << getClassName()
         << ", linearModelResult=" << linearModelResult_
         << ", splitter_=" << splitter_;
}

/* Compute cross-validation predictions */
Sample LinearModelValidation::ComputeMetamodelLeaveOneOutPredictions(
  const LinearModelResult & linearModelResult,
  const LeaveOneOutSplitter & splitter)
{
  const Sample outputSample(linearModelResult.getOutputSample());
  const Sample residualsSample(linearModelResult.getSampleResiduals());
  const Point hMatrixDiag(linearModelResult.getLeverages());
  const Sample cvPredictions(MetaModelValidation::ComputeMetamodelLeaveOneOutPredictions(
                               outputSample, residualsSample, hMatrixDiag, splitter));
  return cvPredictions;
}

/* Compute cross-validation predictions */
Sample LinearModelValidation::ComputeMetamodelKFoldPredictions(
  const LinearModelResult & linearModelResult,
  const KFoldSplitter & splitter)
{
  const Sample outputSample(linearModelResult.getOutputSample());
  const Sample residualsSample(linearModelResult.getSampleResiduals());
  SymmetricMatrix projectionMatrix(linearModelResult.buildMethod().getH());
  const Sample cvPredictions(MetaModelValidation::ComputeMetamodelKFoldPredictions(
                               outputSample, residualsSample, projectionMatrix, splitter));
  return cvPredictions;
}

/* Linear model accessor */
LinearModelResult LinearModelValidation::getLinearModelResult() const
{
  return linearModelResult_;
}

/* Get the splitter */
SplitterImplementation LinearModelValidation::getSplitter() const
{
  return splitter_;
}

/* Method save() stores the object through the StorageManager */
void LinearModelValidation::save(Advocate & adv) const
{
  MetaModelValidation::save(adv);
  adv.saveAttribute("linearModelResult_", linearModelResult_);
  adv.saveAttribute("splitter_", splitter_ );
}


/* Method load() reloads the object from the StorageManager */
void LinearModelValidation::load(Advocate & adv)
{
  MetaModelValidation::load(adv);
  adv.loadAttribute("linearModelResult_", linearModelResult_);
  adv.loadAttribute("splitter_", splitter_);
}

END_NAMESPACE_OPENTURNS
