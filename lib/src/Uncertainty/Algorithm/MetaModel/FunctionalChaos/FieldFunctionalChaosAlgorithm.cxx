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

#include <limits>
#include "openturns/FieldFunctionalChaosAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KarhunenLoeveSVDAlgorithm.hxx"
#include "openturns/RankMCovarianceModel.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(FieldFunctionalChaosAlgorithm)

static const Factory<FieldFunctionalChaosAlgorithm> Factory_FieldFunctionalChaosAlgorithm;


/* Default constructor */
FieldFunctionalChaosAlgorithm::FieldFunctionalChaosAlgorithm()
  : PersistentObject()
  , nbModes_(std::numeric_limits<UnsignedInteger>::max())
  , recompress_(ResourceMap::GetAsBool("FieldFunctionalChaosAlgorithm-DefaultRecompress"))
{
  // Nothing to do
}

/* Virtual constructor */
FieldFunctionalChaosAlgorithm * FieldFunctionalChaosAlgorithm::clone() const
{
  return new FieldFunctionalChaosAlgorithm(*this);
}

/* Threshold accessors */
Scalar FieldFunctionalChaosAlgorithm::getThreshold() const
{
  return threshold_;
}

void FieldFunctionalChaosAlgorithm::setThreshold(const Scalar threshold)
{
  if (!(threshold <= 1.0 && threshold >= 0.0))
    throw InvalidArgumentException(HERE) << "threshold must be between 0.0 and 1.0 but is " << threshold;

  threshold_ = threshold;
}

/* Number of modes accessors */
UnsignedInteger FieldFunctionalChaosAlgorithm::getNbModes() const
{
  return nbModes_;
}

void FieldFunctionalChaosAlgorithm::setNbModes(const UnsignedInteger nbModes)
{
  nbModes_ = nbModes;
}

/* Recompression flag accessors */
Bool FieldFunctionalChaosAlgorithm::getRecompress() const
{
  return recompress_;
}

void FieldFunctionalChaosAlgorithm::setRecompress(const Bool recompress)
{
  recompress_ = recompress;
}

/* Block indices accessors */
Collection<Indices> FieldFunctionalChaosAlgorithm::getBlockIndices() const
{
  return blockIndices_;
}

/* Result accessor */
FieldFunctionalChaosResult FieldFunctionalChaosAlgorithm::getResult() const
{
  return result_;
}

/* String converter */
String FieldFunctionalChaosAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}

void FieldFunctionalChaosAlgorithm::decompose(const ProcessSample & processSample,
                                                Collection<KarhunenLoeveResult> & klResultCollection)
{
  klResultCollection.resize(blockIndices_.getSize());
  Point eigenValues;

  for (UnsignedInteger i = 0; i < blockIndices_.getSize(); ++ i)
  {
    const ProcessSample inputProcessSampleI(processSample.getMarginal(blockIndices_[i]));
    const Bool centered = true;
    KarhunenLoeveSVDAlgorithm algo(inputProcessSampleI, threshold_, centered);
    algo.setNbModes(nbModes_);
    algo.run();
    eigenValues.add(algo.getResult().getEigenvalues());
    klResultCollection[i] = algo.getResult();
    LOGINFO(OSS() << "block #" << i << " indices=" << blockIndices_[i] << " ev size=" << algo.getResult().getEigenvalues().getDimension());
  }

  if (recompress_)
  {
    const Scalar cumulatedVariance = eigenValues.norm1();
    std::sort(eigenValues.begin(), eigenValues.end(), [](const Scalar a, const Scalar b)
    {
      return a > b;
    });
    const UnsignedInteger nbModesMax = std::min(nbModes_, eigenValues.getSize());
    // Find the cut-off in the eigenvalues
    UnsignedInteger K = 0;
    Scalar selectedVariance = 0.0; // sum of eigenvalues selected after cut-off is applied
    do
    {
      selectedVariance += eigenValues[K];
      ++ K;
    }
    while ((K < nbModesMax) && (selectedVariance < (1.0 - threshold_) * cumulatedVariance));
    LOGINFO(OSS() << "Selected " << K << " eigenvalues out of " << eigenValues.getSize() << " computed");

    const Scalar lambdaCut = eigenValues[K - 1];
    for (UnsignedInteger i = 0; i < blockIndices_.getSize(); ++ i)
    {
      UnsignedInteger Ki = 1;// retain at least 1 mode
      Point eigenValuesI(klResultCollection[i].getEigenvalues());
      Scalar selectedVarianceI = eigenValuesI[0];
      const Scalar cumulatedVarianceI = eigenValuesI.norm1();
      while ((Ki < eigenValuesI.getSize()) && (eigenValuesI[Ki] >= lambdaCut))
      {
        selectedVarianceI += eigenValuesI[Ki];
        ++ Ki;
      }
      eigenValuesI.resize(Ki);
      Collection<Function> modesI(klResultCollection[i].getModes());
      modesI.resize(Ki);
      CovarianceModel covarianceI(RankMCovarianceModel(eigenValuesI, modesI));
      ProcessSample modesAsProcessSampleI(klResultCollection[i].getModesAsProcessSample());
      modesAsProcessSampleI.erase(Ki, modesAsProcessSampleI.getSize());
      MatrixImplementation projectionMatrixI(*klResultCollection[i].getProjectionMatrix().getImplementation());
      projectionMatrixI.resize(Ki, projectionMatrixI.getNbColumns());

      klResultCollection[i] = KarhunenLoeveResult(covarianceI, klResultCollection[i].getThreshold(), eigenValuesI, modesI, modesAsProcessSampleI, projectionMatrixI, selectedVarianceI / cumulatedVarianceI);
    }
  }
}

/* Method save() stores the object through the StorageManager */
void FieldFunctionalChaosAlgorithm::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("blockIndices_", blockIndices_);
  adv.saveAttribute("threshold_", threshold_);
  adv.saveAttribute("nbModes_", nbModes_);
  adv.saveAttribute("recompress_", recompress_);
  adv.saveAttribute("result_", result_);
}

/* Method load() reloads the object from the StorageManager */
void FieldFunctionalChaosAlgorithm::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("blockIndices_", blockIndices_);
  adv.loadAttribute("threshold_", threshold_);
  adv.loadAttribute("nbModes_", nbModes_);
  adv.loadAttribute("recompress_", recompress_);
  adv.loadAttribute("result_", result_);
}

END_NAMESPACE_OPENTURNS
