import os
import openturns as ot


class AmesHousing:
    """
    Data class for the Ames Housing dataset.

    Attributes
    ----------
    data : :class:`~openturns.Sample`
        The dataset.

    Examples
    --------
    >>> from openturns.usecases import ames_housing
    >>> data = ames_housing.AmesHousing().data
    >>> print(data[:3])
    """

    def __init__(self):
        script_dir = os.path.dirname(__file__)
        csv = os.path.join(script_dir, "ames_housing.csv")
        self.data = ot.Sample.ImportFromCSVFile(csv, ";")
