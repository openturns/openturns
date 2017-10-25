SORM
----

The Second Order Reliability Method is used in the same context as the
First Order Reliability: refer to for further details. The objective of
SORM is to evaluate the probability content of the event
:math:`\cD_f = \{\vect{X} \in \Rset^n \, / \, g(\vect{X}\,,\,\vect{d}) \le 0\}`
:

.. math::
  :label: PfX4

    P_f = \Prob{g(\vect{X}\,,\,\vect{d})\leq 0} = \int_{\cD_f}  \pdf\, d\vect{x}

| The principle is the same as for FORM. After having mapped the
  physical space into the standard through an isoprobabilistic
  transformation :eq:`PfX4` becomes:

  .. math::
    :label: PfU2

        P_f = \Prob{G(\vect{U}\,,\,\vect{d})\leq 0} = \int_{\Rset^n} \boldsymbol{1}_{G(\vect{u}\,,\,\vect{d}) \leq 0}\,f_{\vect{U}}(\vect{u})\,d\vect{u}

  where :math:`f_{\vect{U}}` is the density function of the
  distribution in the standard space : that distribution is spherical
  (invariant by rotation by definition). That property implies that
  :math:`f_{\vect{U}}` is a function of :math:`||\vect{U}||^2` only.
  Furthermore, we suppose that outside the sphere which tangents the
  limit state surface in the standard space, :math:`f_{\vect{U}}` is
  decreasing.

| The difference with FORM comes from the approximation of the limit
  state surface at the design point :math:` \vect{P}^*` in the standard
  space: SORM approximates it by a quadratic surface that has the same
  main curvatures at the design point.
| Let us denote by :math:`n` the dimension of the random vector
  :math:`\vect{X}` and :math:`(\kappa_i)_{1 \leq i \leq n-1}` the
  :math:`n-1` main curvatures of the limit state function at the design
  point in the standard space.
| Several approximations are available,
  detailed here in the case where the origin of the standard
  space does not belong to the failure domain :

-  Breitung’s formula is an asymptotic results: the
   usual formula used in the normal standard space, has been generalized
   in [Lebrun2009b]_ to standard spaces where the
   distribution is spherical, with :math:`E` the marginal cumulative
   density function of the spherical distributions in the standard space:

.. math::
  :label: PfSORM_B

    P_{Breitung}^{generalized}  \stackrel{\beta\rightarrow\infty}{=} E(-\beta)\prod_{i=1}^{n-1}\frac{1}{\sqrt{1+\beta\kappa_i}}

where :math:`\Phi` is the cumulative distribution function of the
standard 1D normal distribution.

-  Hohenbichler’s formula is an approximation of :eq:`PfSORM_B`:

   .. math::
     :label: PfSORM_HB

        \displaystyle P_{Hohenbichler} = \Phi(-\beta_{HL}) \prod_{i=1}^{n-1} \left( 1+\frac{\phi(-\beta_{HL})}{\Phi(-\beta_{HL})}\kappa_i  \right)  ^{-1/2}

   **This formula is valid only in the normal standard space and if**
   :math:`\boldsymbol{\forall i, 1+\frac{\phi(-\beta_{HL})}{\Phi(-\beta_{HL})}\kappa_i > 0}`.

-  | Tvedt’s formula (Tvedt, 1988):

     .. math::
       :label: PfSORM_T

            \left\{
                  \begin{array}{lcl}
                    \displaystyle P_{Tvedt} & = & A_1 + A_2 + A_3 \\
                    \displaystyle A_1 & = &  \displaystyle \Phi(-\beta_{HL}) \prod_{i=1}^{N-1} \left( 1+\beta_{HL} \kappa_i \right) ^{-1/2}\\
                    \displaystyle A_2 & = &   \displaystyle\left[ \beta_{HL}  \Phi(-\beta_{HL}) -  \phi(\beta_{HL})\right ]  \left[  \prod_{j=1}^{N-1}  \left( 1+\beta_{HL} \kappa_i \right) ^{-1/2} -    \prod_{j=1}^{N-1}  \left( 1+(1 + \beta_{HL}) \kappa_i \right) ^{-1/2} \right ] \\
                    \displaystyle A_3 & = &  \displaystyle(1 + \beta_{HL}) \left[ \beta_{HL}  \Phi(-\beta_{HL}) -  \phi(\beta_{HL})\right ]  \left[  \prod_{j=1}^{N-1}  \left( 1+\beta_{HL} \kappa_i \right) ^{-1/2} \right.\\
                      & & \displaystyle\left. - {\cR}e \left(   \prod_{j=1}^{N-1}  \left( 1+(i + \beta_{HL}) \kappa_j \right) ^{-1/2}  \right)\right ]
                  \end{array}
                  \right.

     where :math:`{\cR}e(z)` is the real part of the complex number
     :math:`z` and :math:`i` the complex number such that
     :math:`i^2 = -1` and :math:`\Phi` the cumulative distribution
     function of the standard 1D normal distribution.
     **This formula is valid only in the normal standard space and if**
     :math:`\boldsymbol{\forall i, 1+\beta \kappa_i > 0}` and
     :math:`\boldsymbol{\forall i, 1+(1 + \beta) \kappa_i> 0}`.

.. topic:: API:

    - See :class:`~openturns.SORM`


.. topic:: Examples:

    - See :ref:`examples/reliability_sensitivity/estimate_probability_form.ipynb`


.. topic:: References:

    - Breitung K. a, "Asymptotic approximation for probability integral," Probability Engineering Mechanics, 1989, Vol 4, No. 4.
    - Breitung K. b, 1984, "Asymptotic Approximation for multinormal Integrals," Journal of Engineering Mechanics, ASCE, 110(3), 357-366.
    - Hohenbichler M., Rackwitz R., 1988, "Improvement of second order reliability estimates by importance sampling," Journal of Engineering Mechanics, ASCE,114(12), pp 2195-2199.
    - [Lebrun2009b]_
    - [Lebrun2009c]_
    - Tvedt L. 1988, "Second order reliability by an exact integral," proc. of the IFIP Working Conf. Reliability and Optimization of Structural Systems, Thoft-Christensen (Ed), pp377-384.
    - Zhao Y. G., Ono T., 1999, "New approximations for SORM : part 1", Journal of Engineering Mechanics, ASCE,125(1), pp 79-85.
    - Zhao Y. G., Ono T., 1999, "New approximations for SORM : part 2", Journal of Engineering Mechanics, ASCE,125(1), pp 86-93.
    - Adhikari S., 2004, "Reliability analysis using parabolic failure surface approximation", Journal of Engineering Mechanics, ASCE,130(12), pp 1407-1427.

