.. _gaussian_process_regression:

Gaussian process regression
---------------------------

Gaussian process regression (also known as Kriging) is a Bayesian
technique that aims at approximating functions (most often in order to
surrogate it because it is expensive to evaluate).

Let :math:`\model: \Rset^\inputDim \rightarrow \Rset^\outputDim` be a model and a dataset
:math:`(\vect{x}_k, \vect{y}_k)_{1 \leq k \leq \sampleSize}` where :math:`\vect{y}_k = \model(\vect{x}_k)`
for all :math:`k`.

The objective is to build a metamodel :math:`\metaModel`, using a Gaussian process that interpolates the data
set. To build this metamodel, we follow the steps:

- **Step 1: Gaussian process fitting**: we build  the Gaussian process :math:`\vect{Y}(\omega, \vect{x})` defined by
  :math:`\vect{Y}(\omega, \vect{x}) = \vect{\mu}(\vect{x}) + \vect{W}(\omega, \vect{x})`
  where :math:`\vect{\mu}` is the trend function and :math:`\vect{W}` is a Gaussian process of
  dimension :math:`\outputDim` with zero mean and covariance function :math:`\mat{C}`;
- **Step 2:  Gaussian Process Regression**: we condition the Gaussian process :math:`\vect{Y}` to the data set by considering the
  Gaussian Process Regression denoted by
  :math:`\vect{Z}(\omega, \vect{x}) = \vect{Y}(\omega, \vect{x})\, | \, \cC` where :math:`\cC`
  is the condition :math:`\vect{Y}(\omega, \vect{x}_k) =  \vect{y}_k` for :math:`1 \leq k \leq \sampleSize`;
- **Step 3:  Gaussian Process Regression metamodel and its exploitation**:  we define the metamodel as :math:`\metaModel(\vect{x}) =  \Expect{\vect{Y}(\omega, \vect{x})\, | \,  \cC}`. Note
  that this metamodel is interpolating the data set. We can use the conditional covariance in order to quantify
  the error of the metamodel, that is the variation of the Gaussian vector at a given point.


Note that the implementation of a
Gaussian process regression deals with vector-valued functions
(:math:`\metaModel: \vect{x} \mapsto \vect{y}`), without simply looping over
each output.


Step 1: Gaussian process fitting
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first step creates the Gaussian process  :math:`\vect{Y}(\omega, \vect{x})` such that the sample
:math:`(\vect{y}_k)_{1 \leq k \leq \sampleSize}` is considered as its restriction  on
:math:`(\vect{x}_k)_{1 \leq k \leq \sampleSize}`. It is defined by:

.. math::

    \vect{Y}(\omega, \vect{x}) = \vect{\mu}(\vect{x}) + \vect{W}(\omega, \vect{x})

where:

.. math::

    \vect{\mu}(\vect{x}) = \left(
      \begin{array}{l}
        \mu_1(\vect{x}) \\
        \vdots  \\
        \mu_\outputDim(\vect{x})
       \end{array}
     \right)

with :math:`\mu_\ell(\vect{x}) = \sum_{j=1}^{b} \beta_j^\ell \varphi_j(\vect{x})` and
:math:`\varphi_j: \Rset^\inputDim \rightarrow \Rset` the trend functions basis for :math:`1 \leq j \leq b` and
:math:`1 \leq \ell \leq \outputDim`.

Furthermore, :math:`\vect{W}` is a Gaussian process of dimension :math:`\outputDim` with zero mean and
covariance function :math:`\mat{C} = \mat{C}(\vect{\theta}, \vect{\sigma}, \mat{R}, \vect{\lambda})`, where (see
:ref:`covariance_model` for more details on the notations):

- :math:`\vect{\theta} \in \Rset^\inputDim` is the scale vector,
- :math:`\vect{\sigma} \in \Rset^\outputDim` is the standard deviation vector,
- :math:`\mat{R} \in \cS_{\outputDim}^+(\Rset)` is the spatial correlation matrix between the components of :math:`\vect{W}`,
- :math:`\vect{\lambda}` gather some additional parameters specific to each covariance model.

Then, we have to estimate the coefficients :math:`\beta_j^\ell` and :math:`\vect{p}`
where :math:`\vect{p}` is the vector of parameters of the covariance model (a subset of
:math:`\vect{\theta}, \vect{\sigma}, \mat{R}, \vect{\lambda}`) that has been declared as
*active*: by default, the full vectors :math:`\vect{\theta}` and :math:`\vect{\sigma}`. See
:class:`openturns.CovarianceModel` to get details on the activation of the estimation of the other
parameters.

The estimation is done by maximizing the *reduced* log-likelihood of the model (see its expression below).

**Estimation of the parameters:** We want to estimate all the parameters :math:`\left(\beta_j^\ell \right)` for
:math:`1 \leq j \leq b`
and :math:`1 \leq \ell \leq \outputDim`, and :math:`\vect{p}`.

We note:

.. math::

    \vect{y} = \left(
      \begin{array}{l}
        \vect{y}_1 \\
        \vdots  \\
        \vect{y}_{\sampleSize}
       \end{array}
     \right) \in \Rset^{\outputDim \times \sampleSize},
     \quad
     \vect{m}_{\vect{\beta}} = \left(
      \begin{array}{l}
        \vect{\mu}(\vect{x}_1) \\
        \vdots  \\
        \vect{\mu}(\vect{x}_{\sampleSize})
       \end{array}
     \right) \in \Rset^{\outputDim \times \sampleSize}


and:

.. math::

    \mat{C}_{\vect{p}} = \left(
      \begin{array}{lcl}
        \mat{C}_{11} & \dots &  \mat{C}_{1 \times \sampleSize}\\
        \vdots &   & \vdots \\
        \mat{C}_{\sampleSize \times 1} & \dots &  \mat{C}_{\sampleSize \times \sampleSize}
       \end{array}
     \right) \in \cS_{\outputDim \times \sampleSize}^+(\Rset)

where :math:`\mat{C}_{ij} = C_{\vect{p}}(\vect{x}_i, \vect{x}_j)\in \cS_{\outputDim \times \outputDim}^+
(\Rset)`.

The likelihood of the Gaussian process on the data set is defined by:

.. math::

    \cL(\vect{\beta}, \vect{p};(\vect{x}_k, \vect{y}_k)_{1 \leq k \leq \sampleSize}) = \dfrac{1}
    {(2\pi)^{\outputDim \times \sampleSize/2} |\det \left(\mat{C}_{\vect{p}}\right)|^{1/2}} \exp\left( -\dfrac{1}{2}
    \Tr{\left( \vect{y}-\vect{m}_{\vect{\beta}} \right)} \mat{C}_{\vect{p}}^{-1}  \left( \vect{y}-\vect{m}
    _{\vect{\beta}} \right)  \right)

Let :math:`\mat{L}_{\vect{p}}` be the Cholesky factor of :math:`\mat{C}_{\vect{p}}`, i.e. the lower triangular
matrix with positive diagonal such that
:math:`\mat{L}_{\vect{p}} \,\Tr{\mat{L}_{\vect{p}}} = \mat{C}_{\vect{p}}`.
Therefore:

.. math::
    :label: logLikelihoodGP

    \log \cL(\vect{\beta}, \vect{p};(\vect{x}_k, \vect{y}_k)_{1 \leq k \leq \sampleSize}) 
    = \alpha - \log \left( \det \left(\mat{L}_{\vect{p}}\right) \right)
    -\dfrac{1}{2}  \left\| \mat{L}_{\vect{p}}^{-1}(\vect{y}-\vect{m}_{\vect{\beta}}) \right\|^2_2

where :matrh:`\alpha \in \Rset` is a constant independent of :math:`\vect{\beta}` and :math:`\vect{p}`. The maximization of :eq:`logLikelihoodGP` leads to the following optimality condition for :math:`\vect{\beta}`:

.. math::

    \vect{\beta}^*(\vect{p}^*)
    = \argmin_{\vect{\beta}} \left\| \mat{L}_{\vect{p}^*}^{-1} \left(\vect{y} - \vect{m}_{\vect{\beta}} \right) \right\|^2_2

This expression of :math:`\vect{\beta}^*` as a function of :math:`\vect{p}^*` is taken as a general relation
between :math:`\vect{\beta}` and :math:`\vect{p}` and is substituted into :eq:`logLikelihood`, leading to
a *reduced log-likelihood* function depending solely on :math:`\vect{p}`.

In the particular case where :math:`\outputDim=\dim(\vect{\sigma})=1` and :math:`\sigma` is a part of :math:`\vect{p}`,
then a further reduction is possible. In this case, if :math:`\vect{q}` is the vector :math:`\vect{p}` in which
:math:`\sigma` has been substituted by 1, then:

.. math::

    \left\| \mat{L}_{\vect{p}}^{-1}(\vect{y}-\vect{m}_{\vect{\beta}}) \right\|^2 
    = \frac{1}{\sigma^2} \left\| \mat{L}_{\vect{q}}^{-1} \left(\vect{y} - \vect{m}_{\vect{\beta}} \right) \right\|^2_2

showing that :math:`\vect{\beta}^*` is a function of :math:`\vect{q}^*` only. The optimality condition
for :math:`\sigma` reads:

.. math::

    \vect{\sigma}^*(\vect{q}^*)
    =\dfrac{1}{\sampleSize} 
    \left\| \mat{L}_{\vect{q}^*}^{-1} \left(\vect{y} - \vect{m}_{\vect{\beta}^*(\vect{q}^*)}\right) \right\|^2_2.

which leads to a further reduction of the log-likelihood function where both :math:`\vect{\beta}` and
:math:`\sigma` are replaced by their expression in terms of :math:`\vect{q}`.

This step is performed by the class :class:`~openturns.GaussianProcessFitter`.

Step 2:  Gaussian Process Regression
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once the Gaussian process  :math:`\vect{Y}` has been estimated, the Gaussian process regression
aims at conditioning it to the data set: we make the Gaussian process approximation become
interpolating over the dataset.

The final Gaussian process regression denoted by :math:`\vect{Z}` is defined by:

.. math::
    :label: GPRdef

    \vect{Z}(\omega, \vect{x}) = \vect{Y}(\omega, \vect{x})\, | \,  \cC

where :math:`\cC` is the condition :math:`\vect{Y}(\omega, \vect{x}_k) = \vect{y}_k` for
:math:`1 \leq k \leq \sampleSize`.

Then, :math:`\vect{Z}` is a Gaussian process, which mean is defined by:

.. math::

   \Expect{\vect{Z}(\omega, \vect{x})}  & =  \Expect{\vect{Y}(\omega, \vect{x})\, | \,  \cC}\\
    & = \vect{\mu}(\vect{x}) + \Cov{\vect{Y}(\omega, \vect{x}), (\vect{Y}(\omega,
    \vect{x}_1), \dots, \vect{Y}(\omega, \vect{x}_{\sampleSize}))} \vect{\gamma}

where:

.. math::

    \Cov{\vect{Y}(\omega, \vect{x}), (\vect{Y}(\omega, \vect{x}_1), \dots, \vect{Y}(\omega, \vect{x}
    _{\sampleSize}))} = \left( \mat{C}( \vect{x},  \vect{x}_1) | \dots | \mat{C}( \vect{x},  \vect{x}
    _{\sampleSize})  \right) \in \cM_{\outputDim,\sampleSize \times \outputDim}(\Rset)

and :math:`\vect{\gamma}` is defined by:

.. math::
    :label: gammaDefinition

    \vect{\gamma} = \mat{C}^{-1} \left( \vect{y}_1 - \vect{\mu}( \vect{x}_1), \dots, \vect{y}_\sampleSize -
    \vect{\mu}( \vect{x}_\sampleSize) \right) \in  \cM_{\sampleSize \times \outputDim, 1}(\Rset)

where:

.. math::

    \mat{C} = \Cov{\vect{Y}(\omega, \vect{x}_1), \dots, \vect{Y}(\omega, \vect{x}_{\sampleSize})}\in
    \cM_{\sampleSize \times \outputDim, \sampleSize \times \outputDim}(\Rset)

Finally, we get the following mean of the Gaussian process regression at the point :math:`\vect{x}`:

.. math::
    :label: expectationGPR

    \Expect{\vect{Z}(\omega, \vect{x})} = \vect{\mu}(\vect{x}) + \sum_{i=1}
    ^\sampleSize \gamma_i \mat{C}( \vect{x},  \vect{x}_i) \in \Rset^{\outputDim}

The covariance matrix of :math:`\vect{Z}` at the point :math:`\vect{x}` is defined by:

.. math::
    :label: covarianceGPR_point

    \Cov{\vect{Z}(\omega, \vect{x})} & =  \Cov{\vect{Y}(\omega, \vect{x}), \vect{Y}(\omega,
    \vect{x})} - \Cov{\vect{Y}(\omega, \vect{x}), (\vect{Y}(\omega,
    \vect{x}_1), \dots, \vect{Y}(\omega, \vect{x}_{\sampleSize}))} \mat{C}^{-1}\Cov{(\vect{Y}
    (\omega, \vect{x}_1), \dots, \vect{Y}(\omega, \vect{x}_{\sampleSize})), \vect{Y}(\omega,
    \vect{x})}

with :math:`\Cov{\vect{Z}(\omega, \vect{x})} \in \cM_{\outputDim \times \outputDim}(\Rset)`.

When computed on the sample :math:`(\vect{\xi}_1, \dots, \vect{\xi}_N)`, the covariance matrix is
defined by:

.. math::
    :label: covarianceGPR_sample

    \Cov{(\vect{Z}(\omega, \vect{\xi}_1), \dots, \vect{Z}(\omega, \vect{\xi}_N)} =
        \left(
          \begin{array}{lcl}
            \Sigma_{11} & \dots & \Sigma_{1N} \\
            \dots  \\
            \Sigma_{N1} & \dots & \Sigma_{NN}
          \end{array}
        \right)

where :math:`\Sigma_{ij} = \Cov{\vect{Z}(\omega, \vect{\xi}_i), \vect{Z}(\omega, \vect{\xi}_j)}`.

This step is performed by the class :class:`~openturns.GaussianProcessRegression`.


Step 3:  Gaussian Process Regression metamodel and its exploitation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Gaussian Process Regression metamodel :math:`\metaModel` is defined by:

.. math::
    :label: GPRmetamodel

    \metaModel(\vect{x}) = \Expect{\vect{Z}(\omega, \vect{x})} =  \vect{\mu}(\vect{x}) + \sum_{i=1}
    ^\sampleSize \gamma_i \mat{C}( \vect{x},  \vect{x}_i).

We can use the conditional covariance of :math:`\vect{Y}` in order to quantify the error of the metamodel. The
:class:`~openturns.GaussianProcessConditionalCovariance` provides all the services to get the error at any point.


.. topic:: API:

    - See :class:`~openturns.GaussianProcessFitter`
    - See :class:`~openturns.GaussianProcessRegression`
    - See :class:`~openturns.GaussianProcessConditionalCovariance`


.. topic:: Examples:

    - See :doc:`/auto_surrogate_modeling/gaussian_process_regression/plot_gpr_1d`


.. topic:: References:

    - [dubourg2011]_
    - S. Lophaven, H. Nielsen and J. Sondergaard, 2002, "DACE, A Matlab kriging toolbox", Technichal University
      of Denmark. https://www.omicron.dk/dace/dace.pdf
    - T. Santner, B. Williams and W. Notz, 2003. "The design and analysis of computer experiments", Springer, New York.
    - C. Rasmussen and C. Williams, 2006, T. Dietterich (Ed.), "Gaussian processes for machine learning", MIT Press.

