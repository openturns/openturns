.. _interpolation:

Interpolation of scalar functions
---------------------------------

| This section presents the interpolation methods used by several
  classes to build a numerical approximation of a function from its
  values on a finite set of points. These methods are notably used
  to transfer the values of a field from one mesh onto another one.

Piecewise linear interpolation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

| Let :math:`(t_0, \dots, t_{N-1})` be :math:`N` strictly increasing
  scalar locations, called *nodes*, and let
  :math:`(y_0, \dots, y_{N-1})` be :math:`N` values attached to these
  nodes. The *piecewise linear interpolation* is the continuous
  function that coincides with :math:`y_k` at node :math:`t_k` and is
  affine on each interval :math:`[t_k, t_{k+1}]`. For
  :math:`x \in [t_k, t_{k+1}]`, with :math:`h_k = t_{k+1} - t_k`, it
  writes:

  .. math::

    f(x) = \frac{t_{k+1} - x}{h_k}\, y_k + \frac{x - t_k}{h_k}\,
    y_{k+1}

| Equivalently, it can be written as a linear combination of *hat
  basis functions* :math:`\lambda_k`. For the interior nodes
  :math:`1 \le k \le N-2`:

  .. math::

    f(x) = \sum_{k=0}^{N-1} y_k\, \lambda_k(x), \quad
    \lambda_k(x) =
    \begin{cases}
      \dfrac{x - t_{k-1}}{t_k - t_{k-1}} & \text{if } x \in [t_{k-1}, t_k], \\
      \dfrac{t_{k+1} - x}{t_{k+1} - t_k} & \text{if } x \in [t_k, t_{k+1}], \\
      0 & \text{otherwise},
    \end{cases}

  and only one-sided at the boundary nodes:

  .. math::

    \lambda_0(x) =
    \begin{cases}
      \dfrac{t_1 - x}{t_1 - t_0} & \text{if } x \in [t_0, t_1], \\
      0 & \text{otherwise},
    \end{cases}
    \qquad
    \lambda_{N-1}(x) =
    \begin{cases}
      \dfrac{x - t_{N-2}}{t_{N-1} - t_{N-2}} & \text{if } x \in [t_{N-2}, t_{N-1}], \\
      0 & \text{otherwise}.
    \end{cases}

| Each basis function is valued 1 at its own node and 0 at all the
  others. The resulting function is continuous but only piecewise
  differentiable: its derivative is discontinuous at the nodes.
  Outside the range :math:`[t_0, t_{N-1}]`, the interpolation is
  extended constantly using the first or last value.

This interpolation is implemented by the
:class:`~openturns.PiecewiseLinearEvaluation` class.

Piecewise cubic Hermite interpolation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

| The *piecewise cubic Hermite interpolation* additionally uses the
  derivatives :math:`d_k` of the function at the nodes. It is the
  unique function that is cubic on each interval
  :math:`[t_k, t_{k+1}]`, matches :math:`y_k` and :math:`y_{k+1}` at
  the interval ends and has derivatives :math:`d_k` and :math:`d_{k+1}`
  there. With :math:`h_k = t_{k+1} - t_k` and
  :math:`s = (x - t_k) / h_k`, it writes for
  :math:`x \in [t_k, t_{k+1}]`:

  .. math::

    f(x) = h_{00}(s)\, y_k + h_{10}(s)\, h_k d_k
         + h_{01}(s)\, y_{k+1} + h_{11}(s)\, h_k d_{k+1}

  where the four Hermite basis functions are:

  .. math::

    \begin{aligned}
      h_{00}(s) &= 2 s^3 - 3 s^2 + 1, &
      h_{10}(s) &= s^3 - 2 s^2 + s, \\
      h_{01}(s) &= -2 s^3 + 3 s^2, &
      h_{11}(s) &= s^3 - s^2.
    \end{aligned}

| The resulting function is continuously differentiable over
  :math:`[t_0, t_{N-1}]`, which makes it well suited for the
  interpolation of fields whose regularity matters. As for the linear
  case, the interpolation is extended constantly outside the node
  range.

This interpolation is implemented by the
:class:`~openturns.PiecewiseHermiteEvaluation` class.

P1 Lagrange interpolation on a mesh
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

| The previous constructions apply to scalar locations. When the field
  is defined over an arbitrary simplicial mesh of
  :math:`\Rset^n`, OpenTURNS uses *P1 Lagrange interpolation*: the
  values :math:`y_i` are attached to the vertices
  :math:`\vect{v}_i` of the mesh, and the interpolated function is
  affine on every simplex. On a given simplex, the interpolation of a
  point :math:`\vect{x}` is written using the barycentric coordinates
  :math:`(\lambda_0, \dots, \lambda_p)` of :math:`\vect{x}` with
  respect to the vertices of that simplex:

  .. math::

    f(\vect{x}) = \sum_{i=0}^{p} \lambda_i(\vect{x})\, y_i, \quad
    \lambda_i(\vect{x}) \geq 0, \quad
    \sum_{i=0}^{p} \lambda_i(\vect{x}) = 1

| The functions :math:`\lambda_i` play the same role as the hat basis
  functions of the 1D case: they equal 1 at vertex
  :math:`\vect{v}_i`, vanish at all the other vertices, and are affine
  on each simplex. The interpolation is therefore continuous over the
  whole mesh.

| This interpolation is used in particular to evaluate a field defined
  on a first mesh at the vertices of a second mesh, which is what the
  :class:`~openturns.P1LagrangeInterpolation` class implements; the
  :class:`~openturns.P1LagrangeEvaluation` class evaluates a field on
  its own mesh.

References
----------
- P. Lancaster and K. Salkauskas, *Curve and Surface Fitting: An
  Introduction*, Academic Press (1986).
- P. G. Ciarlet, *The Finite Element Method for Elliptic Problems*,
  SIAM Classics in Applied Mathematics 40 (2002).
