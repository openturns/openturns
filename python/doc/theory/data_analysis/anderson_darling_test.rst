Anderson-Darling goodness-of-fit test
-------------------------------------

This method deals with the modelling of a probability distribution of a
random vector :math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`. It
seeks to verify the compatibility between a sample of data
:math:`\left\{ \vect{x}_1,\vect{x}_2,\ldots,\vect{x}_N \right\}` and a
candidate probability distribution previous chosen. The Anderson-Darling
Goodness-of-Fit test allows to answer this
question in the one dimensional case :math:`n_X =1`, and with a
continuous distribution. The current version is limited to the case of
the Normal distribution.

Let us limit the case to :math:`n_X = 1`. Thus we denote
:math:`\vect{X} = X^1 = X`. This goodness-of-fit test is based on the
distance between the cumulative distribution function
:math:`\widehat{F}_N` of the sample
:math:`\left\{ x_1,x_2,\ldots,x_N \right\}` and that of the
candidate distribution, denoted :math:`F`. This distance is a quadratic
type, as in the :ref:`Cramer-Von Mises test <cramer_vonmises_test>`,
but gives more weight to deviations of extreme values:

.. math::

   \begin{aligned}
       D = \int^{\infty}_{-\infty} \frac{\displaystyle \left[F\left(x\right) - \widehat{F}_N\left(x\right)\right]^2 }{\displaystyle F(x) \left( 1-F(x) \right) } \, dF(x)
     \end{aligned}

With a sample :math:`\left\{ x_1,x_2,\ldots,x_N \right\}`, the distance
is estimated by:

.. math::

   \begin{aligned}
       \widehat{D}_N = -N-\sum^{N}_{i=1} \frac{2i-1}{N} \left[\ln F(x_{(i)})+\ln\left(1-F(x_{(N+1-i)})\right)\right]
     \end{aligned}

where :math:`\left\{x_{(1)}, \ldots, x_{(N)}\right\}` describes the
sample placed in increasing order.

The probability distribution of the distance :math:`\widehat{D}_N` is
asymptotically known (i.e. as the size of the sample tends to infinity).
If :math:`N` is sufficiently large, this means that for a probability
:math:`\alpha` and a candidate distribution type, one can calculate the
threshold / critical value :math:`d_\alpha` such that:

-  if :math:`\widehat{D}_N>d_{\alpha}`, we reject the candidate
   distribution with a risk of error :math:`\alpha`,

-  if :math:`\widehat{D}_N \leq d_{\alpha}`, the candidate distribution
   is considered acceptable.

Note that :math:`d_\alpha` depends on the candidate distribution
:math:`F` being tested; the current version is limited to
the case of the Normal distribution.

An important notion is the so-called “:math:`p`-value” of the test. This
quantity is equal to the limit error probability
:math:`\alpha_\textrm{lim}` under which the candidate distribution is
rejected. Thus, the candidate distribution will be accepted if and only
if :math:`\alpha_\textrm{lim}` is greater than the value :math:`\alpha`
desired by the user. Note that the higher
:math:`\alpha_\textrm{lim} - \alpha`, the more robust the decision.


.. topic:: API:

    - See :py:func:`~openturns.NormalityTest_AndersonDarlingNormal`

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/andersondarling_test`

.. topic:: References:

    - [Saporta1990]_
    - [Dixon1983]_
    - [NISThandbook]_
    - [Dagostino1986]_
    - [Sprent2001]_
    - [Bhattacharyya1997]_
    - [Burnham2002]_
