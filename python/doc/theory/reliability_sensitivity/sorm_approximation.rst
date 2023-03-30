.. _sorm_approximation:

SORM
----

The Second Order Reliability Method is used under the following context: let :math:`\vect{X}` be a probabilistic
input vector with joint density probability  :math:`\pdf`, let
:math:`\vect{d}` be a  deterministic vector, let :math:`g(\vect{X}\,,\,\vect{d})` be the limit state function of
the model and let :math:`\cD_f = \{\vect{X} \in \Rset^n \,
/ \, g(\vect{X}\,,\,\vect{d}) \le 0\}` be an event whose probability
:math:`P_f` is defined as:

.. math::
  :label: PfX4

  P_f = \Prob{g(\vect{X}\,,\,\vect{d})\leq 0} = \int_{\cD_f}  \pdf\, d\vect{x}

The principle is the same as for :ref:`form_approximation`: we map the physical space into the standard space
through an :ref:`isoprobabilistic transformation <isoprobabilistic_transformation>`).

The integral :eq:`PfX4` can be written as:

.. math::
    :label: PfU2

    P_f = \Prob{h(\vect{U}\,,\,\vect{d})\leq 0} = \int_{\Rset^n} \boldsymbol{1}_{h(\vect{u}\,,\,\vect{d}) \leq 0}
    \,f_{\vect{U}}(\vect{u})\,d\vect{u}

where :math:`f_{\vect{U}}` is the density function of the   distribution in the standard space: that distribution is
spherical (invariant by rotation by definition). That property implies that :math:`f_{\vect{U}}` is a function of
:math:`||\vect{U}||^2` only.

Furthermore, we suppose that outside the sphere which tangents the limit state surface in the standard
space, :math:`f_{\vect{U}}` is decreasing.

The difference with FORM comes from the approximation of the limit state surface at the design point :math:`P^*` in
the standard space: SORM approximates it by a quadratic surface that has the same main curvatures at the design point.
Let  :math:`n` be the dimension of the random vector :math:`\vect{X}` and
:math:`(\kappa_i)_{1 \leq i \leq n-1}` the :math:`(n-1)` main curvatures of the limit state function at the design
point in the standard space.
Several approximations are available,
detailed here in the case where the origin of the standard
space does not belong to the failure domain:

**Breitung’s formula** is an asymptotic result. The
usual formula used in the normal standard space has been generalized
in [lebrun2009b]_ to standard spaces where the
distribution is spherical, with :math:`E` the marginal cumulative
density function of the spherical distributions in the standard space:

.. math::
  :label: PfSORM_B

    P_{Breitung}^{generalized}  \stackrel{\beta\rightarrow\infty}{=} \Phi(-\beta)\prod_{i=1}^{n-1}\frac{1}
    {\sqrt{1+\kappa_i^0}}


where :math:`\Phi` is the cumulative distribution function of the standard 1D normal
distribution and  :math:`(\kappa_1^0, \dots, \kappa_d^0)` the main curvatures of the
homothetic of the failure domain at distance 1 from the origin.

**Hohenbichler’s formula** is an approximation of :eq:`PfSORM_B`:

.. math::
    :label: PfSORM_HB

       P_{Hohenbichler} = \Phi(-\beta) \prod_{i=1}^{n-1} \left( 1+\frac{\phi(-\beta)}{\beta \Phi(-\beta)}\kappa_i^0
       \right)  ^{-1/2}

Recording to the Mill's ratio, :math:`\frac{\phi(-\beta)}{\beta \Phi(-\beta)}` tends to 1 when :math:`\beta` tends
to :math:`+\infty`.
This formula is valid **only** in the normal standard space and if:

.. math::

    1+\frac{\phi(-\beta)}{\beta\Phi(-\beta)}\kappa_i^0 > 0

for any :math:`i`.

**Tvedt’s formula** (Tvedt, 1988):

.. math::
       :label: PfSORM_T

        P_{Tvedt} =  A_1 + A_2 + A_3

where :math:`A_1`, :math:`A_2` and :math:`A_3` are defined by:

.. math::

   A_1  & =    \Phi(-\beta) \prod_{j=1}^{N-1} \left( 1+ \kappa_j^0 \right) ^{-1/2}\\
   A_2 & =    \left[ \beta  \Phi(-\beta) -  \phi(\beta)\right ]  \left[  \prod_{j=1}^{N-1}  \left( 1+\kappa_j^0
   \right) ^{-1/2} -    \prod_{j=1}^{N-1}  \left( 1+(1 / \beta + 1) \kappa_j^0 \right) ^{-1/2} \right] \\
   A_3 & =   (1 + \beta) \left[ \beta  \Phi(-\beta) -  \phi(\beta)\right ] \\
       & \quad \times \left[  \prod_{j=1}^{N-1}  \left( 1+\kappa_j^0 \right)^{-1/2} - \operatorname{Re}
       \left(\prod_{j=1}^{N-1}\left( 1+(\imath / \beta + 1) \kappa_j^0 \right) ^{-1/2} \right) \right]

where :math:`{\cR}e(z)` is the real part of the complex number
:math:`z` and :math:`\imath` the complex number such that
:math:`\imath^2 = -1` and :math:`\Phi` the cumulative distribution
function of the standard 1D normal distribution.

This formula is valid **only** in the normal standard space and if
:math:`1+\kappa_j^0 > 0` and
:math:`1+(1/\beta + 1) \kappa_j^0> 0` for any :math:`j`.

.. topic:: API:

    - See :class:`~openturns.SORM`


.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/reliability/plot_estimate_probability_form`


.. topic:: References:

    - Breitung K. a, "Asymptotic approximation for probability integral,"
      Probability Engineering Mechanics, 1989, Vol 4, No. 4.
    - Breitung K. b, 1984, "Asymptotic Approximation for multinormal Integrals,"
      Journal of Engineering Mechanics, ASCE, 110(3), 357-366.
    - Hohenbichler M., Rackwitz R., 1988, "Improvement of second order reliability
      estimates by importance sampling," Journal of Engineering Mechanics, ASCE,114(12), pp 2195-2199.
    - [lebrun2009b]_
    - [lebrun2009c]_
    - Tvedt L. 1988, "Second order reliability by an exact integral," proc. of
      the IFIP Working Conf. Reliability and Optimization of Structural Systems,
      Thoft-Christensen (Ed), pp377-384.
    - Zhao Y. G., Ono T., 1999, "New approximations for SORM : part 1", Journal of
      Engineering Mechanics, ASCE,125(1), pp 79-85.
    - Zhao Y. G., Ono T., 1999, "New approximations for SORM : part 2", Journal of
      Engineering Mechanics, ASCE,125(1), pp 86-93.
    - Adhikari S., 2004, "Reliability analysis using parabolic failure surface
      approximation", Journal of Engineering Mechanics, ASCE,130(12), pp 1407-1427.

