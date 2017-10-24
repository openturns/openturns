Design of Experiments
---------------------

The method is used in the following context:
:math:`\vect{x}= \left( x^1,\ldots,x^{n_X} \right)` is a vector of input
parameters. We want to determine a particular set of values of
:math:`\vect{x}` according to a particular design of experiments .

Different types of design of experiments can be determined:

-  some *stratified* patterns: axial, composite, factorial or box
   patterns,

-  some *weighted* patterns that we can split into different categories:
   the random patterns, the low discrepancy sequences and the Gauss
   product.

| **Stratified design of experiments** 
| All stratified design of experiments are defined from the data of a
  center point and some discretization levels. The
  same number of levels for each direction is proposed: let us denote by
  :math:`n_{level}` that discretization number.
| The axial pattern contains points only along the axes. It is not
  convenient to model interactions between variables. The pattern is
  obtained by discretizing each direction according to specified levels,
  symmetrically with respect to the center of the design of experiments
  . The number of points generated is :math:`1 + 2dn_{level}`.
| The factorial pattern contains points only on diagonals. It is not
  convenient to model influences of single input variables. The pattern
  is obtained by discretizing each principal diagonal according to the
  specified levels, symmetrically with respect to the center of the
  design of experiments . The number of points generated is
  :math:`1 + 2^dn_{level}`.
| The composite pattern is the union of both previous ones. The number
  of points generated is :math:`1 + 2dn_{level} + 2^dn_{level}`.
| The box pattern is a simple regular discretization of a pavement
  around the center of the design of experiments , with the number of
  intermediate points specified for each direction (denoted
  :math:`n_{level\_direction\_i}`). The number of points generated
  is
  :math:`\displaystyle \prod_{i=1}^{d} (2+n_{level\_direction\_i})`.
| The following figures illustrates the different patterns obtained.


.. plot::

    import openturns as ot
    from openturns.viewer import View

    d = ot.Axial([1.5, 2.5, 3.5], [1, 2, 3])
    s = d.generate()
    s.setDescription(["X1", "X2", "X3"])
    g = ot.Graph()
    g.setTitle("Axial experiment")
    g.setGridColor("black")
    p = ot.Pairs(s)
    g.add(p)
    View(g)


.. plot::

    import openturns as ot
    from openturns.viewer import View

    # Factorial
    d = ot.Factorial([1.5, 2.5, 3.5], [1, 2, 3])
    s = d.generate()
    s.setDescription(["X1", "X2", "X3"])
    g = ot.Graph()
    g.setTitle("Factorial experiment")
    g.setGridColor("black")
    p = ot.Pairs(s)
    g.add(p)
    View(g)


.. plot::

    import openturns as ot
    from openturns.viewer import View

    # Composite
    d = ot.Composite([1.5, 2.5, 3.5], [1, 2, 3])
    s = d.generate()
    s.setDescription(["X1", "X2", "X3"])
    g = ot.Graph()
    g.setTitle("Composite experiment")
    g.setGridColor("black")
    p = ot.Pairs(s)
    g.add(p)
    View(g)


.. plot::

    import openturns as ot
    from openturns.viewer import View

    # Box
    d = ot.Box([3, 4, 5])
    s = d.generate()
    s.setDescription(["X1", "X2", "X3"])
    g = ot.Graph()
    g.setTitle("Box experiment")
    g.setGridColor("black")
    p = ot.Pairs(s)
    g.add(p)
    View(g)


| **Weighted design of experiments** 
| The first category is the *random patterns*, where the set of input
  data is generated from the joint distribution of the input random
  vector, according to the Monte Carlo sampling technique or the LHS one
  (refer to and ).
| Care: the LHS sampling method requires the independence of the input
  random variables.
| The second category is the *low discrepancy sequences*.
  The Faure, Halton, Haselgrove, Reverse Halton and
  Sobol sequences are proposed.
| The third category is the *Gauss product* which is the set of points
  which components are the respective Gauss set (i.e. the roots of the
  orthogonal polynomials with respect to the univariate distribution).


.. plot::

    import openturns as ot
    from openturns.viewer import View

    # MonteCarlo
    d = ot.MonteCarloExperiment(ot.ComposedDistribution([ot.Uniform()]*3), 32)
    s = d.generate()
    s.setDescription(["X1", "X2", "X3"])
    g = ot.Graph()
    g.setTitle("MonteCarlo experiment")
    g.setGridColor("black")
    p = ot.Pairs(s)
    g.add(p)
    View(g)


.. plot::

    import openturns as ot
    from openturns.viewer import View

    # LHS
    d = ot.LHSExperiment(ot.ComposedDistribution([ot.Uniform()]*3), 32)
    s = d.generate()
    s.setDescription(["X1", "X2", "X3"])
    g = ot.Graph()
    g.setTitle("LHS experiment")
    g.setGridColor("black")
    p = ot.Pairs(s)
    g.add(p)
    View(g)


.. plot::

    import openturns as ot
    from openturns.viewer import View

    # Sobol
    d = ot.LowDiscrepancyExperiment(ot.SobolSequence(), ot.ComposedDistribution([ot.Uniform()]*3), 32)
    s = d.generate()
    s.setDescription(["X1", "X2", "X3"])
    g = ot.Graph()
    g.setTitle("Low discrepancy experiment")
    g.setGridColor("black")
    p = ot.Pairs(s)
    g.add(p)
    View(g)


.. plot::

    import openturns as ot
    from openturns.viewer import View

    # GaussProduct
    d = ot.GaussProductExperiment(ot.ComposedDistribution([ot.Uniform()]*3), [4,6,8])
    s = d.generate()
    s.setDescription(["X1", "X2", "X3"])
    g = ot.Graph()
    g.setTitle("Gauss product experiment")
    g.setGridColor("black")
    p = ot.Pairs(s)
    g.add(p)
    View(g)



| **Combinatorial generators** 
| In some situations, one want to explore all the possibilities related
  to constrained discrete uncertainties. In this case, we need to obtain
  all the sets of indices fulfilling the constraints. Examples of
  constraints are:

-  being a subset with :math:`k` elements of a set with :math:`n`
   elements, with :math:`k\leq n`;

-  being a permutation of :math:`k` elements taken into a set of
   :math:`n` elements, with :math:`k\leq n`;

-  being an element of a Cartesian product of sets with
   :math:`n_1,\hdots,n_d` elements.

It is important to get indices and not real-valued vectors. The
distinction is made explicit by calling these design of experiments
*Combinatorial Generators*, which produce collections of indices instead
of samples.

The following figures illustrates the different patterns obtained.


.. plot::

    import openturns as ot
    from openturns.viewer import View

    # Combinations
    d = ot.Combinations(3, 12)
    s = ot.Sample(d.generate())
    s.setDescription(["X1", "X2", "X3"])
    g = ot.Graph()
    g.setTitle("Combinations generator")
    g.setGridColor("black")
    p = ot.Pairs(s)
    g.add(p)
    View(g)


.. plot::

    import openturns as ot
    from openturns.viewer import View

    # KPermutations
    d = ot.KPermutations(3, 12)
    s = ot.Sample(d.generate())
    s.setDescription(["X1", "X2", "X3"])
    g = ot.Graph()
    g.setTitle("KPermutations generator")
    g.setGridColor("black")
    p = ot.Pairs(s)
    g.add(p)
    View(g)


.. plot::

    import openturns as ot
    from openturns.viewer import View

    # Tuples
    d = ot.Tuples([3, 4, 5])
    s = ot.Sample(d.generate())
    s.setDescription(["X1", "X2", "X3"])
    g = ot.Graph()
    g.setTitle("Tuples generator")
    g.setGridColor("black")
    p = ot.Pairs(s)
    g.add(p)
    View(g)


.. topic:: API:

    - See the available :ref:`design of experiments <design_experiments>`.

.. topic:: Examples:

    - See :ref:`examples/numerical_methods/deterministic_design.ipynb`
    - See :ref:`examples/numerical_methods/probabilistic_design.ipynb`

