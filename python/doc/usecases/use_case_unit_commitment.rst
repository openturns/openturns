.. _use-case-unit-commitment:

Unit commitment
===============

A power system made of thermal power plants and of a hydro valley must meet
an hourly electricity demand over a one day horizon, at the lowest cost.
Each thermal plant has a minimum and a maximum power, a start-up cost, a
minimum up time, a minimum down time and ramp limits. The hydro valley is a
cascade of reservoirs connected by turbines: the water turbined during an
hour reaches the downstream reservoir at the end of that hour.
The difficulty is to decide *which* plants are running at each hour, which
makes the problem a mixed integer linear program (MILP).

This case is adapted from the notebook `Unit commitment
<https://github.com/alicelebrun/Unit_commitment>`_ by **Alice Lebrun**,
graduated from ENSTA Paris (class of 2024), who formulated and solved the
problem with PuLP and HiGHS. The formulation below is hers, apart from the
corrections listed at the end of this page; the example reimplements it with
OpenTURNS only, with the same solver and a sparse constraint matrix.

.. note::

    The NetCDF files used by the original notebook were provided for an
    ENSTA Paris student project and their provenance is not documented, so
    they are not shipped with OpenTURNS. The data set of this use case is
    synthetic and generated in their spirit by the
    :download:`generate_unit_commitment_data.py
    <generate_unit_commitment_data.py>` script: same variables, same
    magnitudes, same structure of the hydro valley. It contains 200 thermal
    units, 24 hours, 6 reservoirs and 12 turbines.

Description of the data
-----------------------

The instance is described by four comma separated files, whose columns keep
the names of the variables of the original NetCDF format.

``unit_commitment_thermal.csv``
    One row per thermal unit:

    - ``MinPower``, ``MaxPower``: production bounds (MW);
    - ``LinearTerm``: production cost (currency per MWh);
    - ``StartUpCost``: cost of a start-up (currency);
    - ``MinUpTime``, ``MinDownTime``: minimum up and down times (hours);
    - ``InitialPower``: power before the first hour (MW), zero if the unit is
      off;
    - ``DeltaRampUp``, ``DeltaRampDown``: ramp limits (MW per hour).

``unit_commitment_reservoirs.csv``
    One row per reservoir: ``MaxVolumetric`` (capacity), ``InitialVolumetric``
    (initial volume) and ``Inflow`` (volume per hour).

``unit_commitment_turbines.csv``
    One row per turbine: ``StartArc`` and ``EndArc`` (upstream and downstream
    reservoirs, the special index 6 denoting the outlet of the valley),
    ``MaxFlow`` (volume per hour) and ``ProductionTerm`` (MW per unit of
    volume and per hour).

``unit_commitment_demand.csv``
    One row per hour: ``Demand`` (MW).

Mathematical formulation
------------------------

Let :math:`T` be the number of hours, :math:`N` the number of thermal units,
:math:`R` the number of reservoirs and :math:`K` the number of turbines.
The decision variables are:

- :math:`u_{t,i}\in\{0,1\}`: the thermal unit :math:`i` is on at hour
  :math:`t`, for :math:`i\in\{0,\dots,N-1\}`;
- :math:`s_{t,i}\in\{0,1\}`: the thermal unit :math:`i` starts at hour
  :math:`t`;
- :math:`p_{t,i}\geq 0`: the power of the thermal unit :math:`i` at hour
  :math:`t`, the last column :math:`p_{t,N}` being the power delivered by the
  whole hydro valley;
- :math:`v_{t,j}\geq 0`: the volume of the reservoir :math:`j`;
- :math:`f_{t,k}\geq 0`: the flow of the turbine :math:`k`.

The cost of the thermal production is minimized, the hydro production being
free:

.. math::

    \min \sum_{t=0}^{T-1}\sum_{i=0}^{N-1}
    \left(c_i\, p_{t,i} + c^s_i\, s_{t,i}\right),

where :math:`c_i` is the production cost and :math:`c^s_i` the start-up cost
of the unit :math:`i`. The demand :math:`d_t` must be met at every hour, and
a unit produces either nothing when it is off or between its bounds when it
is on:

.. math::

    \sum_{i=0}^{N} p_{t,i} \geq d_t,
    \qquad
    p^{min}_i u_{t,i} \leq p_{t,i} \leq p^{max}_i u_{t,i}.

The start-up variable is the variation of the state of the unit, the state
:math:`u_{-1,i}` before the first hour being known:

.. math::

    s_{t,i} \geq u_{t,i} - u_{t-1,i}.

The power of a unit cannot vary faster than its ramp limits, and a unit that
has just been started (resp. stopped) must remain on (resp. off) long enough:

.. math::

    -\delta^{down}_i \leq p_{t,i} - p_{t-1,i} \leq \delta^{up}_i,

.. math::

    \sum_{\tau=k}^{k+T^{up}_i-1} u_{\tau,i} \geq T^{up}_i
    \left(u_{k,i} - u_{k-1,i}\right),
    \qquad
    \sum_{\tau=k}^{k+T^{down}_i-1} u_{\tau,i} \leq T^{down}_i
    \left(1 + u_{k,i} - u_{k-1,i}\right).

In the valley the volume of a reservoir increases with its own inflow and
with the water released by the upstream turbines, and decreases with the
water it turbines itself, for every hour including the first one:

.. math::

    v_{t,j} = v_{t-1,j} + a_j
    + \sum_{k:\, e_k=j} f_{t,k} - \sum_{k:\, b_k=j} f_{t,k},

where :math:`a_j` is the inflow of the reservoir :math:`j`, :math:`b_k` and
:math:`e_k` being the upstream and downstream reservoirs of the turbine
:math:`k`, and :math:`v_{-1,j}` the initial volume of the reservoir. The
volumes and the flows are bounded by the capacities :math:`v^{max}_j` and
:math:`f^{max}_k`, and the power of the valley is the power of its turbines:

.. math::

    p_{t,N} = \sum_{k=0}^{K-1} \gamma_k f_{t,k},

where :math:`\gamma_k` is the production coefficient of the turbine
:math:`k`.

Size and sparsity
-----------------

The instance has 14 856 unknowns, 9 600 of them binary, and 30 904
constraints. Only 106 584 coefficients of the constraint matrix are
non-zero, that is 0.02 % of its entries: the matrix is 30 904 by 14 856 but
almost empty.

| Storing it densely, as a :class:`~openturns.Matrix`, would require
  :math:`30\,904 \times 14\,856 \times 8` bytes, that is 3.4 GiB.
| Storing it in compressed sparse column form, as a
  :class:`~openturns.experimental.SparseMatrix`, requires the 106 584 values,
  their 106 584 row indices and one column pointer per column, that is
  1.3 MiB: about 2 700 times less memory. This is what makes the difference
  between a problem that fits in memory and one that does not, and the reason
  why :class:`~openturns.experimental.LinearProblem` accepts a sparse matrix
  and hands it over to HiGHS without ever densifying it.

Differences with the reference notebook
---------------------------------------

The formulation above is the one of the reference notebook, with three
corrections that were merged upstream and a fourth one applied here only:

- the sign of the minimum down time constraint: with
  :math:`T^{down}_i(1-(u_{k,i}-u_{k-1,i}))` it forbade any start-up, since it
  forced the unit to stay off when it had just been started;
- for :math:`k=0` the constraint referred to the last hour of the horizon
  instead of the initial state, which amounts to assume a cyclic horizon;
- the start-up cost was not paid at the first hour;
- the water balance of the first hour: the volume :math:`v_{0,j}` was fixed
  to the initial volume while the balance was written from the second hour
  on, so the water turbined during the first hour came from nowhere. Here the
  initial volume :math:`v_{-1,j}` is put on the right-hand side of the
  balance of the first hour, which accounts for the flows of every hour
  with the same number of water balance constraints.

Independently of these corrections, the model is stored more compactly, for
the same solution:

- the objective is minimized directly instead of being assigned to an
  auxiliary variable :math:`J`, which removes one unknown and one constraint;
- the initial state of the units that are running before the first hour,
  their power and their on/off state, is expressed with variable bounds
  instead of constraints, which removes two constraints per running unit.

With 96 units running before the first hour, this makes 193 constraints less
on the reference instance: it has 14 857 unknowns and 32 446 constraints with
PuLP, against 14 856 and 32 253 here.

References
----------

- [lebrun2024]_

Examples based on this use case
-------------------------------

.. minigallery:: examples/numerical_methods/optimization/plot_unit_commitment.py
