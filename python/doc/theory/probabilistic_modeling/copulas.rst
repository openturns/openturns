.. _copula:

Copulas
-------

Let :math:`F` be a multivariate distribution function of dimension
:math:`d` whose marginal distribution functions are
:math:`F_1,\dots,F_d`. There exists a copula
:math:`C: [0,1]^d \mapsto [0,1]` of dimension :math:`d` such that for :math:`\vect{x}\in \Rset^d`, we have:

.. math::

   \begin{aligned}
       F(\vect{x})  = C \left( F_1(x_1),\cdots,F_d(x_d) \right)
    \end{aligned}

where :math:`F_i` is the cumulative distribution function of the margin  :math:`X_i`.

In the case of continuous marginal distributions, for all :math:`\vect{u}\in[0,1]^d`, the copula is uniquely defined by:

.. math::

   \begin{aligned}
   C(\vect{u}) & = F(F_1^{-1}(u_1),\hdots,F_d^{-1}(u_d))\\
          & = \Prob{U_1 \leq u_1, \hdots, U_d \leq u_d}
    \end{aligned}

where :math:`U_i = F_i(X_i)` is the random variate following the uniform distribution on :math:`[0,1]`.

A copula of dimension :math:`d` is the restriction to the unit cube :math:`[0,1]^d` of a
multivariate distribution function with uniform univariate marginals on :math:`[0,1]`.
It has the following properties: 

- :math:`\forall \vect{u},\vect{v}\in[0,1]^d, |C(\vect{u})-C(\vect{v})|\leq \sum_{i=1}^d |u_i-v_i|`,

- for all :math:`\vect{u}` with at least one component equal to 0, :math:`C(\vect{u})=0`,

- :math:`C` is :math:`d` -increasing wich means that:

  .. math::

   \sum_{i_1=1}^2 \dots \sum_{i_d=1}^2 (-1)^{i_1 + \dots + i_d} C(x_{1i_1}, \dots, x_{ni_d})\geq 0


where :math:`x_{j1}=a_j` and :math:`x_{j2}=b_j` for all :math:`j \in \{1,\dots,d\}` and
:math:`\vect{a}`, :math:`\vect{b}\in[0,1]^d`, :math:`\vect{a}\leq \vect{b}`,

-  :math:`\vect{u}` with all its components equal to 1 except  :math:`u_k`,
   :math:`C(\vect{u})=u_k`.



The copula represents the part of the joined cumulative density function which is not described by the
marginal distributions. It models the dependence structure of the input variables.
  
In the probabilistic modelisation issue, the choice of the dependence structure is disconnected from the
choice of the marginal distributions.

.. topic:: API:

    - See the list of available :ref:`copulas <copulas>`.

.. topic:: Examples:

    - See :doc:`/auto_probabilistic_modeling/copulas/plot_create_copula`
    - See :doc:`/auto_probabilistic_modeling/copulas/plot_composed_copula`
    - See :doc:`/auto_probabilistic_modeling/copulas/plot_extract_copula`

.. topic:: References:

    - Nelsen, *Introduction to Copulas*
    - Embrechts P., Lindskog F., Mc Neil A., *Modelling dependence with copulas and application to Risk Management*, ETZH 2001.

