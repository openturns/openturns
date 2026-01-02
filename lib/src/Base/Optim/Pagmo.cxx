//                                               -*- C++ -*-
/**
 *  @brief Pagmo optimization algorithm
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
#include "openturns/Pagmo.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/DatabaseFunction.hxx"
#include "openturns/RandomGenerator.hxx"

#ifdef OPENTURNS_HAVE_PAGMO
#include <pagmo/algorithm.hpp>
#include <pagmo/algorithms/gaco.hpp>
#include <pagmo/algorithms/de.hpp>
#include <pagmo/algorithms/sade.hpp>
#include <pagmo/algorithms/de1220.hpp>
#include <pagmo/algorithms/gwo.hpp>
#include <pagmo/algorithms/ihs.hpp>
#include <pagmo/algorithms/pso.hpp>
#include <pagmo/algorithms/pso_gen.hpp>
#include <pagmo/algorithms/sea.hpp>
#include <pagmo/algorithms/sga.hpp>
#include <pagmo/algorithms/simulated_annealing.hpp>
#include <pagmo/algorithms/bee_colony.hpp>
#ifdef PAGMO_WITH_EIGEN3
#include <pagmo/algorithms/cmaes.hpp>
#include <pagmo/algorithms/xnes.hpp>
#endif
#include <pagmo/algorithms/nsga2.hpp>
#include <pagmo/algorithms/moead.hpp>
#define PAGMO_VERSION_NR PAGMO_VERSION_MAJOR * 100000 + PAGMO_VERSION_MINOR * 100 + PAGMO_VERSION_PATCH
#if PAGMO_VERSION_NR >= 201900
#include <pagmo/algorithms/moead_gen.hpp>
#endif
#include <pagmo/algorithms/maco.hpp>
#include <pagmo/algorithms/nspso.hpp>
#include <pagmo/utils/multi_objective.hpp>
#include <pagmo/problems/unconstrain.hpp>
#include <pagmo/batch_evaluators/member_bfe.hpp>
#endif

BEGIN_NAMESPACE_OPENTURNS

#ifdef OPENTURNS_HAVE_PAGMO

struct PagmoProblem
{
  PagmoProblem() {};

  PagmoProblem(Pagmo *algorithm,
               Sample *evaluationInputHistory,
               Sample *evaluationOutputHistory,
               Sample *equalityHistory,
               Sample *inequalityHistory)
    : algorithm_(algorithm)
    , evaluationInputHistory_(evaluationInputHistory)
    , evaluationOutputHistory_(evaluationOutputHistory)
    , equalityHistory_(equalityHistory)
    , inequalityHistory_(inequalityHistory)
  {
    if (!algorithm)
      throw InvalidArgumentException(HERE) << "PagmoProblem null algo";
    if (!evaluationInputHistory || !evaluationOutputHistory)
      throw InvalidArgumentException(HERE) << "PagmoProblem null history";

    // pagmo wants the integer components grouped at the end, so renumbering is in order
    Indices pagmoOrder;
    const Indices types(algorithm_->getProblem().getVariablesType());
    for (UnsignedInteger i = 0; i < types.getSize(); ++ i)
      if (types[i] == OptimizationProblemImplementation::CONTINUOUS)
        pagmoOrder.add(i);
    for (UnsignedInteger i = 0; i < types.getSize(); ++ i)
      if (types[i] != OptimizationProblemImplementation::CONTINUOUS)
        pagmoOrder.add(i);
    Indices ordinal(types.getSize());
    ordinal.fill();
    if (pagmoOrder != ordinal)
    {
      Indices renum(types.getSize());
      for (UnsignedInteger i = 0; i < types.getSize(); ++ i)
        renum[pagmoOrder[i]] = i;
      renum_ = renum;
    }
    *evaluationInputHistory_ = Sample(0, algorithm->getProblem().getDimension());
    *evaluationOutputHistory_ = Sample(0, algorithm->getProblem().getObjective().getOutputDimension());
    *equalityHistory_ = Sample(0, algorithm->getProblem().getEqualityConstraint().getOutputDimension());
    *inequalityHistory_ = Sample(0, algorithm->getProblem().getInequalityConstraint().getOutputDimension());
  }

  Point renumber(const Point & inP, const Bool uToX = true) const
  {
    Point result(inP);
    if (renum_.getSize())
    {
      for (UnsignedInteger i = 0; i < inP.getDimension(); ++ i)
        if (uToX)
          result[i] = inP[renum_[i]]; // pagmo -> OT
        else
          result[renum_[i]] = inP[i]; // OT -> pagmo
    }
    return result;
  }

  pagmo::vector_double fitness(const pagmo::vector_double & inv) const
  {
    const Point inP(renumber(Point(inv.begin(), inv.end())));
    const Point outP(algorithm_->getProblem().getObjective()(inP));
    evaluationInputHistory_->add(inP);
    evaluationOutputHistory_->add(outP);
    Point outV(outP);
    for (UnsignedInteger i = 0; i < outP.getDimension(); ++ i)
      if (!algorithm_->getProblem().isMinimization(i))
        outV[i] *= -1.0;// pagmo always minimizes

    Scalar constraintError = 0.0;
    const Scalar maximumConstraintError = algorithm_->getMaximumConstraintError();
    if (algorithm_->getProblem().hasEqualityConstraint())
    {
      const Point equP(algorithm_->getProblem().getEqualityConstraint()(inP));
      equalityHistory_->add(equP);
      for (UnsignedInteger i = 0; i < equP.getDimension(); ++ i)
        if (std::abs(equP[i]) > maximumConstraintError)
          constraintError = std::max(constraintError, std::abs(equP[i]));
      outV.add(equP);
    }

    if (algorithm_->getProblem().hasInequalityConstraint())
    {
      const Point inequP(algorithm_->getProblem().getInequalityConstraint()(inP));
      inequalityHistory_->add(inequP);
      for (UnsignedInteger i = 0; i < inequP.getDimension(); ++ i)
        if (inequP[i] < -maximumConstraintError)
          constraintError = std::max(constraintError, std::abs(inequP[i]));
      outV.add(-1.0 * inequP);// opposite convention for ineq constraints
    }

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0_).count();
    if (!(algorithm_->getMaximumTimeDuration() <= 0.0) && (timeDuration > algorithm_->getMaximumTimeDuration()))
      throw TimeoutException(HERE) << "Duration (" << timeDuration << "s) exceeds maximum duration (" << algorithm_->getMaximumTimeDuration() << " s)";
    algorithm_->result_.setTimeDuration(timeDuration);
    algorithm_->result_.setCallsNumber(evaluationInputHistory_->getSize());
    algorithm_->result_.store(inP, outP, 0.0, 0.0, 0.0, constraintError, maximumConstraintError);

    // callbacks
    if (algorithm_->progressCallback_.first)
    {
      const UnsignedInteger callsNumber = evaluationInputHistory_->getSize();
      algorithm_->progressCallback_.first((100.0 * callsNumber) / (algorithm_->getStartingSample().getSize() * (algorithm_->getMaximumIterationNumber() + 1)), algorithm_->progressCallback_.second);
    }
    if (algorithm_->stopCallback_.first && algorithm_->stopCallback_.first(algorithm_->stopCallback_.second))
      throw InterruptionException(HERE) << "User stopped optimization";
    return outV.toStdVector();
  }

  std::pair<pagmo::vector_double, pagmo::vector_double> get_bounds() const
  {
    const pagmo::vector_double lb2(renumber(algorithm_->getProblem().getBounds().getLowerBound(), false).toStdVector());
    const pagmo::vector_double ub2(renumber(algorithm_->getProblem().getBounds().getUpperBound(), false).toStdVector());
    return std::make_pair(lb2, ub2);
  }

  pagmo::vector_double::size_type get_nobj() const
  {
    return algorithm_->getProblem().getObjective().getOutputDimension();
  }

  pagmo::vector_double::size_type get_nec() const
  {
    return algorithm_->getProblem().getEqualityConstraint().getOutputDimension();
  }

  pagmo::vector_double::size_type get_nic() const
  {
    return algorithm_->getProblem().getInequalityConstraint().getOutputDimension();
  }

  pagmo::vector_double::size_type get_nix() const
  {
    const Indices types(algorithm_->getProblem().getVariablesType());
    UnsignedInteger nix = 0;
    for (UnsignedInteger i = 0; i < types.getSize(); ++ i)
      if (types[i] != OptimizationProblemImplementation::CONTINUOUS)
        ++ nix;
    return nix;
  }

  pagmo::vector_double batch_fitness(const pagmo::vector_double & xs) const
  {
    const OptimizationProblem problem(algorithm_->getProblem());
    const UnsignedInteger inputDimension = problem.getObjective().getInputDimension();
    const UnsignedInteger outputDimension = problem.getObjective().getOutputDimension();
    const UnsignedInteger blockSize = algorithm_->getBlockSize();
    const UnsignedInteger size = xs.size() / inputDimension;
    const UnsignedInteger blockNumber = static_cast<UnsignedInteger>(ceil(1.0 * size / blockSize));

    UnsignedInteger totalDimension = outputDimension;
    if (problem.hasEqualityConstraint())
      totalDimension += problem.getEqualityConstraint().getOutputDimension();
    if (problem.hasInequalityConstraint())
      totalDimension += problem.getInequalityConstraint().getOutputDimension();
    Sample outV(0, totalDimension);
    UnsignedInteger offset = 0;
    for (UnsignedInteger outerSampling = 0; outerSampling < blockNumber; ++ outerSampling)
    {
      const UnsignedInteger effectiveBlockSize = ((outerSampling == (blockNumber - 1)) && (size % blockSize)) ? (size % blockSize) : blockSize;
      Sample inSb(effectiveBlockSize, inputDimension);
      for (UnsignedInteger i = 0; i < effectiveBlockSize; ++ i)
      {
        const Point xsi(xs.begin() + offset + i * inputDimension, xs.begin() + offset + (i + 1) * inputDimension);
        inSb[i] = renumber(xsi);
      }
      const Sample outSb(problem.getObjective()(inSb));
      evaluationInputHistory_->add(inSb);
      evaluationOutputHistory_->add(outSb);
      Sample outVb(outSb);
      for (UnsignedInteger i = 0; i < effectiveBlockSize; ++ i)
        for (UnsignedInteger j = 0; j < outputDimension; ++ j)
          if (!problem.isMinimization(j))
            outVb(i, j) *= -1.0;

      Point constraintError(effectiveBlockSize);
      const Scalar maximumConstraintError = algorithm_->getMaximumConstraintError();
      if (problem.hasEqualityConstraint())
      {
        const Sample eqSb(problem.getEqualityConstraint()(inSb));
        equalityHistory_->add(eqSb);
        for (UnsignedInteger i = 0; i < effectiveBlockSize; ++ i)
          for (UnsignedInteger j = 0; j < eqSb.getDimension(); ++ j)
            if (std::abs(eqSb(i, j)) > maximumConstraintError)
              constraintError[i] = std::max(constraintError[i], std::abs(eqSb(i, j)));
        outVb.stack(eqSb);
      }
      if (problem.hasInequalityConstraint())
      {
        const Sample ineqSb(problem.getInequalityConstraint()(inSb));
        inequalityHistory_->add(ineqSb);
        for (UnsignedInteger i = 0; i < effectiveBlockSize; ++ i)
          for (UnsignedInteger j = 0; j < ineqSb.getDimension(); ++ j)
            if (ineqSb(i, j) < -maximumConstraintError)
              constraintError[i] = std::max(constraintError[i], std::abs(ineqSb(i, j)));
        outVb.stack(-1.0 * ineqSb);
      }
      outV.add(outVb);
      offset += blockSize * inputDimension;

      std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
      const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0_).count();
      if (!(algorithm_->getMaximumTimeDuration() <= 0.0) && (timeDuration > algorithm_->getMaximumTimeDuration()))
        throw TimeoutException(HERE) << "Duration (" << timeDuration << "s) exceeds maximum duration (" << algorithm_->getMaximumTimeDuration() << " s)";
      algorithm_->result_.setTimeDuration(timeDuration);
      algorithm_->result_.setCallsNumber(evaluationInputHistory_->getSize());

      for (UnsignedInteger i = 0; i < effectiveBlockSize; ++ i)
        algorithm_->result_.store(inSb[i], outSb[i], 0.0, 0.0, 0.0, constraintError[i], maximumConstraintError);

      // callbacks
      if (algorithm_->progressCallback_.first)
      {
        const UnsignedInteger callsNumber = evaluationInputHistory_->getSize();
        algorithm_->progressCallback_.first((100.0 * callsNumber) / (algorithm_->getStartingSample().getSize() * (algorithm_->getMaximumIterationNumber() + 1)), algorithm_->progressCallback_.second);
      }
      if (algorithm_->stopCallback_.first && algorithm_->stopCallback_.first(algorithm_->stopCallback_.second))
        throw InterruptionException(HERE) << "User stopped optimization";
    }
    return outV.getImplementation()->getData().toStdVector();
  }

  bool has_batch_fitness() const
  {
    return true;
  }

  std::string get_name() const
  {
    return algorithm_->getProblem().getName();
  }

  std::string get_extra_info() const
  {
    return algorithm_->getProblem().__repr__();
  }

  pagmo::thread_safety get_thread_safety() const
  {
    return algorithm_->getProblem().getObjective().getImplementation()->isParallel() ?
           pagmo::thread_safety::constant : pagmo::thread_safety::none;
  }

  void setT0(const std::chrono::steady_clock::time_point & t0)
  {
    t0_ = t0;
  }

  Pagmo *algorithm_ = nullptr;
  Indices renum_;
  Sample *evaluationInputHistory_ = nullptr;
  Sample *evaluationOutputHistory_ = nullptr;
  Sample *equalityHistory_ = nullptr;
  Sample *inequalityHistory_ = nullptr;
  std::chrono::steady_clock::time_point t0_;
};
#endif


CLASSNAMEINIT(Pagmo)

static const Factory<Pagmo> Factory_Pagmo;

/* Default constructor */
Pagmo::Pagmo(const String & algoName)
  : OptimizationAlgorithmImplementation()
  , seed_(ResourceMap::GetAsUnsignedInteger("Pagmo-InitialSeed"))
  , incrementalEvolution_(ResourceMap::GetAsBool("Pagmo-DefaultIncrementalEvolution"))
{
  setAlgorithmName(algoName);
}


/* Parameters constructor */
Pagmo::Pagmo(const OptimizationProblem & problem,
             const String & algoName,
             const Sample & startingSample)
  : OptimizationAlgorithmImplementation()
  , startingSample_(startingSample)
  , seed_(ResourceMap::GetAsUnsignedInteger("Pagmo-InitialSeed"))
  , incrementalEvolution_(ResourceMap::GetAsBool("Pagmo-DefaultIncrementalEvolution"))
{
  setAlgorithmName(algoName);
  setProblem(problem);
}


/* Check whether this problem can be solved by this solver */
void Pagmo::checkProblem(const OptimizationProblem & problem) const
{
#ifdef OPENTURNS_HAVE_PAGMO
  if (!problem.hasBounds())
    throw InvalidArgumentException(HERE) << "Pagmo only supports bounded problems";
  if (problem.hasResidualFunction() || problem.hasLevelFunction())
    throw InvalidArgumentException(HERE) << "Pagmo does not support least squares or nearest point problems";
  const Description multiObjectiveAlgorithms = {"nsga2", "moead",
#if PAGMO_VERSION_NR >= 201900
                                               "moead_gen",
#endif
                                               "mhaco", "nspso"
                                              };
  if ((problem.getObjective().getOutputDimension() > 1) && !multiObjectiveAlgorithms.contains(getAlgorithmName()))
    throw InvalidArgumentException(HERE) << getAlgorithmName() << " does not support multi-objective optimization";
  if ((problem.getObjective().getOutputDimension() < 2) && multiObjectiveAlgorithms.contains(getAlgorithmName()))
    throw InvalidArgumentException(HERE) << getAlgorithmName() << " only supports multi-objective optimization";
  const Description integerAlgorithms = {"gaco", "ihs", "sga", "nsga2", "mhaco"};
  if (!problem.isContinuous() && !integerAlgorithms.contains(getAlgorithmName()))
    throw InvalidArgumentException(HERE) << getAlgorithmName() << " does not support non continuous problems";
#else
  (void)problem;
  throw InvalidArgumentException(HERE) << "No Pagmo support";
#endif
}


void Pagmo::run()
{
  Sample startingSample(getStartingSample());
  UnsignedInteger size = startingSample.getSize();
  if (!size)
    throw InvalidArgumentException(HERE) << "Starting sample is empty";
  if (startingSample.getDimension() != getProblem().getDimension())
    throw InvalidArgumentException(HERE) << "Starting sample dimension does not match problem dimension";
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Point inP(startingSample[i]);
    if (!getProblem().getBounds().contains(inP))
      LOGWARN(OSS() << "Starting point #" << i << " (" << inP << ") lies outside bounds");
    if (!getProblem().isContinuous())
    {
      const Indices types(getProblem().getVariablesType());
      const Point lowerBound(getProblem().getBounds().getLowerBound());
      const Point upperBound(getProblem().getBounds().getUpperBound());
      for (UnsignedInteger j = 0; j < getProblem().getDimension(); ++ j)
      {
        if (types[j] != OptimizationProblemImplementation::CONTINUOUS && std::trunc(inP[j]) != inP[j])
          throw InvalidArgumentException(HERE) << "Starting sample components must have integral values, got " << inP[j];
        if (types[j] != OptimizationProblemImplementation::CONTINUOUS && std::trunc(lowerBound[j]) != lowerBound[j])
          throw InvalidArgumentException(HERE) << "Lower bound must have integral values, got " << lowerBound[j];
        if (types[j] != OptimizationProblemImplementation::CONTINUOUS && std::trunc(upperBound[j]) != upperBound[j])
          throw InvalidArgumentException(HERE) << "Upper bound must have integral values, got " << upperBound[j];
        if (types[j] == OptimizationProblemImplementation::BINARY && (std::round(inP[j]) != 0.0) && (std::round(inP[j]) != 1.0))
          throw InvalidArgumentException(HERE) << "Starting sample components must have binary values, got " << inP[j];
      }
    }
  }

#ifdef OPENTURNS_HAVE_PAGMO
  Sample evaluationInputHistory;
  Sample evaluationOutputHistory;
  Sample equalityHistory;
  Sample inequalityHistory;
  PagmoProblem pproblem(this, &evaluationInputHistory, &evaluationOutputHistory, &equalityHistory, &inequalityHistory);
  std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
  pproblem.setT0(t0);
  pagmo::problem prob(pproblem);
  const pagmo::vector_double ctol(prob.get_nc(), getMaximumConstraintError());
  prob.set_c_tol(ctol);

  // most algorithms do not support constraints but they can be emulated (by penalization for example)
  const Bool emulatedConstraints = (getProblem().hasInequalityConstraint() || getProblem().hasEqualityConstraint())
                                   && !Description({"gaco", "ihs"}).contains(getAlgorithmName());
  if (emulatedConstraints)
  {
    const String unconstrainMethod = ResourceMap::GetAsString("Pagmo-UnconstrainMethod");
    prob = pagmo::unconstrain(prob, unconstrainMethod);
  }

  pagmo::algorithm algo;
  const UnsignedInteger gen = incrementalEvolution_ ? 1 : getMaximumIterationNumber();
  if (algoName_ == "gaco")
  {
    // gaco(unsigned gen = 1u, unsigned ker = 63u, double q = 1.0, double oracle = 0., double acc = 0.01, unsigned threshold = 1u, unsigned n_gen_mark = 7u, unsigned impstop = 100000u, unsigned evalstop = 100000u, double focus = 0., bool memory = false, unsigned seed = pagmo::random_device::next())
    UnsignedInteger ker = ResourceMap::GetAsUnsignedInteger("Pagmo-gaco-ker");
    const Scalar q = ResourceMap::GetAsScalar("Pagmo-gaco-q");
    const Scalar oracle = ResourceMap::GetAsScalar("Pagmo-gaco-oracle");
    const Scalar acc = ResourceMap::GetAsScalar("Pagmo-gaco-acc");
    const UnsignedInteger threshold = ResourceMap::GetAsUnsignedInteger("Pagmo-gaco-threshold");
    const UnsignedInteger n_gen_mark = ResourceMap::GetAsUnsignedInteger("Pagmo-gaco-n_gen_mark");
    const UnsignedInteger impstop = ResourceMap::GetAsUnsignedInteger("Pagmo-gaco-impstop");
    const Scalar focus = ResourceMap::GetAsScalar("Pagmo-gaco-focus");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    if (!memory)
      ker = std::min(ker, size);
    pagmo::gaco algorithm_impl(gen, ker, q, oracle, acc, threshold, n_gen_mark, impstop, getMaximumCallsNumber(), focus, memory);
#if PAGMO_VERSION_NR >= 201901
    // requires https://github.com/esa/pagmo2/pull/575
    algorithm_impl.set_bfe(pagmo::bfe{});
#else
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
#endif
    algo = algorithm_impl;
  }
  else if (algoName_ == "de")
  {
    // de(unsigned gen = 1u, double F = 0.8, double CR = 0.9, unsigned variant = 2u, double ftol = 1e-6, double xtol = 1e-6, unsigned seed = pagmo::random_device::next())
    const Scalar F = ResourceMap::GetAsScalar("Pagmo-de-F");
    const Scalar CR = ResourceMap::GetAsScalar("Pagmo-de-CR");
    const UnsignedInteger variant = ResourceMap::GetAsUnsignedInteger("Pagmo-de-variant");
    algo = pagmo::de(gen, F, CR, variant, getMaximumResidualError(), getMaximumAbsoluteError());
  }
  else if (algoName_ == "sade")
  {
    // sade(unsigned gen = 1u, unsigned variant = 2u, unsigned variant_adptv = 1u, double ftol = 1e-6, double xtol = 1e-6, bool memory = false, unsigned seed = pagmo::random_device::next())
    const UnsignedInteger variant = ResourceMap::GetAsUnsignedInteger("Pagmo-sade-variant");
    const UnsignedInteger variant_adptv = ResourceMap::GetAsUnsignedInteger("Pagmo-sade-variant_adptv");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    algo = pagmo::sade(gen, variant, variant_adptv, getMaximumResidualError(), getMaximumAbsoluteError(), memory);
  }
  else if (algoName_ == "de1220")
  {
    // de1220(unsigned gen = 1u, std::vector<unsigned> allowed_variants = de1220_statics<void>::allowed_variants, unsigned variant_adptv = 1u, double ftol = 1e-6, double xtol = 1e-6, bool memory = false, unsigned seed = pagmo::random_device::next())
    const UnsignedInteger variant_adptv = ResourceMap::GetAsUnsignedInteger("Pagmo-de1220-variant_adptv");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    algo = pagmo::de1220(gen, pagmo::de1220_statics<void>::allowed_variants, variant_adptv, getMaximumResidualError(), getMaximumAbsoluteError(), memory);
  }
  else if (algoName_ == "gwo")
  {
    // gwo(unsigned gen = 1u, unsigned seed = pagmo::random_device::next())
    algo = pagmo::gwo(gen);
  }
  else if (algoName_ == "ihs")
  {
    // ihs(unsigned gen = 1u, double phmcr = 0.85, double ppar_min = 0.35, double ppar_max = 0.99, double bw_min = 1E-5, double bw_max = 1., unsigned seed = pagmo::random_device::next())
    const Scalar phmcr = ResourceMap::GetAsScalar("Pagmo-ihs-phmcr");
    const Scalar ppar_min = ResourceMap::GetAsScalar("Pagmo-ihs-ppar_min");
    const Scalar ppar_max = ResourceMap::GetAsScalar("Pagmo-ihs-ppar_max");
    const Scalar bw_min = ResourceMap::GetAsScalar("Pagmo-ihs-bw_min");
    const Scalar bw_max = ResourceMap::GetAsScalar("Pagmo-ihs-bw_max");
    algo = pagmo::ihs(gen, phmcr, ppar_min, ppar_max, bw_min, bw_max);
  }
  else if (algoName_ == "pso")
  {
    // pso(unsigned gen = 1u, double omega = 0.7298, double eta1 = 2.05, double eta2 = 2.05, double max_vel = 0.5, unsigned variant = 5u, unsigned neighb_type = 2u, unsigned neighb_param = 4u, bool memory = false, unsigned seed = pagmo::random_device::next())
    const Scalar omega = ResourceMap::GetAsScalar("Pagmo-pso-omega");
    const Scalar eta1 = ResourceMap::GetAsScalar("Pagmo-pso-eta1");
    const Scalar eta2 = ResourceMap::GetAsScalar("Pagmo-pso-eta2");
    const Scalar max_vel = ResourceMap::GetAsScalar("Pagmo-pso-max_vel");
    const UnsignedInteger variant = ResourceMap::GetAsUnsignedInteger("Pagmo-pso-variant");
    const UnsignedInteger neighb_type = ResourceMap::GetAsUnsignedInteger("Pagmo-pso-neighb_type");
    const UnsignedInteger neighb_param = ResourceMap::GetAsUnsignedInteger("Pagmo-pso-neighb_param");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    algo = pagmo::pso(gen, omega, eta1, eta2, max_vel, variant, neighb_type, neighb_param, memory);
  }
  else if (algoName_ == "pso_gen")
  {
    // pso_gen(unsigned gen = 1u, double omega = 0.7298, double eta1 = 2.05, double eta2 = 2.05, double max_vel = 0.5, unsigned variant = 5u, unsigned neighb_type = 2u, unsigned neighb_param = 4u, bool memory = false, unsigned seed = pagmo::random_device::next())
    const Scalar omega = ResourceMap::GetAsScalar("Pagmo-pso-omega");
    const Scalar eta1 = ResourceMap::GetAsScalar("Pagmo-pso-eta1");
    const Scalar eta2 = ResourceMap::GetAsScalar("Pagmo-pso-eta2");
    const Scalar max_vel = ResourceMap::GetAsScalar("Pagmo-pso-max_vel");
    const UnsignedInteger variant = ResourceMap::GetAsUnsignedInteger("Pagmo-pso-variant");
    const UnsignedInteger neighb_type = ResourceMap::GetAsUnsignedInteger("Pagmo-pso-neighb_type");
    const UnsignedInteger neighb_param = ResourceMap::GetAsUnsignedInteger("Pagmo-pso-neighb_param");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    pagmo::pso_gen algorithm_impl(gen, omega, eta1, eta2, max_vel, variant, neighb_type, neighb_param, memory);
#if PAGMO_VERSION_NR >= 201901
    // requires https://github.com/esa/pagmo2/pull/575
    algorithm_impl.set_bfe(pagmo::bfe{});
#else
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
#endif
    algo = algorithm_impl;
  }
  else if (algoName_ == "sea")
  {
    // sea(unsigned gen = 1u, unsigned seed = pagmo::random_device::next())
    algo = pagmo::sea(gen);
  }
  else if (algoName_ == "sga")
  {
    // sga(unsigned gen = 1u, double cr = .90, double eta_c = 1., double m = 0.02, double param_m = 1., unsigned param_s = 2u, std::string crossover = "exponential", std::string mutation = "polynomial", std::string selection = "tournament", unsigned seed = pagmo::random_device::next())
    const Scalar cr = ResourceMap::GetAsScalar("Pagmo-sga-cr");
    const Scalar eta_c = ResourceMap::GetAsScalar("Pagmo-sga-eta_c");
    const Scalar m = ResourceMap::GetAsScalar("Pagmo-sga-m");
    const Scalar param_m = ResourceMap::GetAsScalar("Pagmo-sga-param_m");
    const UnsignedInteger param_s = ResourceMap::GetAsUnsignedInteger("Pagmo-sga-param_s");
    const String crossover = ResourceMap::GetAsString("Pagmo-sga-crossover");
    const String mutation = ResourceMap::GetAsString("Pagmo-sga-mutation");
    const String selection = ResourceMap::GetAsString("Pagmo-sga-selection");
    algo = pagmo::sga(gen, cr, eta_c, m, param_m, param_s, crossover, mutation, selection);
  }
  else if (algoName_ == "simulated_annealing")
  {
    // simulated_annealing(double Ts = 10., double Tf = .1, unsigned n_T_adj = 10u, unsigned n_range_adj = 1u, unsigned bin_size = 20u, double start_range = 1., unsigned seed = pagmo::random_device::next())
    const Scalar Ts = ResourceMap::GetAsScalar("Pagmo-simulated_annealing-Ts");
    const Scalar Tf = ResourceMap::GetAsScalar("Pagmo-simulated_annealing-Tf");
    const UnsignedInteger n_T_adj = ResourceMap::GetAsUnsignedInteger("Pagmo-simulated_annealing-n_T_adj");
    const UnsignedInteger n_range_adj = ResourceMap::GetAsUnsignedInteger("Pagmo-simulated_annealing-n_range_adj");
    const UnsignedInteger bin_size = ResourceMap::GetAsUnsignedInteger("Pagmo-simulated_annealing-bin_size");
    const Scalar start_range = ResourceMap::GetAsScalar("Pagmo-simulated_annealing-start_range");
    algo = pagmo::simulated_annealing(Ts, Tf, n_T_adj, n_range_adj, bin_size, start_range);
  }
  else if (algoName_ == "bee_colony")
  {
    // bee_colony(unsigned gen = 1u, unsigned limit = 20u, unsigned seed = pagmo::random_device::next())
    const UnsignedInteger limit = ResourceMap::GetAsUnsignedInteger("Pagmo-bee_colony-limit");
    algo = pagmo::bee_colony(gen, limit);
  }
#ifdef PAGMO_WITH_EIGEN3
  else if (algoName_ == "cmaes")
  {
    // cmaes(unsigned gen = 1, double cc = -1, double cs = -1, double c1 = -1, double cmu = -1, double sigma0 = 0.5, double ftol = 1e-6, double xtol = 1e-6, bool memory = false, bool force_bounds = false, unsigned seed = pagmo::random_device::next())
    const Scalar cc = ResourceMap::GetAsScalar("Pagmo-cmaes-cc");
    const Scalar cs = ResourceMap::GetAsScalar("Pagmo-cmaes-cs");
    const Scalar c1 = ResourceMap::GetAsScalar("Pagmo-cmaes-c1");
    const Scalar cmu = ResourceMap::GetAsScalar("Pagmo-cmaes-cmu");
    const Scalar sigma0 = ResourceMap::GetAsScalar("Pagmo-cmaes-sigma0");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    const Bool force_bounds = getProblem().hasBounds();
    pagmo::cmaes algorithm_impl(gen, cc, cs, c1, cmu, sigma0, getMaximumResidualError(), getMaximumAbsoluteError(), memory, force_bounds);
#if PAGMO_VERSION_NR >= 201901
    // requires https://github.com/esa/pagmo2/pull/575
    algorithm_impl.set_bfe(pagmo::bfe{});
#elif PAGMO_VERSION_NR >= 201900
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
#endif
    algo = algorithm_impl;
  }
  else if (algoName_ == "xnes")
  {
    // xnes(unsigned gen = 1, double eta_mu = -1, double eta_sigma = -1, double eta_b = -1, double sigma0 = -1, double ftol = 1e-6, double xtol = 1e-6, bool memory = false, bool force_bounds = false, unsigned seed = pagmo::random_device::next())
    const Scalar eta_mu = ResourceMap::GetAsScalar("Pagmo-xnes-eta_mu");
    const Scalar eta_sigma = ResourceMap::GetAsScalar("Pagmo-xnes-eta_sigma");
    const Scalar eta_b = ResourceMap::GetAsScalar("Pagmo-xnes-eta_b");
    const Scalar sigma0 = ResourceMap::GetAsScalar("Pagmo-xnes-sigma0");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    const Bool force_bounds = getProblem().hasBounds();
    algo = pagmo::xnes(gen, eta_mu, eta_sigma, eta_b, sigma0, getMaximumResidualError(), getMaximumAbsoluteError(), memory, force_bounds);
  }
#endif
  else if (algoName_ == "nsga2")
  {
    const UnsignedInteger reminder = size % 4;
    if (reminder)
    {
      LOGINFO(OSS() << "Pagmo: must drop the last " << reminder << " points of the initial population for NSGA2 as the size (" << size << ") is not a multiple of 4");
      size -= reminder;
      startingSample.split(size);
    }

    // nsga2(unsigned gen = 1u, double cr = 0.95, double eta_c = 10., double m = 0.01, double eta_m = 50., unsigned seed = pagmo::random_device::next())
    const Scalar cr = ResourceMap::GetAsScalar("Pagmo-nsga2-cr");
    const Scalar eta_c = ResourceMap::GetAsScalar("Pagmo-nsga2-eta_c");
    const Scalar m = ResourceMap::GetAsScalar("Pagmo-nsga2-m");
    const Scalar eta_m = ResourceMap::GetAsScalar("Pagmo-nsga2-eta_m");
    pagmo::nsga2 algorithm_impl(gen, cr, eta_c, m, eta_m);
#if PAGMO_VERSION_NR >= 201901
    // requires https://github.com/esa/pagmo2/pull/575
    algorithm_impl.set_bfe(pagmo::bfe{});
#else
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
#endif
    algo = algorithm_impl;
  }
  else if (algoName_ == "moead")
  {
    // moead(unsigned gen = 1u, std::string weight_generation = "grid", std::string decomposition = "tchebycheff", population::size_type neighbours = 20u, double CR = 1.0, double F = 0.5, double eta_m = 20., double realb = 0.9, unsigned limit = 2u, bool preserve_diversity = true, unsigned seed = pagmo::random_device::next())
    const String weight_generation = ResourceMap::GetAsString("Pagmo-moead-weight_generation");
    const String decomposition = ResourceMap::GetAsString("Pagmo-moead-decomposition");
    const UnsignedInteger neighbours = std::min(ResourceMap::GetAsUnsignedInteger("Pagmo-moead-neighbours"), size - 1);
    const Scalar CR = ResourceMap::GetAsScalar("Pagmo-moead-CR");
    const Scalar F = ResourceMap::GetAsScalar("Pagmo-moead-F");
    const Scalar eta_m = ResourceMap::GetAsScalar("Pagmo-moead-eta_m");
    const Scalar realb = ResourceMap::GetAsScalar("Pagmo-moead-realb");
    const UnsignedInteger limit = ResourceMap::GetAsUnsignedInteger("Pagmo-moead-limit");
    const Bool preserve_diversity = ResourceMap::GetAsBool("Pagmo-moead-preserve_diversity");
    algo = pagmo::moead(gen, weight_generation, decomposition, neighbours, CR, F, eta_m, realb, limit, preserve_diversity);
  }
#if PAGMO_VERSION_NR >= 201900
  else if (algoName_ == "moead_gen")
  {
    // moead_gen(unsigned gen = 1u, std::string weight_generation = "grid", std::string decomposition = "tchebycheff", population::size_type neighbours = 20u, double CR = 1.0, double F = 0.5, double eta_m = 20., double realb = 0.9, unsigned limit = 2u, bool preserve_diversity = true, unsigned seed = pagmo::random_device::next())
    const String weight_generation = ResourceMap::GetAsString("Pagmo-moead-weight_generation");
    const String decomposition = ResourceMap::GetAsString("Pagmo-moead-decomposition");
    const UnsignedInteger neighbours = std::min(ResourceMap::GetAsUnsignedInteger("Pagmo-moead-neighbours"), size - 1);
    const Scalar CR = ResourceMap::GetAsScalar("Pagmo-moead-CR");
    const Scalar F = ResourceMap::GetAsScalar("Pagmo-moead-F");
    const Scalar eta_m = ResourceMap::GetAsScalar("Pagmo-moead-eta_m");
    const Scalar realb = ResourceMap::GetAsScalar("Pagmo-moead-realb");
    const UnsignedInteger limit = ResourceMap::GetAsUnsignedInteger("Pagmo-moead-limit");
    const Bool preserve_diversity = ResourceMap::GetAsBool("Pagmo-moead-preserve_diversity");
    pagmo::moead_gen algorithm_impl(gen, weight_generation, decomposition, neighbours, CR, F, eta_m, realb, limit, preserve_diversity);
#if PAGMO_VERSION_NR >= 201901
    // requires https://github.com/esa/pagmo2/pull/575
    algorithm_impl.set_bfe(pagmo::bfe{});
#else
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
#endif
    algo = algorithm_impl;
  }
#endif
  else if (algoName_ == "mhaco")
  {
    // starting population must satisfy constraints
    if (getProblem().hasInequalityConstraint() || getProblem().hasEqualityConstraint())
    {
      Sample startingSampleConstrained(0, getProblem().getDimension());
      Sample ineqOutput;
      if (getProblem().hasInequalityConstraint())
        ineqOutput = getProblem().getInequalityConstraint()(startingSample);
      Sample eqOutput;
      if (getProblem().hasEqualityConstraint())
        eqOutput = getProblem().getEqualityConstraint()(startingSample);
      for (UnsignedInteger i = 0; i < size; ++ i)
      {
        Bool ok = true;
        if (getProblem().hasInequalityConstraint())
          for (UnsignedInteger j = 0; j < ineqOutput.getDimension(); ++ j)
            ok = ok && (ineqOutput(i, j) >= -getMaximumConstraintError());
        if (getProblem().hasEqualityConstraint())
          for (UnsignedInteger j = 0; j < eqOutput.getDimension(); ++ j)
            ok = ok && (std::abs(eqOutput(i, j)) <= getMaximumConstraintError());
        if (ok)
          startingSampleConstrained.add(startingSample[i]);
      }
      if (!startingSampleConstrained.getSize())
        throw InvalidArgumentException(HERE) << "No point in starting population satisfies constraints";
      if (startingSampleConstrained.getSize() < size)
      {
        const RandomGenerator::UnsignedIntegerCollection selection(RandomGenerator::IntegerGenerate(size, startingSampleConstrained.getSize()));
        const Indices indices(selection.begin(), selection.end());
        startingSample = startingSampleConstrained.select(indices);
        LOGINFO(OSS() << "Pagmo: Initial population bootstrapped to satisfy constraints");
      }
    }

    // maco(unsigned gen = 100u, unsigned ker = 63u, double q = 1.0, unsigned threshold = 1u, unsigned n_gen_mark = 7u, unsigned evalstop = 100000u, double focus = 0., bool memory = false, unsigned seed = pagmo::random_device::next())
    UnsignedInteger ker = ResourceMap::GetAsUnsignedInteger("Pagmo-mhaco-ker");
    const Scalar q = ResourceMap::GetAsScalar("Pagmo-mhaco-q");
    const UnsignedInteger threshold = ResourceMap::GetAsUnsignedInteger("Pagmo-mhaco-threshold");
    const UnsignedInteger n_gen_mark = ResourceMap::GetAsUnsignedInteger("Pagmo-mhaco-n_gen_mark");
    const Scalar focus = ResourceMap::GetAsScalar("Pagmo-mhaco-focus");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    if (!memory)
      ker = std::min(ker, size);
    pagmo::maco algorithm_impl(gen, ker, q, threshold, n_gen_mark, getMaximumCallsNumber(), focus, memory);
#if PAGMO_VERSION_NR >= 201901
    // requires https://github.com/esa/pagmo2/pull/575
    algorithm_impl.set_bfe(pagmo::bfe{});
#else
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
#endif
    algo = algorithm_impl;
  }
  else if (algoName_ == "nspso")
  {
    // nspso(unsigned gen = 1u, double omega = 0.6, double c1 = 2.0, double c2 = 2.0, double chi = 1.0, double v_coeff = 0.5, unsigned leader_selection_range = 60u,          std::string diversity_mechanism = "crowding distance", bool memory = false, unsigned seed = pagmo::random_device::next())
    const Scalar omega = ResourceMap::GetAsScalar("Pagmo-nspso-omega");
    const Scalar c1 = ResourceMap::GetAsScalar("Pagmo-nspso-c1");
    const Scalar c2 = ResourceMap::GetAsScalar("Pagmo-nspso-c2");
    const Scalar chi = ResourceMap::GetAsScalar("Pagmo-nspso-chi");
    const Scalar v_coeff = ResourceMap::GetAsScalar("Pagmo-nspso-v_coeff");
    const UnsignedInteger leader_selection_range = ResourceMap::GetAsUnsignedInteger("Pagmo-nspso-leader_selection_range");
    const String diversity_mechanism = ResourceMap::GetAsString("Pagmo-nspso-diversity_mechanism");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    pagmo::nspso algorithm_impl(gen, omega, c1, c2, chi, v_coeff, leader_selection_range, diversity_mechanism, memory);
#if PAGMO_VERSION_NR >= 201901
    // requires https://github.com/esa/pagmo2/pull/575
    algorithm_impl.set_bfe(pagmo::bfe{});
#else
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
#endif
    algo = algorithm_impl;
  }
  else
    throw NotYetImplementedException(HERE) << algoName_;
  algo.set_verbosity(Log::HasDebug());
  algo.set_seed(seed_);

  result_ = OptimizationResult(getProblem());

  // evaluate initial population
  pagmo::population pop(prob);
  // requires https://github.com/esa/pagmo2/pull/575
#if PAGMO_VERSION_NR >= 201901
  const OptimizationProblem problem(getProblem());
  const UnsignedInteger inputDimension = problem.getObjective().getInputDimension();
  const UnsignedInteger blockSize = getBlockSize();
  const UnsignedInteger blockNumber = static_cast<UnsignedInteger>(ceil(1.0 * size / blockSize));
  for (UnsignedInteger outerSampling = 0; outerSampling < blockNumber; ++ outerSampling)
  {
    const UnsignedInteger effectiveBlockSize = ((outerSampling == (blockNumber - 1)) && (size % blockSize)) ? (size % blockSize) : blockSize;
    Sample inSb(effectiveBlockSize, inputDimension);
    for (UnsignedInteger i = 0; i < effectiveBlockSize; ++ i)
      inSb[i] = pproblem.renumber(startingSample[i + outerSampling * blockSize], false);
    const pagmo::vector_double inV(inSb.getImplementation()->getData().toStdVector());
    const pagmo::vector_double outV(prob.batch_fitness(inV));
    const UnsignedInteger nf = outV.size() / effectiveBlockSize;
    for (UnsignedInteger i = 0; i < effectiveBlockSize; ++ i)
    {
      const pagmo::vector_double inVi(inV.begin() + i * inputDimension, inV.begin() + (i + 1) * inputDimension);
      const pagmo::vector_double outVi(outV.begin() + i * nf, outV.begin() + (i + 1) * nf);
      pop.push_back(inVi, outVi);
    }
  }
#else
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Point inP(startingSample[i]);
    pagmo::vector_double x(pproblem.renumber(inP, false).toStdVector());
    pagmo::vector_double y(prob.fitness(x));
    pop.push_back(x, y);
  }
#endif

  // update result after initial population evaluation
  result_.setFinalPoints(evaluationInputHistory);
  result_.setFinalValues(evaluationOutputHistory);
  if (incrementalEvolution_ && (getProblem().getObjective().getOutputDimension() > 1))
  {
    // retrieve non-penalized output values instead of using pop.get_f
    std::vector<std::vector<double> > popf;
    for (UnsignedInteger i = 0; i < evaluationOutputHistory.getSize(); ++ i)
    {
      Point outP(evaluationOutputHistory[i]);
      for (UnsignedInteger j = 0; j < outP.getDimension(); ++ j)
        if (!getProblem().isMinimization(j))
          outP[j] *= -1.0;
      popf.push_back(outP.toStdVector());
    }
    // compute the fronts
    std::vector<std::vector<pagmo::pop_size_t> > fronts(std::get<0>(pagmo::fast_non_dominated_sorting(popf)));
    Collection<Indices> frontIndices(fronts.size());
    for (UnsignedInteger i = 0; i < fronts.size(); ++ i)
      frontIndices[i] = Indices(fronts[i].begin(), fronts[i].end());
    result_.setParetoFrontsIndices(IndicesCollection(frontIndices));
  }

  // evolve initial population over several generations
  const UnsignedInteger ngen = incrementalEvolution_ ? getMaximumIterationNumber() : 1;
  for (UnsignedInteger igen = 1; igen <= ngen; ++ igen)
  {
    pop = algo.evolve(pop);

    result_.setCallsNumber(evaluationInputHistory.getSize());
    result_.setIterationNumber(igen);
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0).count();
    result_.setTimeDuration(timeDuration);

    Sample finalPoints(0, getProblem().getDimension());

    // retrieve final population
    for (UnsignedInteger i = 0; i < pop.size(); ++ i)
    {
      const pagmo::vector_double x(pop.get_x()[i]);
      const Point inP(pproblem.renumber(Point(x.begin(), x.end())));
      finalPoints.add(inP);
    }

    // filter according to constraints
    if (getProblem().hasInequalityConstraint() || getProblem().hasEqualityConstraint())
    {
      Sample finalPointsConstrained(0, getProblem().getDimension());
      Sample ineqOutput;
      if (getProblem().hasInequalityConstraint())
      {
        const DatabaseFunction xToIneq(evaluationInputHistory, inequalityHistory);
        ineqOutput = xToIneq(finalPoints);
      }
      Sample eqOutput;
      if (getProblem().hasEqualityConstraint())
      {
        const DatabaseFunction xToEq(evaluationInputHistory, equalityHistory);
        eqOutput = xToEq(finalPoints);
      }
      for (UnsignedInteger i = 0; i < pop.size(); ++ i)
      {
        Bool ok = true;
        if (getProblem().hasInequalityConstraint())
          for (UnsignedInteger j = 0; j < ineqOutput.getDimension(); ++ j)
            ok = ok && (ineqOutput(i, j) >= -getMaximumConstraintError());
        if (getProblem().hasEqualityConstraint())
          for (UnsignedInteger j = 0; j < eqOutput.getDimension(); ++ j)
            ok = ok && (std::abs(eqOutput(i, j)) <= getMaximumConstraintError());
        if (ok)
          finalPointsConstrained.add(finalPoints[i]);
      }
      // we still want to return something in case there are no feasible points
      if (finalPointsConstrained.getSize())
        finalPoints = finalPointsConstrained;
    }

    // we want to retrieve evaluations before penalization to avoid MaxScalar values
    const DatabaseFunction xToY(evaluationInputHistory, evaluationOutputHistory);
    const Sample finalValues(xToY(finalPoints));
    result_.setFinalPoints(finalPoints);
    result_.setFinalValues(finalValues);

    if (getProblem().getObjective().getOutputDimension() > 1)
    {
      // retrieve non-penalized output values instead of using pop.get_f
      std::vector<std::vector<double> > popf;
      for (UnsignedInteger i = 0; i < finalValues.getSize(); ++ i)
      {
        Point outP(finalValues[i]);
        for (UnsignedInteger j = 0; j < outP.getDimension(); ++ j)
          if (!getProblem().isMinimization(j))
            outP[j] *= -1.0;
        popf.push_back(outP.toStdVector());
      }
      // compute the fronts
      std::vector<std::vector<pagmo::pop_size_t> > fronts(std::get<0>(pagmo::fast_non_dominated_sorting(popf)));
      Collection<Indices> frontIndices(fronts.size());
      for (UnsignedInteger i = 0; i < fronts.size(); ++ i)
        frontIndices[i] = Indices(fronts[i].begin(), fronts[i].end());
      result_.setParetoFrontsIndices(IndicesCollection(frontIndices));
    }
  }
#else
  throw NotYetImplementedException(HERE) << "No Pagmo support";
#endif
}

/* Virtual constructor */
Pagmo * Pagmo::clone() const
{
  return new Pagmo(*this);
}

/* String converter */
String Pagmo::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " algorithm=" << getAlgorithmName()
      << " " << OptimizationAlgorithmImplementation::__repr__()
      << " startingSample=" << startingSample_;
  return oss;
}

/* Useless inherited method: throw */
void Pagmo::setStartingPoint(const Point &)
{
  throw NotDefinedException(HERE) << "setStartingPoint makes no sense in a Pagmo context";
}

/* Useless inherited method: throw */
Point Pagmo::getStartingPoint() const
{
  throw NotDefinedException(HERE) << "getStartingPoint makes no sense in a Pagmo context";
}


/* Starting sample accessor */
void Pagmo::setStartingSample(const Sample & startingSample)
{
  startingSample_ = startingSample;
}

/* Starting points accessor */
Sample Pagmo::getStartingSample() const
{
  return startingSample_;
}

Description Pagmo::GetAlgorithmNames()
{
  const Description algoNames = {"gaco", "de", "sade", "de1220", "gwo", "ihs", "pso", "pso_gen", "sea",
                                 "sga", "simulated_annealing", "bee_colony",
#ifdef PAGMO_WITH_EIGEN3
                                 "cmaes", "xnes",
#endif
                                 "nsga2", "moead",
#if PAGMO_VERSION_NR >= 201900
                                 "moead_gen",
#endif
                                 "mhaco", "nspso"
                                };
  return algoNames;
}

/** Accessors to Bonmin attributes */
void Pagmo::setAlgorithmName(const String & algoName)
{
  // Check algoName
  if (!GetAlgorithmNames().contains(algoName))
    throw InvalidArgumentException(HERE) << "Unknown solver " << algoName;
  algoName_ = algoName;
}

String Pagmo::getAlgorithmName() const
{
  return algoName_;
}

/* Random generator seed accessor */
void Pagmo::setSeed(const UnsignedInteger seed)
{
  seed_ = seed;
}

UnsignedInteger Pagmo::getSeed() const
{
  return seed_;
}

/* Block size accessor */
void Pagmo::setBlockSize(const UnsignedInteger blockSize)
{
  blockSize_ = blockSize;
}

UnsignedInteger Pagmo::getBlockSize() const
{
  return blockSize_;
}

/* Incremental mode accessor */
void Pagmo::setIncrementalEvolution(Bool incrementalEvolution)
{
  incrementalEvolution_ = incrementalEvolution;
}

Bool Pagmo::getIncrementalEvolution() const
{
  return incrementalEvolution_;
}

/* Method save() stores the object through the StorageManager */
void Pagmo::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("algoName_", algoName_);
  adv.saveAttribute("startingSample_", startingSample_);
  adv.saveAttribute("seed_", seed_);
  adv.saveAttribute("blockSize_", blockSize_);
  adv.saveAttribute("incrementalEvolution_", incrementalEvolution_);
}

/* Method load() reloads the object from the StorageManager */
void Pagmo::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  adv.loadAttribute("algoName_", algoName_);
  adv.loadAttribute("startingSample_", startingSample_);
  if (adv.hasAttribute("generationNumber_")) // OT<=1.21
  {
    UnsignedInteger generationNumber = 0;
    adv.loadAttribute("generationNumber_", generationNumber);
    setMaximumIterationNumber(generationNumber);
  }
  adv.loadAttribute("seed_", seed_);
  adv.loadAttribute("blockSize_", blockSize_);
  if (adv.hasAttribute("incrementalEvolution_")) // OT>=1.26
    adv.loadAttribute("incrementalEvolution_", incrementalEvolution_);
}

END_NAMESPACE_OPENTURNS
