import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

model = ot.SymbolicFunction(["E", "F", "L", "I"], ["F*L^3/(3*E*I)"])

# distribution
# Young's modulus E
E = ot.Beta(0.9, 3.5, 2.5e7, 5.0e7) # in N/m^2
E.setDescription("E")
# Load F
F = ot.LogNormal() # in N
F.setParameter(ot.LogNormalMuSigma()([30.e3, 9e3, 15.e3]))
F.setDescription("F")
# Length L
L = ot.Uniform(250., 260.) # in cm
L.setDescription("L")
# Moment of inertia I
I = ot.Beta(2.5, 4, 310, 450) # in cm^4
I.setDescription("I")
distribution = ot.ComposedDistribution([E, F, L, I])

# estimator
estimator = ot.SaltelliSensitivityAlgorithm()
estimator.setUseAsymptoticDistribution(True)

# algorithm
algo = ot.SobolSimulationAlgorithm(distribution, model, estimator)
algo.setMaximumOuterSampling(250) # number of iterations
algo.setBlockSize(100) # size of Sobol experiment at each iteration
algo.setBatchSize(4) # number of points evaluated simultaneously
algo.setIndexQuantileLevel(0.05) # alpha: cirteria checks whether CIs are small enough
algo.setIndexQuantileEpsilon(1e-2) # epsilon: critera checks whether CIs are tight enough
algo.run()

graph = algo.drawFirstOrderIndexConvergence(0)

fig = plt.figure(figsize=(8, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=True)
