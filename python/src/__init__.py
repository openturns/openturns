"""
OpenTURNS, uncertainty engineering software.

Documentation is available online at http://www.openturns.org.

Available submodules
--------------------
common                                --- Utility classes
typ                                   --- Internal data types
statistics                            --- Statistical classes
graph                                 --- Graphical output
geom                                  --- Geometrical primitives
func                                  --- Function primitives
diff                                  --- Differential algorithms
optim                                 --- Optimization routines
solver                                --- Solvers
algo                                  --- Approximation algorithms
experiment                            --- Non probabilistic designs of experiments
base                                  --- Deterministic meta-package
classification                        --- Classification algorithms
model_copula                          --- Copulas
randomvector                          --- Random vectors
dist                                  --- Probabilistic distributions
model_process                         --- Stochastic processes
weightedexperiment                    --- Probabilistic designs of experiments
orthogonalbasis                       --- Polynomial primitives
metamodel                             --- Meta-modelling
transformation                        --- Iso-probabilistic and process transformations
analytical                            --- Analytical uncertainty propagation algorithms
simulation                            --- Simulation uncertainty propagation algorithms
stattests                             --- Statistical tests
bayesian                              --- Bayesian updating
uncertainty                           --- Probabilistic meta-package

Utility modules
---------------
viewer                                --- Matplotlib-based viewer
coupling_tools                        --- External code coupling helpers

Environment variables
---------------------
OPENTURNS_HOME                        --- Custom installation path
OPENTURNS_CONFIG_PATH                 --- Path to configuration file
OPENTURNS_LOG_SEVERITY                --- Log severity flag, for example "ALL"
"""
import sys

# SWIG-related: check if interactive mode
if not hasattr(sys, 'ps1'):
    try:
        # ipython does not define ps1
        __IPYTHON__
    except:
        # Reset the default Crtl-C behavior
        import signal
        try:
            signal.signal(signal.SIGINT, signal.SIG_DFL)
        except ValueError:
            pass

from .common import *
from .typ import *
from .graph import *
from .geom import *
from .func import *
from .statistics import *
from .diff import *
from .optim import *
from .solver import *
from .algo import *
from .experiment import *
from .base import *

from .model_copula import *
from .randomvector import *
from .dist import *
from .model_process import *
from .weightedexperiment import *
from .classification import *
from .orthogonalbasis import *
from .metamodel import *
from .transformation import *
from .analytical import *
from .simulation import *
from .stattests import *
from .bayesian import *
from .uncertainty import *


def Show(graph):
    """
    Display a graph pop-up.

    Parameters
    ----------
    graph : Graph
        the graph to display
    """
    from .viewer import View
    view = View(graph)
    view.ShowAll()


__version__ = PlatformInfo.GetVersion()
