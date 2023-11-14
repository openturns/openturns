//                                               -*- C++ -*-
/**
 *  @brief Result computes the results of an analytical Algorithm
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
#include <iomanip>
#include <cstdlib>

#include "openturns/Analytical.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Sample.hxx"
#include "openturns/SobolIndicesAlgorithm.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/BarPlot.hxx"
#include "openturns/Description.hxx"
#include "openturns/ThresholdEvent.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/Less.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/GaussLegendre.hxx"

BEGIN_NAMESPACE_OPENTURNS


typedef Distribution::InverseIsoProbabilisticTransformation InverseIsoProbabilisticTransformation;
typedef PersistentCollection<PointWithDescription> PersistentSensitivity;

CLASSNAMEINIT(AnalyticalResult)

static const Factory<AnalyticalResult> Factory_AnalyticalResult;

/*
 * @brief  Standard constructor
 */
AnalyticalResult::AnalyticalResult(const Point & standardSpaceDesignPoint,
                                   const RandomVector & limitStateVariable,
                                   const Bool isStandardPointOriginInFailureSpace):
  PersistentObject(),
  standardSpaceDesignPoint_(0),
  physicalSpaceDesignPoint_(0),
  limitStateVariable_(limitStateVariable),
  isStandardPointOriginInFailureSpace_(isStandardPointOriginInFailureSpace),
  hasoferReliabilityIndex_(0.0),
  importanceFactors_(0),
  classicalImportanceFactors_(0),
  hasoferReliabilityIndexSensitivity_(0),
  isAlreadyComputedImportanceFactors_(false),
  isAlreadyComputedClassicalImportanceFactors_(false),
  isAlreadyComputedPhysicalImportanceFactors_(false),
  isAlreadyComputedHasoferReliabilityIndexSensitivity_(false)
{
  /* we use the attribute accessor in order to compute automatically all the other attributes */
  setStandardSpaceDesignPoint(standardSpaceDesignPoint);
}

/* Default constructor, needed by SWIG */
AnalyticalResult::AnalyticalResult():
  PersistentObject(),
  standardSpaceDesignPoint_(0),
  physicalSpaceDesignPoint_(0),
  // Fake event based on a fake 1D composite random vector, which requires a fake 1D Function
  limitStateVariable_(ThresholdEvent()),
  isStandardPointOriginInFailureSpace_(false),
  hasoferReliabilityIndex_(0.0),
  importanceFactors_(0),
  classicalImportanceFactors_(0),
  hasoferReliabilityIndexSensitivity_(0),
  isAlreadyComputedImportanceFactors_(false),
  isAlreadyComputedClassicalImportanceFactors_(false),
  isAlreadyComputedPhysicalImportanceFactors_(false),
  isAlreadyComputedHasoferReliabilityIndexSensitivity_(false)
{
  // Nothing to do
}

/* Virtual constructor */
AnalyticalResult * AnalyticalResult::clone() const
{
  return new AnalyticalResult(*this);
}

/* StandardSpaceDesignPoint accessor */
Point AnalyticalResult::getStandardSpaceDesignPoint() const
{
  return standardSpaceDesignPoint_;
}


/* StandardSpaceDesignPoint accessor */
void AnalyticalResult::setStandardSpaceDesignPoint(const Point & standardSpaceDesignPoint)
{
  standardSpaceDesignPoint_ = standardSpaceDesignPoint;
  computePhysicalSpaceDesignPoint();
  computeHasoferReliabilityIndex();
}

/* PhysicalSpaceDesignPoint evaluation */
void AnalyticalResult::computePhysicalSpaceDesignPoint() const
{
  /* Compute the physical design point */
  physicalSpaceDesignPoint_ = limitStateVariable_.getImplementation()->getAntecedent().getDistribution().getInverseIsoProbabilisticTransformation().operator()(standardSpaceDesignPoint_);
  /* we give to the physical space design point the description of the input random vector */
  //physicalSpaceDesignPoint_.setDescription(limitStateVariable_.getImplementation()->getAntecedent().getDescription());

  /* we give to the physical space design point the name Physical Design Point */
  physicalSpaceDesignPoint_.setName("Physical Space Design Point");
}

/* PhysicalSpaceDesignPoint accessor */
void AnalyticalResult::setPhysicalSpaceDesignPoint(const Point & physicalSpaceDesignPoint)
{
  physicalSpaceDesignPoint_ = physicalSpaceDesignPoint;
}

/* PhysicalSpaceDesignPoint accessor */
Point AnalyticalResult::getPhysicalSpaceDesignPoint() const
{
  return physicalSpaceDesignPoint_;
}

/* LimitStateVariable accessor */
RandomVector AnalyticalResult::getLimitStateVariable() const
{
  return limitStateVariable_;
}

/* IsStandardPointOriginInFailureSpace  accessor */
Bool AnalyticalResult::getIsStandardPointOriginInFailureSpace() const
{
  return isStandardPointOriginInFailureSpace_;
}

/* IsStandardPointOriginInFailureSpace  accessor */
void AnalyticalResult::setIsStandardPointOriginInFailureSpace(const Bool isStandardPointOriginInFailureSpace)
{
  isStandardPointOriginInFailureSpace_ = isStandardPointOriginInFailureSpace;
}

/* ImportanceFactors evaluation */
void AnalyticalResult::computeImportanceFactors() const
{
  const UnsignedInteger dimension = standardSpaceDesignPoint_.getDimension();
  importanceFactors_ = Point(dimension, -1.0);
  /* First, check that the importance factors are well-defined */
  if (standardSpaceDesignPoint_.norm() > 0.0)
  {
    /* get the input distribution */
    const Distribution inputDistribution(limitStateVariable_.getImplementation()->getAntecedent().getDistribution());
    /* get the input standard distribution */
    const Distribution standardDistribution(inputDistribution.getStandardDistribution());
    /* get the marginal 1D from in the standard space where all marginals are identical */
    const Distribution standardMarginalDistribution(standardDistribution.getMarginal(0));
    /* evaluate the corresponding vector in the importance factors space */
    /* if Generalised Nataf Transformation : Z = E1^(-1)o F_{Xi}(Xi*) */
    /* if Rosenblatt  Transformation : Z = Phi^(-1)o F_{Xi}(Xi*) */
    // for each marginals */
    for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++marginalIndex)
    {
      const Scalar y = inputDistribution.getMarginal(marginalIndex).computeCDF(Point(1, physicalSpaceDesignPoint_[marginalIndex]));
      importanceFactors_[marginalIndex] = standardMarginalDistribution.computeQuantile(y)[0];
    }
    importanceFactors_ = importanceFactors_.normalizeSquare();
  }
  /* we give a name to the importance factors vector */
  importanceFactors_.setName("Importance Factors");

  /* we give to the importance factors vector the description of the input random vector */
  importanceFactors_.setDescription(limitStateVariable_.getImplementation()->getAntecedent().getDescription());
  isAlreadyComputedImportanceFactors_ = true;
}

/* Classical ImportanceFactors evaluation */
void AnalyticalResult::computeClassicalImportanceFactors() const
{
  const UnsignedInteger dimension = standardSpaceDesignPoint_.getDimension();
  classicalImportanceFactors_ = Point(dimension, -1.0);
  /* First, check that the importance factors are well-defined */
  const Scalar beta2 = standardSpaceDesignPoint_.normSquare();
  if (beta2 > 0.0) classicalImportanceFactors_ = standardSpaceDesignPoint_.normalizeSquare();
  /* we give a name to the importance factors vector */
  classicalImportanceFactors_.setName("Classical Importance Factors");

  /* we give to the importance factors vector the description of the input random vector */
  classicalImportanceFactors_.setDescription(limitStateVariable_.getImplementation()->getAntecedent().getDescription());

  isAlreadyComputedClassicalImportanceFactors_ = true;
}


void AnalyticalResult::computePhysicalImportanceFactors() const
{
  const RandomVector myEvent(getLimitStateVariable());
  const StandardEvent mystandardEvent(myEvent);
  const Scalar sign = myEvent.getOperator().compare(0., 1.) ? 1.0 : -1.0;
  const Point currentStandardGradient(mystandardEvent.getImplementation()->getFunction().gradient(getStandardSpaceDesignPoint()) * Point(1, 1.0));
  const Point alpha(sign / currentStandardGradient.norm() * currentStandardGradient);
  const Distribution physicalDistribution(myEvent.getImplementation()->getAntecedent().getDistribution());
  const InverseIsoProbabilisticTransformation inverseIsoProbabilisticTransformation(physicalDistribution.getInverseIsoProbabilisticTransformation());
  const Matrix isoGradient(inverseIsoProbabilisticTransformation.gradient(standardSpaceDesignPoint_));
  physicalImportanceFactors_ = (isoGradient * alpha).normalizeSquare();
  const Point currentPhysicalGradient(limitStateVariable_.getImplementation()->getFunction().gradient(getPhysicalSpaceDesignPoint()) * Point(1, 1.0));
  physicalImportanceFactors_ = currentPhysicalGradient.normalizeSquare();
  physicalImportanceFactors_.setName("Physical importance factors");
  physicalImportanceFactors_.setDescription(myEvent.getImplementation()->getAntecedent().getDescription());
  isAlreadyComputedPhysicalImportanceFactors_ = true;
}


/*  Mean point conditioned to the standard event realization accessor */
void AnalyticalResult::computeMeanPointInStandardEventDomain() const
{
  // We use the implementation here in order to have access to the computeRadialDistributionCDF() method
  Distribution::Implementation p_standardDistribution(limitStateVariable_.getImplementation()->getAntecedent().getDistribution().getStandardDistribution().getImplementation());
  // Perform the integration along the ray going from the origin to the infinity
  Scalar scaling = hasoferReliabilityIndex_ * p_standardDistribution->computeRadialDistributionCDF(hasoferReliabilityIndex_, true);
  Scalar a = hasoferReliabilityIndex_;
  // Quadrature rule
  const UnsignedInteger integrationNodesNumber = ResourceMap::GetAsUnsignedInteger("AnalyticalResult-MeanPointIntegrationNodesNumber");
  const GaussLegendre integrator(Indices(1, integrationNodesNumber));
  const Point nodes(integrator.getNodes().getImplementation()->getData() * 2.0 - Point(integrationNodesNumber, 1.0));
  const Point weights(integrator.getWeights() * 2.0);
  Scalar sum = 0.0;
  const Scalar quantileEpsilon = ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
  do
  {
    // Integrate over a unit length segment [a, a+1]
    for (UnsignedInteger k = 0; k < integrationNodesNumber; ++k) sum += weights[k] * p_standardDistribution->computeRadialDistributionCDF(a + 0.5 * (1.0 + nodes[k]), true);
    sum *= 0.5;
    scaling += sum;
    a += 1.0;
  }
  while (sum > quantileEpsilon * scaling);
  meanPointInStandardEventDomain_ = standardSpaceDesignPoint_ * (scaling / hasoferReliabilityIndex_);
}

/* Mean point conditioned to the event realization accessor */
Point AnalyticalResult::getMeanPointInStandardEventDomain() const
{
  if (!meanPointInStandardEventDomain_.getSize()) computeMeanPointInStandardEventDomain();
  return meanPointInStandardEventDomain_;
}

void AnalyticalResult::setMeanPointInStandardEventDomain(const Point & meanPointInStandardEventDomain)
{
  meanPointInStandardEventDomain_ = meanPointInStandardEventDomain;
}

/* ImportanceFactors accessor */
PointWithDescription AnalyticalResult::getImportanceFactors(const ImportanceFactorType type) const
{
  switch(type)
  {
    case ELLIPTICAL:
      if (!isAlreadyComputedImportanceFactors_) computeImportanceFactors();
      return importanceFactors_;
    case CLASSICAL:
      if (!isAlreadyComputedClassicalImportanceFactors_) computeClassicalImportanceFactors();
      return classicalImportanceFactors_;
    case PHYSICAL:
      if (!isAlreadyComputedPhysicalImportanceFactors_) computePhysicalImportanceFactors();
      return physicalImportanceFactors_;
    default:
      throw InvalidArgumentException(HERE) << "Invalid importance factor type (" << type << ")";
  }
}

/* ImportanceFactors graph */
Graph AnalyticalResult::drawImportanceFactors(const ImportanceFactorType type) const
{
  // To ensure that the importance factors are up to date
  getImportanceFactors(type);
  /* we build the graph with a title */
  OSS oss;
  oss << "Importance Factors from Design Point - " << limitStateVariable_.getName();
  return SobolIndicesAlgorithm::DrawImportanceFactors(importanceFactors_, oss.str());
}

/* HasoferReliabilityIndex evaluation */
void AnalyticalResult::computeHasoferReliabilityIndex() const
{
  /* evaluate the HasoferReliabilityIndex */
  if (standardSpaceDesignPoint_.getDimension() > 0) hasoferReliabilityIndex_ = standardSpaceDesignPoint_.norm();
}

/*  HasoferReliabilityIndex accessor */
Scalar AnalyticalResult::getHasoferReliabilityIndex() const
{
  return hasoferReliabilityIndex_;
}


/* HasoferReliabilityIndex accessor */
void AnalyticalResult::setHasoferReliabilityIndex(const Scalar & hasoferReliabilityIndex)
{
  hasoferReliabilityIndex_ = hasoferReliabilityIndex;
}

/* HasoferReliabilityIndex evaluation */
void AnalyticalResult::computeHasoferReliabilityIndexSensitivity() const
{
  /* get Set1 : parameters of the physical distribution */
  const Distribution physicalDistribution(limitStateVariable_.getImplementation()->getAntecedent().getDistribution());
  const PointWithDescriptionCollection set1(physicalDistribution.getParametersCollection());
  /* get Set2 : parameters of the physical model */
  const Function physicalModel(limitStateVariable_.getImplementation()->getFunction());
  PointWithDescription set2(physicalModel.getParameter());
  set2.setDescription(physicalModel.getParameterDescription());
  const Bool isSet2Empty = set2.getDimension() == 0;
  /* get SetIso : parameters included in the isoprobabilistic transformation which is a subset of Set1 */
  const InverseIsoProbabilisticTransformation inverseIsoProbabilisticTransformation(physicalDistribution.getInverseIsoProbabilisticTransformation());
  PointWithDescription setIso(inverseIsoProbabilisticTransformation.getParameter());
  setIso.setDescription(inverseIsoProbabilisticTransformation.getParameterDescription());
  /* scaling factor between sensitivity and gradients (ref doc : factor = -lambda/beta) */
  /* gradient of the standard limite state function */
  const Matrix physicalGradientMatrix(physicalModel.gradient(physicalSpaceDesignPoint_));
  const Matrix isoGradient(inverseIsoProbabilisticTransformation.gradient(standardSpaceDesignPoint_));
  const Point standardFunctionGradient(isoGradient * (physicalGradientMatrix * Point(1, 1.0)));
  const Scalar gradientNorm = standardFunctionGradient.norm();
  Scalar gradientToSensitivity = 0.0;
  if (gradientNorm > 0.0) gradientToSensitivity = -(limitStateVariable_.getOperator().compare(1.0, 0.0) ? 1.0 : -1.0) / gradientNorm;
  /* evaluate the gradients of the physical model with respect to Set2 (ref doc : K2) */
  Point physicalGradient;
  if (!isSet2Empty) physicalGradient = physicalModel.parameterGradient(physicalSpaceDesignPoint_) * Point(1, 1.0);
  /* evaluate the gradients of the isoprobabilistic transformation with respect to SetIso (ref doc : KS) */
  Point isoProbabilisticGradient;
  if (setIso.getDimension() > 0) isoProbabilisticGradient = inverseIsoProbabilisticTransformation.parameterGradient(standardSpaceDesignPoint_) * (physicalGradientMatrix * Point(1, 1.0));
  /* associate to each element of Set1 the gradient value */
  /* hasoferReliabilityIndexSensitivity is the collection Set1 + one other collection which is Set2 */
  const UnsignedInteger set1Size = set1.getSize();
  const UnsignedInteger size = set1Size + (isSet2Empty ? 0 : 1);
  hasoferReliabilityIndexSensitivity_ = Sensitivity(size);

  for (UnsignedInteger sensitivityIndex = 0; sensitivityIndex < set1Size; ++sensitivityIndex)
  {
    const PointWithDescription currentParameters(set1[sensitivityIndex]);
    const UnsignedInteger currentDimension = currentParameters.getDimension();
    PointWithDescription currentSensitivity(currentDimension);
    const Description currentDescription(currentParameters.getDescription());
    // the currentSensitivity gets the description and the name from set1
    currentSensitivity.setDescription(currentDescription);
    const String currentName(currentParameters.getName());
    currentSensitivity.setName(currentName);
    const Description isoDescription(setIso.getDescription());
    for (UnsignedInteger currentIndex = 0; currentIndex < currentDimension; ++currentIndex)
    {
      const UnsignedInteger position = computePosition(currentName, currentDescription[currentIndex], isoDescription);
      /* if currentParameters[currentIndex] is into setIso, then we get the sensitivity value in the corresponding isoProbabilisticGradient */
      if (position < setIso.getDimension()) currentSensitivity[currentIndex] = gradientToSensitivity * isoProbabilisticGradient[position];
      /* else the  sensitivity value is null */
    } // currentIndex
    hasoferReliabilityIndexSensitivity_[sensitivityIndex] = currentSensitivity;
  } // sensitivityIndex

  /* if set2 is not empty : convert the matrix isoProbabilisticGradient, which has n rows and 1 column, into a Point */
  if (size > set1Size)
  {
    hasoferReliabilityIndexSensitivity_[set1Size] = gradientToSensitivity * physicalGradient;
    hasoferReliabilityIndexSensitivity_[set1Size].setName(physicalGradient.getName());
  }
  isAlreadyComputedHasoferReliabilityIndexSensitivity_ = true;
} // end computeHasoferReliabilityIndexSensitivity()


/* Returns the position of the given (value, name) into the Point or the dimension of the Point if failed */
UnsignedInteger AnalyticalResult::computePosition(const String & marginalName, const String & marginalParameterName, const Description & parameterSetNames) const
{
  const UnsignedInteger dimension = parameterSetNames.getSize();
  const String fullName(OSS() << marginalName << "_" << marginalParameterName);
  for (UnsignedInteger index = 0; index < dimension; ++index) if ( parameterSetNames[index] == fullName ) return index;
  return dimension;
}


/* HasoferReliabilityIndexSensitivity Graph */
AnalyticalResult::GraphCollection AnalyticalResult::drawHasoferReliabilityIndexSensitivity(Scalar width) const
{
  GraphCollection hasoferReliabilityIndexSensitivityGraphCollection(0);
  // To ensure that the hasoferReliabilityIndexSensitivity_ are up to date
  if (! isAlreadyComputedHasoferReliabilityIndexSensitivity_) computeHasoferReliabilityIndexSensitivity();
  const UnsignedInteger dimension = standardSpaceDesignPoint_.getDimension();
  const UnsignedInteger size = hasoferReliabilityIndexSensitivity_.getSize();
  // The first graph shows the sensitivities with respect to the marginal parameters if there exist
  // in the cas where the distribution or some marginals are defined by user, it may not have parameters : we create a empty sensitivity graph
  Sensitivity marginalSensitivity(dimension);
  for(UnsignedInteger i = 0; i < dimension; ++i) marginalSensitivity[i] = hasoferReliabilityIndexSensitivity_[i];
  Graph hasoferReliabilityIndexSensitivityGraphMarginal(drawSensitivity(marginalSensitivity, width));
  OSS oss1;
  oss1 << "Hasofer Reliability Index Sensitivities - Marginal parameters - " << limitStateVariable_.getName();
  hasoferReliabilityIndexSensitivityGraphMarginal.setTitle(oss1);
  hasoferReliabilityIndexSensitivityGraphCollection.add(hasoferReliabilityIndexSensitivityGraphMarginal);
  // The second graph shows the sensitivities with respect to the other parameters if there exist
  if (size > dimension)
  {
    Sensitivity otherSensitivity(size - dimension);
    for(UnsignedInteger i = dimension; i < size; ++i) otherSensitivity[i - dimension] = hasoferReliabilityIndexSensitivity_[i];
    Graph hasoferReliabilityIndexSensitivityGraphOther(drawSensitivity(otherSensitivity, width));
    OSS oss2;
    oss2 << "Hasofer Reliability Index Sensitivities - Other parameters - " << limitStateVariable_.getName();
    hasoferReliabilityIndexSensitivityGraphOther.setTitle(oss2);
    hasoferReliabilityIndexSensitivityGraphCollection.add(hasoferReliabilityIndexSensitivityGraphOther);
  }
  return hasoferReliabilityIndexSensitivityGraphCollection;
}

/* Sensitivity Graph */
Graph AnalyticalResult::drawSensitivity(const Sensitivity & sensitivity,
                                        const Scalar width) const
{
  // shift position of the barplots
  Scalar shift = 0.0;

  // Create an empty graph
  Graph sensitivityGraph("Sensitivity", "parameters", "sensitivities", true, "topright");

  BarPlot sensitivityBarPlot(Sample(0, 2), shift, "");

  // Create the barplots
  const UnsignedInteger sensitivitySize = sensitivity.getSize();
  for (UnsignedInteger collectionIndex = 0; collectionIndex < sensitivitySize; ++collectionIndex)
  {
    Sample data(sensitivity[collectionIndex].getDimension(), 2);
    const UnsignedInteger dataSize = data.getSize();
    for (UnsignedInteger sensitivityIndex = 0; sensitivityIndex < dataSize; ++sensitivityIndex)
    {
      data(sensitivityIndex, 0) = width;
      data(sensitivityIndex, 1) = sensitivity[collectionIndex][sensitivityIndex];
    }
    // Add the barplot to the graph
    OSS oss;
    oss << sensitivity[collectionIndex].getName() << " " << sensitivity[collectionIndex].getDescription();
    BarPlot barPlot(data, shift, oss);
    barPlot.setFillStyle("solid");
    barPlot.setLineStyle("solid");
    sensitivityGraph.add(barPlot);
    shift += width * (1 + dataSize);
  }
  return sensitivityGraph;
} //drawSensitivity


/*  HasoferReliabilityIndexSensitivity accessor */
AnalyticalResult::Sensitivity AnalyticalResult::getHasoferReliabilityIndexSensitivity() const
{
  if (! isAlreadyComputedHasoferReliabilityIndexSensitivity_) computeHasoferReliabilityIndexSensitivity();
  return hasoferReliabilityIndexSensitivity_;
}

/* Optimization result accessors */
OptimizationResult AnalyticalResult::getOptimizationResult() const
{
  return optimizationResult_;
}

void AnalyticalResult::setOptimizationResult(const OptimizationResult & optimizationResult)
{
  optimizationResult_ = optimizationResult;
}

/* String converter */
String AnalyticalResult::__repr__() const
{
  OSS oss;
  oss << "class=" << AnalyticalResult::GetClassName()
      << " standardSpaceDesignPoint=" << standardSpaceDesignPoint_
      << " physicalSpaceDesignPoint=" << physicalSpaceDesignPoint_
      << " limitStateVariable=" << limitStateVariable_
      << " isStandardPointOriginInFailureSpace=" << isStandardPointOriginInFailureSpace_
      << " hasoferReliabilityIndex=" << hasoferReliabilityIndex_
      << " importanceFactors=" << importanceFactors_
      << " classicalImportanceFactors=" << classicalImportanceFactors_
      << " hasoferReliabilityIndexSensitivity=" << hasoferReliabilityIndexSensitivity_
      ;//FIXME:<< " optimizationResult=" << optimizationResult_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void AnalyticalResult::save(Advocate & adv) const
{
  PersistentSensitivity sensitivity(hasoferReliabilityIndexSensitivity_ );
  PersistentObject::save(adv);
  adv.saveAttribute( "standardSpaceDesignPoint_", standardSpaceDesignPoint_ );
  adv.saveAttribute( "physicalSpaceDesignPoint_", physicalSpaceDesignPoint_ );
  adv.saveAttribute( "limitStateVariable_", limitStateVariable_ );
  adv.saveAttribute( "isStandardPointOriginInFailureSpace_", isStandardPointOriginInFailureSpace_ );
  adv.saveAttribute( "hasoferReliabilityIndex_", hasoferReliabilityIndex_ );
  adv.saveAttribute( "importanceFactors_", importanceFactors_ );
  adv.saveAttribute( "classicalImportanceFactors_", classicalImportanceFactors_ );
  adv.saveAttribute( "physicalImportanceFactors_", physicalImportanceFactors_ );
  adv.saveAttribute( "hasoferReliabilityIndexSensitivity_", sensitivity );
  adv.saveAttribute( "meanPointInStandardEventDomain_", meanPointInStandardEventDomain_);
  adv.saveAttribute( "isAlreadyComputedImportanceFactors_", isAlreadyComputedImportanceFactors_ );
  adv.saveAttribute( "isAlreadyComputedClassicalImportanceFactors_", isAlreadyComputedClassicalImportanceFactors_ );
  adv.saveAttribute( "isAlreadyComputedPhysicalImportanceFactors_", isAlreadyComputedPhysicalImportanceFactors_ );
  adv.saveAttribute( "isAlreadyComputedHasoferReliabilityIndexSensitivity_", isAlreadyComputedHasoferReliabilityIndexSensitivity_ );
  adv.saveAttribute( "optimizationResult_", optimizationResult_ );
}

/* Method load() reloads the object from the StorageManager */
void AnalyticalResult::load(Advocate & adv)
{
  PersistentSensitivity sensitivity;
  PersistentObject::load(adv);
  adv.loadAttribute( "standardSpaceDesignPoint_", standardSpaceDesignPoint_ );
  adv.loadAttribute( "physicalSpaceDesignPoint_", physicalSpaceDesignPoint_ );
  adv.loadAttribute( "limitStateVariable_", limitStateVariable_ );
  adv.loadAttribute( "isStandardPointOriginInFailureSpace_", isStandardPointOriginInFailureSpace_ );
  adv.loadAttribute( "hasoferReliabilityIndex_", hasoferReliabilityIndex_ );
  adv.loadAttribute( "importanceFactors_", importanceFactors_ );
  adv.loadAttribute( "classicalImportanceFactors_", classicalImportanceFactors_ );
  adv.loadAttribute( "hasoferReliabilityIndexSensitivity_", sensitivity );
  adv.loadAttribute( "meanPointInStandardEventDomain_", meanPointInStandardEventDomain_);
  adv.loadAttribute( "isAlreadyComputedImportanceFactors_", isAlreadyComputedImportanceFactors_ );
  adv.loadAttribute( "isAlreadyComputedClassicalImportanceFactors_", isAlreadyComputedClassicalImportanceFactors_ );
  adv.loadAttribute( "isAlreadyComputedHasoferReliabilityIndexSensitivity_", isAlreadyComputedHasoferReliabilityIndexSensitivity_ );
  adv.loadAttribute( "optimizationResult_", optimizationResult_ );
  hasoferReliabilityIndexSensitivity_ = sensitivity;
}

END_NAMESPACE_OPENTURNS
