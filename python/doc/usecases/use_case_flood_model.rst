.. _use-case-flood-model:

A flood model
=============


Introduction
------------

The following figure presents a dyke protecting industrial facilities.
When the river level exceeds the dyke height, flooding occurs.
The model is based on a crude simplification of the 1D hydrodynamical equations of Saint-Venant
under the assumptions of uniform and constant flow rate and large rectangular sections.
This model was first introduced in [deRocquigny2006]_, and then used in [Limbourg2010]_,
[deRocquigny2012]_, [iooss2015]_, [baudin2015]_.

.. figure:: ../_static/flooding_section.png
    :align: center
    :alt: flooding section
    :width: 50%

    **Figure 1.** Flooding section.


Height independent random variables are considered:

  - :math:`Q`: flow rate :math:`[m^3 s^{-1}]`;
  - :math:`K_s`: Strickler :math:`[m^{1/3} s^{-1}]`;
  - :math:`Z_v`: downstream height :math:`[m]`;
  - :math:`Z_m`: upstream height :math:`[m]`;
  - :math:`B`: river width :math:`[m]`;
  - :math:`L`: river length :math:`[m]`;
  - :math:`Z_b`: altitude of the river banks :math:`[m]`;
  - :math:`H_d`: height of the dyke :math:`[m]`.

When the Strickler coefficient increases, the riverbed generates less friction.

The altitude of the dyke is:

.. math::
    Z_d = Z_b + H_d


The slope :math:`\alpha` of the river is assumed to be close to zero, which implies:

.. math::
    \alpha = \frac{Z_m - Z_v}{L},

if :math:`Z_m \geq Z_v`.
The water depth is ([deRocquigny2012]_ eq. 3.2 page 79) :

.. math::
    H = \left(\frac{Q}{K_s B \sqrt{\alpha}}\right)^{0.6},


for any :math:`K_s, Q>0`.
The flood altitude is:

.. math::
    Z_c = H + Z_v.


The altitude of the surface of the water is greater than the altitude of
the top of the dyke (i.e. there is a flood) if ([deRocquigny2012]_ eq. 3.3 page 79):

.. math::
    S = Z_c - Z_d

is greater than zero.

The following figure presents the model with more details.

.. figure:: ../_static/flooding_section_detail.png
    :align: center
    :alt: flooding section details
    :width: 50%

    **Figure 2.** Flooding section detail.

The cost :math:`C` can be decomposed into the building of the dyke and
the cost of the flood ([iooss2015]_ eq. 5.3 page 103):

.. math::
    C = C_d + C_s

where the cost of the dyke is:

.. math::
    C_d
    =\begin{cases}
    \frac{8}{20}   & \textrm{if } H_d < 8 \\
    \frac{H_d}{20} & \textrm{otherwise},
    \end{cases}

and the cost of the flood is:

.. math::
    C_s
    =\begin{cases}
    1 - 0.8 \exp(-\frac{1000}{S^4}) & \textrm{if } S < 0, \\
    1 & \textrm{otherwise}.
    \end{cases}


We assume that the 8 inputs have the following distributions.
We consider 2 different set of hypotheses.

- In the hypothesis where :math:`H_d` is low, then its distribution
  is uniform in the interval :math:`[2, 4]`.
- In the hypothesis where :math:`H_d` is high, then its distribution
  is uniform in the interval :math:`[7, 9]`.
  This is the hypothesis used in [iooss2015]_.

+----------------+-----------------------------------------------+
| Input variable | Distribution                                  |
+================+===============================================+
| :math:`Q`      | Gumbel(mode=1013, scale=558), :math:`Q` > 0   |
+----------------+-----------------------------------------------+
| :math:`K_s`    | Normal(mu=30.0, sigma=7.5), :math:`K_s` > 0   |
+----------------+-----------------------------------------------+
| :math:`Z_v`    | Uniform(a=49, b=51)                           |
+----------------+-----------------------------------------------+
| :math:`Z_m`    | Uniform(a=54, b=56)                           |
+----------------+-----------------------------------------------+
| :math:`B`      | Triangular(min=295, mode=300, max=305)        |
+----------------+-----------------------------------------------+
| :math:`L`      | Triangular(min=4990, mode=5000, max=5010)     |
+----------------+-----------------------------------------------+
| :math:`Z_b`    | Triangular(min=55, mode=55.5, max=56)         |
+----------------+-----------------------------------------------+
| :math:`H_d`    | Uniform(min=2, max=4)                         |
+----------------+-----------------------------------------------+

**Table 1.** Distribution of the input random variables in the scenario where :math:`H_d` is low.

+----------------+-----------------------------------------------+
| Input variable | Distribution                                  |
+================+===============================================+
| :math:`H_d`    | Uniform(min=7, max=9)                         |
+----------------+-----------------------------------------------+

**Table 2.** Distribution of the input random variables in the scenario where :math:`H_d` is high.
The other variables have the same distribution.

Moreover, we assume that the input random variables are independent.

We want to estimate the flood probability:

 .. math::
    P_f = \Prob{S > 0}.

The results depends on the hypothesis chosen for :math:`H_d`.

- If :math:`H_d` is low, then :math:`P_f = 7.3 \times 10^{-4}`
  (with coefficient of variation lower than 0.01).
  In this case, the model is mostly additive.
- If :math:`H_d` is high, then :math:`P_f = 7.6 \times 10^{-5}`
  (with coefficient of variation lower than 0.01).
  In this case, the model for :math:`C` has interactions, mainly for :math:`Q`,
  :math:`K_s`, :math:`Z_v` and :math:`H_d`.
  The model is mostly additive for :math:`H` and :math:`S`.

Analysis of the model
---------------------

The next figure presents the river height :math:`H` depending on the flowrate
:math:`Q` when the other parameters are set to their mean values.
We see that the river height as a power model shape which is a property
of the Manning-Strickler model.

.. plot::

    import openturns as ot
    import openturns.viewer as otv
    import pylab as pl
    from openturns.usecases import flood_model

    fm = flood_model.FloodModel()
    heightInputDistribution, heightModel = fm.getHeightModel()

    # %%
    # Set all inputs to constants, except Q
    indices = [1, 2, 3]
    referencePoint = [heightInputDistribution.getMarginal(i).getMean()[0] for i in indices]
    heightModelvsQ = ot.ParametricFunction(heightModel, indices, referencePoint)
    qRange = fm.Q.getRange()
    qMin = qRange.getLowerBound()[0]
    qMax = qRange.getUpperBound()[0]
    graph = heightModelvsQ.draw(qMin, qMax, 200)
    otv.View(graph)

The next figure plots the cost :math:`C` depending on the overflow :math:`S`
in the default scenario where the dyke height is low.
It makes use of a Quasi Monte Carlo sample of size :math:`n = 10000`.

.. plot::

    import openturns as ot
    import openturns.viewer as otv
    import pylab as pl
    from openturns.usecases import flood_model

    fm = flood_model.FloodModel()

    sampleSize = 10000
    experiment = ot.LowDiscrepancyExperiment(ot.SobolSequence(), fm.distribution, sampleSize, True)
    inputSample = experiment.generate()
    outputSample = fm.model(inputSample)

    graph = ot.Graph("Scenario: dyke is low", "S", "C", True)
    cloud = ot.Cloud(outputSample[:, 1], outputSample[:, 2])
    graph.add(cloud)
    otv.View(graph)

The next figure plots the cost :math:`C` depending on the dyke height :math:`H_d`
when the other parameters are set to their mean values.
We notice that the cost first decreases because the flooding cost decreases
when the dyke height increases.
Then the cost increases because cost of the dyke increases when the
dyke height increases.

.. plot::

    import openturns as ot
    import openturns.viewer as otv
    import pylab as pl
    from openturns.usecases import flood_model

    fm = flood_model.FloodModel()

    # Set all inputs to constants, except Hd
    indices = [0, 1, 2, 3, 4, 5, 6]
    referencePoint = [fm.distribution.getMarginal(i).getMean()[0] for i in indices]
    costModelvsHd = ot.ParametricFunction(fm.model.getMarginal(2), indices, referencePoint)
    hdMin = 0.0
    hdMax = 12.0
    graph = costModelvsHd.draw(hdMin, hdMax, 200)
    otv.View(graph)

The next figure presents the distribution of the three outputs in the
default scenario where the height of the dyke is low.
It is based on a Monte-Carlo sample of size :math:`n = 1000` and uses
the histogram to estimate the density of the output of the model.

.. plot::

    import openturns as ot
    import openturns.viewer as otv
    import pylab as pl
    from openturns.usecases import flood_model

    fm = flood_model.FloodModel()

    outputDimension = fm.model.getOutputDimension()

    # Make a plot
    sampleSize = 1000
    inputSample = fm.distribution.getSample(sampleSize)
    outputSample = fm.model(inputSample)
    grid = ot.GridLayout(1, outputDimension)
    for i in range(outputDimension):
        marginalOutputSample = outputSample.getMarginal(i)
        graph = ot.HistogramFactory().build(marginalOutputSample).drawPDF()
        graph.setLegends([""])
        if i > 0:
            graph.setYTitle("")
        grid.setGraph(0, i, graph)

    grid.setTitle("Scenario: dyke is low")
    _ = otv.View(grid, figure_kw={"figsize": (8.0, 2.5)})
    pl.subplots_adjust(wspace=0.4, top=0.8)

The next figure presents the distribution of the three outputs in the
scenario where the height of the dyke is high.

.. plot::

    import openturns as ot
    import openturns.viewer as otv
    import pylab as pl
    from openturns.usecases import flood_model

    fm = flood_model.FloodModel(distributionHdLow=False)

    outputDimension = fm.model.getOutputDimension()

    # Make a plot
    sampleSize = 1000
    inputSample = fm.distribution.getSample(sampleSize)
    outputSample = fm.model(inputSample)
    grid = ot.GridLayout(1, outputDimension)
    for i in range(outputDimension):
        marginalOutputSample = outputSample.getMarginal(i)
        graph = ot.HistogramFactory().build(marginalOutputSample).drawPDF()
        graph.setLegends([""])
        if i > 0:
            graph.setYTitle("")
        grid.setGraph(0, i, graph)

    grid.setTitle("Default scenario: dyke is high")
    _ = otv.View(grid, figure_kw={"figsize": (8.0, 2.5)})
    pl.subplots_adjust(wspace=0.4, top=0.8)

The next figure presents the Sobol' indices of the three outputs in the
scenario where the height of the dyke is low.
We estimate the Sobol' indices from sampling, using a root sample
size equal to :math:`n = 1000`.

.. plot::

    import openturns as ot
    import openturns.viewer as otv
    import pylab as pl
    from openturns.usecases import flood_model

    fm = flood_model.FloodModel()
    outputDimension = fm.model.getOutputDimension()

    sampleSize = 1000
    sie = ot.SobolIndicesExperiment(fm.distribution, sampleSize)
    inputDesign = sie.generate()
    input_names = fm.distribution.getDescription()
    inputDesign.setDescription(input_names)
    inputDesign.getSize()
    outputDesign = fm.model(inputDesign)

    # Make a plot
    grid = ot.GridLayout(outputDimension, 1)
    grid.setTitle(f"n = {sampleSize}")
    for i in range(outputDimension):
        marginalOutputSample = outputDesign.getMarginal(i)
        sensitivityAnalysis = ot.SaltelliSensitivityAlgorithm(
            inputDesign, marginalOutputSample, sampleSize
        )
        graph = sensitivityAnalysis.draw()
        graph.setTitle(marginalOutputSample.getDescription()[0])
        grid.setGraph(i, 0, graph)
    grid.setTitle("Default scenario: dyke is low")
    view = otv.View(
        grid,
        figure_kw={"figsize": (7.0, 9.0)},
        legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"},
    )
    pl.subplots_adjust(wspace=0.4, hspace=0.5, right=0.7)

The next figure presents the Sobol' indices of the three outputs in the
scenario where the height of the dyke is high.

.. plot::

    import openturns as ot
    import openturns.viewer as otv
    import pylab as pl
    from openturns.usecases import flood_model

    fm = flood_model.FloodModel(distributionHdLow=False)
    outputDimension = fm.model.getOutputDimension()

    sampleSize = 1000
    sie = ot.SobolIndicesExperiment(fm.distribution, sampleSize)
    inputDesign = sie.generate()
    input_names = fm.distribution.getDescription()
    inputDesign.setDescription(input_names)
    inputDesign.getSize()
    outputDesign = fm.model(inputDesign)

    # Make a plot
    grid = ot.GridLayout(outputDimension, 1)
    grid.setTitle(f"n = {sampleSize}")
    for i in range(outputDimension):
        marginalOutputSample = outputDesign.getMarginal(i)
        sensitivityAnalysis = ot.SaltelliSensitivityAlgorithm(
            inputDesign, marginalOutputSample, sampleSize
        )
        graph = sensitivityAnalysis.draw()
        graph.setTitle(marginalOutputSample.getDescription()[0])
        grid.setGraph(i, 0, graph)
    grid.setTitle("Scenario: dyke is high")
    view = otv.View(
        grid,
        figure_kw={"figsize": (7.0, 9.0)},
        legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"},
    )
    pl.subplots_adjust(wspace=0.4, hspace=0.5, right=0.7)

The next figure presents the Sobol' indices of the height model output
with four inputs :math:`(Q, K_s, Z_v, Z_m)` only.
This is a simpler model that leads to a simplified analysis.

.. plot::

    import openturns as ot
    import openturns.viewer as otv
    import pylab as pl
    from openturns.usecases import flood_model

    fm = flood_model.FloodModel()
    heightInputDistribution, heightModel = fm.getHeightModel()
    outputDimension = heightModel.getOutputDimension()

    sampleSize = 1000
    sie = ot.SobolIndicesExperiment(heightInputDistribution, sampleSize)
    inputDesign = sie.generate()
    input_names = heightInputDistribution.getDescription()
    inputDesign.setDescription(input_names)
    inputDesign.getSize()
    outputDesign = heightModel(inputDesign)

    # Make a plot
    sensitivityAnalysis = ot.SaltelliSensitivityAlgorithm(
        inputDesign, outputDesign, sampleSize
    )
    graph = sensitivityAnalysis.draw()
    graph.setTitle("Height model with (Q, K_s, Z_v, Z_m) as inputs")
    view = otv.View(
        graph,
        figure_kw={"figsize": (6.0, 4.0)},
        legend_kw={"bbox_to_anchor": (1.0, 1.0), "loc": "upper left"},
    )
    pl.subplots_adjust(right=0.7)

Analysis of the calibration problem
-----------------------------------

In this section, we analyse why calibrating the parameters of this model
may raise some difficulties.

First, the slope :math:`\alpha` only depends on the difference :math:`Z_m - Z_v`.
This is why :math:`Z_v` and :math:`Z_m` cannot be identified at the same time.
In algebraic terms, there is an infinite number of couples :math:`(Z_v, Z_m)` which
generate the same difference :math:`Z_m - Z_v`.

Second, the denominator of the expression of :math:`H` involves the product
:math:`K_s B \sqrt{\alpha}`.
In algebraic terms, there is an infinite number of couples :math:`(K_s, \alpha)` which
generate the same product :math:`K_s \sqrt{\alpha}`.
This is why either :math:`K_s` or :math:`\alpha` can be identified separately,
but not at the same time.
This shows that only one parameter can be identified.

Hence, calibrating this model requires some regularization which can be done
by Bayesian methods.

References
----------

* [deRocquigny2006]_
* [Limbourg2010]_
* [deRocquigny2012]_
* [iooss2015]_
* [baudin2015]_

API documentation
-----------------

.. currentmodule:: openturns.usecases.flood_model

.. autoclass:: FloodModel
    :noindex:

Examples based on this use case
-------------------------------

.. minigallery:: openturns.usecases.flood_model.FloodModel

