"""
Use case: Coles
===============
"""

import os
import openturns as ot


class Coles:
    """
    Data sets for the examples from [coles2001]_.

    Attributes
    ----------
    portpirie : :class:`~openturns.Sample`
        Sea levels dataset
    venice : :class:`~openturns.Sample`
        Sea levels dataset
    racetime : :class:`~openturns.Sample`
        Race time dataset
    fremantle : :class:`~openturns.Sample`
        Sea levels dataset
    rain : :class:`~openturns.Sample`
        Daily rainfall dataset
    wavesurge : :class:`~openturns.Sample`
        Wave and surge heights dataset
    wind : :class:`~openturns.Sample`
        Wind speeds dataset
    wooster : :class:`~openturns.Sample`
        Temperatures dataset

    Examples
    --------
    >>> from openturns.usecases import coles
    >>> data = coles.Coles().portpirie
    >>> print(data[:3])
    """

    def __init__(self):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        for name in [
            "portpirie",
            "venice",
            "racetime",
            "fremantle",
            "rain",
            "wavesurge",
            "wind",
            "wooster",
        ]:
            fn = os.path.join(current_dir, name + ".csv")
            dataset = ot.Sample.ImportFromCSVFile(fn, ",")
            setattr(self, name, dataset)
