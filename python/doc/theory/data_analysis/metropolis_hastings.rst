.. _metropolis_hastings:

The Metropolis-Hastings Algorithm
---------------------------------

| **Markov chain.** Considering a :math:`\sigma`-algebra :math:`\cA` on
  :math:`\Omega`, a Markov chain is a process
  :math:`{(X_k)}_{k\in\Nset}` such that

  .. math::

     \begin{aligned}
         \forall{}(A,x_0,\ldots,x_{k-1})\in\cA\times\Omega^k
         \quad \Prob{X_k\in A \,|\, X_0=x_0, \ldots, X_{k-1}=x_{k-1}}
         = \Prob{X_k\in A \,|\, X_{k-1}=x_{k-1}}.
       \end{aligned}

An example is the *random walk* for which
:math:`X_k = X_{k-1} + \varepsilon_k` where the steps
:math:`\varepsilon_k` are independent and identically distributed.

| **Transition kernel.** A transition kernel on :math:`(\Omega, \cA)` is
  a mapping :math:`K: (\Omega, \cA) \rightarrow [0, 1]` such that

-  :math:`\forall{}A\in\cA \quad K(., A)` is measurable;

-  | :math:`\forall{}x\in\Omega \quad K(x, .)` is a probability
     distribution on :math:`(\Omega, \cA)`.

The kernel :math:`K` has density :math:`k` if
:math:`\forall(x,A)\in\Omega\times\cA \quad K(x, A) = \displaystyle\int_A \: k(x, y) \mbox{d}y`.

| :math:`{(X_k)}_{k\in\Nset}` is a homogeneous Markov Chain of
  transition :math:`K` if
  :math:`\forall(A,x)\in\cA\times\Omega \quad \Prob{X_k\in{}A | X_{k-1}=x} = K(x, A)`.
| **Some Notations.** Let :math:`{(X_k)}_{k\in\Nset}` be a homogeneous
  Markov Chain of transition :math:`K` on :math:`(\Omega, \cA)` with
  initial distribution :math:`\nu` (that is :math:`X_0 \sim \nu`):

-  :math:`K_\nu` denotes the probability distribution of the Markov
   Chain :math:`{(X_k)}_{k\in\Nset}`;

-  :math:`\nu{}K^k` denotes the probability distribution of :math:`X_k`
   (:math:`X_k \sim \nu{}K^k`);

-  | :math:`K^k` denotes the mapping defined by
     :math:`K^k(x,A) = \Prob{X_k\in{}A|X_0=x}` for all
     :math:`(x,A)\in\Omega\times\cA`.

| **Total variation convergence.** A Markov Chain of distribution
  :math:`K_\nu` is said to converge in total variation distance towards
  the distribution :math:`t` if

  .. math::

     \begin{aligned}
         \lim_{k\to+\infty} \sup_{A\in\cA} \left|
         \nu{}K^k(A) - t(A)
         \right| = 0.
       \end{aligned}

Then the notation used here is :math:`\nu{}K^k \rightarrow_{TV} t`.

| **Some interesting properties.** Let :math:`t` be a (target)
  distribution on :math:`(\Omega, \cA)`, then a transition kernel
  :math:`K` is said to be:

-  :math:`t`-invariant if :math:`t{}K = t`;

-  :math:`t`-irreducible if, :math:`\forall(A,x)\in\Omega\times\cA` such
   that :math:`t(A)>0`, :math:`\exists{}k\in\cN^* \quad {}K^k(x, A) > 0`
   holds.

Markov Chain Monte-Carlo techniques allows to sample and integrate
according to a distribution :math:`t` which is only known up to a
multiplicative constant. This situation is common in Bayesian statistics
where the “target” distribution, the posterior one
:math:`t(\vect{\theta})=\pi(\vect{\theta} | \mat{y})`, is proportional
to the product of prior and likelihood: see equation :eq:`postdistribution`.

In particular, given a “target” distribution :math:`t` and a
:math:`t`-irreducible kernel transition :math:`Q`, the
Metropolis-Hastings algorithm produces a Markov chain
:math:`{(X_k)}_{k\in\Nset}` of distribution :math:`K_\nu` with the
following properties:

-  the transition kernel of the Markov chain is :math:`t`-invariant;

-  :math:`\nu{}K^k \rightarrow_{TV} t`;

-  the Markov chain satisfies the *ergodic theorem*: let :math:`\phi` be
   a real-valued function such that
   :math:`\mathbb{E}_{X\sim{}t}\left[ |\phi(X)| \right] <+\infty`, then, whatever the
   initial distribution :math:`\nu` is:

   .. math::

      \begin{aligned}
            \displaystyle\frac{1}{n} \sum_{k=1}^n \: \phi(X_k) \tendto{k}{+\infty} \mathbb{E}_{X\sim{}t}\left[ |\phi(X)| \right]
            \mbox{ almost surely}.
          \end{aligned}

In that sense, simulating :math:`{(X_k)}_{k\in\Nset}` amounts to
sampling according to :math:`t` and can be used to integrate relatively
to the probability measure :math:`t`. Let us remark that the ergodic
theorem implies that
:math:`\displaystyle\frac{1}{n} \sum_{k=1}^n \: \fcar{A}{X_k} \tendto{k}{+\infty} \ProbCond{X\sim{}t}{X\in{}A}` almost surely.

By abusing the notation, :math:`t(x)` represents, in the remainder of
this section, a function of :math:`x` which is proportional to the PDF
of the target distribution :math:`t`. Given a transition kernel
:math:`Q` of density :math:`q`, the scheme of the Metropolis-Hastings
algorithm is the following (lower case letters are used hereafter for
both random variables and realizations as usual in the Bayesian
literature):

0)
    Draw :math:`x_0 \sim \nu` and set :math:`k = 1`.

1)
    Draw a candidate for :math:`x_k` according to the given transition
    kernel :math:`Q`: :math:`\tilde{x} \sim Q(x_{k-1}, .)`.

2)
    Compute the ratio
    :math:`\rho = \displaystyle\frac{t(\tilde{x})/q(x_{k-1},\tilde{x})} {t(x_{k-1})/q(\tilde{x},x_{k-1})}`.

3)
    Draw :math:`u \sim \cU([0, 1])`; if :math:`u \leq \rho` then set
    :math:`x_k = \tilde{x}`, otherwise set :math:`x_k = x_{k-1}`.

4)
    Set :math:`k=k+1` and go back to 1).

Of course, if :math:`t` is replaced by a different function of :math:`x`
which is proportional to it, the algorithm keeps unchanged, since
:math:`t` only takes part in the latter in the ratio
:math:`\frac{t(\tilde{x})}{t(x_{k-1})}`. Moreover, if :math:`Q` proposes
some candidates in a uniform manner (constant density :math:`q`), the
candidate :math:`\tilde{x}` is accepted according to a ratio
:math:`\rho` which reduces to the previous “natural” ratio
:math:`\frac{t(\tilde{x})}{t(x_{k-1})}` of PDF. The introduction of
:math:`q` in the ratio :math:`\rho` prevents from the bias of a
non-uniform proposition of candidates which would favor some areas of
:math:`\Omega`.

The :math:`t`-invariance is ensured by the symmetry of the expression of
:math:`\rho` (:math:`t`-reversibility).

In practice, :math:`Q` is specified as a random walk
(:math:`\exists{}q_{RW}` such that :math:`q(x,y)=q_{RW}(x-y)`) or as a
independent sampling (:math:`\exists{}q_{IS}` such that
:math:`q(x,y)=q_{IS}(y)`), or as a mixture of random walk and
independent sampling.

| The important property the practitioner have to keep in mind when
  choosing the transition kernel :math:`Q` is the
  :math:`t`-irreducibility. Moreover, for efficient convergence,
  :math:`Q` has to be chosen so as to explore quickly the whole support
  of :math:`t` without conducting to a too small acceptance ratio (the
  ratio of accepted candidates :math:`\tilde{x}` ). It is usually
  recommended that this latter ratio is about :math:`0.2` but such a
  ratio is neither a warranty of efficiency, nor a substitute to a
  convergence diagnosis.

.. topic:: API:

    - See :class:`~openturns.RandomWalkMetropolisHastings`

.. topic:: Examples:

    - See :ref:`examples/data_analysis/bayesian_calibration.ipynb`

.. topic:: References:

    - Robert, C.P. and Casella, G. (2004). *Monte Carlo Statistical Methods* (Second Edition), Springer.
    - Meyn, S. and Tweedie R.L. (2009). *Markov Chains ans Stochastic Stability* (Second Edition), Cambridge University Press.
