from __future__ import print_function
import openturns as ot
from math import sin, pi
#import matplotlib
#matplotlib.use('Agg')
#import matplotlib.pylab as plt

# Set seed
ot.RandomGenerator.SetSeed(0)
# Definition of the Ishigami function
dimension = 3
a = 7.0
b = 0.1

# Create the Ishigami function
input_variables = ['xi1','xi2','xi3']
formula = ['sin(xi1) + (' + str(a) + ') * (sin(xi2)) ^ 2 + (' + str(b) + ') * xi3^4 * sin(xi1)']
ishigami_model = ot.NumericalMathFunction(input_variables, formula)
ishigami_model.setName('Ishigami')

#  Generating a design of size 
N = 150
# Considering independent Uniform distributions of dimension 3
# Bounds are (-pi,pi), (-pi,pi) and (-pi,pi)
distribution = ot.ComposedDistribution([ot.Uniform(-pi, pi)] * dimension)
bounds = distribution.getRange()
# Random LHS
lhs = ot.LHSExperiment(distribution, N)
lhs.setAlwaysShuffle(True) # randomized
# Fixing C2 crit
space_filling = ot.SpaceFillingC2()
# Defining a temperature profile
temperatureProfile = ot.GeometricProfile()
# Pre conditionning : generate an optimal design with MC
nSimu = 100
algo = ot.MonteCarloLHS(lhs, nSimu, space_filling)
initialDesign = algo.generate()
result = algo.getResult()

print('initial design pre-computed. Performing SA optimization...')
# Use of initial design
algo = ot.SimulatedAnnealingLHS(initialDesign, distribution, temperatureProfile, space_filling)
# Retrieve optimal design
input_database = algo.generate()

result = algo.getResult()

print('initial design computed')

# Response of the model
print('sampling size = ', N)
output_database = ishigami_model(input_database)

# Learning input/output
# Usual chaos meta model
enumerate_function = ot.HyperbolicAnisotropicEnumerateFunction(dimension)
orthogonal_basis = ot.OrthogonalProductPolynomialFactory(dimension*[ot.LegendreFactory()], enumerate_function)
basis_size = 100
# Initial chaos algorithm
adaptive_strategy = ot.FixedStrategy(orthogonal_basis, basis_size)
# ProjectionStrategy ==> Sparse
fitting_algorithm = ot.KFold()
approximation_algorithm = ot.LeastSquaresMetaModelSelectionFactory(ot.LARS(), fitting_algorithm)
projection_strategy = ot.LeastSquaresStrategy(input_database, output_database, approximation_algorithm)
print('Surrogate model...')
distribution_ishigami = ot.ComposedDistribution(dimension * [ot.Uniform(-pi,pi)])
algo_pc = ot.FunctionalChaosAlgorithm(input_database, output_database, distribution_ishigami, adaptive_strategy, projection_strategy)
algo_pc.run()
chaos_result = algo_pc.getResult()
print('Surrogate model computed')

# Validation
lhs_validation = ot.LHSExperiment(distribution_ishigami, 100)
input_validation = lhs_validation.generate()
output_validation = ishigami_model(input_validation)
# Chaos model evaluation
output_metamodel_sample = chaos_result.getMetaModel()(input_validation)

# Cloud validation ==> change from matplotlib to pure OT
#fig = plt.figure()
#plt.plot(output_validation, output_validation, 'b-', label='Model')
#plt.plot(output_validation, output_metamodel_sample, 'r.', label='SM')
#plt.title('Surrogate Model validation - Ishigami use case')
#plt.savefig('ishigami_model_validation.png')
