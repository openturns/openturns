.. _chi2_fitting_test:

Chi-squared goodness of fit test
--------------------------------

This method deals with the modelling of a probability distribution of a
random vector :math:`\vect{X} = \left( X^1,\ldots,X^{n_X} \right)`. It
seeks to verify the compatibility between a sample of data
:math:`\left\{ \vect{x}_1,\vect{x}_2,\ldots,\vect{x}_N \right\}` and a
candidate probability distribution previous chosen. The use of the
:math:`\chi^2` Goodness-of-Fit test allows to answer this
question in the one dimensional case :math:`n_X =1`, and with a discrete
distribution.

Let us limit the case to :math:`n_X = 1`. Thus we denote
:math:`\vect{X} = X^1 = X`. We also note that as we are considering
discrete distributions i.e. those for which the possible values of
:math:`X` belong to a discrete set :math:`\cE`, the candidate
distribution is characterised by the probabilities
:math:`\left\{ p(x;\vect{\theta}) \right\}_{x \in \cE}`.

| The chi squared test is based on the fact that if the candidate
  distribution is appropriate, the number of values in the sample x1,
  x2, ..., xN that are equal to :math:`x` should be on average equal to
  :math:`N p(x;\vect{\theta})`. The idea is therefore to compare the
  “theoretical values” with the actual observed values. This comparison
  is performed with the aid of the following “distance”.

  .. math::

     \begin{aligned}
         \widehat{D}_N^2 = \sum_{x \in \cE_N} \frac{\left(Np(x)-n(x)\right)^2}{n(x)}
       \end{aligned}

where :math:`\cE_N` denotes the elements of :math:`\cE` which have
been observed at least once in the data sample and where :math:`n(x)`
denotes the number of data values in the sample that are equal to
:math:`x`.

| The probability distribution of the distance :math:`\widehat{D}_N^2`
  is asymptotically known (i.e. as the size of the sample tends to
  infinity), and this asymptotic distribution does not depend on the
  candidate distribution being tested. If :math:`N` is sufficiently
  large, this means that for a probability :math:`\alpha`, one can
  calculate the threshold / critical value) :math:`d_\alpha` such that:

-  if :math:`\widehat{D}_N>d_{\alpha}`, we reject the candidate
   distribution with a risk of error :math:`\alpha`,

-  if :math:`\widehat{D}_N \leq d_{\alpha}`, the candidate distribution
   is considered acceptable.

An important notion is the so-called “:math:`p`-value” of the test. This
quantity is equal to the limit error probability
:math:`\alpha_\textrm{lim}` under which the candidate distribution is
rejected. Thus, the candidate distribution will be accepted if and only
if :math:`\alpha_\textrm{lim}` is greater than the value :math:`\alpha`
desired by the user. Note that the higher
:math:`\alpha_\textrm{lim} - \alpha`, the more robust the decision.


.. topic:: API:

    - See :py:func:`~openturns.FittingTest_ChiSquared`

.. topic:: Examples:

    - See :doc:`/examples/data_analysis/chi2_fitting_test`

.. topic:: References:

    - [Saporta1990]_
    - [Dixon1983]_
    - [NISThandbook]_
    - [Dagostino1986]_
    - [Sprent2001]_
    - [Bhattacharyya1997]_
    - [Burnham2002]_
