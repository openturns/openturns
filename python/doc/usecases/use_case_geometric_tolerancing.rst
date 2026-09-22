.. _use-case-geometric-tolerancing:

The Geometric Tolerancing model
===============================

This use case assesses the form-defect conformity of a machined plate. The
plate is the unit square meshed with a 4x4 grid (32 triangles). The form
defect is the out-of-plane field ``z = 0.05 sin(2 pi x) sin(2 pi y)``
transported by :class:`~openturns.experimental.PushForwardOverMesh`, while
the probe location is uniform over the plate
(:class:`~openturns.UniformOverMesh`). A part is non-conforming when the
local defect amplitude exceeds the tolerance (default 0.03). The mesh is
fully synthetic: no external data is required.

Workflow
--------

- Build :class:`~openturns.UniformOverMesh` over the plate mesh (or estimate
  it with :class:`~openturns.experimental.UniformOverMeshFactory` from a
  measurement sample).
- Transport the defect field with
  :class:`~openturns.experimental.PushForwardOverMesh`.
- Compute the non-conformity probability against the tolerance.

Load the use case
-----------------

We can load this model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases.geometric_tolerancing import GeometricTolerancing
    >>> m = GeometricTolerancing()
    >>> # Load the plate mesh (32 triangles) and the defect distribution
    >>> mesh = m.plate
    >>> defect = m.defect

API documentation
-----------------

.. currentmodule:: openturns.usecases.geometric_tolerancing

.. autoclass:: GeometricTolerancing
    :noindex:
