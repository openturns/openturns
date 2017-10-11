//                                               -*- C++ -*-
/**
 *  @brief Karhunen-Loeve decomposition and by-products
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/KarhunenLoeveResultImplementation.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/Point.hxx"
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
    const Basis & modes,
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
Point KarhunenLoeveResultImplementation::getEigenValues() const
{
  return eigenvalues_;
}

/* Modes accessors */
Basis KarhunenLoeveResultImplementation::getModes() const
{
  return modes_;
}

ProcessSample KarhunenLoeveResultImplementation::getModesAsProcessSample() const
{
  return modesAsProcessSample_;
}

/* Scaled modes accessors */
Basis KarhunenLoeveResultImplementation::getScaledModes() const
{
  Collection<Function> scaledModes(modes_.getSize());
  const UnsignedInteger dimension = modes_.getDimension();
  const Point zero(dimension);
  const IdentityMatrix id(dimension);
  for (UnsignedInteger i = 0; i < scaledModes.getSize(); ++i)
  {
    const Function modeI(modes_.build(i));
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
Point KarhunenLoeveResultImplementation::project(const Function & function) const
{
  // Evaluate the function over the vertices of the mesh and cast it into a Point
  const Point functionValues(function(modesAsProcessSample_.getMesh().getVertices()).getImplementation()->getData());
  return projection_ * functionValues;
}

Point KarhunenLoeveResultImplementation::project(const Field & field) const
{
  if (field.getMesh() == modesAsProcessSample_.getMesh())
    return projection_ * field.getValues().getImplementation()->getData();
  return project(Function(P1LagrangeEvaluation(field).clone()));
}

struct ProjectBasisPolicy
{
  const Basis & basis_;
  Sample & output_;
  const KarhunenLoeveResultImplementation & result_;

  ProjectBasisPolicy( const Basis & basis,
                      Sample & output,
                      const KarhunenLoeveResultImplementation & result)
    : basis_(basis)
    , output_(output)
    , result_(result)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++ i) output_[i] = result_.project(basis_.build(i));
  }

}; /* end struct ProjectBasisPolicy */

Sample KarhunenLoeveResultImplementation::project(const Basis & basis) const
{
  const UnsignedInteger size = basis.getSize();
  Sample result(size, projection_.getNbRows());
  const ProjectBasisPolicy policy( basis, result, *this );
  TBB::ParallelFor( 0, size, policy );
  return result;
}

struct ProjectSamplePolicy
{
  const ProcessSample & sample_;
  Sample & output_;
  const KarhunenLoeveResultImplementation & result_;

  ProjectSamplePolicy( const ProcessSample & sample,
                       Sample & output,
                       const KarhunenLoeveResultImplementation & result)
    : sample_(sample)
    , output_(output)
    , result_(result)
  {}

  inline void operator()( const TBB::BlockedRange<UnsignedInteger> & r ) const
  {
    for (UnsignedInteger i = r.begin(); i != r.end(); ++ i)
      output_[i] = result_.project(sample_.getField(i));
  }

}; /* end struct ProjectSamplePolicy */

Sample KarhunenLoeveResultImplementation::project(const ProcessSample & sample) const
{
  const UnsignedInteger size = sample.getSize();
  Sample result(size, projection_.getNbRows());
  const ProjectSamplePolicy policy( sample, result, *this );
  TBB::ParallelFor( 0, size, policy );
  return result;
}

/* Lift method */
Function KarhunenLoeveResultImplementation::lift(const Point & coefficients) const
{
  const UnsignedInteger dimension = eigenvalues_.getDimension();
  Point scaledCoefficients(dimension);
  Collection<Function> functions(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    scaledCoefficients[i] = std::sqrt(eigenvalues_[i]) * coefficients[i];
    functions[i] = modes_.build(i);
  }
  return LinearCombinationFunction(functions, scaledCoefficients);
}

Field KarhunenLoeveResultImplementation::liftAsField(const Point & coefficients) const
{
  const UnsignedInteger dimension = eigenvalues_.getDimension();
  Sample values(modesAsProcessSample_.getMesh().getVerticesNumber(), modesAsProcessSample_.getDimension());
  for (UnsignedInteger i = 0; i < dimension; ++i)
    values += modesAsProcessSample_[i] * (std::sqrt(eigenvalues_[i]) * coefficients[i]);
  return Field(modesAsProcessSample_.getMesh(), values);
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

String KarhunenLoeveResultImplementation::__str__(const String & offset) const
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

