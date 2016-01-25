//                                               -*- C++ -*-
/**
 *  @brief LinearModel implements the linear model
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <fstream>
#include "LinearModel.hxx"
#include "Path.hxx"
#include "ResourceMap.hxx"
#include "Exception.hxx"
#include "PersistentObjectFactory.hxx"
#include "Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

static const Factory<PersistentCollection<NumericalScalar> > RegisteredFactory2;


CLASSNAMEINIT(LinearModel);

static const Factory<LinearModel> RegisteredFactory;

/* Default constructor */
LinearModel::LinearModel()
{
  // Nothing to do
}

/* Standard constructor */
LinearModel::LinearModel(const NumericalPoint & vectorR,
                         const Interval & intervals,
                         const NumericalScalarCollection & pValuesOfR)
  : PersistentObject(),
    regression_(vectorR),
    confidenceIntervals_(intervals),
    pValues_(pValuesOfR)
{
  // Nothing to do
}

/* Constructor from NumericalPoint */
LinearModel::LinearModel(const NumericalPoint & vectorR)
  : PersistentObject()
  , regression_(vectorR)
  , confidenceIntervals_(vectorR, vectorR)
  , pValues_(vectorR.getDimension(), 0.0)
{
  // Nothing to do
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
String LinearModel::__str__(const String & offset) const
{
  return OSS(false) << "LinearModel name=" << getName()
         << " regression=" << regression_
         << " confidence intervals=" << confidenceIntervals_
         << " p-Values=" << pValues_;
}

/* get vector, get intervals, get p-Values*/
NumericalPoint LinearModel::getRegression() const
{
  return regression_;
}

Interval LinearModel::getConfidenceIntervals() const
{
  return confidenceIntervals_;
}

LinearModel::NumericalScalarCollection LinearModel::getPValues() const
{
  return pValues_;
}

/* getPredicted : build an sample of predicted values */
NumericalSample LinearModel::getPredicted(const NumericalSample & predictor) const
{
  if (predictor.getDimension() + 1 != regression_.getDimension()) throw InvalidArgumentException(HERE) << "Error: predictors must have a dimension compatible with the linear model dimension";
  UnsignedInteger size(predictor.getSize());
  NumericalSample predicted(size, 1);
  UnsignedInteger dimension(predictor.getDimension());
  NumericalPoint linear(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) linear[i] = regression_[i + 1];
  for (UnsignedInteger i = 0; i < size; ++i) predicted[i][0] = dot(linear, predictor[i]) + regression_[0];
  return predicted;
}

/* getResidual */
NumericalSample LinearModel::getResidual(const NumericalSample & predictor,
    const NumericalSample & measured) const
{
  if (predictor.getDimension() + 1 != regression_.getDimension()) throw InvalidArgumentException(HERE) << "Error: predictors must have a dimension compatible with the linear model dimension";
  UnsignedInteger size(predictor.getSize());
  if (measured.getSize() != size) throw InvalidArgumentException(HERE) << "Error: measured must have the same size as predictor";
  NumericalSample residual(size, 1);
  UnsignedInteger dimension(predictor.getDimension());
  NumericalPoint linear(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) linear[i] = regression_[i + 1];
  for (UnsignedInteger i = 0; i < size; ++i) residual[i][0] = measured[i][0] - dot(linear, predictor[i]) - regression_[0];
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
