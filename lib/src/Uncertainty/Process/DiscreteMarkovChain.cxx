//                                               -*- C++ -*-
/**
 *  @brief A class which implements a discrete Markov chain process
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

#include <iostream>
#include "openturns/DiscreteMarkovChain.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/Dirac.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/DistFunc.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DiscreteMarkovChain)

static const Factory<DiscreteMarkovChain> Factory_DiscreteMarkovChain;

/* Default constructor */
DiscreteMarkovChain::DiscreteMarkovChain()
  : ProcessImplementation()
  , origin_(Dirac(0))
  , transitionMatrix_(IdentityMatrix(1))
  , currentState_(0)
{
  // Set the dimension of the process
  setOutputDimension(1);
}

/* Standard constructors */
DiscreteMarkovChain::DiscreteMarkovChain(const Distribution & origin,
                       const SquareMatrix & transitionMatrix)
  : ProcessImplementation()
  , origin_(origin)
  , transitionMatrix_(transitionMatrix.transpose())
  , currentState_(0)
{
  // Set the dimension of the process
  setOutputDimension(1);
  
  // Set the transition matrix of the process
  setTransitionMatrix(transitionMatrix);
  
  // Set the origin of the process
  setOrigin(origin);
  
  // Set the currentState of the process
  currentState_ = origin.getRealization()[0];
}

DiscreteMarkovChain::DiscreteMarkovChain(const Distribution & origin,
                       const SquareMatrix & transitionMatrix,
                       const RegularGrid & timeGrid)
  : ProcessImplementation()
  , origin_(origin)
  , transitionMatrix_(transitionMatrix.transpose())
  , currentState_(0)
{
  // Set the dimension of the process
  setOutputDimension(1);
  
  // Set the transition matrix of the process
  setTransitionMatrix(transitionMatrix);
  
  // Set the origin of the process
  setOrigin(origin);
  
  // Set the currentState of the process
  currentState_ = origin.getRealization()[0];

  // Set the time grid of the process
  setTimeGrid(timeGrid);
}

/* Constructors with fixed origin */
DiscreteMarkovChain::DiscreteMarkovChain(const UnsignedInteger origin,
                       const SquareMatrix & transitionMatrix)
  : ProcessImplementation()
  , origin_(Dirac(origin))
  , transitionMatrix_(transitionMatrix.transpose())
  , currentState_(origin)
{
  // Set the dimension of the process
  setOutputDimension(1);
  
  // Set the transition matrix of the process
  setTransitionMatrix(transitionMatrix);
  
  // Set the origin of the process
  setOrigin(Dirac(origin));
  
  // Set the currentState of the process
  currentState_ = origin;
}

DiscreteMarkovChain::DiscreteMarkovChain(const UnsignedInteger origin,
                       const SquareMatrix & transitionMatrix,
                       const RegularGrid & timeGrid)
  : ProcessImplementation()
  , origin_(Dirac(origin))
  , transitionMatrix_(transitionMatrix.transpose())
  , currentState_(origin)
{
  // Set the dimension of the process
  setOutputDimension(1);
  
  // Set the transition matrix of the process
  setTransitionMatrix(transitionMatrix);
  
  // Set the origin of the process
  setOrigin(Dirac(origin));

  // Set the time grid of the process
  setTimeGrid(timeGrid);
}

/* Virtual constructor */
DiscreteMarkovChain * DiscreteMarkovChain::clone() const
{
  return new DiscreteMarkovChain(*this);
}

/* String converters */
String DiscreteMarkovChain::__repr__() const
{
  OSS oss;
  oss << "class=" << DiscreteMarkovChain::GetClassName()
      << ", origin=" << origin_
      << ", transition matrix=" << transitionMatrix_.transpose()
      << ", time grid=" << getTimeGrid();
  return oss;
}

String DiscreteMarkovChain::__str__(const String & offset) const
{
  OSS oss;
  oss << "DiscreteMarkovChain(origin=" << origin_.__str__(offset)
      << ", transition=" << PersistentCollection<Scalar>(*transitionMatrix_.transpose().getImplementation()).__str__(offset) 
      << ")";
  return oss;
}

/* Realization accessor */
Field DiscreteMarkovChain::getRealization() const
{
  const UnsignedInteger size = mesh_.getVerticesNumber();
  const UnsignedInteger dimension = transitionMatrix_.getNbColumns();

  /* Loop on mesh points */
  Sample data(size, 1);
  data(0, 0) = origin_.getRealization()[0];
  currentState_ = data(0, 0) ;
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    /* Definition of the weights */
    Point weights(dimension, 0.0);
    std::copy(&transitionMatrix_(0,currentState_), &transitionMatrix_(0,currentState_)+dimension, weights.begin());
        
    /* Get a realization of the distribution */
    data(i, 0) = DistFunc::rDiscrete(weights);
    currentState_ = data(i, 0);
  }
  return Field(mesh_, data);
}

/* Compute the next steps of a Markov chain */
TimeSeries DiscreteMarkovChain::getFuture(const UnsignedInteger stepNumber) const
{
  // TimeGrid of the process
  RegularGrid timeGrid(getTimeGrid());

  if (stepNumber == 0) throw InvalidArgumentException(HERE) << "Error: the number of future steps must be positive.";
  
  // TimeGrid associated with the possible future
  const Scalar timeStep = RegularGrid(mesh_).getStep();
  const RegularGrid futurTimeGrid(RegularGrid(mesh_).getEnd(), timeStep, stepNumber);
  const UnsignedInteger size = futurTimeGrid.getVerticesNumber();

  // Loop on mesh points
  const UnsignedInteger dimension = transitionMatrix_.getNbColumns();
  UnsignedInteger previous = currentState_;
  
  Sample data(size, 1);
  data(0, 0) = previous;
  for (UnsignedInteger i = 1; i < size; ++i)
  {
    // Definition of the distribution
    Point weights(dimension, 0.0);
    std::copy(&transitionMatrix_(0,previous), &transitionMatrix_(0,previous)+dimension, weights.begin());
    
    /* Get a realization of the distribution */
    data(i, 0) = DistFunc::rDiscrete(weights);
    previous = data(i, 0);
  }
  return TimeSeries(futurTimeGrid, data);
}

/* Transition matrix accessors */
SquareMatrix DiscreteMarkovChain::getTransitionMatrix() const
{
  // For memory access purposes, transition matrix is stored transposed
  return transitionMatrix_.transpose();
}

void DiscreteMarkovChain::setTransitionMatrix(const SquareMatrix & transitionMatrix)
{ 
  // For memory access purposes, transition matrix is stored transposed
  const SquareMatrix transitionTransposed(transitionMatrix.transpose());
    
  // Check transition matrix dimension consistency with current state
  if (transitionTransposed.getNbColumns() <= currentState_) 
  {
    LOGWARN(OSS()<< "Warning: the given transition matrix has a dimension = " << transitionTransposed.getNbColumns() << " incompatible with the current state = " << currentState_<<". Current state is set to 0.");
    currentState_ = 0;
  }
    
  // Check transition matrix dimension consistency with origin
  if (transitionTransposed.getNbColumns() <= origin_.getRange().getUpperBound()[0]) 
  {
    LOGWARN(OSS()<< "Warning: the given transition matrix has a dimension = " << transitionTransposed.getNbColumns() << " incompatible with the origin distribution. Origin distribution is set to Dirac(0).");
    setOrigin(0);
    }
    
    // Check if transition matrix is stochastic
    const Point ones(transitionTransposed.getNbColumns(), 1.0);
    const Point sums(transitionMatrix * ones);
    for (UnsignedInteger i = 0; i < transitionMatrix.getNbRows(); ++i)
    {
        if (std::abs(sums[i] - 1.0) > ResourceMap::GetAsScalar("DiscreteMarkovChain-ProbabilitySumPrecision")) throw InvalidArgumentException(HERE) << "Error: the given transition matrix is not stochastic. Sum of line " << i+1 << " is equal to " << sums[i];
    }
    
    // Set transitionMatrix_ attribute
    transitionMatrix_ = transitionTransposed;
}

/* Origin accessors */
Distribution DiscreteMarkovChain::getOrigin() const
{
  return origin_;
}

void DiscreteMarkovChain::setOrigin(const Distribution & origin)
{
  // Check if distribution is 1D
  if (origin.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given origin distribution must be 1D.";
  
  // Check integral
  if (!origin.isIntegral()) throw InvalidArgumentException(HERE) << "Error: the given origin distribution must be integer-valued.";
  
  // Check support consistence with transition matrix dimension
  const Interval support(origin.getRange());
  const Bool flb = support.getFiniteLowerBound()[0];
  const Scalar lb(support.getLowerBound()[0]);
  const Bool fub = support.getFiniteUpperBound()[0];
  const Scalar ub(support.getUpperBound()[0]);

  if (!flb || lb < 0.0) throw InvalidArgumentException(HERE) << "Error: the given origin distribution support must be positive.";
  if (!fub || ub >= transitionMatrix_.getNbRows()) throw InvalidArgumentException(HERE) << "Error: the given origin distribution support exceeds the number of states of the process = " << transitionMatrix_.getNbRows();

  origin_ = origin;
}
  

void DiscreteMarkovChain::setOrigin(const UnsignedInteger origin)
{
  if (origin >= transitionMatrix_.getNbRows()) throw InvalidArgumentException(HERE) << "Error: the given origin state exceeds the number of states of the process = " << transitionMatrix_.getNbRows();
  origin_ = Dirac(origin);
}

/* Mesh accessor */
void DiscreteMarkovChain::setMesh(const Mesh & mesh)
{
  if(!mesh.isRegular()) throw InvalidArgumentException(HERE) << "Error: the mesh must be regular.";
  ProcessImplementation::setMesh(mesh);
}

/* Method save() stores the object through the StorageManager */
void DiscreteMarkovChain::save(Advocate & adv) const
{
  ProcessImplementation::save(adv);
  adv.saveAttribute( "transitionMatrix", transitionMatrix_ );
  adv.saveAttribute( "origin_", origin_ );
  adv.saveAttribute( "currentState_", currentState_ );
}

/* Method load() reloads the object from the StorageManager */
void DiscreteMarkovChain::load(Advocate & adv)
{
  ProcessImplementation::load(adv);
  adv.loadAttribute( "transitionMatrix_", transitionMatrix_ );
  adv.loadAttribute( "origin_", origin_ );
  adv.loadAttribute( "currentState_", currentState_ );
}

END_NAMESPACE_OPENTURNS
