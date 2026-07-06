//                                               -*- C++ -*-
/**
 *  @brief Approximation with multiple design points based on FORM
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Normal.hxx"
#include "openturns/NearestPointProblem.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/EvaluationImplementation.hxx"
#include "openturns/GradientImplementation.hxx"
#include "openturns/HessianImplementation.hxx"
#include "openturns/SymmetricTensor.hxx"
#include "openturns/Function.hxx"


BEGIN_NAMESPACE_OPENTURNS

namespace MultiFORMFunctions
{

class BulgeEvaluation : public EvaluationImplementation
{
public:
  BulgeEvaluation(const Point & u,
                  const Scalar r,
                  const Scalar h)
    : EvaluationImplementation()
    , u_(u)
    , h_(h)
    , rsq_(r * r)
  {}

  BulgeEvaluation * clone() const override
  {
    return new BulgeEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return u_.getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Point operator()(const Point & x) const override
  {
    const Point diff(x - u_);
    const Scalar d2 = diff.normSquare();
    if (d2 <= rsq_)
    {
      const Scalar v = rsq_ - d2;
      return Point(1, h_ * v * v);
    }
    return Point(1, 0.0);
  }

  String __repr__() const override
  {
    OSS oss;
    oss << "BulgeEvaluation";
    return oss;
  }

private:
  const Point u_;
  const Scalar h_;
  const Scalar rsq_;
};

class BulgeGradient : public GradientImplementation
{
public:
  BulgeGradient(const Point & u,
                const Scalar r,
                const Scalar h)
    : GradientImplementation()
    , u_(u)
    , h_(h)
    , rsq_(r * r)
  {}

  BulgeGradient * clone() const override
  {
    return new BulgeGradient(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return u_.getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  Matrix gradient(const Point & x) const override
  {
    const UnsignedInteger dimension = u_.getDimension();
    const Point diff(x - u_);
    const Scalar d2 = diff.normSquare();
    Matrix grad(dimension, 1);
    if (d2 <= rsq_)
    {
      const Scalar v = rsq_ - d2;
      const Scalar factor = -4.0 * h_ * v;
      for (UnsignedInteger i = 0; i < dimension; ++ i)
        grad(i, 0) = factor * diff[i];
    }
    return grad;
  }

  String __repr__() const override
  {
    OSS oss;
    oss << "BulgeGradient";
    return oss;
  }

private:
  const Point u_;
  const Scalar h_;
  const Scalar rsq_;
};

class BulgeHessian : public HessianImplementation
{
public:
  BulgeHessian(const Point & u,
               const Scalar r,
               const Scalar h)
    : HessianImplementation()
    , u_(u)
    , h_(h)
    , rsq_(r * r)
  {}

  BulgeHessian * clone() const override
  {
    return new BulgeHessian(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return u_.getDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

  SymmetricTensor hessian(const Point & x) const override
  {
    const UnsignedInteger dimension = u_.getDimension();
    const Point diff(x - u_);
    const Scalar d2 = diff.normSquare();
    SymmetricTensor hess(dimension, 1);
    if (d2 <= rsq_)
    {
      const Scalar v = rsq_ - d2;
      // hess_ij = 8 * h * diff_i * diff_j - 4 * h * v * delta_ij
      for (UnsignedInteger i = 0; i < dimension; ++i)
      {
        for (UnsignedInteger j = 0; j <= i; ++j)
        {
          Scalar value = 8.0 * h_ * diff[i] * diff[j];
          if (i == j) value -= 4.0 * h_ * v;
          hess(i, j, 0) = value;
        }
      }
    }
    return hess;
  }

  String __repr__() const override
  {
    OSS oss;
    oss << "BulgeHessian";
    return oss;
  }

private:
  const Point u_;
  const Scalar h_;
  const Scalar rsq_;
};

} // namespace MultiFORMFunctions

CLASSNAMEINIT(MultiFORM)

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
                     const RandomVector & event)
  : FORM(solver, event)
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
  return multiFORMResult_;
}

/* String converter */
String MultiFORM::__repr__() const
{
  OSS oss;
  oss << "class=" << MultiFORM::GetClassName()
      << " derived from" << FORM::__repr__()
      << " result=" << multiFORMResult_;
  return oss;
}

/* Function that computes the design point by reusing the Analytical::run() and creates a MultiFORMResult */
void MultiFORM::run()
{
  const Scalar gamma = ResourceMap::GetAsScalar("MultiFORM-DefaultGamma");
  const Scalar delta = ResourceMap::GetAsScalar("MultiFORM-DefaultDelta");
  const Scalar gamma2 = gamma * gamma;
  const Scalar delta2 = delta * delta;
  const Scalar bound = std::acos(1.0 - 0.5 * gamma2);

  Collection<Point> designPointCollection;
  Point betaCollection;
  UnsignedInteger designPointNumber = 0;
  const StandardEvent standardEvent(getEvent());

  // initialize the new limit state function
  Point coefficients(1, 1.0);
  Collection<Function> collection(1, standardEvent.getImplementation()->getFunction());

  Collection<FORMResult> formResultCollection;
  Bool designPointValidity = true;
  while (designPointValidity)
  {
    // Build solver with modified level function (original + bulges)
    OptimizationAlgorithm solver(getNearestPointAlgorithm());
    const LinearCombinationFunction levelFunction(collection, coefficients);
    NearestPointProblem optimizationProblem(levelFunction, standardEvent.getThreshold());
    solver.setProblem(optimizationProblem);

    // Transform starting point to standard space
    try
    {
      solver.setStartingPoint(getEvent().getImplementation()->getAntecedent().getDistribution().getIsoProbabilisticTransformation().operator()(solver.getStartingPoint()));
    }
    catch (const NotDefinedException &)
    {
      solver.setStartingSample(getEvent().getImplementation()->getAntecedent().getDistribution().getIsoProbabilisticTransformation().operator()(solver.getStartingSample()));
    }

    try
    {
      solver.run();
    }
    catch (const Exception & ex)
    {
      LOGWARN(OSS() << "MultiFORM: " << ex.what());
      break;
    }

    // Get standard space design point
    Point standardSpaceDesignPoint(solver.getResult().getOptimalPoint());
    standardSpaceDesignPoint.setName("Standard Space Design Point");

    // Check limit state constraint on the modified level function
    const Scalar constraintError = solver.getResult().getConstraintError();
    const Scalar limitStateTolerance = solver.getMaximumConstraintError();
    const Scalar toleranceFactor = ResourceMap::GetAsScalar("Analytical-LimitStateToleranceFactor");

    if (!(constraintError <= toleranceFactor * limitStateTolerance))
    {
      LOGWARN(OSS() << "MultiFORM: design point not on limit state, constraintError=" << constraintError);
      break;
    }

    // Build FORM result from the original event
    const Point origin(standardSpaceDesignPoint.getDimension(), 0.0);
    const Point value(standardEvent.getImplementation()->getFunction().operator()(origin));
    const Bool isOriginInFailure = standardEvent.getOperator().compare(value[0], standardEvent.getThreshold());

    FORMResult formResult(standardSpaceDesignPoint, getEvent(), isOriginInFailure);
    formResult.setOptimizationResult(solver.getResult());

    // retrieve some results
    const Scalar beta = formResult.getHasoferReliabilityIndex();
    designPointCollection.add(standardSpaceDesignPoint);
    betaCollection.add(beta);

    for (UnsignedInteger i = 0; i < designPointNumber; ++ i)
    {
      for (UnsignedInteger j = i + 1; j <= designPointNumber; ++ j)
      {
        if (betaCollection[i] > 0.0 && betaCollection[j] > 0.0)
        {
          const Scalar v = designPointCollection[i].dot(designPointCollection[j]);
          const Scalar rho = v / (betaCollection[i] * betaCollection[j]);
          const Scalar clampedRho = std::clamp(rho, -1.0, 1.0);
          designPointValidity = designPointValidity && (std::acos(clampedRho) > bound);
        }
      }
    }

    if (designPointValidity)
    {
      ++ designPointNumber;
      // the symmetric of the design point is the next starting point
      nearestPointAlgorithm_.setStartingPoint(getEvent().getImplementation()->getAntecedent().getDistribution().getInverseIsoProbabilisticTransformation().operator()(formResult.getStandardSpaceDesignPoint() * -1.0));
      formResultCollection.add(formResult);
    }

    // check if we exceeded maximumDesignPointsNumber
    if (designPointNumber >= maximumDesignPointsNumber_)
      break;

    // bulge parameters
    if (gamma2 <= delta2) break;
    const Scalar radius = gamma * beta;
    if (radius <= 0.0) break;
    const Matrix gradient(standardEvent.getImplementation()->getFunction().gradient(standardSpaceDesignPoint));
    const Scalar gradientNorm = std::sqrt(gradient.computeGram()(0, 0));
    // height at center of the bulge
    const Scalar height = (beta > 0.0) ? delta * gamma2 * gradientNorm / ((gamma2 - delta2) * (gamma2 - delta2) * beta) : 0.0;

    // create the bulge function: h * (r^2 - ||u-p||^2)^2 when ||u-p|| <= r, 0 elsewhere
    const Function bulge(MultiFORMFunctions::BulgeEvaluation(standardSpaceDesignPoint, radius, height).clone(),
                         MultiFORMFunctions::BulgeGradient(standardSpaceDesignPoint, radius, height).clone(),
                         MultiFORMFunctions::BulgeHessian(standardSpaceDesignPoint, radius, height).clone());

    // add the bulge function to the level function
    coefficients.add(1.0);
    collection.add(bulge);

    // print some results
    LOGINFO(OSS() << "MultiFORM: u*=" << standardSpaceDesignPoint << " beta=" << formResult.getGeneralisedReliabilityIndex());
  }

  multiFORMResult_ = MultiFORMResult(formResultCollection);
  if (designPointNumber > 1)
  {
    betaCollection = Point(designPointNumber);// only the accepted candidates
    Collection<Point> directionCosines(designPointNumber);
    for (UnsignedInteger i = 0; i < designPointNumber; ++ i)
    {
      StandardEvent standardEventI(formResultCollection[i].getLimitStateVariable());
      //   to take in consideration the sens of the limitStatefunction we look at the comparaisonOperator
      const Scalar sign = standardEventI.getOperator()(-1.0, 1.0) ? 1.0 : -1.0;
      const Point standardSpaceDesignPoint(formResultCollection[i].getStandardSpaceDesignPoint());
      const Point currentStandardGradient(sign * (standardEventI.getImplementation()->getFunction().gradient(standardSpaceDesignPoint) * Point(1, 1.0)));
      directionCosines[i] = 1.0 / currentStandardGradient.norm() * currentStandardGradient;
      betaCollection[i] = formResultCollection[i].getHasoferReliabilityIndex();
    }
    CovarianceMatrix cov(designPointNumber);
    for (UnsignedInteger i = 0; i < designPointNumber; ++ i)
      for (UnsignedInteger j = 0; j < i; ++ j)
        cov(i, j) = directionCosines[i].dot(directionCosines[j]);

    const Scalar eventProbability = Normal(Point(designPointNumber), cov).computeComplementaryCDF(betaCollection);
    multiFORMResult_.setEventProbability(eventProbability);
  }
}


/* Method save() stores the object through the StorageManager */
void MultiFORM::save(Advocate & adv) const
{
  FORM::save(adv);
  adv.saveAttribute("maximumDesignPointsNumber_", maximumDesignPointsNumber_);
  adv.saveAttribute("multiFORMResult_", multiFORMResult_);
}


/* Method load() reloads the object from the StorageManager */
void MultiFORM::load(Advocate & adv)
{
  FORM::load(adv);
  adv.loadAttribute("maximumDesignPointsNumber_", maximumDesignPointsNumber_);
  adv.loadAttribute("multiFORMResult_", multiFORMResult_);
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

END_NAMESPACE_OPENTURNS
