"""Generate the synthetic data set of the unit commitment use case.

The data are synthetic. They reproduce the structure and the orders of
magnitude of the data used in the notebook

    https://github.com/alicelebrun/Unit_commitment

by Alice Lebrun, graduated from ENSTA Paris (class of 2024). The original
files were provided for an ENSTA Paris student project and their provenance
is not documented, so they are not shipped with OpenTURNS; this generator
produces a synthetic instance in their spirit instead.

The generator is deterministic: running it again produces the same files.

    python3 generate_unit_commitment_data.py
"""

import os
import numpy as np

SEED = 20240117

# thermal fleet
N_UNITS = 200
N_HOURS = 24
N_RUNNING = 96

# hydro valley
MAX_VOLUMETRIC = [2600.0, 1400.0, 800.0, 700.0, 500.0, 300.0]
INITIAL_RATIO = 0.5
INFLOW = [26.0, 18.0, 12.0, 10.0, 6.0, 4.0]

# each turbine is an arc (start reservoir, end reservoir, max flow, production term)
# reservoir 6 is the outlet of the valley: water leaving it is lost
TURBINES = [
    (0, 2, 72.0, 3.2),
    (0, 3, 66.0, 3.0),
    (1, 2, 54.0, 3.1),
    (1, 3, 48.0, 2.9),
    (2, 4, 45.0, 2.8),
    (2, 5, 39.0, 2.7),
    (3, 4, 36.0, 2.6),
    (3, 5, 33.0, 2.5),
    (4, 6, 30.0, 2.4),
    (4, 5, 24.0, 2.3),
    (5, 6, 27.0, 2.2),
    (5, 6, 21.0, 2.1),
]

# relative profile of the demand over a winter day
DEMAND_SHAPE = [
    0.62, 0.58, 0.55, 0.54, 0.56, 0.62, 0.72, 0.82,
    0.88, 0.90, 0.89, 0.87, 0.86, 0.85, 0.84, 0.85,
    0.90, 0.97, 1.00, 0.98, 0.94, 0.87, 0.78, 0.69,
]
DEMAND_PEAK = 27350.0


def thermal_data():
    """Return the thermal unit parameters, one column per attribute."""
    rng = np.random.default_rng(SEED)
    max_power = np.round(rng.uniform(100.0, 330.0, N_UNITS), 1)
    min_power = np.round(max_power * rng.uniform(0.30, 0.45, N_UNITS), 1)
    linear_term = np.round(rng.uniform(37.0, 66.0, N_UNITS), 2)
    start_up_cost = np.round(rng.uniform(137.0, 515.0, N_UNITS), 2)
    min_up_time = rng.integers(1, 15, N_UNITS)
    min_down_time = rng.integers(1, 15, N_UNITS)
    # a unit must be able to reach its minimum power in one hour
    ramp_up = np.round(np.maximum(min_power, rng.uniform(20.0, 116.0, N_UNITS)), 1)
    ramp_down = np.round(np.maximum(min_power, rng.uniform(21.0, 120.0, N_UNITS)), 1)
    initial_power = np.zeros(N_UNITS)
    running = rng.choice(N_UNITS, N_RUNNING, replace=False)
    initial_power[running] = np.round(rng.uniform(min_power[running], max_power[running]), 1)
    return np.column_stack([
        min_power, max_power, linear_term, start_up_cost,
        min_up_time, min_down_time, initial_power, ramp_up, ramp_down,
    ])


def hydro_data():
    """Return the reservoir and the turbine tables."""
    reservoirs = np.column_stack([
        MAX_VOLUMETRIC,
        INITIAL_RATIO * np.array(MAX_VOLUMETRIC),
        INFLOW,
    ])
    turbines = np.array(TURBINES, dtype=float)
    return reservoirs, turbines


def demand_data():
    """Return the hourly demand."""
    return (DEMAND_PEAK * np.array(DEMAND_SHAPE)).round(1)


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    thermal = thermal_data()
    reservoirs, turbines = hydro_data()
    demand = demand_data()
    np.savetxt(
        os.path.join(here, "unit_commitment_thermal.csv"), thermal, delimiter=",",
        fmt=["%.1f", "%.1f", "%.2f", "%.2f", "%d", "%d", "%.1f", "%.1f", "%.1f"],
        header="MinPower,MaxPower,LinearTerm,StartUpCost,MinUpTime,MinDownTime,"
               "InitialPower,DeltaRampUp,DeltaRampDown",
        comments="",
    )
    np.savetxt(
        os.path.join(here, "unit_commitment_reservoirs.csv"), reservoirs, delimiter=",",
        fmt="%.3f", header="MaxVolumetric,InitialVolumetric,Inflow", comments="",
    )
    np.savetxt(
        os.path.join(here, "unit_commitment_turbines.csv"), turbines, delimiter=",",
        fmt=["%d", "%d", "%.1f", "%.2f"], header="StartArc,EndArc,MaxFlow,ProductionTerm",
        comments="",
    )
    np.savetxt(
        os.path.join(here, "unit_commitment_demand.csv"), demand, delimiter=",",
        fmt="%.1f", header="Demand", comments="",
    )
    print("thermal fleet   : %d units, %d hours" % (N_UNITS, N_HOURS))
    print("running at t=0  : %d units" % int((thermal[:, 6] > 0.0).sum()))
    print("demand          : min %.1f MW, max %.1f MW" % (demand.min(), demand.max()))
    print("hydro valley    : %d reservoirs, %d turbines" % (reservoirs.shape[0], turbines.shape[0]))
    print("hydro capacity  : %.1f MW" % (turbines[:, 2] * turbines[:, 3]).sum())


if __name__ == "__main__":
    main()
