.. _enumeration_strategy:

Multivariate indices enumeration functions
------------------------------------------
Enumeration functions are bijections from :math:`\Nset` to :math:`\Nset^{\inputDim}`. We detail here some particular bijections:

- Hyperbolic enumeration function,
- Linear enumeration function,
- Anisotropic hyperbolic enumeration function,
- Infinity norm enumeration function.

All these enumeration functions are illustrated in :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_enumeratefunction`.

A possible use is to build a multivariate basis as the tensorization of univariate basis: this is the
case for example in the functional chaos expansion setting (refer to  :ref:`functional_chaos` and
:ref:`enumeration_multivariate_basis`).

Let  :math:`\vect{\alpha}` be a multi-index is defined by:

.. math::

    \vect{\alpha} = (\alpha_0, \dots, \alpha_{\inputDim-1}) \in \Nset^{\inputDim}

For any real number :math:`q > 0`, we consider the
:math:`q`-*hyperbolic norm* (or :math:`q`-*norm* for short) of a
multi-index :math:`\vect{\alpha}` be defined by:

.. math::
   :label: qnorm_def

    \|\vect{\alpha}\|_{q} \, \, = \, \, \left(\sum_{i=0}^{\inputDim-1} \; \alpha_i^q \right)^{1/q}

The operator :math:`\|\cdot\|_q` is  a norm if anf only if :math:`q \geq 1` and is
a *pseudo-norm* if :math:`0 < q < 1` since it does not satisfy the triangular
inequality. However this abuse of language will be used in the
following. Note that the case :math:`q=1` corresponds to the
definition of the length of :math:`\vect{\alpha}`.

An enumeration function :math:`\tau` is a bijection from :math:`\Nset` to :math:`\Nset^{\inputDim}`,
which creates a one-to-one mapping between an integer :math:`j` and a multi-index :math:`\vect{\alpha}`. The function :math:`\tau`
is defined by:

.. math::

   \begin{array}{llcl}
         \tau \, : & \Nset & \longrightarrow & \Nset^{\inputDim} \\
         &  j & \longmapsto & \vect{\alpha}(j)
    \end{array}

The enumeration function orders the elements according to the *graded reverse lexicographic order* ([sullivan2015]_) as follows:

- :math:`\tau(0) = \vect{0}`,
- for any :math:`k \in \Nset^*` and any :math:`j \in \left\{ 0, \dots, k-1 \right\}` we have:

  .. math::

      \tau(j) < \tau(k)


which means that:

- either :math:`\|\tau(j)\|_{q} < \|\tau(k)\|_{q}`,
- or :math:`\|\tau(j)\|_{q} = \|\tau(k)\|_{q}` and : either :math:`\alpha_0(j) > \alpha_0(k)`,
  or there exists :math:`m \in \{1,\dots,\inputDim-1\}` such that:

  .. math::

      \begin{array}{lcl}
      \alpha_0(j) & = & \alpha_0(k) \\
                & \vdots & \\
      \alpha_{m - 1}(j) & = & \alpha_{m - 1}(k) \\
      \alpha_m(j) & > & \alpha_m(k)
      \end{array}


  which means that in that case, either the first component of :math:`\tau(j)` is greater
  than the first component of :math:`\tau(k)`, or all the components of :math:`\tau(j)`
  and :math:`\tau(k)` are equal until the component :math:`m-1` and the component :math:`m`
  of :math:`\tau(j)` is greater than the component :math:`m` of :math:`\tau(k)`.

Hyperbolic enumeration function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Let :math:`\lambda` be a real positive number. Let :math:`\cA_{\lambda}` be the set of
multi-indices with :math:`q`-norm not greater than :math:`\lambda` as
follows:

.. math::
    :label: def_Aset

    \cA_{\lambda} \, \, = \, \, \{\vect{\alpha} \in \Nset^{\inputDim} \, : \, \|\vect{\alpha}\|_q \, \leq \lambda \}.

Moreover, let the *front* of :math:`\cA_{\lambda}` be defined by:

.. math::

   \partial \cA_{\lambda} \, \, = \, \, \left\{\vect{\alpha} \in \cA_{\lambda} \, : \, \exists \; i \; \in \; \{0,\dots,\inputDim-1\} \, , \, \, \vect{\alpha} \, + \, \vect{e_i} \, \notin \, \cA_{\lambda} \right\}

where :math:`\vect{e_i}` is a multi-index with a unit :math:`i`-entry
and zero :math:`k`-entries, :math:`k\neq i`.

We also define the set of *candidates* from the elements of :math:`\cA_\lambda`. The set of
the candidates is denoted by :math:`\cC_\lambda` and is defined by:

.. math::

     \cC_\lambda\, \, = \, \, \left\{\vect{\alpha} \, + \, \vect{e_i} \, : \,
     \vect{\alpha} \in \partial \cA_{\lambda} \, , \,
     \vect{\alpha} + \, \vect{e_i} \notin  \cA_{\lambda} \, , \,
     0 \leq i \leq \inputDim-1, \right\}


We note that for all :math:`\lambda`, :math:`\cC_\lambda \neq \emptyset` because for any :math:`\lambda \in \Rset^+`,
there exists :math:`\vect{\alpha} \in \Nset^\inputDim` such that  :math:`\|\vect{\alpha}\|_{q} > \lambda`.

The principle consists in exploring the space :math:`\Nset^{\inputDim}` through the
:math:`q`-norm of its elements. In this purpose, we define an appropriate
increasing sequence :math:`(\lambda_n)_{n \in \Nset}`  as follows:

.. math::

     \left\{
       \begin{array}{l}
         \lambda_0  =  0 \\
         \lambda_{n+1}  =  \min_{\vect{\alpha} \in \cC_{\lambda_n}}  \left\{ \|\vect{\alpha}\|_{q} \right\}
       \end{array}
     \right.

The sequence is well defined because by definition, all the elements of
:math:`\cC_{\lambda_n}` have a :math:`q`-*norm* strictly greater than :math:`\lambda_n`.

From the sequence :math:`(\lambda_n)_{n \in \Nset}`, we call :math:`(\cA_{\lambda_n})_{n \in \Nset}` the sequence of *cumulated strata*.
The sequence :math:`(\Delta_n)_{n \in \Nset}` of the strata is defined by:

.. math::

   \left\{
       \begin{array}{l}
         \Delta_0 =  \cA_{\lambda_{0}} \\
         \Delta_{n+1} =  \cA_{\lambda_{n+1}}  \setminus  \cA_{\lambda_n}
       \end{array}
    \right.

Note that we have :math:`\cA_{\lambda_{n+1}} \subset \cA_{\lambda_n} \cup \cC_n` and that
:math:`\Delta_n =  \left\{ \vect{\alpha} \in \Nset^\inputDim, \|\vect{\alpha}\|_{q} = \lambda_n \right\}`.

The sequence  of strata is depicted in  :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_enumeratefunction`.

Anisotropic hyperbolic enumeration function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We consider enumeration functions based on an
*anisotropic* hyperbolic norm defined by:

.. math::
    :label: anisotropEnumFct

    \|\vect{\alpha}\|_{\vect{w},q} \, \, = \, \, \left(\sum_{i=0}^{\inputDim-1} \; w_i\; \alpha_i^q \right)^{1/q}

where the weights :math:`w_i` are real positive numbers. They enable to weight
some specific marginal indices.

In this setup, we consider both schemes outlined in the previous paragraph: it is only necessary to
replace the isotropic :math:`q`-norm in :eq:`def_Aset` with the
:math:`(\vect{w},q)`-anisotropic one.

This enumerate function emphasizes multi-indices whose components are larger
when the associated weights are smaller.

Infinity norm enumeration function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
We consider the enumeration function based on the infinite norm:

.. math::
     :label: def_infNorm

     \|\vect{\alpha}\|_{\infty} \, \, = \, \, \max_{0 \leq i \leq \inputDim-1} \; \alpha_i


In that case, we have :math:`\lambda_n = n`.

If :math:`q \rightarrow 0`, then
:math:`\|\vect{\alpha}\|_{q} \rightarrow \|\vect{\alpha}\|_{\infty}`.


.. topic:: API:

    - See :class:`~openturns.LinearEnumerateFunction`
    - See :class:`~openturns.HyperbolicAnisotropicEnumerateFunction`
    - See :class:`~openturns.NormInfEnumerateFunction`

.. topic:: Examples:

    - See :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_functional_chaos`
    - See :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_enumeratefunction`
    - See :doc:`/auto_surrogate_modeling/fields_surrogate_models/plot_fieldfunction_metamodel`

.. topic:: References:

    - [blatman2009]_
