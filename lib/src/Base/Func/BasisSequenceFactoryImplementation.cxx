//                                               -*- C++ -*-
/**
 *  @brief A factory for building BasisSequence objects
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include <limits>

#include "BasisSequenceFactoryImplementation.hxx"
#include "PersistentObjectFactory.hxx"
#include "Exception.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BasisSequenceFactoryImplementation);

static const Factory<BasisSequenceFactoryImplementation> RegisteredFactory;

/* Default constructor */
BasisSequenceFactoryImplementation::BasisSequenceFactoryImplementation(const Bool verbose)
  : PersistentObject()
  , verbose_(verbose)
  , maximumRelativeConvergence_(SpecFunc::MinNumericalScalar)
{
  // Nothing to do
}

/* Virtual constructor */
BasisSequenceFactoryImplementation * BasisSequenceFactoryImplementation::clone() const
{
  return new BasisSequenceFactoryImplementation(*this);
}

/* String converter */
String BasisSequenceFactoryImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " verbose=" << verbose_ ;
}


String BasisSequenceFactoryImplementation::__str__(const String & offset) const
{
  return OSS(false) << offset << __repr__();
}


/* Verbosity accessor */
void BasisSequenceFactoryImplementation::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}

Bool BasisSequenceFactoryImplementation::getVerbose() const
{
  return verbose_;
}

/* Method to create new BasisSequence objects */
BasisSequence BasisSequenceFactoryImplementation::build(const NumericalSample & x,
    const NumericalSample & y,
    const Basis & psi,
    const Indices & indices)
{
  const DesignProxy proxy(x, psi);
  return build(y, indices, proxy);
}

BasisSequence BasisSequenceFactoryImplementation::build(const NumericalSample & y,
    const Indices & indices,
    const DesignProxy & proxy)
{
  LeastSquaresMethod method(proxy, y, indices);
  return build(method);
}

BasisSequence BasisSequenceFactoryImplementation::build(LeastSquaresMethod & method)
{
//   BasisSequence basisSequence(method.buildCurrentBasis());
  BasisSequence basisSequence(method.getBasis());
  initialize();

  updateBasis(method);
  while ((addedPsi_k_ranks_.getSize() > 0) || (removedPsi_k_ranks_.getSize() > 0))
  {
    basisSequence.add(currentIndices_);
    updateBasis(method);
  }
  return basisSequence;
}

void BasisSequenceFactoryImplementation::initialize()
{
  currentIndices_.clear();
  addedPsi_k_ranks_.clear();
  conservedPsi_k_ranks_.clear();
  removedPsi_k_ranks_.clear();
}


void BasisSequenceFactoryImplementation::updateBasis(LeastSquaresMethod & method)
{
  throw NotYetImplementedException(HERE) << " in BasisSequenceFactoryImplementation::updateBasis";
}

/* Stopping criterion on the L1-norm of the coefficients accessor */
void BasisSequenceFactoryImplementation::setMaximumRelativeConvergence(const NumericalScalar maximumRelativeConvergence)
{
  maximumRelativeConvergence_ = maximumRelativeConvergence;
}

NumericalScalar BasisSequenceFactoryImplementation::getMaximumRelativeConvergence() const
{
  return maximumRelativeConvergence_;
}

/* Method save() stores the object through the StorageManager */
void BasisSequenceFactoryImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "verbose_", verbose_ );
  adv.saveAttribute( "maximumRelativeConvergence_", maximumRelativeConvergence_ );
}

/* Method load() reloads the object from the StorageManager */
void BasisSequenceFactoryImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "verbose_", verbose_ );
  adv.loadAttribute( "maximumRelativeConvergence_", maximumRelativeConvergence_ );
}


END_NAMESPACE_OPENTURNS
