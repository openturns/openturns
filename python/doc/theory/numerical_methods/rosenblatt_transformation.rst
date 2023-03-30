.. _rosenblatt_transformation:

Rosenblatt Transformation
-------------------------

The Rosenblatt transformation is an :ref:`isoprobabilistic transformation <isoprobabilistic_transformation>` which
is used under
the following context: the input random vector is :math:`\vect{X}` with marginal cumulative
density functions  :math:`F_i` and copula :math:`C`. Nothing special is assumed about the
copula.

Introduction
~~~~~~~~~~~~

Let :math:`\vect{d}` be a  deterministic vector, let :math:`g(\vect{X}\,,\,\vect{d})` be the
limit state function of the model and let :math:`\cD_f = \{\vect{X} \in \Rset^n \,/ \,
g(\vect{X}\,,\,\vect{d}) \le 0\}` be an event whose probability :math:`P_f` is defined as:

.. math::
    :label: PfX2

      P_f = \Prob{g(\vect{X}\,,\,\vect{d})\leq 0}=   \int_{\cD_f}  \pdf\, d\vect{x}

One way to evaluate the probability :math:`P_f` is to use the Rosenblatt transformation :math:`T` which is a
diffeomorphism from the support of the distribution :math:`f_{\vect{X}}` into the Rosenblatt standard
space :math:`\Rset^n`, where distributions are normal, with zero mean, unit
variance and unit correlation matrix (which is equivalent in that
normal case to independent components).

Let us recall some definitions.
The *cumulative distribution function* :math:`F_{1,k}` of the
:math:`k`-dimensional random vector :math:`(X_1, \dots, X_k)` is
defined by its marginal distributions :math:`F_i` and the copula
:math:`C_{1,k}` through the relation:

.. math::

    F_{1,k}(x_1,\dots, x_k) = C_{1,k}(F_1(x_1),\dots, F_k(x_k))

with

.. math::
    :label: subCopula

      C_{1,k}(u_1, \dots, u_k) = C(u_1, \dots, u_k, 1, \dots, 1)

The *cumulative distribution function* of the conditional variable
:math:`X_k|X_1, \dots, X_{k-1}` is defined by:

.. math::

    F_{k|1, \dots, k-1} (x_k|x_1, \dots, x_{k-1})
    = \frac{ \frac{\partial^{k-1} F_{1,k}(x_1, \dots, x_k)}{\partial x_1 \dots
    \partial x_{k-1}} }{ \frac{\partial^{k-1} F_{1,k-1}(x_1, \dots, x_{k-1})}
    {\partial x_1 \dots \partial x_{k-1}}}

Rosenblatt transformation
~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`\vect{X}` in :math:`\Rset^n`
be a continuous random vector defined by its marginal cumulative
distribution functions :math:`F_i` and its copula :math:`C`. The
*Rosenblatt transformation* :math:`T_{Ros}` of :math:`\vect{X}` is
defined by:

.. math::
  :label: usualRos

    \vect{U} = T_{Ros}(\vect{X})=T_2\circ T_1(\vect{X})

where both transformations :math:`T_1`, and :math:`T_2` are given by:

.. math::

    T_1 : \Rset^n & \rightarrow \Rset^n\\
         \vect{X} & \mapsto     \vect{Y}=
         \left(
         \begin{array}{l}
           F_1(X_1)\\
           \dots \\
           F_{k|1, \dots, k-1}(X_k|X_1, \dots, X_{k-1})\\
           \dots \\
           F_{n|1, \dots, n-1}(X_n|X_1, \dots, X_{n-1})
         \end{array}
         \right) \\
    T_2 : \Rset^n & \rightarrow \Rset^n\\
         \vect{Y} & \mapsto     \vect{U}=
         \left(
         \begin{array}{l}
           \Phi^{-1}(Y_1)\\
           \dots \\
           \Phi^{-1}(Y_n)
         \end{array}
         \right)

where :math:`F_{k|1, \dots, k-1}` is the cumulative distribution function of the conditional
random variable :math:`X_k|X_1, \dots, X_{k-1}` and :math:`\Phi` is the cumulative distribution
function of the standard :math:`1`-dimensional Normal distribution.


.. topic:: API:

    - See  :ref:`Rosenblatt transformation <isoprobabilistic_transformations>`.


.. topic:: References:

    - O. Ditlevsen and H.O. Madsen, 2004, "Structural reliability methods,"
      Department of mechanical engineering technical university of Denmark -
      Maritime engineering, internet publication.
    - J. Goyet, 1998,"Sécurité probabiliste des structures -
      Fiabilité d'un élément de structure," Collège de Polytechnique.
    - A. Der Kiureghian, P.L. Liu, 1986,"Structural Reliability Under
      Incomplete Probabilistic Information", Journal of Engineering Mechanics,
      vol 112, no. 1, p85-104.
    - [lebrun2009a]_
    - [lebrun2009b]_
    - [lebrun2009c]_
    - H.O. Madsen, Krenk, S., Lind, N. C., 1986, "Methods of Structural Safety,"
      Prentice Hall.
    - [nataf1962]_
    - M. Rosenblatt, "Remarks on a Multivariat Transformation",
      The Annals of Mathematical Statistics, Vol. 23, No 3, pp. 470-472.

