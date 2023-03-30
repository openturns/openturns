Reliability Index
-----------------

The generalized reliability index :math:`\beta_{gen}` is used under the
following context: let :math:`\vect{X}` be a probabilistic input
vector with joint density probability  :math:`\pdf`, let
:math:`\vect{d}` be a  deterministic vector, let :math:`g(\vect{X}\,,\,\vect{d})` be the limit state function of
the model and let :math:`\cD_f = \{\vect{X} \in \Rset^n \,
/ \, g(\vect{X}\,,\,\vect{d}) \le 0\}` be an event whose probability
:math:`P_f` is defined as:

.. math::
    :label: PfX6

    P_f = \int_{g(\vect{X}\,,\,\vect{d}) \le 0}  \pdf\, d\vect{x}.

The generalized reliability index is defined as:

.. math::

    \beta_{gen} = \Phi^{-1}(1-P_f) = -\Phi^{-1}(P_f),

where :math:`\Phi` is the CDF of the normal distribution with zero mean and unit variance.

As :math:`\beta_{gen}` increases, :math:`P_f` decreases rapidly.

According to the method used to evaluate :math:`P_f`, the generalized reliability index differs:

-  when :math:`P_f` has been obtained from the :ref:`form_approximation` approximation, then :math:`\beta_{gen}` is
   equal to the  Hasofer-Lindt reliability index :math:`\beta`, which is the distance of the design point from the
   origin of the standard space,

-  when :math:`P_f` has been obtained  from a :ref:`sorm_approximation` approximation, then :math:`\beta_{gen}` is
   equal to  :math:`\beta_{Breitung}`, :math:`\beta_{Tvedt}` or :math:`\beta_{Hohenbichler}`,

-  when :math:`P_f` has been obtained from  another technique (Monte Carlo simulations, importance samplings,...), we
   get the generalized index  :math:`\beta_{gen}`.

.. topic:: API:

    - See :class:`~openturns.FORMResult`
    - See :class:`~openturns.SORMResult`


.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/reliability/plot_estimate_probability_form`


.. topic:: References:

    - Cornell, "A probability-based structural code," Journal of the American Concrete
      Institute, 1969, 66(12), 974-985.
    - O. Ditlevsen, 1979, "Generalized Second moment reliability index,"
      Journal of Structural Mechanics, ASCE, Vol.7, pp. 453-472.
    - O. Ditlevsen and H.O. Madsen, 2004, "Structural reliability methods,"
      Department of mechanical engineering technical university of Denmark - Maritime engineering,
      internet publication.
    - Hasofer and Lind, 1974, "Exact and invariant second moment code format,"
      Journal of Engineering Mechanics Division, ASCE, Vol. 100, pp. 111-121.
