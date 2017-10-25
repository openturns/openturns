Reliability Index
-----------------

| The generalised reliability index :math:`\beta` is used under the
  following context: :math:`\vect{X}` is a probabilistic input vector,
  :math:`\pdf` its joint density probability, :math:`\vect{d}` a
  deterministic vector, :math:`g(\vect{X}\,,\,\vect{d})` the limit state
  function of the model,
  :math:`\cD_f = \{\vect{X} \in \Rset^n \, / \, g(\vect{X}\,,\,\vect{d}) \le 0\}`
  the event considered here and g(,) = 0 its boundary.
| The probability content of the event :math:`\cD_f` is :math:`P_f`:

  .. math::
    :label: PfX6

      P_f = \int_{g(\vect{X}\,,\,\vect{d}) \le 0}  \pdf\, d\vect{x}.

The generalised reliability index is defined as:

.. math::

    \beta_g = \Phi^{-1}(1-P_f) = -\Phi^{-1}(P_f).

| As :math:`\beta_g` increases, :math:`P_f` decreases rapidly.

These indices are available:

-  :math:`\beta_{FORM}` the FORM reliability index, where :math:`P_f` is
   obtained with a FORM approximation (refer to ~): in this case, the
   generalised reliability index is equal to the Hasofer-Lindt
   reliability index :math:`\beta_{HL}`, which is the distance of the
   design point from the origin of the standard space,

-  :math:`\beta_{SORM}` the SORM reliability index, where :math:`P_f` is
   obtained with a SORM approximation : Breitung, Hohen-Bichler or Tvedt
   (refer to ),

-  :math:`\beta_g` the generalised reliability index, where :math:`P_f`
   is obtained with another technique : Monte Carlo simulations,
   importance samplings,...


.. topic:: API:

    - See :class:`~openturns.FORMResult`
    - See :class:`~openturns.SORMResult`


.. topic:: Examples:

    - See :ref:`examples/reliability_sensitivity/estimate_probability_form.ipynb`


.. topic:: References:

    - Cornell, "A probability-based structural code," Journal of the American Concrete Institute, 1969, 66(12), 974-985.
    - O. Ditlevsen, 1979, "Generalised Second moment reliability index," Journal of Structural Mechanics, ASCE, Vol.7, pp. 453-472.
    - O. Ditlevsen and H.O. Madsen, 2004, "Structural reliability methods," Department of mechanical engineering technical university of Denmark - Maritime engineering, internet publication.
    - Hasofer and Lind, 1974, "Exact and invariant second moment code format," Journal of Engineering Mechanics Division, ASCE, Vol. 100, pp. 111-121.

