//                                               -*- C++ -*-
/**
 *  @brief Pagmo optimization algorithm
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SpecFunc.hxx"
#include "openturns/OTconfig.hxx"
#include "openturns/DatabaseFunction.hxx"

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

  PagmoProblem(const Pagmo * algorithm)
    : algorithm_(algorithm)
  {
    if (!algorithm)
      throw InvalidArgumentException(HERE) << "Null algo";

    // pagmo wants the integer components grouped at the end, so renumbering is in order
    Indices renum;
    Indices renum_inv;
    const Indices types(algorithm_->getProblem().getVariablesType());
    for (UnsignedInteger i = 0; i < types.getSize(); ++ i)
      if (types[i] == OptimizationProblemImplementation::CONTINUOUS)
        renum.add(i);
    for (UnsignedInteger i = 0; i < types.getSize(); ++ i)
      if (types[i] != OptimizationProblemImplementation::CONTINUOUS)
        renum.add(i);
    Indices ordinal(types.getSize());
    ordinal.fill();
    if (renum != ordinal)
      renum_ = renum;

    evaluationInputHistory_ = Sample(0, algorithm->getProblem().getDimension());
    evaluationOutputHistory_ = Sample(0, algorithm->getProblem().getObjective().getOutputDimension());
  }

  Point renumber(const Point & inP) const
  {
    Point result(inP);
    if (renum_.getSize())
    {
      for (UnsignedInteger i = 0; i < inP.getDimension(); ++ i)
        result[i] = inP[renum_[i]];
    }
    return result;
  }

  pagmo::vector_double fitness(const pagmo::vector_double & inv) const
  {
    const Point inP(renumber(Point(inv.begin(), inv.end())));
    evaluationInputHistory_.add(inP);
    Point outP(algorithm_->getProblem().getObjective()(inP));
    evaluationOutputHistory_.add(outP);
    for (UnsignedInteger i = 0; i < outP.getDimension(); ++ i)
      if (!algorithm_->getProblem().isMinimization(i))
        outP[i] *= -1.0;
    ++ evaluationNumber_;
    if (algorithm_->getProblem().hasEqualityConstraint())
      outP.add(algorithm_->getProblem().getEqualityConstraint()(inP));
    if (algorithm_->getProblem().hasInequalityConstraint())
      outP.add(-1.0 * algorithm_->getProblem().getInequalityConstraint()(inP));// opposite convention for ineq constraints

    // callbacks
    if (algorithm_->progressCallback_.first)
    {
      algorithm_->progressCallback_.first((100.0 * evaluationNumber_) / (algorithm_->getStartingSample().getSize() * algorithm_->getGenerationNumber()), algorithm_->progressCallback_.second);
    }
    if (algorithm_->stopCallback_.first)
    {
      const Bool stop = algorithm_->stopCallback_.first(algorithm_->stopCallback_.second);
      if (stop)
        throw InternalException(HERE) << "User stopped optimization";
    }
    return outP.toStdVector();
  }

  std::pair<pagmo::vector_double, pagmo::vector_double> get_bounds() const
  {
    const pagmo::vector_double lb2(renumber(algorithm_->getProblem().getBounds().getLowerBound()).toStdVector());
    const pagmo::vector_double ub2(renumber(algorithm_->getProblem().getBounds().getUpperBound()).toStdVector());
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
    Sample outS(0, totalDimension);
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
      evaluationInputHistory_.add(inSb);
      Sample outSb(problem.getObjective()(inSb));
      evaluationOutputHistory_.add(outSb);
      for (UnsignedInteger i = 0; i < effectiveBlockSize; ++ i)
        for (UnsignedInteger j = 0; j < outputDimension; ++ j)
          if (!problem.isMinimization(j))
            outSb(i, j) *= -1.0;
      if (problem.hasEqualityConstraint())
        outSb.stack(problem.getEqualityConstraint()(inSb));
      if (problem.hasInequalityConstraint())
        outSb.stack(-1.0 * problem.getInequalityConstraint()(inSb));
      outS.add(outSb);
      offset += blockSize * inputDimension;
      evaluationNumber_ += effectiveBlockSize;

      // callbacks
      if (algorithm_->progressCallback_.first)
      {
        algorithm_->progressCallback_.first((100.0 * evaluationNumber_) / (algorithm_->getStartingSample().getSize() * algorithm_->getGenerationNumber()), algorithm_->progressCallback_.second);
      }
      if (algorithm_->stopCallback_.first)
      {
        const Bool stop = algorithm_->stopCallback_.first(algorithm_->stopCallback_.second);
        if (stop)
          throw InternalException(HERE) << "User stopped optimization";
      }
    }
    return outS.getImplementation()->getData().toStdVector();
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

  static UnsignedInteger evaluationNumber_;
  static Sample evaluationInputHistory_;
  static Sample evaluationOutputHistory_;
  const Pagmo * algorithm_ = 0;
  Indices renum_;
};

UnsignedInteger PagmoProblem::evaluationNumber_ = 0;
Sample PagmoProblem::evaluationInputHistory_;
Sample PagmoProblem::evaluationOutputHistory_;
#endif


CLASSNAMEINIT(Pagmo)

static const Factory<Pagmo> Factory_Pagmo;

/* Default constructor */
Pagmo::Pagmo(const String & algoName)
  : OptimizationAlgorithmImplementation()
{
  setAlgorithmName(algoName);
}


/* Parameters constructor */
Pagmo::Pagmo(const OptimizationProblem & problem,
             const String & algoName,
             const Sample & startingSample)
  : OptimizationAlgorithmImplementation()
  , startingSample_(startingSample)
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
  const Description multiObjectiveAgorithms = {"nsga2", "moead", "mhaco", "nspso"};
  if ((problem.getObjective().getOutputDimension() > 1) && !multiObjectiveAgorithms.contains(getAlgorithmName()))
    throw InvalidArgumentException(HERE) << getAlgorithmName() << " does not support multi-objective optimization";
  if ((problem.getObjective().getOutputDimension() < 2) && multiObjectiveAgorithms.contains(getAlgorithmName()))
    throw InvalidArgumentException(HERE) << getAlgorithmName() << " only supports multi-objective optimization";
  const Description integerAgorithms = {"gaco", "ihs", "sga", "nsga2", "mhaco"};
  if (!problem.isContinuous() && !integerAgorithms.contains(getAlgorithmName()))
    throw InvalidArgumentException(HERE) << getAlgorithmName() << " does not support non continuous problems";
#else
  (void)problem;
  throw InvalidArgumentException(HERE) << "No Pagmo support";
#endif
}


void Pagmo::run()
{
  if (!startingSample_.getSize())
    throw InvalidArgumentException(HERE) << "Starting sample is empty";
  if (startingSample_.getDimension() != getProblem().getDimension())
    throw InvalidArgumentException(HERE) << "Starting sample dimension does not match problem dimension";
  for (UnsignedInteger i = 0; i < startingSample_.getSize(); ++ i)
  {
    const Point inP(startingSample_[i]);
    if (!getProblem().getBounds().contains(inP))
      LOGWARN(OSS() << "Starting point " << i << " lies outside bounds");
    if (!getProblem().isContinuous())
    {
      const Indices types(getProblem().getVariablesType());
      for (UnsignedInteger j = 0; j < getProblem().getDimension(); ++ j)
      {
        if (types[j] != OptimizationProblemImplementation::CONTINUOUS && std::trunc(inP[j]) != inP[j])
          throw InvalidArgumentException(HERE) << "Starting sample components must have integral values";
        if (types[j] == OptimizationProblemImplementation::BINARY && (std::round(inP[j]) != 0.0) && (std::round(inP[j]) != 1.0))
          throw InvalidArgumentException(HERE) << "Starting sample components must have binary values";
      }
    }
  }

#ifdef OPENTURNS_HAVE_PAGMO
  PagmoProblem pproblem(this);
  pagmo::problem prob(pproblem);
  const pagmo::vector_double ctol(prob.get_nc(), getMaximumConstraintError());
  prob.set_c_tol(ctol);
  const Description constrainedAgorithms = {"gaco", "ihs"};
  Bool emulatedConstraints = false;
  if ((getProblem().hasInequalityConstraint() || getProblem().hasEqualityConstraint()) && !constrainedAgorithms.contains(getAlgorithmName()))
  {
    emulatedConstraints = true;
    const String unconstrainMethod = ResourceMap::GetAsString("Pagmo-UnconstrainMethod");
    prob = pagmo::unconstrain(prob, unconstrainMethod);
  }
  pagmo::population pop(prob, 0, 0);

  // nsga2 needs the population size to be a multiple of 4
  UnsignedInteger populationSize = startingSample_.getSize();
  if ((algoName_ == "nsga2") && (populationSize % 4))
  {
    LOGINFO(OSS() << "Pagmo: must drop the last " << (populationSize % 4) << " points of the initial population for NSGA2 as the size=" << populationSize << " is not a multiple of 4");
    populationSize = 4 * (populationSize / 4);
  }
  for (UnsignedInteger i = 0; i < populationSize; ++ i)
  {
    const Point inP(startingSample_[i]);
    pagmo::vector_double x(pproblem.renumber(inP).toStdVector());
    pop.push_back(x);
  }
  pagmo::algorithm algo;
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
      ker = std::min(ker, populationSize);
    pagmo::gaco algorithm_impl(generationNumber_, ker, q, oracle, acc, threshold, n_gen_mark, impstop, getMaximumEvaluationNumber(), focus, memory);
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
    algo = algorithm_impl;
  }
  else if (algoName_ == "de")
  {
    // de(unsigned gen = 1u, double F = 0.8, double CR = 0.9, unsigned variant = 2u, double ftol = 1e-6, double xtol = 1e-6, unsigned seed = pagmo::random_device::next())
    const Scalar F = ResourceMap::GetAsScalar("Pagmo-de-F");
    const Scalar CR = ResourceMap::GetAsScalar("Pagmo-de-CR");
    const UnsignedInteger variant = ResourceMap::GetAsUnsignedInteger("Pagmo-de-variant");
    algo = pagmo::de(generationNumber_, F, CR, variant, getMaximumResidualError(), getMaximumAbsoluteError());
  }
  else if (algoName_ == "sade")
  {
    // sade(unsigned gen = 1u, unsigned variant = 2u, unsigned variant_adptv = 1u, double ftol = 1e-6, double xtol = 1e-6, bool memory = false, unsigned seed = pagmo::random_device::next())
    const UnsignedInteger variant = ResourceMap::GetAsUnsignedInteger("Pagmo-sade-variant");
    const UnsignedInteger variant_adptv = ResourceMap::GetAsUnsignedInteger("Pagmo-sade-variant_adptv");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    algo = pagmo::sade(generationNumber_, variant, variant_adptv, getMaximumResidualError(), getMaximumAbsoluteError(), memory);
  }
  else if (algoName_ == "de1220")
  {
    // de1220(unsigned gen = 1u, std::vector<unsigned> allowed_variants = de1220_statics<void>::allowed_variants, unsigned variant_adptv = 1u, double ftol = 1e-6, double xtol = 1e-6, bool memory = false, unsigned seed = pagmo::random_device::next())
    const UnsignedInteger variant_adptv = ResourceMap::GetAsUnsignedInteger("Pagmo-de1220-variant_adptv");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    algo = pagmo::de1220(generationNumber_, pagmo::de1220_statics<void>::allowed_variants, variant_adptv, getMaximumResidualError(), getMaximumAbsoluteError(), memory);
  }
  else if (algoName_ == "gwo")
  {
    // gwo(unsigned gen = 1u, unsigned seed = pagmo::random_device::next())
    algo = pagmo::gwo(generationNumber_);
  }
  else if (algoName_ == "ihs")
  {
    // ihs(unsigned gen = 1u, double phmcr = 0.85, double ppar_min = 0.35, double ppar_max = 0.99, double bw_min = 1E-5, double bw_max = 1., unsigned seed = pagmo::random_device::next())
    const Scalar phmcr = ResourceMap::GetAsScalar("Pagmo-ihs-phmcr");
    const Scalar ppar_min = ResourceMap::GetAsScalar("Pagmo-ihs-ppar_min");
    const Scalar ppar_max = ResourceMap::GetAsScalar("Pagmo-ihs-ppar_max");
    const Scalar bw_min = ResourceMap::GetAsScalar("Pagmo-ihs-bw_min");
    const Scalar bw_max = ResourceMap::GetAsScalar("Pagmo-ihs-bw_max");
    algo = pagmo::ihs(generationNumber_, phmcr, ppar_min, ppar_max, bw_min, bw_max);
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
    algo = pagmo::pso(generationNumber_, omega, eta1, eta2, max_vel, variant, neighb_type, neighb_param, memory);
  }
  else if (algoName_ == "pso_gen")
  {
    // pso_gen(unsigned gen = 1u, double omega = 0.7298, double eta1 = 2.05, double eta2 = 2.05, double max_vel = 0.5, unsigned variant = 5u, unsigned neighb_type = 2u, unsigned neighb_param = 4u, bool memory = false, unsigned seed = pagmo::random_device::next())
    const Scalar omega = ResourceMap::GetAsScalar("Pagmo-pso_gen-omega");
    const Scalar eta1 = ResourceMap::GetAsScalar("Pagmo-pso_gen-eta1");
    const Scalar eta2 = ResourceMap::GetAsScalar("Pagmo-pso_gen-eta2");
    const Scalar max_vel = ResourceMap::GetAsScalar("Pagmo-pso_gen-max_vel");
    const UnsignedInteger variant = ResourceMap::GetAsUnsignedInteger("Pagmo-pso_gen-variant");
    const UnsignedInteger neighb_type = ResourceMap::GetAsUnsignedInteger("Pagmo-pso_gen-neighb_type");
    const UnsignedInteger neighb_param = ResourceMap::GetAsUnsignedInteger("Pagmo-pso_gen-neighb_param");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    pagmo::pso_gen algorithm_impl(generationNumber_, omega, eta1, eta2, max_vel, variant, neighb_type, neighb_param, memory);
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
    algo = algorithm_impl;
  }
  else if (algoName_ == "sea")
  {
    // sea(unsigned gen = 1u, unsigned seed = pagmo::random_device::next())
    algo = pagmo::sea(generationNumber_);
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
    algo = pagmo::sga(generationNumber_, cr, eta_c, m, param_m, param_s, crossover, mutation, selection);
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
    algo = pagmo::bee_colony(generationNumber_, limit);
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
    algo = pagmo::cmaes(generationNumber_, cc, cs, c1, cmu, sigma0, getMaximumResidualError(), getMaximumAbsoluteError(), memory, getProblem().hasBounds());
  }
  else if (algoName_ == "xnes")
  {
    // xnes(unsigned gen = 1, double eta_mu = -1, double eta_sigma = -1, double eta_b = -1, double sigma0 = -1, double ftol = 1e-6, double xtol = 1e-6, bool memory = false, bool force_bounds = false, unsigned seed = pagmo::random_device::next())
    const Scalar eta_mu = ResourceMap::GetAsScalar("Pagmo-xnes-eta_mu");
    const Scalar eta_sigma = ResourceMap::GetAsScalar("Pagmo-xnes-eta_sigma");
    const Scalar eta_b = ResourceMap::GetAsScalar("Pagmo-xnes-eta_b");
    const Scalar sigma0 = ResourceMap::GetAsScalar("Pagmo-xnes-sigma0");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    algo = pagmo::xnes(generationNumber_, eta_mu, eta_sigma, eta_b, sigma0, getMaximumResidualError(), getMaximumAbsoluteError(), memory, getProblem().hasBounds());
  }
#endif
  else if (algoName_ == "nsga2")
  {
    // nsga2(unsigned gen = 1u, double cr = 0.95, double eta_c = 10., double m = 0.01, double eta_m = 50., unsigned seed = pagmo::random_device::next())
    const Scalar cr = ResourceMap::GetAsScalar("Pagmo-nsga2-cr");
    const Scalar eta_c = ResourceMap::GetAsScalar("Pagmo-nsga2-eta_c");
    const Scalar m = ResourceMap::GetAsScalar("Pagmo-nsga2-m");
    const Scalar eta_m = ResourceMap::GetAsScalar("Pagmo-nsga2-eta_m");
    pagmo::nsga2 algorithm_impl(generationNumber_, cr, eta_c, m, eta_m);
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
    algo = algorithm_impl;
  }
  else if (algoName_ == "moead")
  {
    // moead(unsigned gen = 1u, std::string weight_generation = "grid", std::string decomposition = "tchebycheff", population::size_type neighbours = 20u, double CR = 1.0, double F = 0.5, double eta_m = 20., double realb = 0.9, unsigned limit = 2u, bool preserve_diversity = true, unsigned seed = pagmo::random_device::next())
    const String weight_generation = ResourceMap::GetAsString("Pagmo-moead-weight_generation");
    const String decomposition = ResourceMap::GetAsString("Pagmo-moead-decomposition");
    const UnsignedInteger neighbours = ResourceMap::GetAsUnsignedInteger("Pagmo-moead-neighbours");
    const Scalar CR = ResourceMap::GetAsScalar("Pagmo-moead-CR");
    const Scalar F = ResourceMap::GetAsScalar("Pagmo-moead-F");
    const Scalar eta_m = ResourceMap::GetAsScalar("Pagmo-moead-eta_m");
    const Scalar realb = ResourceMap::GetAsScalar("Pagmo-moead-realb");
    const UnsignedInteger limit = ResourceMap::GetAsUnsignedInteger("Pagmo-moead-limit");
    const Bool preserve_diversity = ResourceMap::GetAsBool("Pagmo-moead-preserve_diversity");
    algo = pagmo::moead(generationNumber_, weight_generation, decomposition, neighbours, CR, F, eta_m, realb, limit, preserve_diversity);
  }
  else if (algoName_ == "mhaco")
  {
    // maco(unsigned gen = 100u, unsigned ker = 63u, double q = 1.0, unsigned threshold = 1u, unsigned n_gen_mark = 7u, unsigned evalstop = 100000u, double focus = 0., bool memory = false, unsigned seed = pagmo::random_device::next())
    UnsignedInteger ker = ResourceMap::GetAsUnsignedInteger("Pagmo-mhaco-ker");
    const Scalar q = ResourceMap::GetAsScalar("Pagmo-mhaco-q");
    const UnsignedInteger threshold = ResourceMap::GetAsUnsignedInteger("Pagmo-mhaco-threshold");
    const UnsignedInteger n_gen_mark = ResourceMap::GetAsUnsignedInteger("Pagmo-mhaco-n_gen_mark");
    const Scalar focus = ResourceMap::GetAsScalar("Pagmo-mhaco-focus");
    const Bool memory = ResourceMap::GetAsBool("Pagmo-memory");
    if (!memory)
      ker = std::min(ker, populationSize);
    pagmo::maco algorithm_impl(generationNumber_, ker, q, threshold, n_gen_mark, getMaximumEvaluationNumber(), focus, memory);
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
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
    pagmo::nspso algorithm_impl(generationNumber_, omega, c1, c2, chi, v_coeff, leader_selection_range, diversity_mechanism, memory);
    if (!emulatedConstraints)
      algorithm_impl.set_bfe(pagmo::bfe{});
    algo = algorithm_impl;
  }
  else
    throw NotYetImplementedException(HERE) << algoName_;
  algo.set_verbosity(getVerbose());
  algo.set_seed(seed_);
  PagmoProblem::evaluationNumber_ = 0;
  pop = algo.evolve(pop);
  result_ = OptimizationResult(getProblem());
  result_.setEvaluationNumber(PagmoProblem::evaluationNumber_);
  Scalar optimalValue = 0.0;
  Point optimalPoint;
  Sample finalPoints(0, getProblem().getDimension());

  // retrieve final population
  for (UnsignedInteger i = 0; i < pop.size(); ++ i)
  {
    const pagmo::vector_double x(pop.get_x()[i]);
    const Point inP(pproblem.renumber(Point(x.begin(), x.end())));
    finalPoints.add(inP);
  }
  // we want to retrieve evaluations before penalization to avoid MaxScalar values
  const DatabaseFunction xToY(PagmoProblem::evaluationInputHistory_,
                              PagmoProblem::evaluationOutputHistory_);
  const Sample finalValues(xToY(finalPoints));
  result_.setFinalPoints(finalPoints);
  result_.setFinalValues(finalValues);

  const UnsignedInteger objectiveDimension = getProblem().getObjective().getOutputDimension();
  if (objectiveDimension == 1)
  {
    Point optimalPoint;
    for (UnsignedInteger i = 0; i < finalPoints.getSize(); ++ i)
    {
      const Point inP(finalPoints[i]);
      const Point outP(finalValues[i]);
      if (i == 0)
      {
        optimalPoint = inP;
        optimalValue = outP[0];
      }
      if ((getProblem().isMinimization() && (outP[0] < optimalValue))
          || (!getProblem().isMinimization() && (outP[0] > optimalValue)))
      {
        optimalPoint = inP;
        optimalValue = outP[0];
      }
    }
    result_.setOptimalPoint(optimalPoint);
    result_.setOptimalValue(Point(1, optimalValue));
  }
  else
  {
    // retrieve non-penalized output values instead of using pop.get_f
    std::vector<std::vector<double> > popf;
    for (UnsignedInteger i = 0; i < finalPoints.getSize(); ++ i)
      popf.push_back(Point(finalPoints[i]).toStdVector());
    // compute the fronts
    std::vector<std::vector<pagmo::pop_size_t> > fronts(std::get<0>(pagmo::fast_non_dominated_sorting(popf)));
    Collection<Indices> frontIndices(fronts.size());
    for (UnsignedInteger i = 0; i < fronts.size(); ++ i)
      frontIndices[i] = Indices(fronts[i].begin(), fronts[i].end());
    result_.setParetoFrontsIndices(IndicesCollection(frontIndices));
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
//   checkStartingSampleConsistentWithOptimizationProblem(startingSample, getProblem());
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
                                 "nsga2", "moead", "mhaco", "nspso"
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

/* Number of generations to evolve */
void Pagmo::setGenerationNumber(const UnsignedInteger generationNumber)
{
  generationNumber_ = generationNumber;
}

UnsignedInteger Pagmo::getGenerationNumber() const
{
  return generationNumber_;
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

/* Method save() stores the object through the StorageManager */
void Pagmo::save(Advocate & adv) const
{
  OptimizationAlgorithmImplementation::save(adv);
  adv.saveAttribute("algoName_", algoName_);
  adv.saveAttribute("startingSample_", startingSample_);
  adv.saveAttribute("generationNumber_", generationNumber_);
  adv.saveAttribute("seed_", seed_);
  adv.saveAttribute("blockSize_", blockSize_);
}

/* Method load() reloads the object from the StorageManager */
void Pagmo::load(Advocate & adv)
{
  OptimizationAlgorithmImplementation::load(adv);
  adv.loadAttribute("algoName_", algoName_);
  adv.loadAttribute("startingSample_", startingSample_);
  adv.loadAttribute("generationNumber_", generationNumber_);
  adv.loadAttribute("seed_", seed_);
  adv.loadAttribute("blockSize_", blockSize_);
}

END_NAMESPACE_OPENTURNS
