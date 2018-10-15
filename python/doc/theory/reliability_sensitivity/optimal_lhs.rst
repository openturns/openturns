.. _optimal_lhs:

Optimal LHS design
^^^^^^^^^^^^^^^^^^

Let :math:`X=(x_1,\cdots, x_d)` be a random vector of input parameters.
Latin Hypercube Sample (LHS) is a way to distribute N sample points: each
parameter range is divided into N equal intervals, and sample points are
chosen such that any hyperplane in that dimension contains one and only one
sample point.

The goal of this module is to improve standard LHS techniques by minimizing
a space filling criterion.

Principles
~~~~~~~~~~
We may notice two types of LHS designs:

- Centered design is obtained by choosing for each point the center of the corresponding cell
- Randomized LHS is obtained by adding random perturbations inside each cell

Let the input vector X whose marginals are independent and associated probabilistic measure :math:`\mathcal{L}_{X}(x_1,...,x_d)`

In practice, we look for a design in the space :math:`[0,1]^d` and we use an inverse
iso-probabilistic transformation to get the result in the original domain.

Let :math:`\phi : [0,1]^d \rightarrow \mathbb{R}^{+}` be a a space filling criterion, which is a measure of *accuracy* of an optimal LHS design. 
Most of these criteria focus on discrepancy, which measures how far a given distribution of points deviates from a perfectly uniform one.

Two space filling criteria are implemented:

- The centered :math:`L^2`-discrepancy, called :math:`C_2` and given by:

.. math::

    C_2(X_{d}^N)^2 = \left(\frac{13}{12}\right)^{d} - \frac{2}{N} \sum_{i=1}^{N} \prod_{k=1}^{d} \left( 1 + \frac{1}{2} |x_k^{(i)} - 0.5| - \frac{1}{2} |x_k^{(i)} - 0.5|^2 \right)\\
                + \frac{1}{N^2} \sum_{i,j=1}^{N} \prod_{k=1}^{d} \left( 1 + \frac{1}{2} |x_k^{(i)} - 0.5| + \frac{1}{2} |x_k^{(j)} - 0.5| - \frac{1}{2} |x_k^{(i)} - x_k^{(j)}| \right)

This discrepancy is to be minimized to get an optimal design.

- The mindist criterion (minimal distance between two points in the design):

.. math:: \phi(X) = min_{} ||x^{(i)} - x^{(j)} ||_{L^2}, \forall i\neq j=1,\cdots N


This criterion is to be maximized.

- In practice, the :math:`\phi_p` criterion is used instead of mindist and writes:

.. math::
  \phi_p(X) = \left( \sum_{1\leq i < j \leq N} ||x^{(i)} - x^{(j)}||^{-p}_{L^2} \right)^{\frac{1}{p}}

This is supposed to be more robust. When p tends to infinity, optimizing a design with :math:`\phi_p` is equivalent to optimizing a design with *mindist*.
This criterion is to be minimized to get an optimal design.

The objective is to generate an LHS design :math:`X_{d}^{N}` that minimizes a space filling criterion :math:`\phi` (or maximizes mindist).
For that purpose, two techniques are implemented and presented
hereafter.

Monte Carlo
~~~~~~~~~~~
This problem can be approximated by a Monte Carlo algorithm: a fixed number of designs are generated, and the optimal one is kept.
This algorithm is trivial and available in :class:`~openturns.MonteCarloLHS`.
One of the major drawbacks of Monte Carlo sampling is the CPU time consumption, because the number of generated designs must be high.

Simulated Annealing
~~~~~~~~~~~~~~~~~~~
An alternate solution is to use an adapted simulated annealing method, available in :class:`~openturns.SimulatedAnnealingLHS`, which we will now describe.
Starting from an LHS design, a new design is obtained by permuting one random coordinate of two randomly chosen elements; by construction, this design is also an LHS design.
If the new design is better than the previous one, it is kept.
If it is worse, it may anyway be kept with some probability, which depends on how these designs compare, but also on a temperature profile T which decreases over time.
This means that jumping away from local extrema becomes less probable over time.

It is important to highlight here that this specific permutation has been chosen in this algorithm because it allows highly efficient computations of criterion during simulated annealing process.
he naive criterion evaluation, as is done in Monte Carlo algorithm, has a complexity of :math:`\mathcal{O}(d\times N^2)` for :math:`C_2` and :math:`\phi_p` criteria.


Let us first illustrate with the :math:`C_2` criterion. We set :math:`z_{ik}=x_{ik}-0.5`, equation rewrites:

.. math::

    C_2(X_{d}^N)^2 = \left(\frac{13}{12}\right)^{d} +\sum_{i=1}^{N}\sum_{j=1}^{N} c_{ij}

with:

.. math::
 :label: prior_GN

    c_{ij}= \,\,\,\left \{
    \begin{aligned}
    &\frac{1}{N^2}\prod_{k=1}^{d}\frac{1}{2}(2+|z_{ik}|+|z_{jk}|-|z_{ik}-z_{jk}|)\,\,\,\, \textrm{if}\,\, i\neq j \\
    &\frac{1}{N^2}\prod_{k=1}^{d}(1+|z_{ik}|)-\frac{2}{N}\prod_{k=1}^{d}(1+\frac{1}{2}|z_{ik}|-\frac{1}{2}z_{ik}^2) \,\,\,\,\textrm{otherwise} \\
    \end{aligned}
    \right.

We set :math:`c^{\prime}` the elements of a new design :math:`X^{\prime N}_{d}` obtained by permuting a coordinate of sample points :math:`i_1` and :math:`i_2`.
We can see that

.. math::
 :label: cond_update

    \left \{
    \begin{aligned}
    & c^{\prime}_{ij}=c_{ij} \;\forall i, j \text{ such that } 1\leq i,j\leq N,\, i\notin \{i_1,i_2\},\, j\notin \{i_1,i_2\}\\
    & c^{\prime}_{i_1i_2}=c_{i_1i_2}\\
    & c_{ij}=c_{ji} \;\forall 1\leq i,j\leq N
    \end{aligned}
    \right.

and thus, :math:`C_2(X')` becomes:

.. math::

    C_2(X^{\prime N}_{d})^2 = C_2(X^N_d)^2
        + c^{\prime}_{i_1i_1} + c^{\prime}_{i_2i_2} + 2\sum_{\substack{1\leq j\leq N\\j\neq i_1,i_2}} (c^{\prime}_{i_1j}+c^{\prime}_{i_2j})\\
      {} - c_{i_1i_1} - c_{i_2i_2} - 2\sum_{\substack{1\leq j\leq N\\j\neq i_1,i_2}} (c_{i_1j}+c_{i_2j})

Updating :math:`C_2` criterion can be performed by a :math:`\mathcal{O}(N)` algorithm, which has a much better complexity than a naive computation.

The same trick can also be applied on :math:`\phi_p` criterion, because we can write

.. math::

    \phi_p(X)^p
    = \sum_{1\leq i < j \leq N} ||x^{(i)} - x^{(j)}||^{-p}_{L^2}
    = \frac{1}{2} \sum_{i=1}^N \sum_{\substack{1\leq j\leq N\\j\neq i}} ||x^{(i)} - x^{(j)}||^{-p}_{L^2}
    = \sum_{i=1}^N \sum_{j=1}^N f_{ij}

with

.. math::

    f_{ij}= \,\,\,\left \{
    \begin{aligned}
    & \frac{||x^{(i)} - x^{(j)}||^{-p}_{L^2}}{2}, & i \neq j\\
    & 0, & i=j
    \end{aligned}
    \right.

These :math:`f_{ij}` coefficients satisfy the same conditions, so the same computations give:

.. math::

    \phi_p(X_{d}^{\prime N})^p = \phi_p(X_{d}^N)^p
      + 2\sum_{\substack{1\leq j\leq N\\j\neq i_1,i_2}} (f^{\prime}_{i_1j}+f^{\prime}_{i_2j})
      - 2\sum_{\substack{1\leq j\leq N\\j\neq i_1,i_2}} (f_{i_1j}+f_{i_2j})

In practice, a marginal transformation is performed to map the initial multivariate distribution into :math:`[0,1]^d`.
Optimization is performed in :math:`[0,1]^d` and the inverse transformation maps the design into the initial space.

.. topic:: API:

    - :class:`~openturns.MonteCarloLHS`
    - :class:`~openturns.SimulatedAnnealingLHS`


.. topic:: Examples:

    - See :doc:`/examples/optimal_lhs`


.. topic:: References:

    - [damblin2013]_
    - [fang2006]_
    - [jin2005]_
    - [koehler1996]_
    - [johnson1990]_
    - [morris1995]_
    - [pronzato2012]_

