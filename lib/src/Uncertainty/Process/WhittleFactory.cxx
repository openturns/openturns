//                                               -*- C++ -*-
/**
 *  @brief An interface for all implementation class of process
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/WhittleFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ARMACoefficients.hxx"
#include "openturns/UserDefinedSpectralModel.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"
#include "openturns/Normal.hxx"
#include "openturns/UniVariatePolynomial.hxx"
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/Cobyla.hxx"
#include "openturns/MethodBoundEvaluation.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<WhittleFactoryState>)
static const Factory<PersistentCollection<WhittleFactoryState> > Factory_PersistentCollection_WhittleFactoryState;

CLASSNAMEINIT(WhittleFactory)

static const Factory<WhittleFactory> Factory_WhittleFactory;


/* Default constructor */
WhittleFactory::WhittleFactory()
  : ARMAFactoryImplementation()
  , spectralFactory_(WelchFactory())
  , normalizedFrequencies_(0)
  , timeGrid_()
  , m_(0)
  , spectralDensity_()
  , sigma2_(0.0)
  , isHistoryEnabled_(true)
  , history_(0)
  , startingPoints_(0)
  , solver_(new Cobyla())
{
  // Create the optimization solver parameters using the parameters in the ResourceMap
  initializeCobylaSolverParameter();
}

/* Standard constructor */
WhittleFactory::WhittleFactory(const UnsignedInteger p,
                               const UnsignedInteger q,
                               const Bool invertible)
  : ARMAFactoryImplementation(p, q, invertible)
  , spectralFactory_(WelchFactory())
  , normalizedFrequencies_(0)
  , timeGrid_()
  , m_(0)
  , spectralDensity_()
  , sigma2_(0.0)
  , isHistoryEnabled_(true)
  , history_(0)
  , startingPoints_(0)
  , solver_(new Cobyla())
{
  // Create the set of starting points using the parameters in the ResourceMap
  initializeStartingPoints();
  // Create the optimization solver parameters using the parameters in the ResourceMap
  initializeCobylaSolverParameter();
}

/* Standard constructor */
WhittleFactory::WhittleFactory(const Indices & p,
                               const Indices & q,
                               const Bool invertible)
  : ARMAFactoryImplementation(p, q, invertible)
  , spectralFactory_(WelchFactory())
  , normalizedFrequencies_(0)
  , timeGrid_()
  , m_(0)
  , spectralDensity_()
  , sigma2_(0.0)
  , isHistoryEnabled_(true)
  , history_(0)
  , startingPoints_(0)
  , solver_(new Cobyla())
{
  // Create the set of starting points using the parameters in the ResourceMap
  initializeStartingPoints();
  // Create the optimization solver parameters using the parameters in the ResourceMap
  initializeCobylaSolverParameter();
}

/* Virtual constructor */
WhittleFactory * WhittleFactory::clone() const
{
  return new WhittleFactory(*this);
}
/* Estimate the spectral density */
void WhittleFactory::buildSpectralDensity(const TimeSeries & timeSeries) const
{
  timeGrid_ = timeSeries.getTimeGrid();
  // Estimate a spectral model
  computeSpectralDensity(spectralFactory_.buildAsUserDefinedSpectralModel(timeSeries));
}

/* Estimate the spectral density */
void WhittleFactory::buildSpectralDensity(const ProcessSample & sample) const
{
  timeGrid_ = sample.getTimeGrid();
  // Estimate a spectral model
  computeSpectralDensity(spectralFactory_.buildAsUserDefinedSpectralModel(sample));
}

/* Compute the spectral density in normalized frequencies */
void WhittleFactory::computeSpectralDensity(const UserDefinedSpectralModel & spectralModel) const
{
  // We get the frequency grid and fix it for computation purposes
  const RegularGrid frequencyGrid(spectralModel.getFrequencyGrid());
  m_ = frequencyGrid.getN();
  // Index of the first frequency to take into account
  UnsignedInteger kStart = 0;
  // Suppress first value if the associated frequency is zero
  if (frequencyGrid.getStart() == 0.0)
  {
    --m_;
    kStart = 1;
  }
  const Scalar fMax = frequencyGrid.getEnd();
  normalizedFrequencies_ = Point(m_);
  spectralDensity_ = Point(m_);
  Scalar dt = timeGrid_.getStep();
  for (UnsignedInteger k = 0; k < m_ ; ++k)
  {
    const Scalar frequency = frequencyGrid.getValue(k + kStart);
    const Scalar estimatedValue = std::real(spectralModel(frequency)(0, 0)) / dt;
    spectralDensity_[k] = estimatedValue;
    normalizedFrequencies_[k] = M_PI * frequency / fMax;
  }
}

/* Compute the log-likelihood function */
Scalar WhittleFactory::computeLogLikelihood(const Point & theta) const
{
  Scalar logTerm = 0.0;
  Scalar ratioTerm = 0.0;
  for (UnsignedInteger j = 0; j < m_; ++j)
  {
    const Scalar frequency = normalizedFrequencies_[j];
    // Gj computation
    const UnsignedInteger n = std::max(currentP_, currentQ_);
    Complex numerator(1.0, 0.0);
    Complex denominator(1.0, 0.0);
    Complex y(std::polar(1.0, -frequency));
    Complex z(1.0, 0.0);
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      z *= y;
      if (i < currentQ_) numerator += theta[currentP_ + i] * z;
      if (i < currentP_) denominator += theta[i] * z;
    }
    const Scalar gJ = std::norm(numerator) / std::norm(denominator);
    // Whittle likelihood update
    logTerm += log(gJ);
    ratioTerm += spectralDensity_[j] / gJ;
  }
  sigma2_ = ratioTerm / m_;
  return -logTerm - m_ * log(ratioTerm);
}


/* Compute the log-likelihood constraint */
Point WhittleFactory::computeLogLikelihoodInequalityConstraint(const Point & theta) const
{
  const Scalar epsilon = ResourceMap::GetAsScalar("WhittleFactory-RootEpsilon");

  Point result(nbInequalityConstraint_, 0.0);

  UnsignedInteger constraintIndex = 0;
  // If not pure MA, check the roots of the AR polynom
  if (currentP_ > 0)
  {
    Point arCoefficients(currentP_ + 1, 1.0);
    for (UnsignedInteger i = 0; i < currentP_; ++i) arCoefficients[i + 1] = theta[i];
    UniVariatePolynomial polynom(arCoefficients);
    // Check the roots only if the polynom is not constant
    if (polynom.getDegree() > 0)
    {
      Collection<Complex> roots(polynom.getRoots());
      Scalar minRootModule = std::norm(roots[0]);
      for (UnsignedInteger i = 1; i < currentP_; ++i)
      {
        const Scalar rootModule = std::norm(roots[i]);
        if (rootModule < minRootModule) minRootModule = rootModule;
      }
      result[constraintIndex] = minRootModule - 1.0 - epsilon;
    }
    // For constant polynom, assume root 0
    else result[constraintIndex] = -1.0 - epsilon;
    ++constraintIndex;
  }
  // If invertible and not pure AR, check the roots of the MA polynom
  if (invertible_ && currentQ_ > 0)
  {
    Point maCoefficients(currentQ_ + 1, 1.0);
    for (UnsignedInteger i = 0; i < currentQ_; ++i) maCoefficients[i + 1] = theta[i + currentP_];
    UniVariatePolynomial polynom(maCoefficients);
    // Check the roots only if the polynom is not constant
    if (polynom.getDegree() > 0)
    {
      Collection<Complex> roots(polynom.getRoots());
      Scalar minRootModule = std::norm(roots[0]);
      for (UnsignedInteger i = 1; i < currentQ_; ++i)
      {
        const Scalar rootModule = std::norm(roots[i]);
        if (rootModule < minRootModule) minRootModule = rootModule;
      }
      result[constraintIndex] = minRootModule - 1.0 - epsilon;
    }
    // For constant polynom, assume root 0
    else result[constraintIndex] = -1.0 - epsilon;
  }
  return result;
}

/* Compute the log-likelihood function accessor */
Function WhittleFactory::getLogLikelihoodFunction() const
{
  return bindMethod <WhittleFactory, Scalar, Point> ( *this, &WhittleFactory::computeLogLikelihood, currentP_ + currentQ_, 1);
}


Function WhittleFactory::getLogLikelihoodInequalityConstraint() const
{
  return bindMethod <WhittleFactory, Point, Point> ( *this, &WhittleFactory::computeLogLikelihoodInequalityConstraint, currentP_ + currentQ_, nbInequalityConstraint_);
}

/* Initialize optimization solver parameter using the ResourceMap */
void WhittleFactory::initializeCobylaSolverParameter()
{
  Cobyla* cobyla = dynamic_cast<Cobyla *>(solver_.getImplementation().get());
  if (cobyla == NULL) throw InternalException(HERE);
  cobyla->setRhoBeg(ResourceMap::GetAsScalar("WhittleFactory-DefaultRhoBeg"));
  cobyla->setIgnoreFailure(true);

  solver_.setMaximumAbsoluteError(ResourceMap::GetAsScalar("WhittleFactory-DefaultRhoEnd"));
  solver_.setMaximumEvaluationNumber(ResourceMap::GetAsUnsignedInteger("WhittleFactory-DefaultMaximumEvaluationNumber"));
}

/* Optimization solver accessor */
OptimizationAlgorithm WhittleFactory::getOptimizationAlgorithm() const
{
  return solver_;
}

void WhittleFactory::setOptimizationAlgorithm(const OptimizationAlgorithm & solver)
{
  solver_ = solver;
}


/* String converter */
String WhittleFactory::__repr__() const
{
  OSS oss;
  oss << "class= " << WhittleFactory::GetClassName()
      << " p = " << p_ << " q = " << q_
      << " spectralModelFactory = " << spectralFactory_;
  return oss;
}

/* String converter */
String WhittleFactory::__str__(const String & ) const
{
  return this->__repr__();
}

/* SpectralModelFactory accessor */
WelchFactory WhittleFactory::getSpectralModelFactory() const
{
  return spectralFactory_;
}

/* Enable or disable the estimation history */
void WhittleFactory::enableHistory() const
{
  isHistoryEnabled_ = true;
}

void WhittleFactory::disableHistory() const
{
  isHistoryEnabled_ = false;
}

Bool WhittleFactory::isHistoryEnabled() const
{
  return isHistoryEnabled_;
}

void WhittleFactory::clearHistory() const
{
  history_ = Collection< WhittleFactoryState >(0);
}

Collection< WhittleFactoryState > WhittleFactory::getHistory() const
{
  return history_;
}

/* SpectralModelFactory accessor */
void WhittleFactory::setSpectralModelFactory(const WelchFactory & factory)
{
  spectralFactory_ = factory;
}

/* Build method
 * The Whittle likelihood is a ratio of spectral densities:
 * \log L(\theta, \sigma^2) = -\sum_{j=1}^m\log f(\lambda_j|\theta, \sigma^2)-\frac{1}{2\pi}\sum_{j=1}^m\frac{I(\lambda_j)}{f(\lambda_j|\theta,\sigma^2)}
 * with:
 * I(\lambda_j]=\frac{1}{n}\left|\sum_{k=1}^n w_ky_k\exp(-ik\lambda_j\right|^2
 * f(\lambda | \theta, \sigma^~)=\frac{\sigma^2}{2\pi}\left|\frac{\beta(\exp(-i\lambda))}{\alpha(\exp(-i\lambda))}\right|^2
 * and \theta = (\alpha, \lambda), m=ceil((n-1)/2)
 *
 * In its reduced form, we have:
 * \log L(\theta) = m\log(2\pi)-m\log\left[\frac{1}{m}\sum_{j=1}^m\frac{I(\lambda_j)}{g(\lambda_j|\theta)}\right]-\sum_{j=1}^m\log g(\lambda_j|\theta)-m
 * and
 * \sigma^2=\frac{1}{m}\sum_{j=1}^m\frac{I(\lambda_j)}{g(\lambda_j|\theta)}
 * with g(\lambda|\theta)=\frax{2\pi}{\sigma^2}f(\lambda|\theta,\sigma^2)
 *                       =\left|\frac{\beta(\exp(-i\lambda))}{\alpha(\exp(-i\lambda))}\right|^2
 * It can be noticed that dropping the term -\sum_{j=1}^m\log f(\lambda_j|\theta, \sigma^2) in the Whittle likelihood or the term
 */
ARMA WhittleFactory::buildWithCriteria(const TimeSeries & timeSeries,
                                       Point & informationCriteria) const
{
  if (timeSeries.getOutputDimension() != 1)
    throw NotYetImplementedException(HERE) << "In WhittleFactory::build(const TimeSeries & timeSeries, Point & informationCriteria) const: currently implemented for 1 d case only";

  // Compute the tapered periodogram for the time series using the Welch method
  // The computation is done once
  buildSpectralDensity(timeSeries);
  return maximizeLogLikelihood(informationCriteria);
}

ARMA WhittleFactory::build(const TimeSeries & timeSeries) const
{
  Point informationCriteria;
  return buildWithCriteria(timeSeries, informationCriteria);
}

/* Build method */
ARMA WhittleFactory::buildWithCriteria(const ProcessSample & sample,
                                       Point & informationCriteria) const
{
  if (sample.getDimension() != 1)
    throw NotYetImplementedException(HERE) << "In WhittleFactory::build(const ProcessSample & sample, Point & informationCriteria) const: currently implemented for 1 d case only";

  // Compute the tapered periodogram for the process sample using the Welch method
  buildSpectralDensity(sample);
  return maximizeLogLikelihood(informationCriteria);
}

ARMA WhittleFactory::build(const ProcessSample & sample) const
{
  Point informationCriteria;
  return buildWithCriteria(sample, informationCriteria);
}

/* Do the likelihood maximization */
ARMA WhittleFactory::maximizeLogLikelihood(Point & informationCriteria) const
{
  // First, clean the history
  clearHistory();
  const UnsignedInteger sizeP = p_.getSize();
  const UnsignedInteger sizeQ = q_.getSize();

  // Best parameters
  Point bestTheta(0);
  Scalar bestSigma2 = 0.0;
  Point bestInformationCriteria(3, SpecFunc::MaxScalar);
  UnsignedInteger bestP = 0;
  UnsignedInteger bestQ = 0;

  UnsignedInteger pointIndex = 0;

  for (UnsignedInteger pIndex = 0; pIndex < sizeP; ++pIndex)
  {
    currentP_ = p_[pIndex];
    for (UnsignedInteger qIndex = 0; qIndex < sizeQ; ++qIndex)
    {
      currentQ_ = q_[qIndex];

      LOGDEBUG(OSS() << "Current parameters p=" << currentP_ << ", q=" << currentQ_);


      // Dimension of the optimization problem
      int n(currentP_ + currentQ_);
      int m(0);
      // Check the minimum root module of the AR polynom only if not pure MA process
      if (currentP_ > 0) ++m;
      // Check the minimum root module of the MA polynom only if not pure AR process and if invertible
      if (invertible_ && currentQ_ > 0) ++m;

      // Current parameters vector
      Point theta(startingPoints_[pointIndex]);

      // Optimize only if there is some ARMA parameters to estimate
      if (n > 0)
      {
        // Define Objective and Constraint functions for Optimization problem
        OptimizationProblem problem(getLogLikelihoodFunction());
        problem.setMinimization(false);
        // use attributes to pass the data
        nbInequalityConstraint_ = m;
        problem.setInequalityConstraint(getLogLikelihoodInequalityConstraint());
        OptimizationAlgorithm solver(solver_);
        solver.setProblem(problem);
        solver.setStartingPoint(startingPoints_[pointIndex]);

        // run Optimization problem
        solver.run();

        // optimal point
        const Point optpoint(solver.getResult().getOptimalPoint());
        theta = optpoint;
      }
      // Compute the information criteria
      // First, the corrected AIC
      const Scalar logLikelihood = computeLogLikelihood(theta);
      Point currentInformationCriteria(3);
      if (m_ > static_cast<UnsignedInteger>(n + 2)) currentInformationCriteria[0] = -2.0 * logLikelihood + 2.0 * (n + 1) * m_ / (m_ - n - 2);
      else
      {
        LOGWARN(OSS() << "Warning! Unable to compute the corrected AIC criteria, too few data (" << m_ << ") for the model complexity (" << n + 1);
        currentInformationCriteria[0] = SpecFunc::MaxScalar;
      }
      // Second, the AIC
      currentInformationCriteria[1] = -2.0 * logLikelihood + 2.0 * (n + 1);
      // Third, the BIC
      currentInformationCriteria[2] = -2.0 * logLikelihood + 2.0 * (n + 1) * log(1.0 * m_);
      LOGDEBUG(OSS(false) << "Current estimate: theta=" << theta << ", sigma2=" << sigma2_ << ", Current information criteria=" << currentInformationCriteria);
      if (isHistoryEnabled_) history_.add(WhittleFactoryState(currentP_, theta, sigma2_, currentInformationCriteria, timeGrid_));
      // Keep the best model according to the first criteria
      if (currentInformationCriteria[0] < bestInformationCriteria[0])
      {
        bestInformationCriteria = currentInformationCriteria;
        bestSigma2 = sigma2_;
        bestTheta = theta;
        bestP = currentP_;
        bestQ = currentQ_;
      }
      LOGDEBUG(OSS(false) << "Best so far: p=" << bestP << ", q=" << bestQ << ", theta=" << bestTheta << ", sigma2=" << bestSigma2 << ", information criteria=" << bestInformationCriteria);
      ++pointIndex;
    } // Loop over q
  } // Loop over p
  // Return the best information criteria
  informationCriteria = bestInformationCriteria;
  return WhittleFactoryState(bestP, bestTheta, bestSigma2, bestInformationCriteria, timeGrid_).getARMA();
}

/* Starting points accessor */
void WhittleFactory::setStartingPoints(const Collection< Point > & startingPoints)
{
  const UnsignedInteger sizeP = p_.getSize();
  const UnsignedInteger sizeQ = q_.getSize();
  // First, check the size of the collection
  if (startingPoints.getSize() != sizeP * sizeQ) throw InvalidArgumentException(HERE) << "Error: the given collection has size=" << startingPoints.getSize() << " but should have size=" << sizeP * sizeQ;
  // Second, check that all the points have the correct dimension
  UnsignedInteger k = 0;
  for (UnsignedInteger pIndex = 0; pIndex < sizeP; ++pIndex)
  {
    const UnsignedInteger p = p_[pIndex];
    for (UnsignedInteger qIndex = 0; qIndex < sizeQ; ++qIndex)
    {
      const UnsignedInteger q = q_[qIndex];
      // Dimension of the optimization problem
      const UnsignedInteger n = p + q;
      if (startingPoints[k].getSize() != n) throw InvalidArgumentException(HERE) << "Error: the point at index=" << k << " should have a size=" << n << " instead of size=" << startingPoints[k].getSize();
      ++k;
    }
  }
  // Everything is ok
  startingPoints_ = startingPoints;
}

Collection< Point > WhittleFactory::getStartingPoints() const
{
  return startingPoints_;
}


/* Initialize the starting points using the ResourceMap */
void WhittleFactory::initializeStartingPoints()
{
  startingPoints_ = Collection< Point >(0);
  // Initialization of the starting point
  const Scalar theta0 = ResourceMap::GetAsScalar("WhittleFactory-DefaultStartingPointScale");
  const UnsignedInteger sizeP = p_.getSize();
  const UnsignedInteger sizeQ = q_.getSize();
  for (UnsignedInteger pIndex = 0; pIndex < sizeP; ++pIndex)
  {
    const UnsignedInteger p = p_[pIndex];
    for (UnsignedInteger qIndex = 0; qIndex < sizeQ; ++qIndex)
    {
      const UnsignedInteger q = q_[qIndex];
      // Dimension of the optimization problem
      const UnsignedInteger n = p + q;
      Point theta(n);
      if (n > 0)
      {
        theta[0] = theta0;
        for (UnsignedInteger k = 1; k < static_cast<UnsignedInteger>(n); ++k) theta[k] = 0.5 * theta[k - 1];
      }
      startingPoints_.add(theta);
    } // Loop over q
  } // Loop over p
}

/* Method save() stores the object through the StorageManager */
void WhittleFactory::save(Advocate & adv) const
{
  ARMAFactoryImplementation::save(adv);
  adv.saveAttribute( "spectralFactory_", spectralFactory_);
  adv.saveAttribute( "isHistoryEnabled_", isHistoryEnabled_);
  adv.saveAttribute( "history_", history_);
  adv.saveAttribute( "startingPoints_", startingPoints_);
}

/* Method load() reloads the object from the StorageManager */
void WhittleFactory::load(Advocate & adv)
{
  ARMAFactoryImplementation::load(adv);
  adv.loadAttribute( "spectralFactory_", spectralFactory_);
  adv.loadAttribute( "isHistoryEnabled_", isHistoryEnabled_);
  adv.loadAttribute( "history_", history_);
  adv.loadAttribute( "startingPoints_", startingPoints_);
}

END_NAMESPACE_OPENTURNS
