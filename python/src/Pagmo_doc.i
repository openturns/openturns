%feature("docstring") OT::Pagmo
"Pagmo algorithms.

This class exposes bio-inspired and evolutionary global optimization algorithms
from the `Pagmo <https://esa.github.io/pagmo2/>`_ library.
These algorithms start from an initial population and make it evolve to obtain
a final population after a defined number of generations (by :meth:`setMaximumIterationNumber`).
A few of these algorithms allow for multi-objective optimization, and in that
case the result is not the best point among the final population but a set of
dominant points: a pareto front.

Parameters
----------
problem : :class:`~openturns.OptimizationProblem`
    Optimization problem to solve
algoName : str, default='gaco'
    Identifier of the optimization method to use.
startingSample : 2-d sequence of float, optional
    Initial population

Notes
-----
The total number of evaluations is the size of the initial population multiplied by the iteration number plus one.
Starting points provided through the *startingSample* parameter should be
within the bounds of the :class:`~openturns.OptimizationProblem`, but this is not enforced.

Pagmo provides the following global `heuristics <https://esa.github.io/pagmo2/overview.html#list-of-algorithms>`_:

+---------------------+-------------------------------------------------+-----------------+-------+-------+
| Algorithm           | Description                                     | Multi-objective | MINLP | Batch |
+=====================+=================================================+=================+=======+=======+
| gaco                | Extended Ant Colony Optimization                | no              | yes   | yes   |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| de                  | Differential Evolution                          | no              | no    | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| sade                | Self-adaptive DE (jDE and iDE)                  | no              | no    | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| de1220              | Self-adaptive DE (de_1220 aka pDE)              | no              | no    | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| gwo                 | Grey wolf optimizer                             | no              | no    | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| ihs                 | Improved Harmony Search                         | no              | yes   | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| pso                 | Particle Swarm Optimization                     | no              | no    | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| pso_gen             | Particle Swarm Optimization Generational        | no              | no    | yes   |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| sea                 | (N+1)-ES Simple Evolutionary Algorithm          | no              | no    | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| sga                 | Simple Genetic Algorithm                        | no              | yes   | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| simulated_annealing | Corana's Simulated Annealing                    | no              | no    | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| bee_colony          | Artificial Bee Colony                           | no              | no    | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| cmaes               | Covariance Matrix Adaptation Evo. Strategy      | no              | no    | yes   |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| xnes                | Exponential Evolution Strategies                | no              | no    | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| nsga2               | Non-dominated Sorting GA                        | yes             | yes   | yes   |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| moead               | Multi-objective EA with Decomposition           | yes             | no    | no    |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| moead_gen           | Multi-objective EA with Decomposition Gen.      | yes             | no    | yes   |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| mhaco               | Multi-objective Hypervolume-based ACO           | yes             | yes   | yes   |
+---------------------+-------------------------------------------------+-----------------+-------+-------+
| nspso               | Non-dominated Sorting PSO                       | yes             | no    | yes   |
+---------------------+-------------------------------------------------+-----------------+-------+-------+

Only gaco and ihs natively support constraints, but for the other algorithms
constraints are emulated through penalization.
For mhaco, the initial population must satisfy constraints, else it is built
by bootstrap on valid points with the same population size as the one provided.
Some algorithms support batch evaluation, see :meth:`setBlockSize`.
Default parameters are available in the :class:`~openturns.ResourceMap` for
each algorithm, refer to the correspondings keys in the Pagmo `documentation <https://esa.github.io/pagmo2/>`_.

This class uses the following entries of :class:`~openturns.ResourceMap`:

- *Pagmo-InitialSeed* (:raw:`UnsignedInteger`, default: `0`): seed for the random number generator.
- *Pagmo-memory* (:raw:`Bool`, default: `false`): whether to use memory in the algorithm.
- *Pagmo-DefaultIncrementalEvolution* (:raw:`Bool`, default: `false`): default incremental evolution mode.
- *Pagmo-gaco-ker* (:raw:`UnsignedInteger`, default: `63`): number of ants to evaluate the solution.
- *Pagmo-gaco-q* (:raw:`Scalar`, default: `1.0`): input parameter for the pheromone evaluation.
- *Pagmo-gaco-oracle* (:raw:`Scalar`, default: `0.0`): oracle parameter for the GACO algorithm.
- *Pagmo-gaco-acc* (:raw:`Scalar`, default: `0.01`): convergence speed parameter.
- *Pagmo-gaco-threshold* (:raw:`UnsignedInteger`, default: `1`): threshold for the pheromone.
- *Pagmo-gaco-n_gen_mark* (:raw:`UnsignedInteger`, default: `7`): number of generations for the convergence check.
- *Pagmo-gaco-impstop* (:raw:`UnsignedInteger`, default: `100000`): number of generations without improvement before stopping.
- *Pagmo-gaco-focus* (:raw:`Scalar`, default: `0.0`): focus parameter for the GACO algorithm.
- *Pagmo-de-F* (:raw:`Scalar`, default: `0.8`): DE scaling factor.
- *Pagmo-de-CR* (:raw:`Scalar`, default: `0.9`): DE crossover probability.
- *Pagmo-de-variant* (:raw:`UnsignedInteger`, default: `2`): DE variant.
- *Pagmo-sade-variant* (:raw:`UnsignedInteger`, default: `2`): SADE variant.
- *Pagmo-sade-variant_adptv* (:raw:`UnsignedInteger`, default: `1`): SADE adaptive variant.
- *Pagmo-de1220-variant_adptv* (:raw:`UnsignedInteger`, default: `1`): de1220 adaptive variant.
- *Pagmo-ihs-phmcr* (:raw:`Scalar`, default: `0.85`): IHS pitch adjusting rate.
- *Pagmo-ihs-ppar_min* (:raw:`Scalar`, default: `0.35`): IHS minimum pitch adjustment.
- *Pagmo-ihs-ppar_max* (:raw:`Scalar`, default: `0.99`): IHS maximum pitch adjustment.
- *Pagmo-ihs-bw_min* (:raw:`Scalar`, default: `1e-05`): IHS minimum bandwidth.
- *Pagmo-ihs-bw_max* (:raw:`Scalar`, default: `1.0`): IHS maximum bandwidth.
- *Pagmo-pso-omega* (:raw:`Scalar`, default: `0.7298`): PSO inertia weight.
- *Pagmo-pso-eta1* (:raw:`Scalar`, default: `2.05`): PSO personal learning factor.
- *Pagmo-pso-eta2* (:raw:`Scalar`, default: `2.05`): PSO social learning factor.
- *Pagmo-pso-max_vel* (:raw:`Scalar`, default: `0.5`): PSO maximum velocity.
- *Pagmo-pso-variant* (:raw:`UnsignedInteger`, default: `5`): PSO variant.
- *Pagmo-pso-neighb_type* (:raw:`UnsignedInteger`, default: `2`): PSO neighbor topology type.
- *Pagmo-pso-neighb_param* (:raw:`UnsignedInteger`, default: `4`): PSO neighbor topology parameter.
- *Pagmo-sga-cr* (:raw:`Scalar`, default: `0.9`): SGA crossover probability.
- *Pagmo-sga-eta_c* (:raw:`Scalar`, default: `1.0`): SGA crossover distribution index.
- *Pagmo-sga-m* (:raw:`Scalar`, default: `0.01`): SGA mutation probability.
- *Pagmo-sga-param_m* (:raw:`Scalar`, default: `1.0`): SGA mutation distribution index.
- *Pagmo-sga-param_s* (:raw:`UnsignedInteger`, default: `2`): SGA tournament selection parameter.
- *Pagmo-sga-crossover* (:raw:`String`, default: `exponential`): SGA crossover type.
- *Pagmo-sga-mutation* (:raw:`String`, default: `polynomial`): SGA mutation type.
- *Pagmo-sga-selection* (:raw:`String`, default: `tournament`): SGA selection type.
- *Pagmo-simulated_annealing-Ts* (:raw:`Scalar`, default: `10.0`): SA starting temperature.
- *Pagmo-simulated_annealing-Tf* (:raw:`Scalar`, default: `0.1`): SA final temperature.
- *Pagmo-simulated_annealing-n_T_adj* (:raw:`UnsignedInteger`, default: `10`): SA temperature adjustment step.
- *Pagmo-simulated_annealing-n_range_adj* (:raw:`UnsignedInteger`, default: `1`): SA range adjustment step.
- *Pagmo-simulated_annealing-bin_size* (:raw:`UnsignedInteger`, default: `20`): SA bin size.
- *Pagmo-simulated_annealing-start_range* (:raw:`Scalar`, default: `1.0`): SA initial range.
- *Pagmo-bee_colony-limit* (:raw:`UnsignedInteger`, default: `20`): ABC limit parameter.
- *Pagmo-cmaes-cc* (:raw:`Scalar`, default: `-1.0`): CMA-ES cc parameter.
- *Pagmo-cmaes-cs* (:raw:`Scalar`, default: `-1.0`): CMA-ES cs parameter.
- *Pagmo-cmaes-c1* (:raw:`Scalar`, default: `-1.0`): CMA-ES c1 parameter.
- *Pagmo-cmaes-cmu* (:raw:`Scalar`, default: `-1.0`): CMA-ES cmu parameter.
- *Pagmo-cmaes-sigma0* (:raw:`Scalar`, default: `0.5`): CMA-ES initial step size.
- *Pagmo-xnes-eta_mu* (:raw:`Scalar`, default: `-1.0`): xNES eta_mu parameter.
- *Pagmo-xnes-eta_sigma* (:raw:`Scalar`, default: `-1.0`): xNES eta_sigma parameter.
- *Pagmo-xnes-eta_b* (:raw:`Scalar`, default: `-1.0`): xNES eta_b parameter.
- *Pagmo-xnes-sigma0* (:raw:`Scalar`, default: `-1.0`): xNES initial step size.
- *Pagmo-nsga2-cr* (:raw:`Scalar`, default: `0.95`): NSGA2 crossover probability.
- *Pagmo-nsga2-eta_c* (:raw:`Scalar`, default: `10.0`): NSGA2 crossover distribution index.
- *Pagmo-nsga2-m* (:raw:`Scalar`, default: `0.01`): NSGA2 mutation probability.
- *Pagmo-nsga2-eta_m* (:raw:`Scalar`, default: `50.0`): NSGA2 mutation distribution index.
- *Pagmo-moead-weight_generation* (:raw:`String`, default: `grid`): MOEAD weight generation method.
- *Pagmo-moead-decomposition* (:raw:`String`, default: `tchebycheff`): MOEAD decomposition method.
- *Pagmo-moead-neighbours* (:raw:`UnsignedInteger`, default: `20`): MOEAD neighbourhood size.
- *Pagmo-moead-CR* (:raw:`Scalar`, default: `1.0`): MOEAD crossover probability.
- *Pagmo-moead-F* (:raw:`Scalar`, default: `0.5`): MOEAD scaling factor.
- *Pagmo-moead-eta_m* (:raw:`Scalar`, default: `20.0`): MOEAD mutation distribution index.
- *Pagmo-moead-realb* (:raw:`Scalar`, default: `0.9`): MOEAD probability of using polynomial mutation.
- *Pagmo-moead-limit* (:raw:`UnsignedInteger`, default: `2`): MOEAD maximum number of replacements.
- *Pagmo-moead-preserve_diversity* (:raw:`Bool`, default: `true`): MOEAD preserve diversity flag.
- *Pagmo-mhaco-ker* (:raw:`UnsignedInteger`, default: `63`): MHACO number of ants.
- *Pagmo-mhaco-q* (:raw:`Scalar`, default: `1.0`): MHACO pheromone parameter.
- *Pagmo-mhaco-threshold* (:raw:`UnsignedInteger`, default: `1`): MHACO threshold.
- *Pagmo-mhaco-n_gen_mark* (:raw:`UnsignedInteger`, default: `7`): MHACO convergence check generations.
- *Pagmo-mhaco-focus* (:raw:`Scalar`, default: `0.0`): MHACO focus parameter.
- *Pagmo-nspso-omega* (:raw:`Scalar`, default: `0.6`): NSPSO inertia weight.
- *Pagmo-nspso-c1* (:raw:`Scalar`, default: `2.0`): NSPSO personal learning factor.
- *Pagmo-nspso-c2* (:raw:`Scalar`, default: `2.0`): NSPSO social learning factor.
- *Pagmo-nspso-chi* (:raw:`Scalar`, default: `1.0`): NSPSO velocity parameter.
- *Pagmo-nspso-v_coeff* (:raw:`Scalar`, default: `0.5`): NSPSO velocity coefficient.
- *Pagmo-nspso-leader_selection_range* (:raw:`UnsignedInteger`, default: `60`): NSPSO leader selection range.

Examples
--------
Define an optimization problem to find the minimum of the Rosenbrock function:

>>> import openturns as ot
>>> dim = 2
>>> rosenbrock = ot.SymbolicFunction(['x1', 'x2'], ['(1-x1)^2+100*(x2-x1^2)^2'])
>>> bounds = ot.Interval([-5.0] * dim, [5.0] * dim)
>>> problem = ot.OptimizationProblem(rosenbrock)
>>> problem.setBounds(bounds)

Sample the initial population inside a box:

>>> uniform = ot.JointDistribution([ot.Uniform(-2.0, 2.0)] * dim)
>>> ot.RandomGenerator.SetSeed(0)
>>> init_pop = uniform.getSample(5)

Run GACO on our problem:

>>> algo = ot.Pagmo(problem, 'gaco', init_pop) # doctest: +SKIP
>>> algo.setMaximumIterationNumber(5) # doctest: +SKIP
>>> algo.run() # doctest: +SKIP
>>> result = algo.getResult() # doctest: +SKIP
>>> x_star = result.getOptimalPoint() # doctest: +SKIP
>>> y_star = result.getOptimalValue() # doctest: +SKIP

Get the final population:

>>> final_pop_x = result.getFinalPoints() # doctest: +SKIP
>>> final_pop_y = result.getFinalValues() # doctest: +SKIP

Define a multi-objective problem:

>>> dim = 2
>>> model = ot.SymbolicFunction(['x', 'y'], ['x^2+y^2*(1-x)^3', '-x^2'])
>>> bounds = ot.Interval([-2.0] * dim, [3.0] * dim)
>>> problem = ot.OptimizationProblem(model)
>>> problem.setBounds(bounds)

Sample the initial population inside a box:

>>> uniform = ot.JointDistribution([ot.Uniform(-2.0, 3.0)] * dim)
>>> ot.RandomGenerator.SetSeed(0)
>>> init_pop = uniform.getSample(5)

Run NSGA2 on our problem:

>>> algo = ot.Pagmo(problem, 'nsga2', init_pop) # doctest: +SKIP
>>> algo.setMaximumIterationNumber(5) # doctest: +SKIP
>>> algo.run() # doctest: +SKIP
>>> result = algo.getResult() # doctest: +SKIP
>>> final_pop_x = result.getFinalPoints() # doctest: +SKIP
>>> final_pop_y = result.getFinalValues() # doctest: +SKIP

Get the best front points and values:

>>> front0 = result.getParetoFrontsIndices()[0] # doctest: +SKIP
>>> front0_x = final_pop_x.select(front0) # doctest: +SKIP
>>> front0_y = final_pop_y.select(front0) # doctest: +SKIP
"

// ---------------------------------------------------------------------

%feature("docstring") OT::Pagmo::getStartingSample
"Accessor to the sample of starting points.

Returns
-------
startingSample : :class:`~openturns.Sample`
    The initial population.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::Pagmo::setStartingSample
"Accessor to the sample of starting points.

Parameters
----------
startingSample : 2-d sequence of float
    The initial population."

// ---------------------------------------------------------------------

%feature("docstring") OT::Pagmo::setSeed
"Random generator seed accessor.

Parameters
----------
seed : int
    Seed.

Notes
-----
The default is set by the `Pagmo-InitialSeed` ResourceMap entry."

// ---------------------------------------------------------------------

%feature("docstring") OT::Pagmo::getSeed
"Random generator seed accessor.

Returns
-------
seed : int
    Seed."

// ---------------------------------------------------------------------

%feature("docstring") OT::Pagmo::setBlockSize
"Block size accessor.

Parameters
----------
blockSize : int
    Batch evaluation granularity."

// ---------------------------------------------------------------------

%feature("docstring") OT::Pagmo::getBlockSize
"Block size accessor.

Returns
-------
blockSize : int
    Batch evaluation granularity."

// ---------------------------------------------------------------------

%feature("docstring") OT::Pagmo::GetAlgorithmNames
"Accessor to the list of algorithm names provided.

Returns
-------
names : :class:`~openturns.Description`
    List of algorithm names provided, according to its naming convention."

// ---------------------------------------------------------------------

%feature("docstring") OT::Pagmo::setAlgorithmName
"Accessor to the algorithm name.

Parameters
----------
algoName : str
    The identifier of the algorithm."

// ---------------------------------------------------------------------

%feature("docstring") OT::Pagmo::getAlgorithmName
"Accessor to the algorithm name.

Returns
-------
algoName : str
    The identifier of the algorithm."

// ---------------------------------------------------------------------

%feature("docstring") OT::Pagmo::setIncrementalEvolution
"Accessor to the evolution mode.

Parameters
----------
incrementalGeneration : bool
    Whether to externally loop over each generation instead of evolving all generations in one go,
    allowing to continuously report the population and fronts in the result."

// ---------------------------------------------------------------------

%feature("docstring") OT::Pagmo::getIncrementalEvolution
"Accessor to the evolution mode.

Returns
-------
incrementalGeneration : bool
    Whether to externally loop over each generation instead of evolving all generations in one go,
    allowing to continuously report the population and fronts in the result."
