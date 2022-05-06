.. _sensitivity_hsic:

Sensitivity analysis using Hilbert-Schmidt Indepencence Criterion (HSIC)
------------------------------------------------------------------------

Introduction
~~~~~~~~~~~~

The Hilbert-Schmidt Indepencence Criterion deals with analyzing the influence that the random vector
:math:`\vect{X} = \left( X^1,\ldots,X^{d} \right)` has on a random variable
:math:`Y`, which is being studied for uncertainty. Here, we attempt to evaluate the influence
through the dependence between the two random variables :math:`Y` and :math:`X^i`.
In practice, we compute the dependence between :math:`Y` and :math:`X^i` as the
distance between the joint distribution :math:`P_{Y,X^i}` and the 
product of the marginal distributions :math:`P_{X^i}P_{Y}`. 

In the following paragraphs, we consider an independent and identically distributed
learning sample of size :math:`n`, which can for instance be obtained through
Monte Carlo sampling or real life observations:

.. math::

    \left(\mathbf{X}^{(j)}, Y^{(j)}\right)_{(1\leq j\leq n)} = \left(X_1^{(j)}, X_2^{(j)}, \dots, X_d^{(j)}, Y^{(j)}\right)_{(1\leq j\leq n)}

where :math:`\mathbf{X}` and :math:`Y` respectively follow :math:`P_{\mathbf{X}}` and :math:`P_{Y}`.
In many cases, only :math:`\mathbf{X}` is sampled, while :math:`P_{Y}` is obtained
as the output of a computer code: :math:`Y^{(j)} = \mathcal{M}\left(X_1^{(j)}, X_2^{(j)}, \dots, X_d^{(j)}\right)`


HSIC definition
~~~~~~~~~~~~~~~

Suppose :math:`\cX_{i} \ \forall i \ \in \{1,\dots,d\}` and :math:`\cY` are measurable spaces.
Let :math:`\cF_{i} : \cX_i \rightarrow \Rset` and :math:`\cG : \cY \rightarrow \Rset` 
be two (universal) Reproducing Kernel Hilbert Spaces (RKHS).
These functional spaces are equipped with their characteristic kernels: (resp.) 
:math:`\kappa_{i}(\cdot,\cdot)` and :math:`\kappa(\cdot,\cdot)` and the associated
scalar products are denoted by :math:`\langle \cdot, \cdot \rangle_{\cF_{i}}` and 
:math:`\langle \cdot, \cdot \rangle_{\cG}`. This allows to define the evaluation
operator: :math:`f \in \cF_i \rightarrow  f(\mathbf{x}) = \langle f, \kappa_i (\mathbf{x}, \cdot) \rangle_{\cF_{i}}`

Let us now consider  :math:`\cV_{i}`, an RKHS over :math:`\cX_{i} \times \cY`
with kernel :math:`v_{i}(\cdot, \cdot)`. We can define the mean embedding of :math:`P_YP_{X_i}`
and :math:`P_{Y,X_i}` in :math:`\mathcal{V}_{i}` as:

.. math::

    \mu [ P_YP_{X_i} ]  = \Eset_{Y} \Eset_{X_i} [v_{i}((Y, X_i),\cdot) ] 

.. math::

    \mu [ P_{Y,X_i} ] = \Eset_{Y,X_i} [v_{i}((Y, X_i),\cdot) ]	
    
We can then define a dependence measure between :math:`Y^k` and :math:`X^i`, under
the form of HSIC, as the squared distance between the mean embeddings of
:math:`P_YP_{X_i}` and :math:`P_{Y,X_i}`:

.. math::

    \mathrm{HSIC}(X_i,Y)_{\cF_{i},\cG} := || \mu [ P_{Y,X_i} ] - \mu [ P_YP_{X_i} ]  ||^2

Assuming :math:`v_{i}((Y, X_i),(Y', X_i') ) = \kappa(Y,Y') \kappa_i (X_i, X_i')`, it can be shown that:

.. math::

    \mathrm{HSIC}(X_i,Y)_{\cF_{i},\cG} = \Eset[\kappa_i(X_i,X_i^{'})\kappa(Y,Y^{'})]
	+ \Eset[\kappa_i(X_i,X_i^{'})]\Eset[\kappa(Y,Y^{'})] -
    2\Eset[\Eset[\kappa_i(X_i,X_i^{'})|X_i]~\Eset[\kappa(Y,Y^{'})|Y]]

where :math:`(X_i^{'}, Y^{'})` is an independent and identically distributed copy of :math:`(X_i,Y)`.

HSIC estimators
~~~~~~~~~~~~~~~

Two alternative estimators exist in order to compute the HSIC value. 
The first one is a biased, but asymptotically unbiased estimator
based on V-statistics:

.. math::

    \widehat{\mathrm{HSIC}}(X_i,Y) = \frac{1}{n^2} \mathrm{Tr}(\mat{L_iHLH})

where :math:`\mat{L_i}` and :math:`\mat{L}` are Gram matrices computed with the respective kernels 
:math:`L_{i_{j,k}} = \kappa_i(x_i^j,x_i^k)` and :math:`L_{j,k}= \kappa(y^j,y^k)`, 
while :math:`\mat{H}` a shift matrix defined as:
:math:`H_{j,k} = \left(\delta_{j,k} - \frac{1}{n}\right)_{1 \leq j, k \leq n}`.

The second estimator is an unbiased estimator based on U-statistics:

.. math::

	\widehat{\mathrm{HSIC}}(X_i,Y) = \frac{1}{m(m-3)} \left[\mathrm{Tr}(\mat{\tilde{L}_i \tilde{L}}) + \frac{\mathbf{1}^{\top} \mat{\tilde{L}_i} \mathbf{1}\mathbf{1}^{\top} \mat{\tilde{L}} \mathbf{1}}{(m-1)(m-2)} - \frac{2}{m-2} \mathbf{1}^{\top} \mat{\tilde{L}_i} \mat{\tilde{L}} \mathbf{1}\right]


where :math:`\tilde{L}_{i_{j,k}}` and :math:`\tilde{L}_{j,k}` are computed as:

.. math::

    \tilde{L}_{i_{j,k}} = (1-\delta_{j,k}) \kappa\left( Y^{(j)}, Y^{(k)}\right) 

	\tilde{L}_{j,k} = (1-\delta_{j,k}) \kappa_i\left( X_i^{(j)}, X_i^{(k)}\right) 

In order to compare the HSIC values associated to various input variables :math:`X^i`,
it is common practice to consider a normalized index (bounded between 0 and 1) called R2-HSIC,
and defined as: 

.. math::

    \widehat{R_{\mathrm{HSIC},i}^2} = \frac{\widehat{\mathrm{HSIC}}(X_i,Y)}{\sqrt{\widehat{\mathrm{HSIC}}(X_i,X_i)~\widehat{\mathrm{HSIC}}(Y,Y)}}

Please note that, differently from the Sobol indices typically used in the context
of sensitivity analysis, the sum of all normalized R2-HSIC indices for a given set
of considered input variables does not sum to 1.

Most covariance kernels that can be used in order to compute the Gram matrices of the
HSIC estimators are characterized by one or several hyper-parameters. No universal
rule exists allowing to determine the optimal value for these hyper-parameters, however, 
for some specific kernels empirical rules are proposed. For instance, the squared
exponential (or Gaussian) kernel can be parameterized as a function of the sample 
empirical variance. In this case, we obtain :

.. math::

    \kappa_i(x_i^j,x_i^k) = \exp (- \frac{(x_i^j - x_i^k)^2}{2\theta_i^2})

with :math:`\theta_i = \sigma_i`, where :math:`\sigma_i` is the empirical 
standard deviation of the sample :math:`X_i`.

Screening with HSIC-based statistical tests
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The HSIC can also be used in order to perform screening on a set of input variables.
This can be defined as the the process of identifying the input variables which are 
significantly influential on the considered output. 
More specifically, within the framework of HSIC this can be done by relying on 
statistical hypothesis tests. In practice, we wish to test the the following hypothesis:

.. math::
    
    \cT: \textrm{Test}\quad (\cH_{0,i}):\mathrm{HSIC}(X_i,Y) = 0

which, thanks to the HSIC properties, is equivalent to assessing the hypothesis of
independence between :math:`Y` and :math:`X^i`.

We define the test statistic as: :math:`\widehat{S}_{\cT} := n \times \widehat{\mathrm{HSIC}}(X_i,Y)`,
and the associated p-value: 
:math:`p_{\textrm{val}} = \Pset\left(\widehat{S}_{\cT} \geq \widehat{S}_{\cT,\textrm{obs}}~|~\cH_{0,i}\right)`,
where :math:`\widehat{S}_{\cT,\textrm{obs}}` is the stastistic observed on the given sample.
In other words, the p-value represents the probability of obtaining a 
:math:`\mbox{HSIC}(X_i,Y)` value as large as the observed one under the assumption 
that :math:`Y` and :math:`X_i` are independent. Therefore, the lower the p-value is,
the higher are the chances that the two considered variables are actually dependent.
In order to discriminate influential inputs from non-influential ones, it is common
practice to fix an acceptance level :math:`\alpha` (typically equal to 0.05, or 0.1),
and to consider all variables associated to a p-value larger than :math:`\alpha`
as being non-influential, and all variables associated to p-values lower than :math:`\alpha`
as having a non-negligible influence on the considered output.

Depending on the size of the available data set, the p-value of a given input variable
can be either computed with an asymptotic estimator, or with a permutation-based estimator.
The asymptotic estimator is used when dealing with sufficiently large data sets, 
and stems from the fact that the considered test statistic :math:`\widehat{S}_{\cT}`
can be approached by a Gamma distribution. As a consequence, the p-value can be approximated
as follows:
 
.. math::

    p_{\textrm{val}} \approx 1 - F_{\textrm{Ga}}\left(n\times\widehat{\mathrm{HSIC}}(X_i,Y)_{\textrm{obs}}\right)

where :math:`F_{\textrm{Ga}}(\cdot)` is the cumulative distribution function of 
the Gamma distribution. The parameters of this distribution are estimated as a 
function of the sample values.

Alternatively, when dealing with small data sets, a permutation-based estimator of
the p-value can be considered. The underlying idea is that under the independence 
hypothesis :math:`\cH_{0,i}`, considering a permutation of the considered output sample
:math:`Y` should have no impact on the estimated HSIC value. We therefore consider
an initial n-size pair of samples :math:`Z_{n} := \left(X_i^{(j)}, Y^{(j)}\right)_{(1\leq j\leq n)}`
and :math:`\widehat{\mathrm{HSIC}}(Z_{n})`. From this samples, we can generate a set of 
B independent permutations :math:`\{\tau_1,\dots,\tau_B\}` of :math:`X^{(j)}_{i_{(1 \leq j \leq n)}}`
and compute the associated HSIC values: 
:math:`\widehat{H}^{*b} := \widehat{\mathrm{HSIC}} \left(X^{(j)}, Y^{(\tau_{b}(j))}\right)_{(1\leq j\leq n)}`.
We can then finally estimate the p-value (under :math:`\cH_{0,i}`) as :

.. math::

    p_{\textrm{val}} \approx \frac{1}{B} \sum_{b=1}^{B} \mathbf{1}_{\widehat{H}^{*b}>\widehat{\mathrm{HSIC}}(X,Y)}


Target sensitivity analysis using HSIC
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

On top of the standard screening and global sensitivity analysis described in the 
previous paragraphs, HSIC also allows to perform target sensitivity analysis.
The underlying concept is to identify the most influential input parameters which
cause the considered output :math:`Y` to cross into a user-defined critical domain:
:math:`\cC`. In practice, rather than directly computing the HSIC values on a given
set of output values :math:`Y^{(j)}_{(1\leq j\leq n)}`, we first apply a transformation
through the use of a filter function :math:`w(\cdot)` : :math:`\tilde{Y^j} = w(Y^j)`.
We can then estimate the target HSIC value associated to the input variable :math:`X_i` as:

.. math::

    \widehat{\mathrm{T-HSIC},i} = \widehat{\mathrm{HSIC}}(X_i,\tilde{Y})

Please note that both the U-statistics a the V-statistics estimators described
in the previous section can be used.

Depending on the application, different filter functions can be considered. 
A first common example of filter function is the exponential function:

.. math::

    w(Y) = \exp (-d_{\cC}(Y) /s)

where :math:`-d_{\cC}(Y)` characterizes the minimum distance between :math:`Y` and
any point contained in the critical domain :math:`\cC`, while :math:`s` is a tunable scale parameter.

Alternatively, we can also consider a step filter function defined as:

.. math::

    w(Y) = \begin{cases}  0 \ \ \mathrm{ if } \ \ Y \notin \cC \\ 1 \ \  \mathrm{ if } \ \ Y \in \cC & \end{cases}

This filter function presents the advantage of being simpler and requiring no parameterization.
However, it also makes no distinction between points being very close to the critical domain
and points which are far from it. This may  partially limit the performance of
the sensitivity analysis, especially when dealing with small data sets. It is
important to note that when considering this step filter function, it is advisable
to rely on a covariance kernel adapted to binary variables (for the considered output), 
such as:

.. math::

    \kappa(\tilde{Y^j},\tilde{Y^k}) = \begin{cases}  1/n_z \ \ & \mathrm{ if } \ \ \tilde{Y^j} = \tilde{Y^k} \\ 0 \ \  & \mathrm{ if } \ \ \tilde{Y^j} \neq \tilde{Y^k} \end{cases}

where :math:`n_z` is the number of samples in the available data set belonging to 
the same category as  :math:`\tilde{Y^j} \ \mathrm{ and } \ \tilde{Y^k}`.
Please note that this specific kernel can also be used when performing sensitivity
analysis on discrete variables.

Conditional sensitivity analysis using HSIC
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Similarly to the target sensitivity analysis discussed in the previous paragraph,
the HSIC also allows the possibility of performing conditional sensitivity analysis.
In this case, the objective is to identify the most influential input variables under
the condition that the considered output variable is within a user-defined critical domain.
In other words, we are interested in identifying the variables that drive the
output variability within the critical domain.
This analysis can be achieved by relying on a diagonal weight matrix computed through
the use of a weight function :math:`w(\cdot)` on the considered data set:
:math:`W_{j,j} = w(Y^j)`. The underlying purpose of this matrix is to associate to
each sample in the data set a weight characterizing its distance from the critical domain.
Different definitions of the weight function can be considered. For instance, the exponential
and step weight functions defined in the previous paragraph can be used.

Having defined a proper weight function, the conditional HSIC values can be computed
by relying on an adapted V-statistics estimator:

.. math::

    \widehat{\mathrm{C-HSIC}} (X_i,Y) = \frac{1}{n^2} \mathrm{Tr} (\hat{W} L_i \hat{W} H_1 L H_2)

where :math:`\hat{W} = \frac{W}{\frac{1}{n}\sum_{j = 1}^{n} W_{j,j} }`, :math:`H_1 = I_n - \frac{1}{n} U\hat{W}`
and :math:`H_2 = I_n - \frac{1}{n} \hat{W}U`.

Please note that no U-statistics estimator exists for the conditional HSIC. Furhtermore,
differently than in the target analysis case, standard continuous covariance kernels
can be used, regardless of the type of weight function that is being considered.

In most applications, it may be worth performing all three types of sensitivity 
analysis presented in the previous paragaph, i.e., global, target and conditional,
in order to gain a more precise understanding of the degree and type of influence 
of every input variable.

.. topic:: API:

    - See :class:`~openturns.HSICEstimatorGlobalSensitivity` for global sensitivity analysis HSIC estimators
    - See :class:`~openturns.HSICEstimatorTargetSensitivity` for target sensitivity analysis HSIC estimators
    - See :class:`~openturns.HSICEstimatorConditionalSensitivity` for conditional sensitivity analysis HSIC estimators
    - See :class:`~openturns.HSICUStat` for U-statistic specific HSIC computations
    - See :class:`~openturns.HSICVStat` for V-statistic specific HSIC computations


.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/sensitivity_analysis/plot_hsic_estimators_ishigami`

.. topic:: References:

    - [gretton2005]_
    - [daveiga2015]_
    - [marrel2021]_
