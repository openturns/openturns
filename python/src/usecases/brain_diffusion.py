"""
Use case: BrainDiffusion
========================
"""

import os
import openturns as ot


class BrainDiffusion:
    """
    Brain diffusion tensors of a region of interest (reduced synthetic stand-in).

    Symmetric positive definite matrices of size 3 mimicking voxel-wise
    diffusion tensors estimated from open Human Connectome Project diffusion
    data (DataLad). Each row is a tensor stored as its upper-triangular part
    ``t11, t12, t13, t22, t23, t33``. The reference Frechet mean is
    ``[[1.1, 0.1, 0.0], [0.1, 0.9, 0.05], [0.0, 0.05, 1.0]]`` with unit
    tangent covariance, following the model of Said et al. (2017). The sample
    was drawn with :class:`~openturns.experimental.RiemannianGaussian`
    (seed 67890).

    Attributes
    ----------
    tensors : :class:`~openturns.Sample`
        400 diffusion tensors (upper-triangular 6-vectors).

    Examples
    --------
    >>> from openturns.usecases import brain_diffusion
    >>> data = brain_diffusion.BrainDiffusion().tensors
    >>> print(data.getSize(), data.getDimension())
    400 6
    """

    def __init__(self):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        self.tensors = ot.Sample.ImportFromCSVFile(
            os.path.join(current_dir, "brain_tensors.csv"), ","
        )

    def getReferenceMean(self):
        """Reference Frechet mean tensor (flattened upper-triangular part)."""
        return [1.1, 0.1, 0.0, 0.9, 0.05, 1.0]
