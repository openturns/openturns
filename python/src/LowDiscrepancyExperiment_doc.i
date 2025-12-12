%feature("docstring") OT::LowDiscrepancyExperiment
R"RAW(LowDiscrepancy experiment.

Available constructors:
    LowDiscrepancyExperiment(*size, restart*)

    LowDiscrepancyExperiment(*sequence, size, restart*)

    LowDiscrepancyExperiment(*sequence, distribution, size, restart*)

Parameters
----------
size : positive int
    Number :math:`n` of points of the sequence.
sequence : :class:`~openturns.LowDiscrepancySequence`
    Low discrepancy sequence type.

    If not specified, the sequence is a :class:`~openturns.SobolSequence`.
distribution : :class:`~openturns.Distribution`
    Distribution :math:`\mu` of dimension :math:`n`.
restart : bool
    Flag to tell if the low discrepancy sequence must be restarted from
    its initial state at each change of distribution or not.

    Default is *True*: the sequence is restarted at each change of
    distribution.

Notes
-----
The :meth:`generate` method generates a low discrepancy sequence :math:`(u_1, \cdots, u_n)`
over :math:`[0,1]^n` using the Sobol sequence if no particular other sequence has been fixed
through the parameter *sequence*.

Then we use an :ref:`Isoprobabilistic transformations <isoprobabilistic_transformation>` that
maps the independent copula of dimension :math:`n`
into the distribution given through the parameter *distribution*: the sample :math:`(u_1, \cdots, u_n)`
is transformed into the sample :math:`(x_1, \cdots, x_n)` that is finally returned.

The weights are all equal to :math:`1/n`.

When the :meth:`generate` method is called again, the generated sample changes. But as soon as the
parameter *distribution* is modified, the sequence is restarted at its initial state.

Refer to [cambou2017]_ to get more details especially when *distribution* has dependent components.

See also
--------
WeightedExperiment

Examples
--------
>>> import openturns as ot
>>> distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 2)

Generate the sample with a reinitialization of the sequence at each change
of distribution:

>>> experiment = ot.LowDiscrepancyExperiment(ot.SobolSequence(), distribution, 5, True)
>>> print(experiment.generate())
    [ y0    y1    ]
0 : [ 0.5   0.5   ]
1 : [ 0.75  0.25  ]
2 : [ 0.25  0.75  ]
3 : [ 0.375 0.375 ]
4 : [ 0.875 0.875 ]
>>> print(experiment.generate())
    [ y0     y1     ]
0 : [ 0.625  0.125  ]
1 : [ 0.125  0.625  ]
2 : [ 0.1875 0.3125 ]
3 : [ 0.6875 0.8125 ]
4 : [ 0.9375 0.0625 ]
>>> experiment.setDistribution(distribution)
>>> print(experiment.generate())
    [ y0    y1    ]
0 : [ 0.5   0.5   ]
1 : [ 0.75  0.25  ]
2 : [ 0.25  0.75  ]
3 : [ 0.375 0.375 ]
4 : [ 0.875 0.875 ]

Generate the sample keeping the previous state of the sequence at each change
of distribution:

>>> experiment = ot.LowDiscrepancyExperiment(ot.SobolSequence(), distribution, 5, False)
>>> print(experiment.generate())
    [ y0    y1    ]
0 : [ 0.5   0.5   ]
1 : [ 0.75  0.25  ]
2 : [ 0.25  0.75  ]
3 : [ 0.375 0.375 ]
4 : [ 0.875 0.875 ]
>>> print(experiment.generate())
    [ y0     y1     ]
0 : [ 0.625  0.125  ]
1 : [ 0.125  0.625  ]
2 : [ 0.1875 0.3125 ]
3 : [ 0.6875 0.8125 ]
4 : [ 0.9375 0.0625 ]
>>> experiment.setDistribution(distribution)
>>> print(experiment.generate())
    [ y0     y1     ]
0 : [ 0.4375 0.5625 ]
1 : [ 0.3125 0.1875 ]
2 : [ 0.8125 0.6875 ]
3 : [ 0.5625 0.4375 ]
4 : [ 0.0625 0.9375 ]

Generate a sample according to a distribution with dependent marginals:

>>> distribution = ot.Normal([0.0]*2, ot.CovarianceMatrix(2, [4.0, 1.0, 1.0, 9.0]))
>>> experiment = ot.LowDiscrepancyExperiment(ot.SobolSequence(), distribution, 5, False)
>>> print(experiment.generate())
    [ y0        y1        ]
0 : [  0         0        ]
1 : [  1.34898  -1.65792  ]
2 : [ -1.34898   1.65792  ]
3 : [ -0.637279 -1.10187  ]
4 : [  2.3007    3.97795  ]
)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::LowDiscrepancyExperiment::getSequence
R"RAW(Return the sequence.

Returns
-------
sequence : :class:`~openturns.LowDiscrepancySequence`
    Sequence of points :math:`(u_1, \cdots, u_N)` with low discrepancy.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::LowDiscrepancyExperiment::getRestart
"Return the value of the *restart* flag.

Returns
-------
restart : bool
    The value of the *restart* flag."

// ---------------------------------------------------------------------

%feature("docstring") OT::LowDiscrepancyExperiment::setRestart
"Set the value of the *restart* flag.

Parameters
----------
restart : bool
    The value of the *restart* flag. If equals to *True*, the low
    discrepancy sequence is restarted at each change of distribution,
    else it is changed only if the new distribution has a dimension
    different from the current one.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::LowDiscrepancyExperiment::getRandomize
"Return the value of the *randomize* flag.

Returns
-------
randomize : bool
    The value of the *randomize* flag."

// ---------------------------------------------------------------------

%feature("docstring") OT::LowDiscrepancyExperiment::setRandomize
"Set the value of the *randomize* flag.

Parameters
----------
randomize : bool
    Use a cyclic scrambling of the low discrepancy sequence, it means, the whole
    low discrepancy sequence is translated by a random vector modulo 1.
    See [lecuyer2005]_ for the interest of such a scrambling.
    Default is False.
"
