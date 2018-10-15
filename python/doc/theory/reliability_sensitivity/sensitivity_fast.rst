.. _sensitivity_fast:

Sensivity analysis by Fourier decomposition
-------------------------------------------

FAST is a sensitivity analysis method which is based upon the ANOVA
decomposition of the variance of the model response
:math:`y = f(\vect{X})`, the latter being represented by its Fourier
expansion. :math:`\vect{X}=\{X^1,\dots,X^{n_X}\}` is an input random
vector of :math:`n_X` independent components.

| The extended FAST method consists in computing
  alternately the first order and the total-effect indices of each
  input. This approach relies upon a Fourier decomposition of the model
  response. Its key idea is to recast this representation as a function
  of a :math:`\mathit{scalar}` parameter :math:`s`, by defining
  parametric curves :math:`s \mapsto x_i(s)`, :math:`i=1, \dots, n_X`
  exploring the support of the input random vector :math:`\vect{X}`.
| For each input, the same procedure is realized in three steps:

-  Sampling:

   Deterministic space-filling paths with random starting points are
   defined, i.e. each input :math:`X^i` is transformed as follows:

   .. math::

      x^i_j = \frac{1}{2} + \frac{1}{\pi} \arcsin(\sin(\omega_i s_j + \phi_i)),
          \quad i=1, \dots, n_X, \, j=1, \dots, N

   where :math:`n_X` is the number of input variables. :math:`N` is the
   length of the discretization of the s-space, with :math:`s` varying
   in :math:`(-\pi, \pi)` by step of :math:`2\pi/N`. :math:`\phi_i` is a
   random phase-shift chosen uniformly in :math:`[0, 2\pi]` which
   enables to make the curves start anywhere within the unit hypercube
   :math:`K^{n_X}=(\vect{X}|0\leq x_i\leq1; i=1, \dots, n_X)`. The
   selection of the set :math:`\{\phi_1, \dots, \phi_{n_X}\}` induces a
   part of randomness in the procedure. So it can be asked to realize
   the procedure :math:`Nr` times and then to calculate the arithmetic
   means of the results over the :math:`Nr` estimates. This operation is
   called :math:`\mathit{resampling}`.

   :math:`\{\omega_i\}, \forall i=1, \dots, n_X` is a set of integer
   frequencies assigned to each input :math:`X^i`. The frequency
   associated with the input of interest is set to the maximum
   admissible frequency satisfying the Nyquist criterion (which ensures
   to avoid aliasing effects):

   .. math:: \omega_i = \frac{N - 1}{2M}

   with :math:`M` the interference factor usually equal to 4 or higher.
   It corresponds to the truncation level of the Fourier series, i.e.
   the number of harmonics that are retained in the decomposition
   realized in the third step of the procedure.

   In the paper [saltelli1999]_, for high sample size, it is
   suggested that :math:`16 \leq \omega_i/N_r \leq 64`.

   And the maximum frequency of the complementary set of frequencies is:

   .. math:: \max(\omega_{-i}) = \frac{\omega_i}{2M} = \frac{N - 1}{4M^2}

   with the index ’\ :math:`-i`\ ’ which meaning ’all but :math:`i`\ ’.

   The other frequencies are distributed uniformly between :math:`1` and
   :math:`\max(\omega_{-i})`. The set of frequencies is the same
   whatever the number of resamplings is.

   Let us make an example with eight input factors, :math:`N=513` and
   :math:`M=4` i.e. :math:`\omega_i = \frac{N - 1}{2M} = 64` and
   :math:`\max(\omega_{-i}) = \frac{N - 1}{4M^2} = 8` with :math:`i` the
   index of the input of interest.

   | When computing the sensitivity indices for the first input, the
     considered set of frequencies is :
     :math:`\{64, 1, 2, 3, 4, 5, 6, 8\}`.
   | When computing the sensitivity indices for the second input, the
     considered set of frequencies is :
     :math:`\{1, 64, 2, 3, 4, 5, 6, 8\}`.
   | etc.

   The transformation defined above provides a uniformly distributed
   sample for the :math:`x_i, \forall i=1, \dots, n_X` oscillating
   between :math:`0` and :math:`1`. In order to take into account the
   real distributions of the inputs, we apply an isoprobabilistic
   transformation on each :math:`x_i` before the next step of the
   procedure.

-  Simulations:

   Output is computed such as:
   :math:`y = f(s) = f(x_1(s), \dots, x_{n_X}(s))`

   Then :math:`f(s)` is expanded onto a Fourier series:

   .. math:: f(s) = \sum_{k \in \Zset^N} A_k \cos(ks) + B_k \sin(ks)

   where :math:`A_k` and :math:`B_k` are Fourier coefficients defined as
   follows:

   .. math::

      \begin{aligned}
          A_k &=& \frac{1}{2\pi}\int_{-\pi}^{\pi}f(s) \cos(ks) \, ds \\
          B_k &=& \frac{1}{2\pi}\int_{-\pi}^{\pi}f(s) \sin(ks) \, ds
        \end{aligned}

   These coefficients are estimated thanks to the following discrete
   formulations:

   .. math::

      \begin{aligned}
          \hat{A}_k &=& \frac{1}{N} \sum_{j=1}^N f(x_j^1,\dots,x_j^{N_X}) cos \left( \frac{2k\pi (j-1)}{N} \right) \quad , \quad -\frac{N}{2} \leq k \leq \frac{N}{2}\\
          \hat{B}_k &=& \frac{1}{N} \sum_{j=1}^N f(x_j^1,\dots,x_j^{N_X}) sin \left( \frac{2k\pi (j-1)}{N} \right) \quad , \quad -\frac{N}{2} \leq k \leq \frac{N}{2}
        \end{aligned}

-  Estimations by frequency analysis:

   The first order indices are estimated as follows:

   .. math::

      \hat{S}_i = \frac{\hat{D}_i}{\hat{D}} \\
                    = \frac{\sum_{p=1}^M(\hat{A}_{p\omega_i}^2 + \hat{B}_{p\omega_i}^2)^2}
                           {\sum_{n=1}^{(N-1)/2}(\hat{A}_n^2 + \hat{B}_n^2)^2}

   where :math:`\hat{D}` is the total variance and :math:`\hat{D}_i` the
   portion of :math:`D` arising from the uncertainty of the
   :math:`i^{th}` input. :math:`N` the size of the sample using to
   compute the Fourier series and :math:`M` is the interference factor.
   Saltelli et al. (1999) recommanded to set :math:`M` to a value in the
   range :math:`[4, 6]`.

   The total order indices are estimated as follows:

   .. math::

      \hat{T}_i = 1 - \frac{\hat{D}_{-i}}{\hat{D}} \\
                    = 1 - \frac{\sum_{k=1}^{\omega_i/2}(\hat{A}_k^2 + \hat{B}_k^2)^2}
                               {\sum_{n=1}^{(N-1)/2}(\hat{A}_n^2 + \hat{B}_n^2)^2}

   where :math:`\hat{D}_{-i}` is the part of the variance due to all the
   inputs except the :math:`i^{th}` input.


.. topic:: API:

    - See :class:`~openturns.FAST`


.. topic:: Examples:

    - See :doc:`/examples/reliability_sensitivity/sensitivity_fast`


.. topic:: References:

    - [saltelli1999]_

