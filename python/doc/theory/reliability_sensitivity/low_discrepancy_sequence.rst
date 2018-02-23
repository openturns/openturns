.. _low_discrepancy_sequence:

Low Discrepancy Sequence
------------------------

| A low-discrepancy sequence is a sequence with the property that for
  all values of :math:`N`, its subsequence :math:`(x_1, \hdots, x_N)`
  has a low discrepancy.
| The discrepancy of a sequence is low if the number of points in the
  sequence falling into an arbitrary set B is close to proportional to
  the measure of B, as would happen on average (but not for particular
  samples) in the case of a uniform distribution. Specific definitions
  of discrepancy differ regarding the choice of B (hyperspheres,
  hypercubes, etc.) and how the discrepancy for every B is computed
  (usually normalized) and combined (usually by taking the worst value).
| Low-discrepancy sequences are also called quasi-random or sub-random
  sequences, due to their common use as a replacement of uniformly
  distributed random numbers. The “quasi” modifier is used to denote
  more clearly that the values of a low-discrepancy sequence are neither
  random nor pseudorandom, but such sequences share some properties of
  random variables and in certain applications such as the quasi-Monte
  Carlo method their lower discrepancy is an important advantage.

At least three methods of numerical integration can be phrased as
follows. Given a set :math:`\{x_1, \hdots, x_N\}` in the interval [0,1],
approximate the integral of a function f as the average of the function
evaluated at those points:

.. math::

   \begin{aligned}
       \int_0^1 f(u)\,du \approx \frac{1}{N}\,\sum_{i=1}^N f(x_i).
     \end{aligned}

-  If the points are chosen as :math:`x_i = i/N`, this is the rectangle
   rule.

-  If the points are chosen to be randomly distributed, this is the
   Monte Carlo method: – see page .

-  If the points are chosen as elements of a low-discrepancy sequence,
   this is the quasi-Monte Carlo method: – see page .

The **discrepancy** of a set :math:`P = \{x_1, \hdots, x_N\}` is
defined, using Niederreiter’s notation, as

.. math::

   \begin{aligned}
       D_N(P) = \sup_{B\in J} \left| \frac{A(B;P)}{N} - \lambda_s(B) \right|
     \end{aligned}

where :math:`\lambda-s` is the s-dimensional Lebesgue measure,
:math:`A(B;P)` is the number of points in :math:`P` that fall into
:math:`B`, and :math:`J` is the set of s-dimensional intervals or boxes
of the form:

.. math::

   \begin{aligned}
       \prod_{i=1}^s [a_i, b_i) = \{ \mathbf{x} \in \mathbf{R}^s : a_i \le x_i < b_i \} \,
     \end{aligned}

where :math:`0 \le a_i < b_i \le 1` .

The star-discrepancy D\*N(P) is defined similarly, except that the
supremum is taken over the set J\* of intervals of the form

.. math::

   \begin{aligned}
       \prod_{i=1}^s [0, u_i)
     \end{aligned}

where :math:`u_i` is in the half-open interval :math:`[0, 1)`.

The two are related by

.. math::

   \begin{aligned}
         D^*_N \le D_N \le 2^s D^*_N . \,
       \end{aligned}

| The **Koksma-lawka inequality**, shows that the error of such a method
  can be bounded by the product of two terms, one of which depends only
  on f, and the other one is the discrepancy of the set
  :math:`\{x_1, \hdots, x_N\}`.
| Let :math:`\bar I^s` be the s-dimensional unit cube,
  :math:`\bar I^s = [0, 1] \times ... \times [0, 1]`. Let :math:`f` have
  bounded variation :math:`V(f)` on :math:`I^s` in the sense of Hardy
  and Krause. Then for any :math:`(x_1, \hdots, x_N)` in
  :math:`I^s = [0, 1) \times ... \times [0, 1)`,

  .. math::

     \begin{aligned}
               \left| \frac{1}{N} \sum_{i=1}^N f(x_i) - \int_{\bar I^s} f(u)\,du \right| \le V(f)\, D_N^* (x_1,\ldots,x_N).
             \end{aligned}

The Koksma-Hlawka inequality is sharp in the following sense: For any
point set :math:`\{x_1, \hdots, x_N\}` in :math:`I^s` and any
:math:`\varepsilon >0` > 0, there is a function :math:`f` with bounded
variation and :math:`V(f)=1` such that:

.. math::

   \begin{aligned}
             \left| \frac{1}{N} \sum_{i=1}^N f(x_i) - \int_{\bar I^s} f(u)\,du \right|>D_{N}^{*}(x_1,\ldots,x_N)-\varepsilon.
           \end{aligned}

Therefore, the quality of a numerical integration rule depends only on
the discrepancy :math:`D^*_N(x_1,\hdots,x_N)`.

| Constructions of sequence are known, due to Faure, Halton, Hammersley,
  Sobol’, Niederreiter and van der Corput, such that:

  .. math::

     \begin{aligned}
               D_{N}^{*}(x_1,\ldots,x_N)\leq C\frac{(\ln N)^{s}}{N}.
             \end{aligned}

where :math:`C` is a certain constant, depending on the sequence.
These sequences are believed to have the best possible order of
convergence. See also: van der Corput sequence, Halton sequences,
Sobol sequences. In the case of the Haselgrove sequence, we have:

  .. math::

     \begin{aligned}
               \forall \varepsilon>0,\exists C_{\varepsilon}>0\mbox{ such that }D_{N}^{*}(x_1,\ldots,x_N)\leq \frac{C_{\varepsilon}}{N^{1-\varepsilon}}.
             \end{aligned}

which means a worse asymptotic performance than the previous
sequence, but can be interesting for finite sample size.

**Remark 1**:

| If :math:`\{x_1, \hdots, x_N\}` is a low-discrepancy sequence, then
  :math:`\displaystyle \frac{1}{N} \sum_{i=1}^{N} \delta_{x_i}`
  converges weakly towards :math:`\lambda^s` the :math:`s`-dimensional
  Lebesgue measure on :math:`[0,1]^s`, which garanties that for all test
  function (continuous and bounded) :math:`\phi`,
  :math:`\displaystyle <\frac{1}{N} \sum_{i=1}^{N} \delta_{x_i},\phi>`
  converges towards :math:`<\lambda^s, \phi> = \int \phi \, d\lambda^s`.

We then obtain:

  .. math::

    \begin{aligned}
      \displaystyle \frac{1}{N} \sum_{i=1}^{N} \phi(x_i) \longrightarrow \int \phi \, dx
    \end{aligned}

Be careful: using low discrepancy sequences instead of random
distributed points do not lead to the same control of the variance of
the approximation : in the case of random distributed points, this
control is given by the Central Limit Theorem that provides confidence
intervals. In the case of low discrepancy sequences, it is given by
the Koksma-Hlawka inequality.

**Remark 2**:

| It is possible to generate low discrepancy sequence according to
  measures different from the Lebesgue one, by using the inverse CDF
  technique. But be careful: the inverse CDF technique
  is not the one used in all the cases (some distributions are generated
  thanks to the rejection method for example): that’s why it is not
  recommended, in the general case, to substitute a low discrepancy
  sequence to the uniform random generator.

**Remark 3**:

| The low-discrepancy sequences have performances that deteriorate
  rapidly with the problem dimension, as the bound on the discrepancy
  increases exponentially with the dimension. This behaviour is shared
  by all the low discrepancy sequences, even if all the standard
  low-discrepancy sequences don’t exhibit this behaviour with the same
  intensity. According to the given reference, the following
  recommandation can be made:

-  The Sobol can be used for dimensions up to several hundreds (but
   our implementation of the Sobol sequence is limited to
   dimension less or equal to 40).

-  The Halton or reverse Halton sequences should preferably not be used for dimensions greater than 8;

-  The Faure sequences should preferably not be used for dimensions greater than 25;

-  Use Haselgrove sequences should preferably not be used for dimensions greater than 50;

Low-discrepancy sequences are also called quasi-random or sub-random
sequences, but it can be confusing as they are deterministic and that
they don’t have the same statistical properties as traditional
pseudo-random sequences.


.. topic:: API:

    - See :ref:`low discrepancy sequences <low_discrepancy_sequence_api>`

.. topic:: Examples:

    - See :doc:`/examples/reliability_sensitivity/low_discrepancy_sequence`

.. topic:: References:

    - Inna Krykova, *Evaluating of path-dependent securities with low discrepancy methods*, Master of Science Thesis, Worcester Polytechnic Institute, 2003.
    - Wikipedia contributors, *Low-discrepancy sequence.*, Wikipedia, The Free Encyclopedia, 10 April 2012, 17:48 UTC, <en.wikipedia.org/wiki/Low-discrepancy_sequence>
