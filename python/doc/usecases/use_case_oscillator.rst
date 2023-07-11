.. _use-case-oscillator:

A two degree-of-fredom primary/secondary damped oscillator
==========================================================

We consider a two degree-of-fredom primary/secondary damped oscillator.
This system is composed of a two-stage oscillator characterized by a mass, a stiffness and a damping ratio for each of the two oscillators.
This system is submitted to a white-noise excitation.
The limit-state function is highly non-linear, mainly due to the interactions between the two stages of the system, and presents one failure zone.


.. figure:: ../_static/oscillator.png
    :align: center
    :alt: use case oscillator
    :width: 50%

    Two stage oscillator


The limit state function is defined as follows:

.. math:: G = F_s - 3 k_s \sqrt{\frac{\pi S_0}{4 \zeta_s \omega_s^3} \left[\frac{\zeta_a \zeta_s}{\zeta_p \zeta_s (4 \zeta_a^2 + \theta^2)+\gamma \zeta_a^2}\frac{(\zeta_p \omega_p^3 + \zeta_s \omega_s^3)\omega_p}{4 \zeta_a \omega_a^4}\right]}


The natural frequency of the first oscillator is equal to:

.. math:: \omega_p = \sqrt{\frac{k_p}{m_p}}

The natural frequency of the secondary oscillator is equal to:

.. math:: \omega_s = \sqrt{\frac{k_s}{m_s}}

The average natural frequency of the system is equal to:

.. math:: \omega_a = \frac{\omega_p+\omega_s}{2}

The average damping ratio of the system is equal to:

.. math:: \zeta_a = \frac{\zeta_p+\zeta_s}{2}

The mass ratio is equal to:

.. math:: \gamma = \frac{m_s}{m_p}

The tuning parameter of the system is equal to:

.. math:: \theta = \frac{\omega_p - \omega_s}{\omega_a}

Eight uncertainties are considered in the system:
- on the masses of the primary and secondary systems (:math:`m_p` and :math:`m_s`),
- on the spring stiffeness of the primary and secondary oscillators (:math:`k_p` and :math:`k_s`),
- on the damping ratios of the primary and secondary systems (:math:`\zeta_p` and :math:`\zeta_s`),
- on the force capacity of the secondary spring (:math:`F_s`),
- on the intensity of the white noise excitation (:math:`S_0`).


We consider the following distribution functions:

- :math:`m_p \sim \text{LogNormalSigmaOverMu}(  \mu_{m_p} = 1.5, \delta_{m_p} = 0.1)`
- :math:`m_s \sim \text{LogNormalSigmaOverMu}(  \mu_{m_s} = 0.01,  \delta_{m_s} = 0.1)`
- :math:`k_p \sim \text{LogNormalSigmaOverMu}(  \mu_{k_p} = 1,  \delta_{k_p} = 0.2)`
- :math:`k_s \sim \text{LogNormalSigmaOverMu}(  \mu_{k_s} = 0.01,  \delta_{k_s} = 0.2)`
- :math:`\zeta_p \sim \text{LogNormalSigmaOverMu}(  \mu_{\zeta_p} = 0.05, \delta_{\zeta_p} = 0.4)`
- :math:`\zeta_s \sim \text{LogNormalSigmaOverMu}(  \mu_{\zeta_s} = 0.02,  \delta_{\zeta_s} = 0.5)`
- :math:`F_s \sim \text{LogNormalSigmaOverMu}(  \mu_{F_s} = 27.5,  \delta_{F_s} = 0.1)`
- :math:`S_0 \sim \text{LogNormalSigmaOverMu}( \mu_{S_0} = 100,  \delta_{S_0} = 0.1)`



The failure probability is:

.. math:: P_f = \text{Prob}(G(m_p,m_s,k_p,k_z,\zeta_p,\zeta_s,F_s,Z_0) \leq 0).


The value of :math:`P_f` is:

.. math:: P_f = 3.78 \times 10^{-7}.

References
----------
* Der Kiureghian, A. and De Stefano, M. (1991). Efficient Algorithm for Second-Order Reliability Analysis, Journal of engineering mechanics, 117(12), 2904-2923

* Dubourg, V., Sudret, B., Deheeger, F. (2013). Metamodel-based importance sampling for structural reliability analysis. Probabilistic Engineering Mechanics, 33, pp. 47–57

API documentation
-----------------

.. currentmodule:: openturns.usecases.oscillator

.. autoclass:: Oscillator
    :noindex:

Examples based on this use case
-------------------------------

.. minigallery:: openturns.usecases.oscillator.Oscillator

