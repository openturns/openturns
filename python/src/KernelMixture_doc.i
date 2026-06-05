%feature("docstring") OT::KernelMixture
R"RAW(Build a particular linear combination of probability density functions.

A :class:`~openturns.KernelMixture` is a particular :class:`~openturns.Mixture`: all the weights are identical and
all the kernels of the combination are of the same
discrete or continuous family. The kernels are centered on the sample points. The multivariate kernel
is a tensorized product of the same univariate kernel.

The treatment of such a Mixture :math:`\inputRV` has been optimized.

We denote by :math:`\vect{s} = (\vect{s}_1, \dots, \vect{s}_\sampleSize) \in \Rset^\inputDim`
a sample from :math:`\inputRV`.

For continuous atoms, the probability density function :math:`\inputRV` is defined by:

.. math::

    \inputMeasure(\vect{x}) =  \sum_{i=1}^\sampleSize \dfrac{1}{\sampleSize} \prod_{j=1}^\inputDim \dfrac{1}{h^j}k\left(\frac{x^j-s^j_i}{h^j}\right)

where :math:`k` is the kernel pdf.

The mean of the Mixture is the vector :math:`(\mu^1, \dots, \mu^\inputDim)` defined by:

.. math::

    \mu^j = \bar{s}^j + h^j \mu_K

where :math:`\bar{s}^j` is the :math:`j`-th component of the mean vector of the sample points
:math:`\vect{s}` and :math:`\mu_K` the mean of the kernel.

The covariance matrix :math:`\Cov{\inputRV}` is defined by:

.. math::

    \Cov{\inputRV} = \left( 1 - \dfrac{1}{\sampleSize} \right) \Cov{\vect{s}} + \sigma_K
    \left( \mbox{Diag}(\vect{h})\right)^2

where :math:`\sigma_K` is the standard deviation of the kernel, :math:`\Cov{\vect{s}}` the covariance
matrix of the sample points :math:`\vect{s}` and :math:`\mbox{Diag}(\vect{h})` is the diagonal matrix
with the :math:`h^j` on the diagonal.


Parameters
----------
kernel : :class:`~openturns.Distribution`
    Univariate distribution of the kernel.
bandwidth : sequence of float
    Contains the bandwidth in each direction, :math:`(h^1, \dots, h^d)`.
sample : 2-d sequence of float
    The data on which each kernel is centered,
    :math:`(\vect{s}_1, \dots, \vect{s}_n)` of dimension :math:`\inputDim`.

Notes
-----
If :math:`\inputDim = 1` and the `KernelMixture-EnableInterpolation` ResourceMap key is set to `True` (it is `False` by default),
then the PDF and CDF of the kernel mixture are interpolated with :class:`PiecewiseHermiteEvaluation`.

Examples
--------
Create a *KernelMixture*:

>>> import openturns as ot
>>> kernel = ot.Uniform()
>>> sample = ot.Normal().getSample(5)
>>> bandwidth = [1.0]
>>> distribution = ot.KernelMixture(kernel, bandwidth, sample)
 
Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------
%feature("docstring") OT::KernelMixture::getKernel
"Accessor to kernel used in the linear combination.

Returns
-------
kernel : :class:`~openturns.Distribution`
    Univariate distribution used to build the kernel."


// ---------------------------------------------------------------------
%feature("docstring") OT::KernelMixture::setKernel
"Set the kernel used in the linear combination.

Parameters
----------
kernel : :class:`~openturns.Distribution`
    Univariate distribution used to build the kernel."

// ---------------------------------------------------------------------
%feature("docstring") OT::KernelMixture::getBandwidth
"Accessor to the bandwidth used in the linear combination.

Returns
-------
bandwidth : :class:`~openturns.Point`
    Bandwidth used in each direction."

// ---------------------------------------------------------------------
%feature("docstring") OT::KernelMixture::setBandwidth
"Set the bandwidth used in the linear combination.

Parameters
----------
bandwidth : sequence of float
    Bandwidth used in each direction."

// ---------------------------------------------------------------------
%feature("docstring") OT::KernelMixture::setInternalSample
"Set the sample used in the linear combination.

Parameters
----------
sample : 2-d sequence of float
    Sample on which the kernels are centered."

// ---------------------------------------------------------------------

%feature("docstring") OT::KernelMixture::getInternalSample
"Get the sample used in the linear combination.

Returns
-------
sample : :class:`~openturns.Sample`
    Sample on which the kernels are centered."
