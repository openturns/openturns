.. _use-case-brain-diffusion:

The Brain Diffusion model
=========================

This use case collects brain diffusion tensors of a region of interest, as
estimated voxel-wise from open Human Connectome Project diffusion data
(DataLad). Each tensor is a symmetric positive definite matrix of size 3,
stored through its upper-triangular part ``t11, t12, t13, t22, t23, t33``.
The tensors are modelled by a Riemannian Gaussian on SPD(3) following Said
et al., "Gaussian Distributions on Riemannian Symmetric Spaces: Statistical
Learning with Structured Covariance Matrices", IEEE Trans. Inf. Theory, 2017.

The dataset is a reduced stand-in (400 tensors): the reference Frechet mean
is ``[[1.1, 0.1, 0.0], [0.1, 0.9, 0.05], [0.0, 0.05, 1.0]]`` with unit tangent
covariance.

Workflow
--------

- Fit :class:`~openturns.experimental.RiemannianGaussianFactory` on the
  tensors (Frechet mean, tangent covariance).
- Compare the estimated mean and covariance with the model of Said et al.

References
----------

- Human Connectome Project, open diffusion data (DataLad).
- Said et al., IEEE Trans. Inf. Theory, 2017 (reference model).

Load the use case
-----------------

We can load this model from the use cases module as follows :

.. code-block:: python

    >>> from openturns.usecases.brain_diffusion import BrainDiffusion
    >>> m = BrainDiffusion()
    >>> # Load the 400 diffusion tensors (upper-triangular 6-vectors)
    >>> data = m.tensors

API documentation
-----------------

.. currentmodule:: openturns.usecases.brain_diffusion

.. autoclass:: BrainDiffusion
    :noindex:
