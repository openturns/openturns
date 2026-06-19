%feature("docstring") OT::SobolSequence
R"RAW(Sobol sequence.

The scrambling is applied as a digital permutation of the binary digits of the
generated integer coefficients before their conversion to floating-point values.
This preserves the Sobol sequence properties while reducing the risk of
spurious correlations in high dimensions.

The 'MULTIDIGIT' scrambling uses the multi-digit scrambling algorithm from
[chi2005]_: the top :math:`k` bits of each component are extracted, linearly
scrambled via an LCG (:math:`y^* = a y \mod m` with :math:`m = 2^k - 1` and
:math:`a = 16807`), and reinserted. The number of bits :math:`k` and the LCG multiplier
a can be configured via the 'SobolSequence-MultidigitBits' and
'SobolSequence-MultidigitMultiplier' keys in the :class:`~openturns.ResourceMap`.

Parameters
----------
dimension : positive int, :math:`1\leq d \leq 1111`
    Dimension of the points.
scrambling : str
    Identifier of the scrambling method. Available methods:
    multi-digit scrambling (scrambling='MULTIDIGIT') or no scrambling
    ('NONE', default value). Default value is given by the
    'SobolSequence-Scrambling' key in the :class:`~openturns.ResourceMap`.

Examples
--------
>>> import openturns as ot
>>> sequence = ot.SobolSequence(2)
>>> print(sequence.generate(5))
0 : [ 0.5   0.5   ]
1 : [ 0.75  0.25  ]
2 : [ 0.25  0.75  ]
3 : [ 0.375 0.375 ]
4 : [ 0.875 0.875 ])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSequence::getScrambling
"Accessor to the scrambling method.

Returns
-------
scrambling : str
    Name of the scrambling method. Possible values are 'NONE' and 'MULTIDIGIT'."

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSequence::setScrambling
"Accessor to the scrambling method.

Parameters
----------
scrambling : str
    Name of the scrambling method. Possible values are 'NONE' and 'MULTIDIGIT'."
