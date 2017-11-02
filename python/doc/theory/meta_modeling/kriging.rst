Kriging
-------

Kriging (also known as Gaussian process regression) is a Bayesian
technique that aim at approximating functions (most often in order to
surrogate it because it is expensive to evaluate). In the following it
is assumed we aim at creating a surrogate model of a scalar-valued model
:math:`\cM: \vect{x} \mapsto y`. Note the implementation of
Kriging deals with vector-valued functions
(:math:`\cM: \vect{x} \mapsto \vect{y}`), without simply looping over
each output. It is also assumed the model is obtained over a design of
experiments in order to produce a set of observations gathered in the
following dataset:
:math:`\left(\left(\vect{x}^{(i)}, y^{(i)}\right), i = 1, \ldots, m\right)`.
Ultimately Kriging aims at producing a predictor (also known as a
response surface or metamodel) denoted as :math:`\tilde{\cM}`.

We put the following Gaussian process prior on the model :math:`\cM`:

.. math:: Y(\vect{x}) = \Tr{\vect{f}(\vect{x})} \vect{\beta} + Z(\vect{x})

where:

-  :math:`\Tr{\vect{f}(\vect{x})} \vect{\beta}` is a generalized linear
   model based upon a functional basis
   :math:`\vect{f} = \left(f_j, j = 1, \ldots, p\right)` and a vector of
   coefficients
   :math:`\vect{\beta} = \left(\beta_j, j = 1, \ldots, p\right)`,

-  :math:`Z` is a zero-mean stationary Gaussian process whose covariance
   function reads:

   .. math:: \mathbb{E}[Z(\vect{x})\,Z(\vect{x'})] = \sigma^2 R(\vect{x} - \vect{x'}, \vect{\theta})

   where :math:`\sigma^2 > 0` is the variance and :math:`R` is the
   correlation function that solely depends on the Manhattan distance
   between input points :math:`\vect{x} - \vect{x'}` and a vector of
   parameters :math:`\vect{\theta} \in \Rset^{n_\theta}`.

Under the Gaussian process prior assumption, the observations
:math:`\vect{Y} = \left(Y_i, i = 1, \ldots, m\right)` and a prediction
:math:`Y(\vect{x})` at some unobserved input :math:`\vect{x}` are
jointly normally distributed:

.. math::

   \begin{pmatrix}
         \vect{Y} \\
         Y(\vect{x})
       \end{pmatrix}
       \thicksim
       \cN_{m + 1}\left(
         \begin{pmatrix}
           \mat{F} \vect{\beta} \\
           \Tr{\vect{f}(\vect{x})} \vect{\beta}
         \end{pmatrix}
         ,\,\sigma^2
         \begin{pmatrix}
           \mat{R} & \vect{r}(\vect{x}) \\
           \vect{r}(\vect{x})^t & 1
         \end{pmatrix}
       \right)

where:

.. math:: \mat{F} = \left[f_j\left(\vect{x}^{(i)}\right), i = 1, \ldots, m, j = 1, \ldots, p\right]

is the regression matrix,

.. math:: \mat{R} = \left[R\left(\vect{x}^{(i)} - \vect{x}^{(j)}, \vect{\theta}\right), i,\,j = 1, \ldots, m\right]

is the observations’ correlation matrix, and:

.. math:: \vect{r}(\vect{x}) = \Tr{\left(R\left(\vect{x} - \vect{x}^{(i)}, \vect{\theta}\right), i = 1, \ldots, m\right)}

is the vector of cross-correlations between the prediction and the
observations.

As such, the Kriging predictor is defined as the following conditional
distribution:

.. math:: \tilde{Y}(\vect{x}) = \left[Y(\vect{x}) \mid \vect{Y} = \vect{y}, \vect{\theta}=\vect{\theta}^*, \sigma^2=(\sigma^2)^*\right]

where :math:`\vect{\theta}^*` and :math:`(\sigma^2)^*` are the maximum
likelihood estimates of the correlation parameters :math:`\vect{\theta}`
and variance :math:`\sigma^2` (see references).

It can be shown (see references) that the predictor
:math:`\tilde{Y}(\vect{x})` is also Gaussian:

.. math:: \tilde{Y}(\vect{x}) = \cN_1\left(\mu_{\tilde{Y}}(\vect{x}), \sigma^2_{\tilde{Y}}(\vect{x})\right)

-  with mean:

   .. math:: \mu_{\tilde{Y}}(\vect{x}) = \Tr{\vect{f}(\vect{x})} \tilde{\vect{\beta}} + \Tr{\vect{r}(\vect{x})} \vect{\gamma}

   where :math:`\underline{\tilde{\beta}}` is the generalized least
   squares solution of the underlying linear regression problem:

   .. math:: \tilde{\vect{\beta}} = \left(\Tr{\mat{F}} \mat{R}^{-1} \mat{F}\right)^{-1} \Tr{\mat{F}} \mat{R}^{-1} \vect{y}

    and

   .. math:: \vect{\gamma} = \mat{R}^{-1} \left(\vect{y} - \mat{F} \tilde{\vect{\beta}}\right)

-  and variance:

   .. math::

      \sigma^2_{\tilde{Y}}(\vect{x}) =
              \sigma^2 \left[1 -
                  \Tr{\vect{r}(\vect{x})} \mat{R}^{-1} \vect{r}(\vect{x})
                  + \Tr{\vect{u}(\vect{x})} \left(\Tr{\mat{F}} \mat{R}^{-1} \mat{F}\right)^{-1} \vect{u}(\vect{x})
                  \right]

   where:

   .. math:: \vect{u}(\vect{x}) = \Tr{\mat{F}} \mat{R}^{-1} \vect{r}(\vect{x}) - \vect{f}(\vect{x})

Kriging may also be referred to as *Gaussian process regression*.


.. topic:: API:

    - See :class:`~openturns.KrigingAlgorithm`


.. topic:: Examples:

    - See :ref:`examples/meta_modeling/kriging.ipynb`


.. topic:: References:

    - [Dubourg2011]_
    - S. Lophaven, H. Nielsen and J. Sondergaard, 2002, "DACE, A Matlab kriging toolbox", Technichal University of Denmark. http://www2.imm.dtu.dk/projects/dace/
    - T. Santner, B. Williams and W. Notz, 2003. "The design and analysis of computer experiments", Springer, New York.
    - C. Rasmussen and C. Williams, 2006, T. Dietterich (Ed.), "Gaussian processes for machine learning", MIT Press.

