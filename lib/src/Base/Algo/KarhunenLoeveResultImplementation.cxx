//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition and by-products
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KarhunenLoeveResultImplementation.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Cloud.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/P1LagrangeEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KarhunenLoeveResultImplementation)

static const Factory<KarhunenLoeveResultImplementation> Factory_KarhunenLoeveResultImplementation;

/* Default constructor */
KarhunenLoeveResultImplementation::KarhunenLoeveResultImplementation()
  : PersistentObject()
  , covariance_()
  , threshold_(0.0)
  , eigenvalues_()
  , modes_()
  , modesAsProcessSample_()
  , projection_()
{
  // Nothing to do
}

/* Default constructor */
KarhunenLoeveResultImplementation::KarhunenLoeveResultImplementation(const CovarianceModel & covariance,
    const Scalar threshold,
    const Point & eigenvalues,
    const FunctionCollection & modes,
    const ProcessSample & modesAsProcessSample,
    const Matrix & projection)
  : PersistentObject()
  , covariance_(covariance)
  , threshold_(threshold)
  , eigenvalues_(eigenvalues)
  , modes_(modes)
  , modesAsProcessSample_(modesAsProcessSample)
  , projection_(projection)
{
  // Nothing to do
}

/* Virtual constructor */
KarhunenLoeveResultImplementation * KarhunenLoeveResultImplementation::clone() const
{
  return new KarhunenLoeveResultImplementation(*this);
}

/* Threshold accessor */
Scalar KarhunenLoeveResultImplementation::getThreshold() const
{
  return threshold_;
}

/* Covariance model accessor */
CovarianceModel KarhunenLoeveResultImplementation::getCovarianceModel() const
{
  return covariance_;
}

/* Eigenvalues accessor */
Point KarhunenLoeveResultImplementation::getEigenvalues() const
{
  return eigenvalues_;
}

Point KarhunenLoeveResultImplementation::getEigenValues() const
{
  LOGWARN(OSS() << "KarhunenLoeveResult::getEigenValues is deprecated, use getEigenvalues instead");
  return getEigenvalues();
}

Graph KarhunenLoeveResultImplementation::drawEigenvalues() const
{
  Graph graph("Karhunen-Loeve eigenvalues", "Index", "Eigenvalue", true, "topright");
  const UnsignedInteger K = eigenvalues_.getSize();
  Point indices(K);
  for (UnsignedInteger i = 0; i < K; ++ i)
    indices[i] = i;
  Cloud cloud(indices, eigenvalues_);
  graph.add(cloud);
  graph.setGrid(true);
  return graph;
}

Graph KarhunenLoeveResultImplementation::drawCumulatedEigenvaluesRemainder() const
{
  Graph graph("Karhunen-Loeve eigenvalues", "Index", "Cumulated eigenvalue normalized remainder", true, "topright");
  const UnsignedInteger K = eigenvalues_.getSize();
  Point indices(K);
  for (UnsignedInteger i = 0; i < K; ++ i)
    indices[i] = i;
  Point eigenCumSum(eigenvalues_);
  for (UnsignedInteger i = 1; i < K; ++ i)
    eigenCumSum[i] += eigenCumSum[i - 1];
  if (K > 1)
  {
    eigenCumSum /= eigenCumSum[K - 1];
    eigenCumSum[K - 1] = eigenCumSum[K - 2];// avoids log(0) in log scale
  }
  Cloud cloud(indices, Point(K, 1) - eigenCumSum);
  graph.add(cloud);
  graph.setGrid(true);
  return graph;
}

/* Modes accessors */
KarhunenLoeveResultImplementation::FunctionCollection KarhunenLoeveResultImplementation::getModes() const
{
  return modes_;
}

ProcessSample KarhunenLoeveResultImplementation::getModesAsProcessSample() const
{
  return modesAsProcessSample_;
}

/* Mesh accessor */
Mesh KarhunenLoeveResultImplementation::getMesh() const
{
  return modesAsProcessSample_.getMesh();
}

/* Scaled modes accessors */
KarhunenLoeveResultImplementation::FunctionCollection KarhunenLoeveResultImplementation::getScaledModes() const
{
  Collection<Function> scaledModes(modes_.getSize());
  if (modes_.getSize() == 0) return scaledModes;
  const UnsignedInteger dimension = modes_[0].getOutputDimension();
  const Point zero(dimension);
  const IdentityMatrix id(dimension);
  for (UnsignedInteger i = 0; i < scaledModes.getSize(); ++i)
  {
    const Function modeI(modes_[i]);
    LinearFunction scaling(zero, zero, id * std::sqrt(eigenvalues_[i]));
    scaledModes[i] = ComposedFunction(scaling, modeI);
  }
  return scaledModes;
}

ProcessSample KarhunenLoeveResultImplementation::getScaledModesAsProcessSample() const
{
  ProcessSample scaledModesAsProcessSample(modesAsProcessSample_.getMesh(), modesAsProcessSample_.getSize(), modesAsProcessSample_.getDimension());
  for (UnsignedInteger i = 0; i < modesAsProcessSample_.getSize(); ++i)
    scaledModesAsProcessSample[i] = modesAsProcessSample_[i] * std::sqrt(eigenvalues_[i]);
  return scaledModesAsProcessSample;
}

/* Projection matrix accessor */
Matrix KarhunenLoeveResultImplementation::getProjectionMatrix() const
{
  return projection_;
}

/* Projection method */
Sample KarhunenLoeveResultImplementation::project(const FunctionCollection & functionCollection) const
{
  const UnsignedInteger size = functionCollection.getSize();
  const Sample vertices(modesAsProcessSample_.getMesh().getVertices());
  Sample functionValues(size, projection_.getNbColumns());
  for(UnsignedInteger i = 0; i < size; ++i)
    functionValues[i] = (functionCollection[i](vertices)).getImplementation()->getData();
  return projection_.getImplementation()->genSampleProd(functionValues, true, false, 'R');
}

Point KarhunenLoeveResultImplementation::project(const Function & function) const
{
  // Evaluate the function over the vertices of the mesh and cast it into a Point
  const Point functionValues(function(modesAsProcessSample_.getMesh().getVertices()).getImplementation()->getData());
  return projection_ * functionValues;
}

Point KarhunenLoeveResultImplementation::project(const Sample & values) const
{
  if (values.getDimension() != modesAsProcessSample_.getDimension())
    throw InvalidDimensionException(HERE) << "Expected values of dimension " << modesAsProcessSample_.getDimension() << " got " << values.getDimension();
  return projection_ * values.getImplementation()->getData();
}

Sample KarhunenLoeveResultImplementation::project(const ProcessSample & sample) const
{
  if (sample.getDimension() != modesAsProcessSample_.getDimension())
    throw InvalidDimensionException(HERE) << "Expected values of dimension " << modesAsProcessSample_.getDimension() << " got " << sample.getDimension();
  const UnsignedInteger size = sample.getSize();
  if (!(size != 0)) return Sample();
  const Mesh mesh(modesAsProcessSample_.getMesh());
  const UnsignedInteger dimension = sample.getDimension();
  const UnsignedInteger length = mesh.getVerticesNumber();
  if (sample.getMesh() == mesh)
  {
    // result[i] = projection_ * sample.data_[i] if sample.data_ is viewed as a
    //   Sample(size, length * dimension) and result[i] as a Point of dimension rows(projection_)
    // This can be rewritten with matrix multiplication:
    //   transposed(result) = transposed(sample.data_) * transposed(projection_)
    // As result and sample.data_ are stored as Sample and not Matrix, we have
    //   result = sample.data_ * transposed(projection_)
    Sample values(size, length * dimension);
    for(UnsignedInteger i = 0; i < size; ++i)
      std::copy(&sample[i](0, 0), &sample[i](0, 0) + length * dimension, &values(i, 0));
    return projection_.getImplementation()->genSampleProd(values, true, false, 'R');
  }
  else
  {
    // We build a P1LagrangeEvaluation as if ProcessSample was an aggregated Field.
    P1LagrangeEvaluation evaluation(sample);
    // values is Sample(length, size * dimension)
    const Sample values(evaluation(mesh.getVertices()));
    // Dispatch values so that it can be multiplied by projection_ like above
    Sample dispatched(size, length * dimension);
    for(UnsignedInteger i = 0; i < size; ++i)
      for(UnsignedInteger j = 0; j < length; ++j)
        std::copy(&values(j, i * dimension), &values(j, i * dimension) + dimension, &dispatched(i, j * dimension));
    return projection_.getImplementation()->genSampleProd(dispatched, true, false, 'R');
  }
}

/* Lift method */
Function KarhunenLoeveResultImplementation::lift(const Point & coefficients) const
{
  const UnsignedInteger dimension = eigenvalues_.getDimension();
  if (coefficients.getDimension() != dimension)
    throw InvalidDimensionException(HERE) << "Expected coefficients of dimension " << dimension << " got " << coefficients.getDimension();
  Point scaledCoefficients(dimension);
  Collection<Function> functions(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    scaledCoefficients[i] = std::sqrt(eigenvalues_[i]) * coefficients[i];
    functions[i] = modes_[i];
  }
  return LinearCombinationFunction(functions, scaledCoefficients);
}

Sample KarhunenLoeveResultImplementation::liftAsSample(const Point & coefficients) const
{
  const UnsignedInteger dimension = eigenvalues_.getDimension();
  if (coefficients.getDimension() != dimension)
    throw InvalidDimensionException(HERE) << "Expected coefficients of dimension " << dimension << " got " << coefficients.getDimension();
  Sample values(modesAsProcessSample_.getMesh().getVerticesNumber(), modesAsProcessSample_.getDimension());
  for (UnsignedInteger i = 0; i < dimension; ++i)
    values += modesAsProcessSample_[i] * (std::sqrt(eigenvalues_[i]) * coefficients[i]);
  return values;
}

Field KarhunenLoeveResultImplementation::liftAsField(const Point & coefficients) const
{
  return Field(modesAsProcessSample_.getMesh(), liftAsSample(coefficients));
}

/* String converter */
String KarhunenLoeveResultImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " covariance model=" << covariance_
         << " threshold=" << threshold_
         << " eigenvalues=" << eigenvalues_
         << " modes=" << modes_
         << " modesAsProcessSample=" << modesAsProcessSample_
         << " projection=" << projection_;
}

String KarhunenLoeveResultImplementation::__str__(const String & ) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void KarhunenLoeveResultImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("covariance_", covariance_);
  adv.saveAttribute("threshold_", threshold_);
  adv.saveAttribute("eigenvalues_", eigenvalues_);
  adv.saveAttribute("modes_", modes_);
  adv.saveAttribute("modesAsProcessSample_", modesAsProcessSample_);
  adv.saveAttribute("projection_", projection_);
}


/* Method load() reloads the object from the StorageManager */
void KarhunenLoeveResultImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("covariance_", covariance_);
  adv.loadAttribute("threshold_", threshold_);
  adv.loadAttribute("eigenvalues_", eigenvalues_);
  adv.loadAttribute("modes_", modes_);
  adv.loadAttribute("modesAsProcessSample_", modesAsProcessSample_);
  adv.loadAttribute("projection_", projection_);
}


END_NAMESPACE_OPENTURNS

