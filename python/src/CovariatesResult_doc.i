%feature("docstring") OT::CovariatesResult
R"RAW(Estimation result class for a GEV or GPD model depending on covariates.

Parameters
----------
factory : :class:`~openturns.DistributionFactory`
    Parent distribution factory.
parameterFunction : :class:`~openturns.Function`
    The  function :math:`\vect{\theta}`.
covariates : 2-d sequence of float
    Values of :math:`\vect{y}`.
parameterDistribution : :class:`~openturns.Distribution`
    The distribution of :math:`\vect{\beta}`.
llh : float
    Maximum log-likelihood.

Notes
-----
This class is created by the method
:meth:`~openturns.GeneralizedParetoFactory.buildCovariates` of the classes
:class:`~openturns.GeneralizedExtremeValueFactory` and
:class:`~openturns.GeneralizedParetoFactory`.

Let :math:`Z_{\vect{y}}` be a random variable which follows a GEV distribution or whose
excesses above :math:`u` follow a GPD. We assume that the parameters of the GEV model or the GPD
(except for the threshold of the GPD which is assumed to be known)
depend on :math:`d` covariates
denoted by :math:`\vect{y} = \Tr{(y_1, \dots, y_d)}`:

.. math::

    Z_{\vect{y}} & \sim \mbox{GEV}(\mu(\vect{y}), \sigma(\vect{y}), \xi(\vect{y}))\\
    Z_{\vect{y}} & \sim \mbox{GPD}(\sigma(\vect{y}), \xi(\vect{y}), u)

We denote by :math:`(z_{\vect{y}_1}, \dots, z_{\vect{y}_n})`
the values of :math:`Z_{\vect{y}}` associated to the values of the
covariates :math:`(\vect{y}_1, \dots, \vect{y}_n)`.

For numerical reasons, the covariates have been normalized. Each covariate :math:`y_k`
has its own normalization: 

.. math::

    \tilde{y}_k = \tau_k(y_k) = \dfrac{y_k-c_k}{d_k}

Let :math:`\vect{\theta} = (\theta_1, \dots, \theta_p)` be the set of parameters
:math:`(\mu, \sigma, \xi)` for the GEV model and :math:`(\sigma, \xi)` for the GPD model.
Then, :math:`\vect{\theta}` depends on all the :math:`d`
covariates
even if each component of :math:`\vect{\theta}` only depends on a subset of the covariates.
We denote by :math:`(y_1^q, \dots, y_{d_q}^q)` the :math:`d_q` covariates involved in the
modelling of the component :math:`\theta_q`.

Each component :math:`\theta_q` can be written as a function of the covariates:

.. math::

    \theta_q(y_1^q, \dots, y_{d_q}^q)  & = h_q\left(\sum_{i=1}^{d_q} \beta_i^qy_i^q +
    \beta_{d_q+1}^q \right)

where:

- :math:`h_q: \Rset \mapsto \Rset` is usually referred to as the *inverse-link function* of
  the component :math:`\theta_q`,
- each :math:`\beta_i^{q} \in \Rset`.

To allow one of the parameters to remain constant, i.e. independent of the covariates
(this will generally be the case for the parameter :math:`\xi`, the library systematically
adds the constant covariate to the list speciﬁed by the user, even if it means duplicating it
if the user has already put it in his list.

The complete vector of parameters is defined by:

.. math::

    \Tr{\vect{b}} & = \Tr{( \Tr{\vect{b}_1}, \dots,  \Tr{\vect{b}_p} )} \in  \Rset^{d_t}\\
    \Tr{\vect{b}_q} & =  (\beta_1^q, \dots, \beta_{d_q}^q)\in \Rset^{d_q}

where :math:`d_t = \sum_{i=1}^p d_i`.

The estimator of :math:`\Tr{\vect{b}}` maximizes the likelihood of the Parent distribution.

See also
--------
GeneralizedExtremeValueFactory)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::getDistribution
R"RAW(Accessor to the Parent distribution at a given covariate vector.

Parameters
----------
covariate : sequence of float, 2-d sequence of float
    Covariates value :math:`\vect{y} \in \Rset^d`.

Returns
-------
distribution : :class:`~openturns.Distribution`
    The Parent distribution at *covariate*.)RAW"

// ------------------Time stamps---------------------------------------------------

%feature("docstring") OT::CovariatesResult::getParameterDistribution
R"RAW(Accessor to the distribution of :math:`\vect{\beta}`.

Returns
-------
parameterDistribution : :class:`~openturns.Distribution`
    The distribution of the  estimator of :math:`\vect{\beta}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::setParameterDistribution
R"RAW(Accessor to the distribution of of :math:`\vect{\beta}`.

Parameters
----------
parameterDistribution : :class:`~openturns.Distribution`
    The distribution of the estimator of :math:`\vect{\beta}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::getParameterFunction
R"RAW(Parameter function accessor.

Returns
-------
parameterFunction : :class:`~openturns.Function`
    The function :math:`(\vect{\beta}, \vect{y}) \mapsto \vect{\theta}(\vect{\beta},\vect{y})`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::getCovariates
"Covariates accessor.

Returns
-------
covariates : :class:`~openturns.Sample`
    The sample of covariates.

Notes
-----
If the constant covariate was not specified, a last column has been automatically added
which contains the value 1."

// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::getNormalizationFunction
R"RAW(Normalizing function accessor.

Returns
-------
normalizeFunction : :class:`~openturns.Function`
    The function :math:`\vect{y} \mapsto (\tau_1(y_1), \dots, \tau_d(y_d))`.)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::getOptimalParameter
R"RAW(Optimal parameter accessor.

Returns
-------
optimalParameter : :class:`~openturns.Point`
    Optimal vector of parameters :math:`\vect{\beta}`.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::setLogLikelihood
"Optimal likelihood value accessor.

Parameters
----------
llh : float
    Maximum log-likelihood.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::getLogLikelihood
"Optimal likelihood value accessor.

Returns
-------
llh : float
    Maximum log-likelihood.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::drawParameterFunction1D
R"RAW(Draw the parameter function.

Parameters
----------
parameterIndex : int in [0, 2]
    The index specifying the component  :math:`\theta_q`.
referencePoint : sequence of float, optional
    Reference values for the frozen covariates.
    If not provided the mean of covariates is used.

Returns
-------
grid : :class:`~openturns.GridLayout`
    Graphs of :math:`y_k \mapsto \theta_q(\vect{y})` for :math:`1 \leq k \leq d`.

Notes
-----
Once the index :math:`q` has been chosen, the method draws all the graphs
:math:`y_k \mapsto \theta_q(\vect{y})`, where all the components of
:math:`\vect{y}` are fixed to a reference value excepted for :math:`y_k`, for each
:math:`1 \leq k \leq d`.

Each component of :math:`\vect{\theta}` potentially depends only on a subset of the
covariates. Hence, when the component :math:`\theta_q` does not depend on
:math:`y_k`, the graph is reduced to one horizontal line.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::drawParameterFunction2D
R"RAW(Draw the parameter function.

Parameters
----------
parameterIndex : int in [0, 2]
    The index specifying the component :math:`\theta_q`.
referencePoint : sequence of float, optional
    Reference values for the frozen covariates.
    If not provided the mean of covariates is used.

Returns
-------
grid : :class:`~openturns.GridLayout`
    Graphs of :math:`(y_k, y_\ell) \mapsto \theta_q(\vect{y})` for
    :math:`1 \leq k, \ell \leq d`.

Notes
-----
Once the index :math:`q` has been chosen, the method draws all the graphs
:math:`(y_k, y_\ell) \mapsto \theta_q(\vect{y})`, where all the components of
:math:`\vect{y}` are fixed to a reference value excepted for :math:`(y_k, y_\ell)`, for each
:math:`1 \leq k, \ell \leq d`.

Each component of :math:`\vect{\theta}` potentially depends only on a subset of the
covariates. Hence, when the component :math:`\theta_q` does not depend on
:math:`(y_k, y_\ell)`, the graph is reduced to one point. If it does not depend on one of
the two, the graph is reduced to one line.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::drawQuantileFunction1D
R"RAW(Draw the quantile function.

Parameters
----------
p : float
    The quantile level.
referencePoint : sequence of float, optional
    Reference values for the frozen covariates.
    If not provided the mean of covariates is used.

Returns
-------
grid : :class:`~openturns.GridLayout`
    Graphs of :math:`y_k \mapsto q_p(Z_{\vect{y}})` for :math:`1 \leq k \leq d`.

Notes
-----
The method plots all the graphs of the quantile functions of order :math:`p`: of
:math:`Z_{\vect{y}}`:  :math:`y_k \mapsto q_p(Z_{\vect{y}})` where all the components of
:math:`\vect{y}` are fixed to a reference value excepted for :math:`y_k`, for each
:math:`1 \leq k \leq d`.

Each component of :math:`\vect{\theta}` potentially depends only on a subset of the
covariates. Hence, when the component :math:`\theta_q` does not depend on
:math:`y_k`, the graph is reduced to one horizontal line.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::CovariatesResult::drawQuantileFunction2D
R"RAW(Draw the quantile function.

Parameters
----------
p : float
    The quantile level.
referencePoint : sequence of float, optional
    Reference values for the frozen covariates.
    If not provided the mean of covariates is used.

Returns
-------
grid : :class:`~openturns.GridLayout`
    Graphs of :math:`(y_k, y_\ell) \mapsto q_p(Z_{\vect{y}})` for :math:`1 \leq k, \ell \leq d`.

Notes
-----
The method plots all the graphs of the quantile functions of order :math:`p`: of
:math:`Z_{\vect{y}}`:  :math:`(y_k, y_\ell) \mapsto q_p(Z_{\vect{y}})` where all the
components of :math:`\vect{y}` are fixed to a reference value excepted for
:math:`(y_k, y_\ell)`, for each :math:`1 \leq k, \ell \leq d`.

Each component of :math:`\vect{\theta}` potentially depends only on a subset of the
covariates. Hence, when the component :math:`\theta_q` does not depend on
:math:`(y_k, y_\ell)`, the graph is reduced to one point. If it does not depend on one of
the two, the graph is reduced to one line.)RAW"
