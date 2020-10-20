.. _parametric_models:

Standard parametric models
--------------------------

Parametric models aim to describe probability distributions of a random
variable with the aid of a limited number of parameters
:math:`\vect{\theta}`. Therefore, in the case of continuous variables
(i.e. where all possible values are continuous), this means that the
probability density of
:math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)` can be expressed as
:math:`f_X(\vect{x};\vect{\theta})`. In the case of discrete variables
(i.e. those which take only discrete values), their probabilities can be
described in the form :math:`\Prob{\vect{X} = \vect{x};\vect{\theta}}`.

.. topic:: API:

    - See the available :ref:`parametric distributions <parametric_distributions>`.

.. topic:: Examples:

    - See :doc:`/auto_probabilistic_modeling/distributions/plot_draw_1d_distribution`
    - See :doc:`/auto_probabilistic_modeling/distributions/plot_draw_2d_distribution`
    - See :doc:`/auto_probabilistic_modeling/distributions/plot_gaussian_distribution`
    - See :doc:`/auto_probabilistic_modeling/distributions/plot_2d_gaussian_distribution`
    - See :doc:`/auto_probabilistic_modeling/distributions/plot_geometric_distribution`

.. topic:: References:

    - [saporta1990]_
    - [dixon1983]_
    - [bhattacharyya1997]_
