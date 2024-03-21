.. _functional_chaos:

Functional Chaos Expansion
--------------------------

Accounting for the joint probability density function (PDF)
:math:`f_{\vect{X}}(\vect{x})` of the input random vector
:math:`\vect{X}`, one seeks the joint PDF of the model response
:math:`\vect{Y} = h(\vect{X})`. This may be achieved using
Monte Carlo (MC) simulation, i.e. by evaluating the model :math:`h`
at a large number of realizations :math:`\vect{x}^{(i)}` of
:math:`\vect{X}` and then by estimating the empirical
distribution of the corresponding sample of model output
:math:`h(\vect{x}^{(i)})`. However it is well-known that the MC
method requires a large number of model evaluations, i.e. a great
computational cost, in order to obtain accurate results.
In fact, when using MC simulation, each model run is performed
independently. Thus, whereas it is expected that
:math:`h(\vect{x}^{(i)}) \approx h(\vect{x}^{(j)})` if
:math:`\vect{x}^{(i)} \approx \vect{x}^{(j)}`, the model is
evaluated twice without accounting for this information. In other
words, the functional dependence between :math:`\vect{X}` and
:math:`\vect{Y}` is lost.
A possible solution to overcome this problem and thereby to reduce the
computational cost of MC simulation is to represent the random
response :math:`\vect{Y}` in a suitable functional space, such as
the Hilbert space :math:`L^2` of square-integrable functions with
respect to the PDF :math:`f_{\vect{X}}(\vect{x})`.
Precisely, an expansion of the model response onto an orthonormal
basis of :math:`L^2` is of interest.

The principles of the building of a (infinite numerable) basis of this
space, i.e. the PC basis, are described in the sequel.

Principle of the functional chaos expansion
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Consider a model :math:`h` depending on a set of *random* variables
:math:`\vect{X} = (X_1,\dots,X_{n_X})^{\textsf{T}}`. We call
functional chaos expansion the class of spectral methods which gathers
all types of response surface that can be seen as a projection of the
physical model in an orthonormal basis. This class of methods uses the
Hilbertian space (square-integrable space: :math:`L^2`) to construct
the response surface.
Assuming that the physical model has a finite second order measure
(i.e. :math:`E\left( \|h(\vect{X})\|^2\right)< + \infty`), it may
be uniquely represented as a converging series onto an orthonormal
basis as follows:

  .. math::

      h(\vect{x})= \sum_{i=0}^{+\infty}  \vect{y}_{i}\Phi_{i}(\vect{x}).

where the
:math:`\vect{y}_{i} = (y_{i,1},\dots,y_{i,n_Y})^{\textsf{T}}`\ ’s
are deterministic vectors that fully characterize the random vector
:math:`\vect{Y}`, and the :math:`\Phi_{i}`\ ’s are given basis
functions (e.g. orthonormal polynomials, wavelets).

The orthonormality property of the functional chaos basis reads:

  .. math::

     \langle \Phi_{i},\Phi_{j}\rangle = \int_{D}\Phi_{i}(\vect{x}) \Phi_{j}(\vect{x})~f_{\vect{X}}(\vect{x}) d \vect{x} = \delta_{i,j}.

where :math:`\delta_{i,j} =1` if :math:`i=j` and 0 otherwise. The
metamodel :math:`\widehat{h}(\vect{x})` is represented by a
*finite* subset of coefficients
:math:`\{y_{i}, i \in \cA \subset (N)\}` in a *truncated* basis
:math:`\{\Phi_{i}, i \in \cA \subset (N)\}` as follows:

  .. math::

      \widehat{h}(\vect{x})= \sum_{i \in \cA \subset N}  y_{i}\Phi_{i}(\vect{x})

As an example of this type of expansion, one can mention responses by
wavelet expansion, polynomial chaos expansion, etc.


.. topic:: API:

    - See :class:`~openturns.FunctionalChaosAlgorithm`


.. topic:: Examples:

    - See :doc:`/auto_meta_modeling/polynomial_chaos_metamodel/plot_functional_chaos`


.. topic:: References:

    - [soizeghanem2004]_

