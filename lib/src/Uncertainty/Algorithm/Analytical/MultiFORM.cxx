//                                               -*- C++ -*-
/**
 *  @brief Approximation with multiple design points based on FORM
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
#include "openturns/MultiFORM.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearCombinationFunction.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/Normal.hxx"
#include "openturns/NearestPointProblem.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MultiFORM);

static Factory<MultiFORM> Factory_MultiFORM;

/* Default constructor */
MultiFORM::MultiFORM()
  : FORM()
  , maximumDesignPointsNumber_(ResourceMap::GetAsUnsignedInteger("MultiFORM-DefaultMaximumDesignPointsNumber"))
{
  // Nothing to do
}

/*
  * @brief Standard constructor: the class is defined by an optimization algorithm, a failure event and a physical starting point
  */
MultiFORM::MultiFORM(const OptimizationAlgorithm & solver,
                     const RandomVector & event,
                     const Point & physicalStartingPoint)
  : FORM(solver, event, physicalStartingPoint)
  , maximumDesignPointsNumber_(ResourceMap::GetAsUnsignedInteger("MultiFORM-DefaultMaximumDesignPointsNumber"))
{
  // Nothing to do
}

/* Virtual constructor */
MultiFORM * MultiFORM::clone() const
{
  return new MultiFORM(*this);
}

/* Result accessor */
MultiFORMResult MultiFORM::getResult() const
{
  return result_;
}


/* String converter */
String MultiFORM::__repr__() const
{
  OSS oss;
  oss << "class=" << MultiFORM::GetClassName()
      << " derived from" << FORM::__repr__()
      << " result=" << result_;
  return oss;
}


/* Function that computes the design point by re-using the Analytical::run() and creates a MultiFORMResult */
void MultiFORM::run()
{
  const Scalar gamma = ResourceMap::GetAsScalar("MultiFORM-DefaultGamma");
  const Scalar delta = ResourceMap::GetAsScalar("MultiFORM-DefaultDelta");
  const Scalar gamma2 = gamma * gamma;
  const Scalar delta2 = delta * delta;
  const UnsignedInteger dimension = getEvent().getImplementation()->getAntecedent().getDimension();
  const Scalar bound = std::acos(1.0 - 0.5 * gamma2);

  Collection<Point> designPointCollection;
  Point betaCollection;
  UnsignedInteger numberOfDesignPointsFound = 0;
  const StandardEvent standardEvent(getEvent());

  // initialize the new limit state function
  Point coefficients(1, 1.0);
  Collection<Function> collection(1, standardEvent.getImplementation()->getFunction());

  Collection<FORMResult> formResultCollection;
  Bool designPointValidity = true;
  while (designPointValidity)
  {
    OptimizationAlgorithm solver(getNearestPointAlgorithm());
    const LinearCombinationFunction levelFunction(collection, coefficients);
    NearestPointProblem optimizationProblem(levelFunction, standardEvent.getThreshold());
    solver.setProblem(optimizationProblem);
    setNearestPointAlgorithm(solver);
    try
    {
      FORM::run();
    }
    catch (Exception & ex)
    {
      Log::Warn(String("MultiFORM: ") + ex.what());
      break; // then exit while-loop
    }
    const FORMResult formResult(FORM::getResult());
    const Point standardSpaceDesignPoint(formResult.getStandardSpaceDesignPoint());

    // retrieve some results
    const Scalar beta = formResult.getHasoferReliabilityIndex();
    designPointCollection.add(standardSpaceDesignPoint);
    betaCollection.add(beta);

    for (UnsignedInteger i = 0; i < numberOfDesignPointsFound; ++ i)
    {
      for (UnsignedInteger j = i + 1; j <= numberOfDesignPointsFound; ++ j)
      {
        const Scalar v = designPointCollection[i].dot(designPointCollection[j]);
        designPointValidity = designPointValidity && (std::acos(v / (betaCollection[i] * betaCollection[j])) > bound);
      }
    }

    if (designPointValidity)
    {
      ++ numberOfDesignPointsFound;
      // the symmetric of the design point is the next starting point
      setPhysicalStartingPoint(getEvent().getImplementation()->getAntecedent().getDistribution().getInverseIsoProbabilisticTransformation().operator()(formResult.getStandardSpaceDesignPoint() * -1.0));
      formResultCollection.add(formResult);
    }

    // check if we exceeded maximumDesignPointsNumber
    if (numberOfDesignPointsFound >= maximumDesignPointsNumber_)
      break;

    // bulge parameters
    const Scalar beta2 = beta * beta;
    const Scalar radius = gamma * beta;
    const Scalar radius2 = radius * radius;
    const Matrix gradient(standardEvent.getImplementation()->getFunction().gradient(standardSpaceDesignPoint));
    const Scalar gradientNorm = std::sqrt((gradient.transpose() * gradient)(0, 0));
    Scalar height = delta * beta * gradientNorm / ((gamma2 - delta2) * (gamma2 - delta2) * beta2 * beta2);
    height *= radius2; //NOTE: cause' our bulge is normalized (max height=1.0)

    // create the bulge function: b(u) = h*(1-||u-u*||/r^2)^2
    const Description inputDescription(Description::BuildDefault(dimension, "u"));
    OSS formula;
    formula << height << " * (1.0 - sqrt(";
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      formula << "(" << inputDescription[j] << " - " << standardSpaceDesignPoint[j] << ")^2";
      if (j < dimension - 1)
        formula << " + ";
    }
    formula << ") / " << radius2 << ")^2";
    const SymbolicFunction bulge(Description::BuildDefault(dimension, "u"), Description(1, formula));

    // add the bulge function to the level function
    coefficients.add(1.0);
    collection.add(bulge);

    // print some results
    LOGINFO(OSS() << "SystemFORM: u*=" << standardSpaceDesignPoint << " beta=" << formResult.getGeneralisedReliabilityIndex());
  }

  result_ = MultiFORMResult(formResultCollection);
  if (numberOfDesignPointsFound > 1)
  {
    betaCollection = Point(numberOfDesignPointsFound);// only the accepted candidates
    Collection<Point> directionCosines(numberOfDesignPointsFound);
    for (UnsignedInteger i = 0; i < numberOfDesignPointsFound; ++ i)
    {
      StandardEvent standardEventI(formResultCollection[i].getLimitStateVariable());
      //   to take in consideration the sens of the limitStatefunction we look at the comparaisonOperator
      const Scalar sign = standardEventI.getOperator()(-1.0, 1.0) ? 1.0 : -1.0;
      const Point standardSpaceDesignPoint(formResultCollection[i].getStandardSpaceDesignPoint());
      const Point currentStandardGradient(sign * (standardEventI.getImplementation()->getFunction().gradient(standardSpaceDesignPoint) * Point(1, 1.0)));
      directionCosines[i] = 1.0 / currentStandardGradient.norm() * currentStandardGradient;
      betaCollection[i] = formResultCollection[i].getHasoferReliabilityIndex();
    }
    CovarianceMatrix cov(numberOfDesignPointsFound);
    for (UnsignedInteger i = 0; i < numberOfDesignPointsFound; ++ i)
      for (UnsignedInteger j = 0; j < i; ++ j)
        cov(i, j) = directionCosines[i].dot(directionCosines[j]);

    Scalar eventProbability = Normal(Point(numberOfDesignPointsFound), cov).computeComplementaryCDF(betaCollection);
    result_.setEventProbability(eventProbability);
  }
}


/* Method save() stores the object through the StorageManager */
void MultiFORM::save(Advocate & adv) const
{
  FORM::save(adv);
  adv.saveAttribute("maximumDesignPointsNumber_", maximumDesignPointsNumber_);
}


/* Method load() reloads the object from the StorageManager */
void MultiFORM::load(Advocate & adv)
{
  FORM::load(adv);
  adv.loadAttribute("maximumDesignPointsNumber_", maximumDesignPointsNumber_);
}


/* Number of design points accessor */
void MultiFORM::setMaximumDesignPointsNumber(const UnsignedInteger maximumDesignPointsNumber)
{
  maximumDesignPointsNumber_ = maximumDesignPointsNumber;
}

UnsignedInteger MultiFORM::getMaximumDesignPointsNumber() const
{
  return maximumDesignPointsNumber_;
}

void MultiFORM::setMaximumNumberOfDesignPoints(const UnsignedInteger maximumDesignPointsNumber)
{
  LOGWARN(OSS() << "MultiFORM.setMaximumNumberOfDesignPoints is deprecated, use setMaximumDesignPointsNumber");
  setMaximumDesignPointsNumber(maximumDesignPointsNumber);
}

UnsignedInteger MultiFORM::getMaximumNumberOfDesignPoints() const
{
  LOGWARN(OSS() << "MultiFORM.getMaximumNumberOfDesignPoints is deprecated, use getMaximumDesignPointsNumber");
  return getMaximumDesignPointsNumber();
}


END_NAMESPACE_OPENTURNS
