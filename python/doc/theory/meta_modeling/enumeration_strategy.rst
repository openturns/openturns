.. _enumeration_strategy:

Chaos basis enumeration strategies
----------------------------------

| The polynomial chaos (PC) expansion allows one to obtain an explicit
  representation of the random response :math:`\underline{Y}` of the
  model under consideration. More precisely, the response is cast
  as a converging series featuring orthonormal polynomials. For
  computational purpose, it is necessary though to retain a finite
  number of terms by truncating the expansion. First of all, a specific
  strategy for enumerating the infinite PC series has to be defined.
  This is the scope of the current section.

Given an input random vector :math:`\vect{X}` with prescribed
probability density function (PDF) :math:`f_{\vect{X}}(\vect{x})`, it is
possible to build up a *polynomial chaos* (PC) basis
:math:`\{\psi_{\idx},\idx \in \NM\}` . Of interest is the definition of
enumeration strategies for exploring this basis, i.e. of suitable
*enumeration functions* :math:`\tau` from :math:`\Nset` to :math:`\NM`,
which creates a one-to-one mapping between an integer :math:`j` and a
multi-index :math:`\idx`.

Linear enumeration strategy
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Let us first define the *total degree* of any multi-index :math:`\idx`
in :math:`\NM` by :math:`\sum_{i=1}^{n_X} \alpha_i`. A natural choice to
sort the PC basis (i.e. the multi-indices :math:`\idx`) is the
lexicographical order with a constraint of increasing total degree.
Mathematically speaking, a bijective enumeration function :math:`\tau`
is defined by:

.. math::

   \begin{array}{llcl}
         \tau \, : & \Nset & \longrightarrow & \NM \\
         &  j & \longmapsto & \{\alpha_1,\dots, \alpha_M\} \, \equiv \, \{\tau_1(j),\dots,\tau_M(j)\} \\
       \end{array}

such that:

.. math:: \tau(0) \, \, = \, \, \{0,\dots,0\}

and

.. math::

   \forall 1 \leq j<k  \quad \, , \quad \, \left\{
       \begin{array}{l}
         \displaystyle{\sum_{i=1}^{n_X} \tau_i(j) \, < \,  \sum_{i=1}^{n_X} \tau_i(k)  }  \\
         \\
         \mbox{ or} \\
         \\
         \displaystyle{\exists \; m \; \in \; \{1,\dots,n_X\} \; : \; \left( \; \forall \; i \; \leq \; m \; \; , \; \; \tau_i(j) \, = \, \tau_i(k) \; \right) \, \, \, \mbox{ and } \, \, \, \left( \; \tau_m(j) \, < \, \tau_m(k) \; \right)} \\
       \end{array}
       \right.

Such an enumeration strategy is illustrated in a two-dimensional case
(i.e. :math:`n_X=2`) in the figure below:

.. plot::

    import matplotlib.pyplot as plt

    # Create the figure
    plt.figure(1, figsize=(4, 4))
    ax = plt.subplot(111)

    # Create the points
    ax.plot([0, 0, 1, 0, 1, 2, 3, 2, 1, 0], [
            0, 1, 0, 2, 1, 0, 0, 1, 2, 3], "o", markersize=9)

    # Create the arrows
    ax.annotate("",
                xy=(0.97, 0), xycoords='data',
                xytext=(0, 0), textcoords='data',
                arrowprops=dict(
                    arrowstyle="-|>", linestyle="dashed", mutation_scale=15,
                                connectionstyle="arc3", color='black'),
                )

    ax.arrow(1, 0, -0.97, 0.97, head_width=0.08, head_length=0.08, fc='k',
            ec='k', length_includes_head=True, linestyle="dashed",)
    ax.arrow(0, 1, 1.97, -0.97, head_width=0.08, head_length=0.08, fc='k',
            ec='k', length_includes_head=True, linestyle="dashed",)
    ax.arrow(2, 0, -1.97, 1.97, head_width=0.08, head_length=0.08, fc='k',
            ec='k', length_includes_head=True, linestyle="dashed",)
    ax.arrow(0, 2, 2.97, -1.97, head_width=0.08, head_length=0.08, fc='k',
            ec='k', length_includes_head=True, linestyle="dashed",)
    ax.arrow(3, 0, -2.97, 2.97, head_width=0.08, head_length=0.08, fc='k',
            ec='k', length_includes_head=True, linestyle="dashed",)
    ax.arrow(0, 3, 1.97, -0.97, head_width=0.08, head_length=0.08, fc='k',
            ec='k', length_includes_head=True, linestyle="dashed",)

    # Annotate points
    ax.annotate('4',
                xy=(1, 1), xycoords='data',
                xytext=(-20, -5), textcoords='offset points', fontsize=16)

    ax.annotate('7',
                xy=(2, 1), xycoords='data',
                xytext=(+15, +0), textcoords='offset points', fontsize=16)

    ax.annotate('8',
                xy=(1, 2), xycoords='data',
                xytext=(+15, +0), textcoords='offset points', fontsize=16)

    # Add labels
    ax.annotate(r'$\tau_1$', xy=(1, 0), xytext=(10, 10), ha='left', va='center',
                xycoords='axes fraction', textcoords='offset points', fontsize=20)

    ax.annotate(r'$\tau_2$', xy=(0, 1), xytext=(0, 10), ha='left', va='center',
                xycoords='axes fraction', textcoords='offset points', fontsize=20)

    # Hide spines
    ax.spines['right'].set_color('none')
    ax.spines['top'].set_color('none')

    # Set spines's position
    ax.xaxis.set_ticks_position('bottom')
    ax.spines['bottom'].set_position(('data', 0))
    ax.yaxis.set_ticks_position('left')
    ax.spines['left'].set_position(('data', 0))

    # Add labels
    plt.xticks([-0.1] + list(range(4)) + [3.2])
    ax.set_xticklabels(('', '$0$', '$1$', '$3$', '$6$'), fontsize=20)
    plt.yticks([-0.1] + list(range(4)) + [3.2])
    ax.set_yticklabels(('', '', '$2$', '$5$', '$9$'), fontsize=20)

    # Show the figure
    plt.show()


This corresponds to the following enumeration of the multi-indices:

+-------------+-----------------------------------------------+
| :math:`j`   | :math:`\idx \, = \, \{\alpha_1,\alpha_2\}`    |
+=============+===============================================+
| :math:`0`   | :math:`\{0,0\}`                               |
+-------------+-----------------------------------------------+
| :math:`1`   | :math:`\{0,1\}`                               |
+-------------+-----------------------------------------------+
| :math:`2`   | :math:`\{1,0\}`                               |
+-------------+-----------------------------------------------+
| :math:`3`   | :math:`\{2,0\}`                               |
+-------------+-----------------------------------------------+
| :math:`4`   | :math:`\{1,1\}`                               |
+-------------+-----------------------------------------------+
| :math:`5`   | :math:`\{0,2\}`                               |
+-------------+-----------------------------------------------+
| :math:`6`   | :math:`\{3,0\}`                               |
+-------------+-----------------------------------------------+
| :math:`7`   | :math:`\{2,1\}`                               |
+-------------+-----------------------------------------------+
| :math:`8`   | :math:`\{1,2\}`                               |
+-------------+-----------------------------------------------+
| :math:`9`   | :math:`\{0,3\}`                               |
+-------------+-----------------------------------------------+

Hyperbolic enumeration strategy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

| The *hyperbolic* truncation strategy is inspired by the so-called
  *sparsity-of-effects principle*, which states that most models are
  principally governed by main effects and low-order interactions.
  Accordingly, one wishes to define an enumeration strategy which first
  selects those multi-indices related to main effects, i.e. with a
  reasonably small number of nonzero components, prior to selecting
  those associated with higher-order interactions.
| For any real number :math:`q` in :math:`(0,1]`, one defines the
  :math:`q`-*hyperbolic norm* (or :math:`q`-*norm* for short) of a
  multi-index :math:`\idx` by:

  .. math:: \|\idx\|_{q} \, \, = \, \, \left(\sum_{i=1}^{n_X} \; \alpha^q \right)^{1/q}

  Strictly speaking, :math:`\|\cdot\|_q` is not properly a norm but
  rather a *quasi-norm* since it does not satisfy the triangular
  inequality. However this abuse of language will be used in the
  following. Note that the case :math:`q=1` corresponds to the
  definition of the total degree.

| Let :math:`\lambda` be a real positive number. One defines the set of
  multi-indices with :math:`q`-norm not greater than :math:`\lambda` as
  follows:

  .. math::
    :label: eq_q_set

      \cA_{\lambda} \, \, = \, \, \{\idx \in \NM \, : \, \|\idx\|_q \, \leq \lambda \}

  Moreover, one defines the *front* of :math:`\cA_{\lambda}` by:

  .. math:: \partial \cA_{\lambda} \, \, = \, \, \left\{\idx \in \cA_{\lambda} \, : \, \exists \; i \; \in \; \{1,\dots,n_X\} \, , \, \, \idx \, + \, \vect{e_i} \, \notin \, \cA_{\lambda} \right\}

  where :math:`\vect{e_i}` is a multi-index with a unit :math:`i`-entry
  and zero :math:`k`-entries, :math:`k\neq i`.

| The idea consists in exploring the space :math:`\NM` by progressively
  increasing the :math:`q`-norm of its elements. In this purpose, one
  wants to construct an enumeration function that relies upon (1) the
  bijection :math:`\tau` defined in the previous paragraph and (2) an
  appropriate increasing sequence :math:`(\lambda_n)_{\Nset}` that tends
  to infinity. Such a sequence can be used to define a specific
  partition of :math:`\NM` into *stratas* :math:`(\Delta_n)_{\Nset}`.
  Precisely, the enumeration of the multi-indices is achieved by sorting
  the elements of :math:`\Delta_n` in ascending order of the
  :math:`q`-norm, and then by sorting the possible elements having the
  same :math:`q`-norm using the bijection :math:`\tau`. Several examples
  of partition are given in the sequel.
| *Partition based on the total degree.* We can simply define the
  sequence :math:`(\lambda_n)_{\Nset}` as the set of natural integers
  :math:`\Nset`. Thus we build up a sequence :math:`(\Delta_n)_{\Nset}`
  of stratas as follows:

  .. math::

     \left\{
       \begin{array}{l}
         \Delta_0 \, \, = \, \, \{\vect{0}\} \\
         \forall \; n  \geq  1 \, \, , \, \, \Delta_n \, \, = \, \, \cA_{n} \; \setminus \; \cA_{n-1}  \, \, = \, \,
         \{\idx \in \NM \, : \, n - 1 \, < \, \|\idx\|_q \, \leq n \}      \\
       \end{array}
       \right.

  The progressive exploration of :math:`\NM` is depicted in the
  two-dimensional case for various values of the parameter :math:`q`:

.. plot::

    import openturns as ot
    from matplotlib import pyplot as plt
    from openturns.viewer import View

    nrows=3
    ncols=4

    # coordinates of grid
    grid = ot.Box([5, 5], ot.Interval([0.0]*2, [6.0]*2))
    sample = grid.generate()
    grid_x = sample.getMarginal(0)
    grid_y = sample.getMarginal(1)

    #plt.rc('text', usetex=True)

    q_values = [1.0, 0.75, 0.5]
    fig = plt.figure()
    index = 1
    for i in range(nrows):
        q = q_values[i]
        enumerate = ot.HyperbolicAnisotropicEnumerateFunction(2, q)
        for j in range(ncols):
            ax = fig.add_subplot(nrows, ncols, index, aspect=1.0)
            ax.plot(grid_x, grid_y, 'xr')
            strataIndex = j + 3
            strata_x, strata_y = [], []
            strataCardinal = enumerate.getStrataCumulatedCardinal(strataIndex)
            for ii in range(strataCardinal):
                x = enumerate(ii)
                strata_x.append(x[0])
                strata_y.append(x[1])
            ax.plot(strata_x, strata_y, 'ob')
            ax.set_yticks([])
            #ax.set_title('$||x||_{'+str(q)+'} \leq '+str(strataIndex)+'$')
            ax.set_title('||x||q=' + str(q) + ' < ' + str(strataIndex))
            index += 1
    plt.subplots_adjust(hspace=0.5)
    plt.show()


| As expected, the hyperbolic norms penalize the indices associated with
  high-order interactions all the more since :math:`q` is low. Note that
  setting :math:`q` equal to 1 corresponds to the usual *linear*
  enumeration strategy. Then the retained polynomials are located under
  a straight line, hence the label *linear enumeration strategy*. In
  contrast, when :math:`q < 1`, the retained basis polynomials are
  located under an hyperbola, hence the name *hyperbolic enumeration
  strategy*.
| *Partition based on disjoint fronts.* Instead of considering the
  sequence of natural integers, we define the sequence
  :math:`(\lambda_n)_{\Nset}` recursively by:

  .. math::

     \left\{
       \begin{array}{l}
         \lambda_0 \, \, = \, \, 0 \\
         \forall \; n  \geq  1 \, \, , \, \, \lambda_n \, \, = \, \,
         \inf_{\lambda \in \Rset^+} \; \left\{ \lambda \geq \lambda_{n-1} \, \, \mbox{ and } \, \,\partial \cA_{\lambda} \, \cap \, \partial \cA_{\lambda_{n-1}} \, = \, \emptyset \right\}
       \end{array}
       \right.

  In other words, :math:`\lambda_n` is the infimum of the real numbers
  :math:`\lambda` for which the new front contains only element which do
  not belong to the former one. Hence the sequence of stratas:

  .. math::

     \left\{
       \begin{array}{l}
         \Delta_0 \, \, = \, \, \{\vect{0}\} \\
         \forall \; n  \geq  1 \, \, , \, \, \Delta_n \, \, = \, \, \cA_{\lambda_n} \; \setminus \; \cA_{\lambda_{n-1}} \\
       \end{array}
       \right.

  Note that this partition of :math:`\NM` is finer than the one based
  on total degrees, since the cardinality of the stratas is smaller.

Anisotropic hyperbolic enumeration strategy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

| One might also consider enumeration strategies based on an
  *anisotropic* hyperbolic norm defined by:

  .. math:: \|\idx\|_{\vect{w},q} \, \, = \, \, \left(\sum_{i=1}^{n_X} \; w_i\; \alpha^q \right)^{1/q}

  where the :math:`w_i`\ ’s are real positive numbers. This would lead
  to first select the basis polynomials depending on a specific subset
  of input variables.

| In this setup, it is also possible to explore the space :math:`\NM` of
  the multi-indices by partitioning it according to one of the two
  schemes outlined in the previous paragraph (it is only necessary to
  replace the isotropic :math:`q`-norm in :eq:`eq_q_set` with the
  :math:`(\vect{w},q)`-anisotropic one).
| We may also build up an alternative partition related to the *partial
  degree* of the most important variable, i.e. the one associated to the
  *smallest* weight :math:`w_i`. Then the sequence
  :math:`(\lambda_n)_{\Nset}` is equal to :math:`\Nset` and the sets
  :math:`\cA_{\lambda}` are defined by:

  .. math:: \cA_{\lambda} \, \, = \, \, \{\idx \in \NM \, : \, \alpha_{i^*} \, \leq \lambda \} \quad \quad , \quad \quad i^* \, \, = \, \, \mbox{arg} \min \left\{w_i \; , \; 1\leq i \leq n_X \right\}

  If stratas with larger cardinalities are of interest, one may rather
  consider the partial degree of the least significant variable, i.e.
  the one associated with the *greatest* weight :math:`w_i`. To this
  end, the index :math:`i^*` in the previous formula has to be defined
  by:

  .. math:: i^* \, \, = \, \, \mbox{arg} \max \left\{w_i \; , \; 1\leq i \leq n_X \right\}


.. topic:: API:

    - See :class:`~openturns.LinearEnumerateFunction`
    - See :class:`~openturns.HyperbolicAnisotropicEnumerateFunction`


.. topic:: Examples:

    - See :doc:`/examples/meta_modeling/functional_chaos`
    - See :doc:`/examples/meta_modeling/fieldfunction_metamodel`


.. topic:: References:

    - [blatman2009]_
