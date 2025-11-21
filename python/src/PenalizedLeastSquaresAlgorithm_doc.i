%feature("docstring") OT::PenalizedLeastSquaresAlgorithm
R"RAW(Penalized least squares algorithm.

Refer to :ref:`least_squares`.

Available constructors:
    PenalizedLeastSquaresAlgorithm(*x, y, psi, indices, penalizationFactor=0, useNormal=False*)

    PenalizedLeastSquaresAlgorithm(*x, y, weight, psi, indices, penalizationFactor=0, useNormal=False*)

    PenalizedLeastSquaresAlgorithm(*x, y, weight, psi, indices, penalizationFactor=0, penalizationMatrix, useNormal=False*)

Parameters
----------
x : 2-d sequence of float
    The input random observations :math:`\left\{\vect{x}^{(1)}, ..., \vect{x}^{(n)}\right\}` 
    where :math:`\vect{x} = \Tr{\left(x_1, \dots, x_{n_X}\right)}` is the input of the physical
    model, :math:`n_X \in \Nset` is the input dimension and :math:`n \in \Nset` is the sample size.
y : 2-d sequence of float
    The output random observations :math:`\left\{\vect{y}^{(1)}, ..., \vect{y}^{(n)}\right\}` 
    where :math:`\vect{y} = \Tr{\left(y_1, \dots, y_{n_Y}\right)}` is the output of the physical
    model, :math:`n_Y \in \Nset` is the output dimension and :math:`n \in \Nset` is the sample size.
weight : sequence of float
    Output weights
psi : sequence of :class:`~openturns.Function`
    Basis
indices : sequence of int
    Indices allowed in the basis
penalizationFactor : float, optional
    Penalization factor
penalizationMatrix : 2-d sequence of float
    Penalization matrix
useNormal : bool, optional
    Solve the normal equation

See also
--------
ApproximationAlgorithm, LeastSquaresMetaModelSelection

Notes
-----
For each output marginal :math:`k \in \{1, ..., n_Y\}`,
solve the least-squares problem:

.. math::

    \widehat{\vect{a}} = \argmin_{\vect{a} \in \Rset^P} \left\|\vect{y}_k - \Tr{\mat{\Psi}} \vect{a} \right\|^2

where :math:`\vect{y}_k \in \Rset^{n}` is the :math:`k`-th marginal of the
sample of output observations,
:math:`P \in \Nset` is the number of coefficients,
:math:`\mat{\Psi} \in \Rset^{n \times P}` is the design matrix
computed from the input sample *x* and
:math:`\vect{a} \in \Rset^P` is the vector of coefficients.)RAW"
