//                                               -*- C++ -*-
/**
 *  @brief LinearModel implements the linear model
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#include <fstream>
#include "openturns/LinearModel.hxx"
#include "openturns/Path.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LinearModel)

static const Factory<LinearModel> Factory_LinearModel;

/* Default constructor */
LinearModel::LinearModel()
{
  LOGWARN(OSS() << "LinearModel is deprecated");
}

/* Standard constructor */
LinearModel::LinearModel(const Point & vectorR,
                         const Interval & intervals,
                         const ScalarCollection & pValuesOfR)
  : PersistentObject(),
    regression_(vectorR),
    confidenceIntervals_(intervals),
    pValues_(pValuesOfR)
{
  LOGWARN(OSS() << "LinearModel is deprecated");
}

/* Constructor from Point */
LinearModel::LinearModel(const Point & vectorR)
  : PersistentObject()
  , regression_(vectorR)
  , confidenceIntervals_(vectorR, vectorR)
  , pValues_(vectorR.getDimension(), 0.0)
{
  LOGWARN(OSS() << "LinearModel is deprecated");
}

/* Virtual constructor */
LinearModel * LinearModel::clone() const
{
  return new LinearModel(*this);
}

/* String converter */
String LinearModel::__repr__() const
{
  return OSS(true) << "class=" << LinearModel::GetClassName()
         << " name=" << getName()
         << " regression=" << regression_
         << " confidence intervals=" << confidenceIntervals_
         << " p-Values=" << pValues_;
}

/* String converter */
String LinearModel::__str__(const String & ) const
{
  return OSS(false) << "LinearModel name=" << getName()
         << " regression=" << regression_
         << " confidence intervals=" << confidenceIntervals_
         << " p-Values=" << pValues_;
}

/* get vector, get intervals, get p-Values*/
Point LinearModel::getRegression() const
{
  return regression_;
}

Interval LinearModel::getConfidenceIntervals() const
{
  return confidenceIntervals_;
}

LinearModel::ScalarCollection LinearModel::getPValues() const
{
  return pValues_;
}

/* getPredicted : build an sample of predicted values */
Sample LinearModel::getPredicted(const Sample & predictor) const
{
  if (predictor.getDimension() + 1 != regression_.getDimension()) throw InvalidArgumentException(HERE) << "Error: predictors must have a dimension compatible with the linear model dimension";
  UnsignedInteger size = predictor.getSize();
  Sample predicted(size, 1);
  UnsignedInteger dimension = predictor.getDimension();
  Point linear(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) linear[i] = regression_[i + 1];
  for (UnsignedInteger i = 0; i < size; ++i) predicted(i, 0) = dot(linear, predictor[i]) + regression_[0];
  return predicted;
}

/* getResidual */
Sample LinearModel::getResidual(const Sample & predictor,
                                const Sample & measured) const
{
  if (predictor.getDimension() + 1 != regression_.getDimension()) throw InvalidArgumentException(HERE) << "Error: predictors must have a dimension compatible with the linear model dimension";
  UnsignedInteger size = predictor.getSize();
  if (measured.getSize() != size) throw InvalidArgumentException(HERE) << "Error: measured must have the same size as predictor";
  Sample residual(size, 1);
  UnsignedInteger dimension = predictor.getDimension();
  Point linear(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) linear[i] = regression_[i + 1];
  for (UnsignedInteger i = 0; i < size; ++i) residual(i, 0) = measured(i, 0) - dot(linear, predictor[i]) - regression_[0];
  return residual;
}

/* Method save() stores the object through the StorageManager */
void LinearModel::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "regression_", regression_);
  adv.saveAttribute( "confidenceIntervals_", confidenceIntervals_);
  adv.saveAttribute( "pValues_", pValues_);
}

/* Method load() reloads the object from the StorageManager */
void LinearModel::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "regression_", regression_);
  adv.loadAttribute( "confidenceIntervals_", confidenceIntervals_);
  adv.loadAttribute( "pValues_", pValues_);
}


/* Comparison operator */
Bool operator ==(const LinearModel & lhs, const LinearModel & rhs)
{
  return ( lhs.getRegression() == rhs.getRegression() && lhs.getConfidenceIntervals() == rhs.getConfidenceIntervals() && lhs.getPValues() == rhs.getPValues() );
}

END_NAMESPACE_OPENTURNS
