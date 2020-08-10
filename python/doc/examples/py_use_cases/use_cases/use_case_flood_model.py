"""
Use case : flood model
======================
"""
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
from typing import Any, List
import numpy as np
from dataclasses import dataclass
from dataclasses import field

@dataclass
class flood_model() :
    """Custom class for the flood model.
    """
    dim : int = 4 # number of inputs
    # Q
    Q : Any = ot.TruncatedDistribution(ot.Gumbel(558., 1013.), 0, ot.TruncatedDistribution.LOWER)
    Q.setDescription("Q")
    Q.setName("Q")

    # Ks
    Ks : Any = ot.TruncatedDistribution(ot.Normal(30.0, 7.5), 0, ot.TruncatedDistribution.LOWER)
    Ks.setName("Ks")

    # Zv
    Zv : Any = ot.Uniform(49.0, 51.0)
    Zv.setName("Zv")

    # Zm
    Zm : Any = ot.Uniform(54.0, 56.0)
    #Zm.setDescription(["Zm (m)"])
    Zm.setName("Zm")

    model : Any = ot.SymbolicFunction(['Q', 'Ks', 'Zv', 'Zm'],
                ['(Q/(Ks*300.*sqrt((Zm-Zv)/5000)))^(3.0/5.0)+Zv-58.5'])

    distribution : Any = ot.ComposedDistribution([Q, Ks, Zv, Zm])
    distribution.setDescription(['Q', 'Ks', 'Zv', 'Zm'])
 
    X : Any = ot.RandomVector(distribution)
    X.setDescription(['Q', 'Ks', 'Zv', 'Zm'])

    Y : Any = ot.CompositeRandomVector(model, X)
    Y.setDescription('Y')
    event : Any = ot.ThresholdEvent(Y, ot.Greater(), 0.0)
    event.setName('overflow')

