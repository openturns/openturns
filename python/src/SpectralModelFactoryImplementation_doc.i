%define OT_SpectralModelFactory_doc
R"RAW(Base class for spectral model factory.

Notes
-----
Let :math:`X: \Omega \times \cD \rightarrow \Rset^{\processDim}`  be a multivariate second order stationary process, with zero mean, where :math:`\cD \subset \Rset^{\domainDim}`. We only treat here the case where the domain is of dimension 1: :math:`\cD \subset \Rset` (:math:`\domainDim=1`).

If we note :math:`C(\vect{s}, \vect{t})=\Expect{(X_{\vect{s}}-m(\vect{s}))\Tr{(X_{\vect{t}}-m(\vect{t}))}}` its covariance function, then for all :math:`(i,j), C^{stat}_{i,j} : \Rset^{\domainDim} \rightarrow \Rset` is :math:`\cL^1(\Rset^{\domainDim})` (ie :math:`\int_{\Rset^{\domainDim}} |C^{stat}_{i,j}(\vect{\tau})|\di{\vect{\tau}}\, < +\infty`), with :math:`C^{stat}(\vect{\tau}) = C(\vect{s}, \vect{s}+\vect{\tau})` as this quantity does not depend on :math:`\vect{s}`.

The bilateral spectral density function :math:`S : \Rset^{\domainDim} \rightarrow \cH^+(\processDim)` exists and is defined as the Fourier transform of the covariance function :math:`C^{stat}` :

.. math::

    \forall \vect{f} \in \Rset^{\domainDim}, \,S(\vect{f}) = \int_{\Rset^{\domainDim}}\exp\left\{-2i\pi <\vect{f},\vect{\tau}> \right\} C^{stat}(\vect{\tau})\di{\vect{\tau}}


where :math:`\cH^+(\processDim) \in \cM_{\processDim}(\Cset)` is the set of :math:`\processDim`-dimensional positive definite hermitian matrices.

Depending on the available data, we proceed differently:

- if the data correspond to several independent realizations of the process, the estimation is done using the empirical estimator;
- if the data correspond to one realization of the process, we suppose the process is ergodic to split the realization into several ones.
)RAW"

%enddef
%feature("docstring") OT::SpectralModelFactoryImplementation
OT_SpectralModelFactory_doc

// ---------------------------------------------------------------------

// ---------------------------------------------------------------------

%define OT_SpectralModelFactory_getFFTAlgorithm_doc
"Accessor to the FFT algorithm used for the Fourier transform.

Returns
-------
fftAlgo : :class:`~openturns.FFT` 
     The FFT algorithm used for the Fourier transform.
 "
%enddef
%feature("docstring") OT::SpectralModelFactoryImplementation::getFFTAlgorithm
OT_SpectralModelFactory_getFFTAlgorithm_doc

// ---------------------------------------------------------------------

%define OT_SpectralModelFactory_setFFTAlgorithm_doc
"Accessor to the FFT algorithm used for the Fourier transform.

Parameters
----------
fftAlgo : :class:`~openturns.FFT` 
     The FFT algorithm used for the Fourier transform.
 "
%enddef
%feature("docstring") OT::SpectralModelFactoryImplementation::setFFTAlgorithm
OT_SpectralModelFactory_setFFTAlgorithm_doc

// ---------------------------------------------------------------------

%define OT_SpectralModelFactory_build_doc
"Estimate the spectral model from data.

Available constructors:
    build(*myTimeSeries*)

    build(*myProcessSample*)

Parameters
----------
myTimeSeries : :class:`~openturns.TimeSeries` 
    The time series from which the spectral model is estimated.
myProcessSample : :class:`~openturns.ProcessSample` 
    The sample of time series from which the spectral model is estimated.

Returns
-------
mySpectralModel : :class:`~openturns.SpectralModel`
    The estimated spectral model.
 "
%enddef
%feature("docstring") OT::SpectralModelFactoryImplementation::build
OT_SpectralModelFactory_build_doc
