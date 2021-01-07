//                                               -*- C++ -*-
/**
 *  @brief The class building chaos expansions
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/ExpertMixture.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ExpertMixture)

static const Factory<ExpertMixture> Factory_ExpertMixture;


/* Default constructor */
ExpertMixture::ExpertMixture()
  : EvaluationImplementation()
  , experts_()
  , classifier_()
  , supervised_(true)
{
  // Nothing to do
}


/* Constructor */
ExpertMixture::ExpertMixture(const FunctionCollection & experts,
                             const Classifier & classifier,
                             const Bool supervised)
  : EvaluationImplementation()
  , experts_(experts)
  , classifier_(classifier)
  , supervised_(supervised)
{
  // Check if there is at least one expert
  if (experts.getSize() == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an ExpertMixture with no expert!";
  // Check that the experts and the classifier are compatible
  if (supervised_)
  {
    if (classifier.getDimension() != (experts[0].getInputDimension() + experts[0].getOutputDimension()))
      throw InvalidArgumentException(HERE) << "Error: the classifier dimension must be equal to the sum of the experts input and output dimensions, here the classifier dimension=" << classifier.getDimension() << " and the experts input dimension=" << experts[0].getInputDimension() << " and output dimension=" << experts[0].getOutputDimension();
  }
  else
  {
    if (classifier.getDimension() != experts[0].getInputDimension())
      throw InvalidArgumentException(HERE) << "Error: the classifier dimension must be equal to the experts input dimension, here the classifier dimension=" << classifier.getDimension() << " and the experts input dimension=" << experts[0].getInputDimension();
  }
  if (classifier.getNumberOfClasses() != experts.getSize())
    throw InvalidArgumentException(HERE) << "The number of experts (" << experts.getSize() << ") must match the number of classes (" << classifier.getNumberOfClasses() << ")";
  setDescription(experts[0].getDescription());
}

/* Virtual constructor */
ExpertMixture * ExpertMixture::clone() const
{
  return new ExpertMixture(*this);
}

/* Comparison operator */
Bool ExpertMixture::operator ==(const ExpertMixture & other) const
{
  if (this == &other) return true;
  return (experts_ == other.experts_) && (classifier_ == other.classifier_);
}

/* String converter */
String ExpertMixture::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << ", experts=" << experts_
         << ", classifier=" << classifier_;
}

String ExpertMixture::__str__(const String & offset) const
{
  return OSS(false) << "experts=" << experts_.__str__(offset) << Os::GetEndOfLine() << offset
         << "classifier=" << classifier_.__str__(offset);
}


/* Local experts accessor */
void ExpertMixture::setExperts(const FunctionCollection & experts)
{
  if (experts[0].getInputDimension() != classifier_.getDimension()) throw InvalidArgumentException(HERE) << "Error: the local experts must have the same input dimension as the classifier dimension. Here, experts input dimension=" << experts[0].getInputDimension() << " and classifier dimension=" << classifier_.getDimension();
  experts_ = experts;
}

ExpertMixture::FunctionCollection ExpertMixture::getExperts() const
{
  return experts_;
}

/* Classifier accessor */
void ExpertMixture::setClassifier(const Classifier & classifier)
{
  if (experts_[0].getInputDimension() != classifier.getDimension()) throw InvalidArgumentException(HERE) << "Error: the local experts must have the same input dimension as the classifier dimension. Here, experts input dimension=" << experts_[0].getInputDimension() << " and classifier dimension=" << classifier.getDimension();
  classifier_ = classifier;
}

Classifier ExpertMixture::getClassifier() const
{
  return classifier_;
}

/* Operator () */
Point ExpertMixture::operator() (const Point & inP) const
{
  if (supervised_) return evaluateSupervised(inP);
  return evaluateNonSupervised(inP);
}

/* Operator () */
Point ExpertMixture::evaluateSupervised(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension << " and got a point of dimension=" << inP.getDimension();
  const UnsignedInteger size = experts_.getSize();
  UnsignedInteger bestClass = 0;
  // Build the point z for the first class and grade it according to the classifier
  Point mixedPoint(inP);
  Point bestValue(getOutputDimension());
  mixedPoint.add(bestValue);// z=(x, f(x))
  Scalar bestGrade = SpecFunc::LowestScalar;
  for (UnsignedInteger classIndex = 0; classIndex < size; ++ classIndex)
  {
    // Build the point z for each other class and grade it according to the classifier
    const Point localValue(experts_[classIndex](inP));
    std::copy(localValue.begin(), localValue.end(), mixedPoint.begin() + inputDimension);// z=(x, f(x))
    const Scalar grade = classifier_.grade(mixedPoint, classIndex);
    LOGDEBUG(OSS() << "Class index=" << classIndex << ", grade=" << grade << ", value=" << localValue);
    // The best class will give the output value
    if (grade > bestGrade)
    {
      bestGrade = grade;
      bestClass = classIndex;
      bestValue = localValue;
    }
  }
  LOGDEBUG(OSS() << "Best class index=" << bestClass << ", best grade=" << bestGrade << ", best value=" << bestValue);
  return bestValue;
}

Point ExpertMixture::evaluateNonSupervised(const Point & inP) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inP.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension << " and got a point of dimension=" << inP.getDimension();
  const UnsignedInteger size = experts_.getSize();
  UnsignedInteger bestClass = 0;
  // Build the point z for the first class and grade it according to the classifier
  Scalar bestGrade = SpecFunc::LowestScalar;
  for (UnsignedInteger classIndex = 0; classIndex < size; ++ classIndex)
  {
    // Build the point z for each other class and grade it according to the classifier
    const Scalar grade = classifier_.grade(inP, classIndex);
    LOGDEBUG(OSS() << "Class index=" << classIndex << ", grade=" << grade);
    // The best class will give the output value
    if (grade > bestGrade)
    {
      bestGrade = grade;
      bestClass = classIndex;
    }
  }
  const Point bestValue(experts_[bestClass](inP));
  LOGDEBUG(OSS() << "Best class index=" << bestClass << ", best grade=" << bestGrade << ", best value=" << bestValue);
  return bestValue;
}


Sample ExpertMixture::operator() (const Sample & inS) const
{
  if (supervised_) return evaluateSupervised(inS);
  return evaluateNonSupervised(inS);
}


Sample ExpertMixture::evaluateSupervised(const Sample & inS) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension << " and got a sample of dimension=" << inS.getDimension();
  const UnsignedInteger size = inS.getSize();
  const UnsignedInteger outputDimension = getOutputDimension();
  Sample bestValues(size, outputDimension);
  const UnsignedInteger expertSize = experts_.getSize();
  Point bestGrades(size, SpecFunc::LowestScalar);
  for (UnsignedInteger classIndex = 0; classIndex < expertSize; ++ classIndex)
  {
    // Build the point z for each other class and grade it according to the classifier
    Sample mixedSample(inS);
    // Here is the evaluation of the expert over a sample, benefiting from possible
    // parallelism/vectorization
    const Sample localValues(experts_[classIndex](inS));
    mixedSample.stack(localValues);// z=(x, f(x))
    const Point grades = classifier_.grade(mixedSample, Indices(size, classIndex));
    for (UnsignedInteger i = 0; i < size; ++ i)
      if (grades[i] > bestGrades[i])
      {
        bestGrades[i] = grades[i];
        bestValues[i] = localValues[i];
      }
  } // classIndex
  return bestValues;
}


Sample ExpertMixture::evaluateNonSupervised(const Sample & inS) const
{
  const UnsignedInteger inputDimension = getInputDimension();
  if (inS.getDimension() != inputDimension) throw InvalidArgumentException(HERE) << "Error: expected a point of dimension=" << inputDimension << " and got a sample of dimension=" << inS.getDimension();
  const UnsignedInteger size = inS.getSize();
  const UnsignedInteger outputDimension = getOutputDimension();
  const UnsignedInteger expertSize = experts_.getSize();
  Point bestGrades(size, SpecFunc::LowestScalar);
  Indices bestClasses(size);
  for (UnsignedInteger classIndex = 0; classIndex < expertSize; ++ classIndex)
  {
    // Here is the evaluation of the expert over a sample, benefiting from possible
    // parallelism/vectorization
    const Point grades = classifier_.grade(inS, Indices(size, classIndex));
    for (UnsignedInteger i = 0; i < size; ++ i)
      if (grades[i] > bestGrades[i])
      {
        bestGrades[i] = grades[i];
        bestClasses[i] = classIndex;
      }
  } // classIndex
  Sample bestValues(size, outputDimension);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const UnsignedInteger bestClass_i = bestClasses[i];
    bestValues[i] = experts_[bestClass_i](inS[i]);
  }
  return bestValues;
}


/* Accessor for input point dimension */
UnsignedInteger ExpertMixture::getInputDimension() const
{
  return experts_[0].getInputDimension();
}

/* Accessor for output point dimension */
UnsignedInteger ExpertMixture::getOutputDimension() const
{
  return experts_[0].getOutputDimension();
}


/* Method save() stores the object through the StorageManager */
void ExpertMixture::save(Advocate & adv) const
{
  EvaluationImplementation::save(adv);
  adv.saveAttribute( "experts_", experts_ );
  adv.saveAttribute( "classifier_", classifier_ );
  adv.saveAttribute( "supervised_", supervised_ );
}


/* Method load() reloads the object from the StorageManager */
void ExpertMixture::load(Advocate & adv)
{
  EvaluationImplementation::load(adv);
  adv.loadAttribute( "experts_", experts_ );
  adv.loadAttribute( "classifier_", classifier_ );
  adv.loadAttribute( "supervised_", supervised_ );
}

END_NAMESPACE_OPENTURNS
