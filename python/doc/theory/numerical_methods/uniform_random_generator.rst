.. _uniform_random_generator:

Uniform Random Generator
------------------------

| Generating simulations according to a distribution is based on
  generating simulations according to a Uniform distribution on
  :math:`[0,1]` : several techniques exist then to transform a
  realization according to a uniform distribution onto a realization
  according to a distribution which cumulative distribution function is
  :math:`F` (refer to for each distribution).
| Thus, the quality of the random generation of simulation is entirely
  based on the quality of the *deterministic* algorithm which simulates
  realizations of the Uniform(0,1) distribution.
| We use the DSFTM algorithm described here, which is the
  acronym of **D**\ ouble precision **S**\ IMD oriented **F**\ ast
  **M**\ ersenne **T**\ wister.

Each character is detailed of the acronym is detailed :

-  **S = SIMD = Single Instruction Multiple Data**: the DSFMT algorithm
   is able to detect and take profit of the capacity of the
   microprocessor to realise several operations at a time.

-  **F = Fast**: the transformation of the :math:`k`-th state vector of
   the random generator into the :math:`(k+1)`-th state vector is
   written in order to optimize its performance.

-  **MT = Mersenne Twister**: the algorithm characteristics are the
   following ones :

   #. the algorithm is initialized with a high Mersenne Number, of type
      :math:`2^{2^n}-1`, with :math:`n=19937`.

   #. the algorithm period :math:`T` depends on that initial point :
      :math:`T = 2^{19937} \simeq 10^{6000}`. As a general way, the bad
      effects of the periodicity of the algorithm arise as soon as the
      number of simulations is greater than :math:`\, \simeq \sqrt{T}`
      simulations. Here, we have :
      :math:`\sqrt{T} =2^{9968}\simeq 10^{3000}`.

   #. the realizations of the DSFMT algorithm are uniformly distributed
      within :math:`[0,1]^n` until :math:`n=624`.

.. topic:: API:

    - See :class:`~openturns.RandomGenerator`
    - See :class:`~openturns.RandomGeneratorState` to save the generator state

.. topic:: Examples:

    - See :doc:`/examples/numerical_methods/random_generator`

