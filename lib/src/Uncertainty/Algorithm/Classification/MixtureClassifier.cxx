//                                               -*- C++ -*-
/**
 *  @brief Associate to a given point its class index
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

#include "openturns/MixtureClassifier.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MixtureClassifier)

static const Factory<MixtureClassifier> Factory_MixtureClassifier;

/* Default constructor */
MixtureClassifier::MixtureClassifier()
  : ClassifierImplementation()
{
  // Nothing to do
}

/* Default constructor */
MixtureClassifier::MixtureClassifier(const Mixture & mixture)
  : ClassifierImplementation()
  , mixture_(mixture)
{
  // Nothing to do
}

/* Virtual constructor */
MixtureClassifier * MixtureClassifier::clone() const
{
  return new MixtureClassifier( *this );
}

/* String converter */
String MixtureClassifier::__repr__() const
{
  return OSS() << "class=" << GetClassName()
         << " mixture=" << mixture_;
}

UnsignedInteger MixtureClassifier::getNumberOfClasses() const
{
  return mixture_.getDistributionCollection().getSize();
}


/* Associate a set of points to a class */
UnsignedInteger MixtureClassifier::classify(const Point& inP) const
{
  if (inP.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the point to classify has dimension=" << inP.getDimension() << " but the classifier expects dimension=" << mixture_.getDimension();
  const UnsignedInteger size = mixture_.getDistributionCollection().getSize();
  UnsignedInteger bestClass = 0;

  Scalar bestGrade = std::log(mixture_.getWeights()[0]) + mixture_.getDistributionCollection()[0].computeLogPDF(inP);
  for (UnsignedInteger classIndex = 1; classIndex < size; ++classIndex)
  {
    const Scalar gradeValue = std::log(mixture_.getWeights()[classIndex]) + mixture_.getDistributionCollection()[classIndex].computeLogPDF(inP);
    if (gradeValue > bestGrade)
    {
      bestClass = classIndex;
      bestGrade = gradeValue;
    }
  }
  return bestClass;
}

Indices MixtureClassifier::classify(const Sample & inS) const
{
  const UnsignedInteger mixtureSize = mixture_.getDistributionCollection().getSize();
  const UnsignedInteger size = inS.getSize();
  Point logWeights(mixtureSize);
  Sample atomsLogPDF(mixtureSize, size);
  // The expansive part: the computation of the log-PDF, here we benefit
  // from possible parallelism in computeLogPDF() for each atom
  for (UnsignedInteger classIndex = 0; classIndex < mixtureSize; ++classIndex)
  {
    const Distribution atom(mixture_.getDistributionCollection()[classIndex]);
    atomsLogPDF[classIndex] = atom.computeLogPDF(inS).getImplementation()->getData();
    logWeights[classIndex] = std::log(mixture_.getWeights()[classIndex]);
  }
  // Now grade the points
  // The outer loop is on the classes and the inner loop on the point to
  // benefit from data locality
  Point bestGrades(size, SpecFunc::LowestScalar);
  Indices bestClasses(size);
  for (UnsignedInteger classIndex = 0; classIndex < mixtureSize; ++classIndex)
  {
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      const Scalar grade = logWeights[classIndex] + atomsLogPDF(classIndex, i);
      if (grade > bestGrades[i])
      {
        bestGrades[i] = grade;
        bestClasses[i] = classIndex;
      } // grade > bestGrades[i]
    } // for i
  } // for classIndex
  return bestClasses;
}

Scalar MixtureClassifier::grade(const Point & inP,
                                const UnsignedInteger outC) const
{
  const UnsignedInteger size = mixture_.getDistributionCollection().getSize();
  if (outC >= size) throw InvalidDimensionException(HERE) << "Class number (=" << outC << ") must be lower than size (=" << size << ").";
  return std::log(mixture_.getWeights()[outC]) + mixture_.getDistributionCollection()[outC].computeLogPDF(inP);
}

/* Mixture accessors */
Mixture MixtureClassifier::getMixture() const
{
  return mixture_;
}

void MixtureClassifier::setMixture(const Mixture & mixture)
{
  mixture_ = mixture;
}

/* Dimension accessor */
UnsignedInteger MixtureClassifier::getDimension() const
{
  return mixture_.getDimension();
}

/* Method save() stores the object through the StorageManager */
void MixtureClassifier::save(Advocate & adv) const
{
  ClassifierImplementation::save(adv);
  adv.saveAttribute( "mixture_", mixture_ );
}

/* Method load() reloads the object from the StorageManager */
void MixtureClassifier::load(Advocate & adv)
{
  ClassifierImplementation::load(adv);
  adv.loadAttribute( "mixture_", mixture_ );
}

END_NAMESPACE_OPENTURNS
