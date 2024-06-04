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
    dowjones : str
        Dow Jones Index dataset path
    fremantle : :class:`~openturns.Sample`
        Sea levels dataset
    portpirie : :class:`~openturns.Sample`
        Sea levels dataset
    racetime : :class:`~openturns.Sample`
        Race time dataset
    rain : :class:`~openturns.Sample`
        Daily rainfall dataset
    venice : :class:`~openturns.Sample`
        Sea levels dataset
    wavesurge : :class:`~openturns.Sample`
        Wave and surge heights dataset
    wind : :class:`~openturns.Sample`
        Wind speeds dataset
    wooster : str
        Temperatures dataset path

    Examples
    --------
    >>> from openturns.usecases import coles
    >>> data = coles.Coles().portpirie
    >>> print(data[:3])
    """

    def __getattr__(self, name):
        if name not in [
            "dowjones",
            "portpirie",
            "venice",
            "racetime",
            "fremantle",
            "rain",
            "wavesurge",
            "wind",
            "wooster",
        ]:
            raise AttributeError(f"'Coles' object has no attribute '{name}'")
        current_dir = os.path.dirname(os.path.abspath(__file__))
        fn = os.path.join(current_dir, name + ".csv")
        if name in ["wooster", "dowjones"]:
            # cannot read sample with date column
            dataset = fn
        else:
            dataset = ot.Sample.ImportFromCSVFile(fn, ",")
        return dataset
