.. _copula:

Copulas
-------

| In this part, we will define the concept of copula.

| To define the joined probability density function of the random input
  vector :math:`\uX` by composition, one needs:

-  the specification of the copula of interest :math:`C` with its
   parameters,

-  the specification of the :math:`{n_X}` marginal laws of interest
   :math:`F_{X_i}` of the :math:`{n_X}` input variables :math:`X_i`.

The joined cumulative density function is therefore defined by:

.. math::

   \begin{aligned}
       \Prob{X^1 \leq x^1, X^2 \leq x^2, \cdots, X^{n_X} \leq x^{n_X}}       = C\left(F_{X^1}(x^1),F_{X^2}(x^2),\cdots,F_{X^{n_X}}(x^{n_X}) \right)
     \end{aligned}

| Copulas allow to represent the part of the joined cumulative density
  function which is not described by the marginal laws. It enables to
  represent the dependency structure of the input variables. A copula is
  a special cumulative density function defined on :math:`[0,1]^{n_X}`
  whose marginal distributions are uniform on :math:`[0,1]`. The choice
  of the dependence structure is disconnected from the choice of the
  marginal distributions.
| A copula, restricted to :math:`[0,1]^{n_X}` is a
  :math:`n_U`-dimensional cumulative density function with uniform
  marginals.

-  :math:`C(\underline{u}) \geq 0`,
   :math:`\forall \underline{u} \in [0,1]^{n_U}`

-  :math:`C(\underline{u}) = u_i`,
   :math:`\forall \underline{u}=(1,\ldots,1,u_i,1,\ldots,1)`

-  For all :math:`N`-box
   :math:`\cB = [a_1,b_1] \times \cdots \times [a_{n_U},b_{n_U}] \in [0,1]^{n_U}`,
   we have :math:`\cV_C(\cB) \geq 0`, where:

   -  :math:`\cV_C(\cB) = \sum_{i=1,\cdots, 2^{n_U}} sign(\underline{v}_i) \times C(\underline{v}_i)`,
      the summation being made over the :math:`2^{n_U}` vertices
      :math:`\underline{v_i}` of :math:`\cB`.

   -  | :math:`sign(\underline{v}_i)= +1` if :math:`v_i^k = a_k` *for an
        even number of* :math:`k's`, :math:`sign(\underline{v}_i)= -1`
        *otherwise*.

.. topic:: API:

    - See the list of available :ref:`copulas <copulas>`.

.. topic:: Examples:

    - See :doc:`/examples/probabilistic_modeling/create_copula`
    - See :doc:`/examples/probabilistic_modeling/composed_copula`
    - See :doc:`/examples/probabilistic_modeling/extract_copula`

.. topic:: References:

    - Nelsen, *Introduction to Copulas*
    - Embrechts P., Lindskog F., Mc Neil A., *Modelling dependence with copulas and application to Risk Management*, ETZH 2001.

