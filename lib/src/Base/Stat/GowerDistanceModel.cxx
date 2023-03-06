//                                               -*- C++ -*-
/**
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
#include "openturns/GowerDistanceModel.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Polygon.hxx"
#include "openturns/Text.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(GowerDistanceModel)

static const Factory<GowerDistanceModel> Factory_GowerDistanceModel;


/* Default constructor */
GowerDistanceModel::GowerDistanceModel(const UnsignedInteger nLevels)
  : CovarianceModelImplementation(Point(1, 1.0), Point(1, 1.0))
  , nLevels_(nLevels)
  , latCovMat_(nLevels_)
{
  if (nLevels_ < 2) throw InvalidArgumentException(HERE) << "Error: the number of discrete levels must be >= 2";
  updateLatentCovarianceMatrix();
}

/* Parameters constructor */
GowerDistanceModel::GowerDistanceModel(const UnsignedInteger nLevels, const Point & scale, const Point & amplitude)
  : CovarianceModelImplementation(scale, amplitude)
  , nLevels_(nLevels)
  , latCovMat_(nLevels_)
{
  if (nLevels_ < 2) throw InvalidArgumentException(HERE) << "Error: the number of discrete levels must be >= 2";
  updateLatentCovarianceMatrix();
}

/* Virtual constructor */
GowerDistanceModel * GowerDistanceModel::clone() const
{
  return new GowerDistanceModel(*this);
}

/* Computation of the covariance  function */
Scalar GowerDistanceModel::computeAsScalar(const Scalar z1, const Scalar z2) const
{
  /*check z being one of the levels*/
  bool isLevelz1 = false;
  bool isLevelz2 = false;

  for (UnsignedInteger i = 0; i < nLevels_; ++i)
  {
    if (z1 == i) 
    {
      isLevelz1 = true;
      break;
    }
  }

  for (UnsignedInteger i = 0; i < nLevels_; ++i)
  {
    if (z2 == i)
    {
      isLevelz2 = true;
      break;
    }
  }

  if (!isLevelz1 || !isLevelz2) throw InvalidArgumentException(HERE) << "Error: the input discrete variables values are not amongst the known levels";

  Scalar cov = latCovMat_(z1,z2);

  return cov;
}

Scalar GowerDistanceModel::computeAsScalar(const Point & z1, const Point & z2) const
{
  return computeAsScalar(z1[0],z2[0]);
}


Scalar GowerDistanceModel::computeAsScalar(const Collection<Scalar>::const_iterator & z1_begin,
    const Collection<Scalar>::const_iterator & z2_begin) const
{

  Collection<Scalar>::const_iterator z1_it = z1_begin;
  Collection<Scalar>::const_iterator z2_it = z2_begin;

  return computeAsScalar(*z1_it,*z2_it);
}

void GowerDistanceModel::setFullParameter(const Point & parameter)
{
  /*
    Care! To make the method not bogus, the size of parameter argument
    should be :
     - Size of scale : here 1
     - Size of amplitude : here 1
    CovarianceModelImplementation::setFullParameter checks that size is
    equal to the total number of parameters
  */
  // Check the size
  const UnsignedInteger totalSize = 2;
  if (!(parameter.getSize() == totalSize))
    throw InvalidArgumentException(HERE) << "In GowerDistanceModel::setFullParameter, points have incompatible size. Point size = " << parameter.getSize()
                                         << " whereas expected size = " << totalSize ;
  UnsignedInteger index = 0;
  // First set the scale parameter
  if (!(parameter[index] > 0.0))
    throw InvalidArgumentException(HERE) << "In GowerDistanceModel::setFullParameter, the component " << index << " of scale is non positive";
  scale_[0] = parameter[index];
  ++ index;
  
  // Second the amplitude parameter
  if (!(parameter[index] > 0.0))
    throw InvalidArgumentException(HERE) << "In GowerDistanceModel::setFullParameter, the component " << index << " of amplitude is non positive";
  amplitude_[0] = parameter[index];
  ++ index;

  updateLatentCovarianceMatrix();

}

/* String converter */
String GowerDistanceModel::__repr__() const
{
  OSS oss;
  oss << "class=" << GowerDistanceModel::GetClassName()
      << " scale=" << scale_
      << " amplitude=" << amplitude_;
  return oss;
}

/* String converter */
String GowerDistanceModel::__str__(const String & ) const
{
  OSS oss;
  oss << GowerDistanceModel::GetClassName()
      << "(scale=" << scale_.__str__()
      << ", amplitude=" << amplitude_.__str__()
      << ")";
  return oss;
}

void GowerDistanceModel::updateLatentCovarianceMatrix()
{
/* Setting the terms outside of the diagonal */
  Scalar outDiagCovariance = outputCovariance_(0, 0) * exp(-0.5 / scale_[0] / scale_[0]);
  for (UnsignedInteger i = 0; i < nLevels_; ++ i)
  {
    for (UnsignedInteger j = i+1; j < nLevels_; ++j)
    {
      latCovMat_(i,j) = outDiagCovariance;
    }
  }

/* Setting the diagonal terms */
  for (UnsignedInteger i = 0; i < nLevels_; ++ i) latCovMat_(i,i) = outputCovariance_(0, 0) * (1.0 + nuggetFactor_);
}


/* NLevels accessor */
UnsignedInteger GowerDistanceModel::getNLevels() const
{
  return nLevels_;
}

void GowerDistanceModel::setScale(const Point & scale)
{
  if (scale.getDimension() != inputDimension_) throw InvalidArgumentException(HERE) << "In GowerDistanceModel::setScale: the given scale has a dimension=" << scale.getDimension() << " different from the input dimension=" << inputDimension_;
  for (UnsignedInteger index = 0; index < inputDimension_; ++index)
    if (!(scale[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In GowerDistanceModel::setScale: the component " << index << " of scale is non positive" ;
  scale_ = scale;
  updateLatentCovarianceMatrix();
}

void GowerDistanceModel::setAmplitude(const Point & amplitude)
{
  if (amplitude.getDimension() != outputDimension_) throw InvalidArgumentException(HERE) << "In GowerDistanceModel::setAmplitude: the given amplitude has a dimension=" << amplitude.getDimension() << " different from the dimension=" << outputDimension_;
  for (UnsignedInteger index = 0; index < outputDimension_; ++index)
    if (!(amplitude[index] > 0.0))
      throw InvalidArgumentException(HERE) << "In GowerDistanceModel::setAmplitude, the component " << index << " of amplitude=" << amplitude << " is non positive" ;
  amplitude_ = amplitude;
  updateOutputCovariance();
  updateLatentCovarianceMatrix();
}

void GowerDistanceModel::setNuggetFactor(const Scalar nuggetFactor)
{
  if (!(nuggetFactor >= 0.0)) throw InvalidArgumentException(HERE) << "Error: the nugget factor=" << nuggetFactor << " is negative";
  nuggetFactor_ = nuggetFactor;
  updateLatentCovarianceMatrix();
}

/* Drawing method */
Graph GowerDistanceModel::draw(const UnsignedInteger rowIndex,
    const UnsignedInteger columnIndex,
    const Scalar /*zMin*/,
    const Scalar /*zMax*/,
    const UnsignedInteger pointNumber,
    const Bool /*asStationary*/,
    const Bool /*correlationFlag*/) const
{
 // This method relies on useless parameters, this needs to be changed ASAP
  if (inputDimension_ != 1) throw NotDefinedException(HERE) << "Error: can draw covariance models only if input dimension=1, here input dimension=" << inputDimension_;
  if (!(rowIndex < outputDimension_)) throw InvalidArgumentException(HERE) << "Error: the given row index must be less than " << outputDimension_ << ", here rowIndex=" << rowIndex;
  if (!(columnIndex < outputDimension_)) throw InvalidArgumentException(HERE) << "Error: the given column index must be less than " << outputDimension_ << ", here columnIndex=" << columnIndex;
  if (!(pointNumber >= 2)) throw InvalidArgumentException(HERE) << "Error: cannot draw the model with pointNumber<2, here pointNumber=" << pointNumber;
  // Check if the model is stationary and if we want to draw it this way

  Graph graph("Covariance values", "x", "x", true, "topright");
  graph.setIntegerXTick(true);
  graph.setIntegerYTick(true);
  Drawable drawable = Drawable();
  Description palette = drawable.BuildDefaultPalette(2);
  Sample data = Sample(4,2);
  for (UnsignedInteger i = 0; i < nLevels_; ++i)
  {
    data(0,0) = i;
    data(0,1) = i;
    data(1,0) = i;
    data(1,1) = i+1;
    data(2,0) = i+1;
    data(2,1) = i+1;
    data(3,0) = i+1;
    data(3,1) = i;
    Polygon polygon = Polygon(data);
    polygon.setColor(palette[0]);
    graph.add(polygon);
    Collection<String> description;
	String string = "cov = " + std::to_string(latCovMat_(i,i));
    description.add(string);
    Point locPoint(2);
    locPoint[0] = i+0.5;
    locPoint[1] = i+0.5;
    Sample location = Sample(1, locPoint);
    Text text = Text(location, description);
    graph.add(text);
  }
  for (UnsignedInteger i = 0; i < nLevels_; ++i)
  {
    for (UnsignedInteger j = i+1; j < nLevels_; ++j)
    {
      data(0,0) = i;
      data(0,1) = j;
      data(1,0) = i;
      data(1,1) = j+1;
      data(2,0) = i+1;
      data(2,1) = j+1;
      data(3,0) = i+1;
      data(3,1) = j;
	  Polygon polygon = Polygon(data);
      polygon.setColor(palette[1]);
	  graph.add(polygon);
      Collection<String> description;
   	  String string = "cov = " + std::to_string(latCovMat_(i,j));
      description.add(string);
      Point locPoint(2);
      locPoint[0] = i+0.5;
      locPoint[1] = j+0.5;
      Sample location = Sample(1, locPoint);
	  Text text = Text(location, description);
      graph.add(text);
    }
  }
  return graph;
}

/* Method save() stores the object through the StorageManager */
void GowerDistanceModel::save(Advocate & adv) const
{
  CovarianceModelImplementation::save(adv);
  adv.saveAttribute("nLevels_", nLevels_);
}

/* Method load() reloads the object from the StorageManager */
void GowerDistanceModel::load(Advocate & adv)
{
  CovarianceModelImplementation::load(adv);
  adv.loadAttribute("nLevels_", nLevels_);
}

END_NAMESPACE_OPENTURNS
