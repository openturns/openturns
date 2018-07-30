.. _process_transformation:

Process transformation
======================

The objective here is to create a process :math:`Y` as the image through
a field function :math:`f_{dyn}` of another process :math:`X`:

.. math::

   \begin{aligned}
     Y=f_{dyn}(X)\end{aligned}

**General case**

| In the general case, :math:`X: \Omega \times\cD \rightarrow \Rset^d`
  is a multivariate stochastic process of dimension :math:`d` where
  :math:`\cD \in \Rset^n`,
  :math:`Y: \Omega \times \cD' \rightarrow \Rset^q` a multivariate
  stochastic process of dimension :math:`q` where
  :math:`\cD' \in \Rset^p` and
  :math:`f_{dyn}:\cD \times \Rset^d \rightarrow \cD' \times \Rset^q` and
  :math:`f_{dyn}` is defined in :eq:`dynFct`.

We build the composite process :math:`Y` thanks to function :math:`f_{dyn}`
and the process :math:`X`.

| The library proposes two kinds of field function: the value
  functions defined in :eq:`spatFunc` and the vertex-value functions defined
  in :eq:`tempFunc`.

**Trend modifications**

| Very often, we have to remove a trend from a process or to add it. If
  we note :math:`f_{trend}: \Rset^n \rightarrow \Rset^d` the function
  modelling a trend, then the field function which consists in
  adding the trend to a process is the vertex-value function
  :math:`f_{temp}: \cD \times \Rset^d \rightarrow \Rset^n \times \Rset^d`
  defined by:

  .. math::
    :label: trendTempFunc

     \begin{aligned}
       f_{temp}(\vect{t}, \vect{x})=(\vect{t},  \vect{x} +  f_{trend}(\vect{t}))\end{aligned}

The library enables to directly convert the function
:math:`f_{trend}` into the vertex-value function :math:`f_{temp}` thanks
to the *TrendTranform* object which maps :math:`f_{trend}` into the
vertex-value function :math:`f_{temp}`.

Then, the process :math:`Y` is built with the object
*CompositeProcess* from the data: :math:`f_{temp}` and the process
:math:`X` such that:

.. math::

    \begin{aligned}
      \forall \omega \in \Omega, \forall \vect{t} \in \cD, \quad Y(\omega, \vect{t}) = X(\omega, \vect{t}) + f_{trend}(\vect{t})\end{aligned}

**Box Cox transformation**

If the transformation of the process :math:`X` into :math:`Y`
corresponds to the Box Cox transformation
:math:`f_{BoxCox}: \Rset^d \rightarrow \Rset^d` which transforms
:math:`X` into a process :math:`Y` with stabilized variance, then the
corresponding field function is the value function
:math:`f_{spat}: \cD \times \Rset^d \rightarrow \cD \times \Rset^d`
defined by:

.. math::
  :label: spatFuncBC

    \begin{aligned}
      f_{spat}(\vect{t}, \vect{x})=(\vect{t},f_{BoxCox}(\vect{x}))\end{aligned}

| The library enables to directly convert the function
  :math:`f_{BoxCox}` into the value function :math:`f_{spat}` thanks
  to the *ValueFunction* object.
| Then, the process :math:`Y` is built with the object
  *CompositeProcess* from the data: :math:`f_{spat}` and the process
  :math:`X` such that:

  .. math::

     \begin{aligned}
       \forall \omega \in \Omega, \forall \vect{t} \in \cD, \quad Y(\omega, \vect{t}) = f_{BoxCox}(X(\omega, \vect{t}))\end{aligned}

.. topic:: API:

    - See :class:`~openturns.CompositeProcess`
    - See :class:`~openturns.TrendTransform`
    - See :class:`~openturns.BoxCoxFactory`

.. topic:: Examples:

    - See :doc:`/examples/probabilistic_modeling/add_trend`
    - See :doc:`/examples/probabilistic_modeling/trend_transform`
    - See :doc:`/examples/probabilistic_modeling/box_cox_transform`
