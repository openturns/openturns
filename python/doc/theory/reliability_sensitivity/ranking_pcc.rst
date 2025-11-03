.. _ranking_pcc:

Uncertainty ranking: PCC and PRCC
---------------------------------

Partial Correlation Coefficients deal with analyzing the influence of the random vector
:math:`\inputRV = \left( X_1,\ldots,X_{\inputDim} \right)` on a random variable :math:`Y`, which is being studied.
Here we attempt to measure the linear relationships that exist between :math:`Y`
and the different components :math:`X_i`.

The basic method of hierarchical ordering using Pearson’s coefficients deals with the case
where the variable :math:`Y` depends linearly on the :math:`\inputDim` variables
:math:`\left{ X_1,\ldots,X_{\inputDim} \right}`.

Partial Correlation Coefficients are also useful in this case
but provide a different kind of information:
the partial correlation
coefficient :math:`\textrm{PCC}_{X_i,Y}` between the variables
:math:`Y` and :math:`X_i` measures the residual influence
of :math:`X_i` on :math:`Y` once influences from all other variables
:math:`X_j` have been eliminated.
In particular, if :math:`X_1` and :math:`X_2` are perfectly correlated,
then :math:`\textrm{PCC}_{X_1,Y} = \textrm{PCC}_{X_2,Y} = 0`.

For any variable index :math:`i \in \{1, ..., \inputDim\}`,
the estimation for each partial correlation coefficient
:math:`\textrm{PCC}_{X_i,Y}` uses a sample of size :math:`\sampleSize` denoted by
:math:`\left\{ \left(y^{(1)},x_1^{(1)},\ldots,x_{\inputDim}^{(1)} \right),\ldots, \left(y^{(\sampleSize)},x_1^{(\sampleSize)},\ldots,x_{\inputDim}^{(\sampleSize)} \right) \right\}`
of the vector :math:`(Y,X_1,\ldots,X_{\inputDim})`. This requires the
following three steps to be carried out:

#. Determine the effect of other variables
   :math:`\left\{ X_j,\ j\neq i \right\}` on :math:`Y` by linear
   regression; when the values of the variables
   :math:`\left\{ X_j,\ j\neq i \right\}` are known, the average
   forecast for the value of :math:`Y` is then available in the form
   of the equation:

   .. math::

      \begin{aligned}
            \widehat{Y} = \sum_{k \neq i,\ 1 \leq k \leq d} \widehat{a}_k X_k
          \end{aligned}

#. Determine the effect of other variables
   :math:`\left\{ X_j,\ j\neq i \right\}` on :math:`X_i` by linear
   regression; when the values of the variables
   :math:`\left\{ X_j,\ j\neq i \right\}` are known, the average
   forecast for the value of :math:`X_i` is then available in the form
   of the equation:

   .. math::

      \begin{aligned}
            \widehat{X}_i = \sum_{k \neq i,\ 1 \leq k \leq d} \widehat{b}_k X_k
          \end{aligned}

#. The Pearson Correlation Coefficient coefficient :math:`\textrm{PCC}_{X_i,Y}`
   is then equal to the sample Pearson correlation coefficient
   :math:`\widehat{\rho}_{Y-\widehat{Y},X_i-\widehat{X}_i}`
   estimated for the variables :math:`Y-\widehat{Y}` and :math:`X_i-\widehat{X}_i`.

One can then class the :math:`d` variables :math:`X_1,\ldots, X_{\inputDim}`
according to the absolute value of the partial correlation coefficients:
the higher the value of :math:`\left| \textrm{PCC}_{X_i,Y} \right|`,
the greater the impact the variable :math:`X_i` has on :math:`Y`.

In order to introduce the PRCC, we need to define the *rank* of an observation of a random variable.
Let :math:`X` be a random variable.
Let :math:`\{x_1, ..., x_{\sampleSize}\}` be a sample of size :math:`\sampleSize`.
We can sort the sample in increasing order:

.. math:
    x_{(1)} \leq x_{(2)} \leq \cdots \leq x_{(n)}.

For any :math:`i \in \sampleSize`, the index :math:`j:=\text{rank}(x_i) \in \{1, ..., \sampleSize\}`
is the *rank* of the :math:`i`-th observation if :math:`x_i` is the :math:`j`-th largest observation
in the sample.
In other words, the observation :math:`x_i` appears at the :math:`j`-th index in the ordered
sample :math:`\{x_{(1)}, ..., x_{(\sampleSize)}\}`.

Now that the rank of a random variable is defined, consider again the case of an input random
vector :math:`\inputRV = \left( X_1,\ldots,X_{\inputDim} \right)` and the output random variable :math:`Y`.
The Partial Rank Correlation Coefficients (PRCC) are PRC coefficients computed on the rank
of the input variables :math:`\text{rank}(\inputRV) = \left( \text{rank}(X_1),\ldots, \text{rank}(X_{\inputDim}) \right)`
and the rank of the output variable :math:`\text{rank}(Y)`.


.. topic:: API:

    - See :meth:`~openturns.CorrelationAnalysis.computePCC`
    - See :meth:`~openturns.CorrelationAnalysis.computePRCC`


.. topic:: Examples:

    - See :doc:`/auto_data_analysis/sample_analysis/plot_sample_correlation`


.. topic:: References:

    - [saltelli2000]_
    - [helton2003]_
    - [kleijnen1999]_
    - [clouvel2025]_ section 4.3
