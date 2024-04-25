.. _copula:

Copulas
-------

Let :math:`F` be a multivariate distribution function of dimension
:math:`\inputDim` whose marginal distribution functions are
:math:`F_1,\dots,F_{\inputDim}`. There exists a copula
:math:`C: [0,1]^{\inputDim} \mapsto [0,1]` of dimension :math:`\inputDim` such that for :math:`\vect{x}\in \Rset^{\inputDim}`, we have:

.. math::

   \begin{aligned}
       F(\vect{x})  = C \left( F_1(x_1),\cdots,F_{\inputDim}(x_{\inputDim}) \right)
    \end{aligned}

where :math:`F_i` is the cumulative distribution function of the margin  :math:`X_i`.

In the case of continuous marginal distributions, for all :math:`\vect{u}\in[0,1]^{\inputDim}`, the copula is uniquely defined by:

.. math::

   \begin{aligned}
   C(\vect{u}) & = F(F_1^{-1}(u_1),\hdots,F_{\inputDim}^{-1}(u_{\inputDim}))\\
          & = \Prob{U_1 \leq u_1, \hdots, U_{\inputDim} \leq u_{\inputDim}}
    \end{aligned}

where :math:`U_i = F_i(X_i)` is a random variable following the uniform distribution on :math:`[0,1]`.

A copula of dimension :math:`\inputDim` is the restriction to the unit cube :math:`[0,1]^{\inputDim}` of a
multivariate distribution function with uniform univariate marginals on :math:`[0,1]`.
It has the following properties:

- :math:`\forall \vect{u},\vect{v}\in[0,1]^{\inputDim}, |C(\vect{u})-C(\vect{v})|\leq \sum_{i=1}^{\inputDim} |u_i-v_i|`,

- for all :math:`\vect{u}` with at least one component equal to 0, :math:`C(\vect{u})=0`,

- :math:`C` is :math:`\inputDim`-increasing which means that:

  .. math::

   \sum_{i_1=1}^2 \dots \sum_{i_{\inputDim}=1}^2 (-1)^{i_1 + \dots + i_{\inputDim}} C(x_{1i_1}, \dots, x_{\inputDim i_{\inputDim}})\geq 0


where :math:`x_{j1}=a_j` and :math:`x_{j2}=b_j` for all :math:`j \in \{1,\dots,\inputDim\}` and
:math:`\vect{a}`, :math:`\vect{b}\in[0,1]^{\inputDim}`, :math:`\vect{a}\leq \vect{b}`,

-  :math:`\vect{u}` with all its components equal to 1 except  :math:`u_k`,
   :math:`C(\vect{u})=u_k`.


The copula represents the part of the joint cumulative density function which is not described by the
marginal distributions. It models the dependence structure of the input variables.

Note that a multivariate distribution is characterized by its marginal distributions and its copula.
Therefore, a multivariate distribution can be built by choosing the marginals and the copula independently.

.. topic:: API:

    - See the list of available :ref:`copulas <copulas>`.

.. topic:: Examples:

    - See :doc:`/auto_probabilistic_modeling/copulas/plot_create_copula`
    - See :doc:`/auto_probabilistic_modeling/copulas/plot_composed_copula`
    - See :doc:`/auto_probabilistic_modeling/copulas/plot_extract_copula`

.. topic:: References:

    - Nelsen, *Introduction to Copulas*
    - Embrechts P., Lindskog F., Mc Neil A., *Modelling dependence with copulas and application to Risk Management*, ETZH 2001.

