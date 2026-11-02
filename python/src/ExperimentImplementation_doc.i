%define OT_Experiment_doc
R"RAW(Base class for design of experiments.

Considering :math:`\vect{x}=x^1,\dots, x^n` a vector of input parameters, this
class is used to determine a particular set of values of :math:`\vect{x}`
according to a particular design of experiments.

Different types of design of experiments can be determined:

- some stratified patterns: axial, composite, factorial or box patterns,

- some weighted patterns that we can split into different categories:
  random patterns, low discrepancy sequences and deterministic patterns.

Examples
--------
Define a custom design of experiment:

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> class RandomExp:
...     def generate(self):
...         return ot.Normal(1).getSample(10)
>>> experiment = ot.Experiment(RandomExp())
>>> sample = experiment.generate()

See also
--------
StratifiedExperiment, WeightedExperiment)RAW"
%enddef
%feature("docstring") OT::ExperimentImplementation
OT_Experiment_doc

// ---------------------------------------------------------------------

%define OT_Experiment_generate_doc
"Generate points according to the type of the experiment.

Returns
-------
sample : :class:`~openturns.Sample`
    The points which constitute the design of experiments. The sampling method
    is defined by the nature of the experiment.

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> myExperiment = ot.Experiment(ot.MonteCarloExperiment(ot.Normal(2),5))
>>> print(myExperiment.generate())
    [ X0        X1        ]
0 : [  0.608202 -1.26617  ]
1 : [ -0.438266  1.20548  ]
2 : [ -2.18139   0.350042 ]
3 : [ -0.355007  1.43725  ]
4 : [  0.810668  0.793156 ]"
%enddef
%feature("docstring") OT::ExperimentImplementation::generate
OT_Experiment_generate_doc
%define OT_Experiment_generateWithWeights_doc
"Generate points and weights according to the type of the experiment.

Returns
-------
sample : :class:`~openturns.Sample`
    The points which constitute the design of experiments.
weights : :class:`~openturns.Point`
    The weights associated to the points. Experiments defined in Python
    may provide a *generateWithWeights* method returning a
    (*sample*, *weights*) pair; otherwise uniform weights are returned.

Examples
--------
>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> class WeightedExp:
...     def generate(self):
...         return ot.Normal(1).getSample(3)
...     def generateWithWeights(self):
...         return (ot.Normal(1).getSample(3), [0.2, 0.3, 0.5])
>>> experiment = ot.Experiment(WeightedExp())
>>> weights = ot.Point()
>>> sample = experiment.generateWithWeights(weights)
>>> print(sample.getSize())
3
>>> print(weights)
[0.2,0.3,0.5]"
%enddef
%feature("docstring") OT::ExperimentImplementation::generateWithWeights
OT_Experiment_generateWithWeights_doc
%feature("docstring") OT::Experiment::generateWithWeights
OT_Experiment_generateWithWeights_doc

