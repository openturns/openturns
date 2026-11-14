"""
Unit commitment
===============

A power system made of thermal plants and of a hydro valley must meet an
hourly electricity demand over a one day horizon, at the lowest cost. Each
thermal plant has a minimum and a maximum power, a start-up cost, a minimum
up time, a minimum down time and ramp limits; the hydro valley is a cascade
of reservoirs connected by turbines, where the water turbined during an
hour reaches the downstream reservoir at the end of that hour. Deciding
*which* plants are running at each hour turns the problem into a mixed
integer linear program.

This case is adapted from the notebook
`Unit commitment <https://github.com/alicelebrun/Unit_commitment>`_
by Alice Lebrun, graduated from ENSTA Paris (class of 2024).
The NetCDF instance the notebook relies on was provided for an ENSTA Paris
student project and its provenance is not documented, so it is not shipped
with OpenTURNS; this example uses a synthetic instance generated in its
spirit: see the ``generate_unit_commitment_data.py`` script shipped with the
use case.

Notations
---------

The horizon has :math:`T` hours, the fleet :math:`N` thermal units, the
valley :math:`R` reservoirs and :math:`K` turbines. For the thermal unit
:math:`i`:

- :math:`c_i` is the production cost (currency per MWh) and :math:`c^s_i`
  the start-up cost;
- :math:`p^{min}_i` and :math:`p^{max}_i` are the minimum and maximum
  powers (MW);
- :math:`T^{up}_i` and :math:`T^{down}_i` are the minimum up and down times
  (hours);
- :math:`\\delta^{up}_i` and :math:`\\delta^{down}_i` are the maximum
  increase and decrease of power between two consecutive hours (MW).

For the hydro valley, :math:`a_j` is the inflow of the reservoir :math:`j`
(volume per hour), :math:`v^{max}_j` its capacity, :math:`f^{max}_k` the
capacity of the turbine :math:`k` (volume per hour) and :math:`\\gamma_k`
its production coefficient (MW per unit of volume per hour). The turbine
:math:`k` takes its water from the reservoir :math:`b_k` and releases it
into the reservoir :math:`e_k`; the special value :math:`e_k = R` denotes
the outlet of the valley, where the water is lost.
Finally :math:`d_t` is the demand at hour :math:`t` (MW).

Decision variables
------------------

- :math:`u_{t,i}\\in\\{0,1\\}`: the thermal unit :math:`i` is on at hour :math:`t`;
- :math:`s_{t,i}\\in\\{0,1\\}`: the thermal unit :math:`i` starts at hour :math:`t`;
- :math:`p_{t,i}\\geq 0` for :math:`i<N`: the power of the thermal unit
  :math:`i` at hour :math:`t`, and :math:`p_{t,N}` the power delivered by
  the whole hydro valley at hour :math:`t`;
- :math:`v_{t,j}\\geq 0`: the volume of the reservoir :math:`j` at hour
  :math:`t`;
- :math:`f_{t,k}\\geq 0`: the flow of the turbine :math:`k` at hour :math:`t`.

Objective
---------

The hydro production is free, so only the thermal costs are minimized:

.. math::

    \\min \\sum_{t=0}^{T-1}\\sum_{i=0}^{N-1}
    \\left(c_i\\, p_{t,i} + c^s_i\\, s_{t,i}\\right).

Constraints
-----------

The demand must be met at every hour, and the power of a unit is either
zero when it is off or between its bounds when it is on:

.. math::

    \\sum_{i=0}^{N} p_{t,i} \\geq d_t,
    \\qquad
    p^{min}_i u_{t,i} \\leq p_{t,i} \\leq p^{max}_i u_{t,i}.

A start-up is detected by comparing the state of the unit with the previous
hour, the state :math:`u_{-1,i}` before the first hour being known:

.. math::

    s_{t,i} \\geq u_{t,i} - u_{t-1,i}.

The power cannot vary too fast, and a unit that has just been started (resp.
stopped) must stay on (resp. off) long enough:

.. math::

    -\\delta^{down}_i \\leq p_{t,i} - p_{t-1,i} \\leq \\delta^{up}_i,

.. math::

    \\sum_{\\tau=k}^{k+T^{up}_i-1} u_{\\tau,i} \\geq T^{up}_i
    \\left(u_{k,i} - u_{k-1,i}\\right),
    \\qquad
    \\sum_{\\tau=k}^{k+T^{down}_i-1} u_{\\tau,i} \\leq T^{down}_i
    \\left(1 + u_{k,i} - u_{k-1,i}\\right).

In the valley, the volume of a reservoir increases with its inflow and with
the water released by the upstream turbines, and decreases with the water it
turbines itself:

.. math::

    v_{t,j} = v_{t-1,j} + a_j
    + \\sum_{k:\\,e_k=j} f_{t,k} - \\sum_{k:\\,b_k=j} f_{t,k},

the initial volumes :math:`v_{-1,j}` being known, and the volumes and flows
are bounded by the capacities. The power delivered by the valley is the power
of its turbines:

.. math::

    p_{t,N} = \\sum_{k=0}^{K-1} \\gamma_k f_{t,k}.

The resulting program has 14 856 unknowns, 9 600 of them binary, and
30 904 constraints; 106 584 coefficients of its constraint matrix are
non-zero, i.e. 0.02%. This is why the sparse form matters.
"""

# %%
import os

import numpy as np
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

# %%
# Load the data
# -------------
# The four CSV files hold the thermal fleet, the reservoirs, the turbines
# and the demand. The gallery executes this file from its own directory
# without defining ``__file__``, while the test suite runs it as a script.
try:
    here = os.path.dirname(os.path.abspath(__file__))
except NameError:
    here = os.getcwd()
data_dir = os.path.join(here, "..", "..", "..", "usecases")


def load(name):
    return np.array(ot.Sample.ImportFromCSVFile(os.path.join(data_dir, name), ","))


thermal = load("unit_commitment_thermal.csv")
reservoirs = load("unit_commitment_reservoirs.csv")
turbines = load("unit_commitment_turbines.csv")
demand = load("unit_commitment_demand.csv").ravel()

(
    p_min, p_max, cost, start_cost, min_up, min_down, p_init, ramp_up, ramp_down
) = thermal.T
min_up = min_up.astype(int)
min_down = min_down.astype(int)
v_max, v_init, inflow = reservoirs.T
start_arc, end_arc, f_max, gamma = turbines.T
start_arc = start_arc.astype(int)
end_arc = end_arc.astype(int)

n_hours = len(demand)
n_units = thermal.shape[0]
n_res = reservoirs.shape[0]
n_tur = turbines.shape[0]

# %%
# Index the unknowns
# ------------------
# Once flattened the unknowns form a single vector, addressed through
# shifted grids: ``U[t, i]`` is the index of :math:`u_{t,i}`, ``P[t, i]``
# the index of :math:`p_{t,i}`, and so on.
t_grid = np.arange(n_hours)[:, None]
i_grid = np.arange(n_units)[None, :]
U = (t_grid * n_units + i_grid).ravel()
S = n_hours * n_units + U
P = (2 * n_hours * n_units + t_grid * (n_units + 1) + np.arange(n_units + 1)).ravel()
V = (2 * n_hours * n_units + n_hours * (n_units + 1) + t_grid * n_res + np.arange(n_res)).ravel()
F = (2 * n_hours * n_units + n_hours * (n_units + 1) + n_hours * n_res + t_grid * n_tur + np.arange(n_tur)).ravel()
n_var = int(F[-1] + 1)

# %%
# Bounds and variable types
# -------------------------
# The known initial state of the thermal units is handled with bounds rather
# than with constraints: a unit that is running before the first hour is on
# at that hour and produces its initial power.
binary = ot.OptimizationProblemImplementation.BINARY
lower = np.zeros(n_var)
upper = np.full(n_var, 1e30)
finite_upper = np.zeros(n_var, dtype=bool)
types = np.zeros(n_var, dtype=int)
cost_vector = np.zeros(n_var)

upper[U] = 1.0
upper[S] = 1.0
finite_upper[U] = True
finite_upper[S] = True
types[U] = binary
types[S] = binary

u_init = (p_init > 0.0).astype(float)
running = np.flatnonzero(p_init > 0.0)
lower[P[: n_units + 1][running]] = p_init[running]
upper[P[: n_units + 1][running]] = p_init[running]
finite_upper[P[: n_units + 1][running]] = True
lower[U[:n_units][running]] = 1.0

upper[V] = np.tile(v_max, n_hours)
finite_upper[V] = True
upper[F] = np.tile(f_max, n_hours)
finite_upper[F] = True

cost_vector[P] = np.tile(np.append(cost, 0.0), n_hours)
cost_vector[S] = np.tile(start_cost, n_hours)

# %%
# Assemble the constraint matrix
# ------------------------------
# Every constraint is a row of
# :math:`\\vect{lb} \\leq \\mat{A}\\vect{x} \\leq \\vect{ub}`. The rows are
# accumulated as ``(row, column, value)`` triplets and the matrix is built in
# a single call: only the non-zero coefficients are ever stored, and no dense
# matrix is ever allocated.
ROW, COL, VAL, LB, UB = [], [], [], [], []
INF = 1e30


def add_row(entries, low, high):
    index = len(LB)
    for column, value in entries:
        ROW.append(index)
        COL.append(int(column))
        VAL.append(float(value))
    LB.append(float(low))
    UB.append(float(high))


for t in range(n_hours):
    # demand, met by the thermal units and by the valley
    add_row(zip(P[t * (n_units + 1) : (t + 1) * (n_units + 1)], np.ones(n_units + 1)), demand[t], INF)
    for i in range(n_units):
        power = P[t * (n_units + 1) + i]
        state = U[t * n_units + i]
        # production bounds
        add_row([(power, 1.0), (state, -p_min[i])], 0.0, INF)
        add_row([(power, 1.0), (state, -p_max[i])], -INF, 0.0)
        # start-up indicator
        if t == 0:
            add_row([(S[i], -1.0), (U[i], 1.0)], -INF, u_init[i])
        else:
            add_row([(S[t * n_units + i], -1.0), (state, 1.0), (U[(t - 1) * n_units + i], -1.0)], -INF, 0.0)
        # ramp limits
        if t > 0:
            add_row([(power, 1.0), (P[(t - 1) * (n_units + 1) + i], -1.0)], -INF, ramp_up[i])
            add_row([(power, -1.0), (P[(t - 1) * (n_units + 1) + i], 1.0)], -INF, ramp_down[i])

for i in range(n_units):
    # minimum up time
    for k in range(n_hours - min_up[i]):
        window = [(U[t * n_units + i], 1.0) for t in range(k, k + min_up[i])]
        if k == 0:
            add_row(window + [(U[i], -min_up[i])], -min_up[i] * u_init[i], INF)
        else:
            add_row(window + [(U[k * n_units + i], -min_up[i]), (U[(k - 1) * n_units + i], min_up[i])], 0.0, INF)
    # minimum down time
    for k in range(n_hours - min_down[i]):
        window = [(U[t * n_units + i], 1.0) for t in range(k, k + min_down[i])]
        if k == 0:
            add_row(window + [(U[i], -min_down[i])], -INF, min_down[i] * (1.0 - u_init[i]))
        else:
            add_row(window + [(U[k * n_units + i], -min_down[i]), (U[(k - 1) * n_units + i], min_down[i])], -INF, min_down[i])

# hydro capacities
for t in range(n_hours):
    for j in range(n_res):
        add_row([(V[t * n_res + j], 1.0)], -INF, v_max[j])
    for k in range(n_tur):
        add_row([(F[t * n_tur + k], 1.0)], -INF, f_max[k])

# water balance, the volume of the first hour being the initial one plus the
# water entering and minus the water turbined during that hour
for t in range(n_hours):
    for j in range(n_res):
        entries = [(V[t * n_res + j], 1.0)]
        balance = inflow[j] + (v_init[j] if t == 0 else 0.0)
        if t > 0:
            entries.append((V[(t - 1) * n_res + j], -1.0))
        entries += [(F[t * n_tur + k], -1.0) for k in range(n_tur) if end_arc[k] == j]
        entries += [(F[t * n_tur + k], 1.0) for k in range(n_tur) if start_arc[k] == j]
        add_row(entries, balance, balance)

# conversion of the turbined water into power
for t in range(n_hours):
    entries = [(P[t * (n_units + 1) + n_units], 1.0)]
    entries += [(F[t * n_tur + k], -gamma[k]) for k in range(n_tur)]
    add_row(entries, 0.0, 0.0)

A = otexp.SparseMatrix(len(LB), n_var, ot.Indices(ROW), ot.Indices(COL), ot.Point(VAL))
n_rows, n_cols, nnz = A.getNbRows(), A.getNbColumns(), A.getNbNonZeros()
print("unknowns    : %d (%d binary)" % (n_var, int(types.sum())))
print("constraints : %d" % n_rows)
print("matrix      : %d rows x %d columns, %d non-zeros (%.2f%%)" % (n_rows, n_cols, nnz, 100.0 * nnz / (n_rows * n_cols)))

# %%
# The size of the constraint matrix is what makes the sparse storage
# decisive here. A dense matrix would hold :math:`m n` coefficients, each on
# 8 bytes, although 99.99% of them are zero; the compressed sparse column
# form stores the :math:`nnz` values, their row indices and one column
# pointer per column only.
dense_memory = n_rows * n_cols * 8
sparse_memory = nnz * (8 + 4) + (n_cols + 1) * 4
print("dense form  : %.1f GiB" % (dense_memory / 2**30))
print("sparse form : %.1f MiB (%.0f times less)" % (sparse_memory / 2**20, dense_memory / sparse_memory))

# %%
# Solve the problem
# -----------------
# The linear problem receives the sparse matrix directly, and the binary
# variables are declared with :meth:`~openturns.OptimizationProblem.setVariablesType`
# so that HiGHS runs its mixed integer solver.
bounds = ot.Interval(lower.tolist(), upper.tolist(), [True] * n_var, finite_upper.tolist())
constraint_bounds = ot.Interval(LB, UB, [bound > -INF for bound in LB], [bound < INF for bound in UB])
problem = otexp.LinearProblem(cost_vector.tolist(), bounds, A, constraint_bounds)
problem.setVariablesType(types.tolist())
algo = otexp.HiGHS(problem)
algo.run()
result = algo.getResult()
print("status      : %s" % result.getStatusMessage())
print("total cost  : %.1f" % result.getOptimalValue()[0])

# %%
# Analyse the solution
# --------------------
# The demand is met exactly. The thermal fleet covers most of it and the
# valley saves its water for the two peaks of the load curve.
x = np.array(result.getOptimalPoint())
u = x[U].reshape(n_hours, n_units)
p = x[P].reshape(n_hours, n_units + 1)
v = x[V].reshape(n_hours, n_res)
f = x[F].reshape(n_hours, n_tur)
thermal_power = p[:, :n_units].sum(axis=1)
hydro_power = p[:, n_units]
starts = np.diff(np.vstack([u_init, u]), axis=0) > 0.5
print("start-ups   : %d" % int(starts.sum()))
print("thermal     : %.0f MWh" % thermal_power.sum())
print("hydro       : %.0f MWh (%.0f%% of the demand)" % (hydro_power.sum(), 100.0 * hydro_power.sum() / demand.sum()))
print("demand      : %.0f MWh" % demand.sum())

# %%
# The next figure superimposes the demand, the total production, the thermal
# production and the hydro production: the total production matches the
# demand, the thermal fleet covers the base of the load curve and the valley
# follows its two peaks.
hours = np.arange(1, n_hours + 1)
graph = ot.Graph("Production mix", "Time (h)", "Power (MW)", True, "upper left")
demand_curve = ot.Curve(hours, demand, "Demand")
demand_curve.setColor("black")
demand_curve.setLineStyle("dashed")
total_curve = ot.Curve(hours, thermal_power + hydro_power, "Total production")
total_curve.setColor("darkgreen")
thermal_curve = ot.Curve(hours, thermal_power, "Thermal")
thermal_curve.setColor("red")
hydro_curve = ot.Curve(hours, hydro_power, "Hydro")
hydro_curve.setColor("blue")
for curve in [demand_curve, total_curve, thermal_curve, hydro_curve]:
    graph.add(curve)
view = otv.View(graph)

# %%
# The number of running units drops during the night, reaches a plateau
# during the day when the cheapest units are saturated, and peaks with the
# demand.
graph_units = ot.Graph("Running thermal units", "Time (h)", "Number of units", True, "upper left")
units_curve = ot.Curve(hours, u.sum(axis=1), "Running units")
units_curve.setColor("red")
graph_units.add(units_curve)
view = otv.View(graph_units)

# %%
# The volumes show how the valley is managed: the two upstream reservoirs
# fill during the night and release their water from the morning to the
# evening, while the two lowest ones are emptied at the beginning of the day
# and refilled at its end.
graph_volume = ot.Graph("Reservoir volumes", "Time (h)", "Volume", True, "upper right")
for j in range(n_res):
    graph_volume.add(ot.Curve(hours, v[:, j], "Reservoir %d" % j))
view = otv.View(graph_volume)

# %%
# Finally the flows through the twelve turbines give the detailed operation
# of the valley.
graph_flow = ot.Graph("Turbine flows", "Time (h)", "Flow", True, "upper left")
for k in range(n_tur):
    graph_flow.add(ot.Curve(hours, f[:, k], "Turbine %d" % k))
view = otv.View(graph_flow)
