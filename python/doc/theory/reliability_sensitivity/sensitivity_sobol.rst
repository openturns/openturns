.. _sensitivity_sobol:

Sensitivity analysis using Sobol' indices
-----------------------------------------

Consider the input random vector
:math:`\vect{X} = \left( X_1,\ldots,X_{n_X} \right)`
and let :math:`\vect{Y} = \left( Y_1,\ldots,Y_{n_Y} \right)`
be the output of the physical model:

.. math::
    \vect{Y} = \operatorname{g}(\vect{X}).

We consider the output :math:`Y_k` for any index :math:`k \in \{1, \ldots, n_Y\}`.
Sobol' indices measure the influence of the input :math:`\vect{X}` 
to the output :math:`Y_k`.
The method considers the part of the variance of the output :math:`Y_k` produced by
the different inputs :math:`X_i`.

In the first part of this document, we introduce the Sobol' indices of a 
scalar output :math:`Y_k`.
Hence, the model is simplified to:

.. math::
    Y = \operatorname{g}(\vect{X}).

In the second part of the document, we consider the general case where the 
output is multivariate. 
In this case, aggregated Sobol' indices can be used [gamboa2013]_.

The Sobol' decomposition is described more easily when the domain of the input
is the unit interval :math:`[0,1]^{n_X}`.
It can be easily extended to any input domain using expectations, variances
and variance of conditional expectations.

We assume that the input marginal variables :math:`X_1,\ldots,X_{n_X}` are
independent.
This restrictive hypothesis implies that the only copula of the input 
random vector :math:`\bdX` for which the
Sobol' indices are easy to interpret is the independent copula.
If the input variables are dependent, then the Sobol' indices can be defined,
but some of their properties are lost.

Partition of the input
~~~~~~~~~~~~~~~~~~~~~~

For any  :math:`i\in\{1,\ldots, n_X\}`, let :math:`\bdx_{\overline{\{i\}}} \in [0,1]^{n_X - 1}` be
the vector made of components of :math:`\bdx=(x_1,x_2,` :math:`\ldots,x_p)\in [0,1]^{n_X }` which
indices are different from :math:`i`.
Hence, if :math:`\bdx\in[0,1]^{n_X}`, then:

.. math::
    \bdx_{\overline{\{i\}}} = (x_1,x_2,\ldots,x_{i-1},x_{i+1},\ldots,x_p)^T\in [0,1]^{n_X - 1}.

Consider the function :math:`\operatorname{g}` defined by the equation:

.. math::
    y = \operatorname{g}(\bdx)

where :math:`\bdx=(x_1,\ldots,x_p)^T \in [0,1]^{n_X}`.
With this notation, we can partition the input of :math:`g`:

.. math::
    \operatorname{g}(\bdx) = \operatorname{g} \left(x_i,\bdx_{\overline{\{i\}}} \right).

The goal of sensitivity analysis is to measure the sensitivity of the variance 
of the output :math:`Y` depending on the variable :math:`X_i`. 
This may take into account the dependence of the output to the interactions
of :math:`X_i` and :math:`\bdX_{\overline{\{i\}}}` through the function :math:`g`. 

More generally, let :math:`\bdu \subseteq \{1,2,\ldots,n_X\}` be a group of 
variables.
Therefore:

.. math::
    \operatorname{g}(\bdx) = \operatorname{g} \left(\bdx_\bdu,\bdx_{\overline{\bdu}} \right).

The goal of sensitivity analysis is to measure the sensitivity of the variance 
of the output :math:`Y` depending on the group of variables :math:`\bdX_\bdu`. 
This may take into account the dependence of the output to the interactions
of :math:`\bdX_\bdu` and :math:`\bdX_{\bdu}` through the function :math:`g`. 


Sobol' decomposition
~~~~~~~~~~~~~~~~~~~~

In this section, we introduce the Sobol'-Hoeffding decomposition [sobol1993]_.
If :math:`\operatorname{g}` can be integrated in :math:`[0,1]^{n_X}`, then there is a unique
decomposition:

.. math::
    y &= h_0 + \sum_{i=1,2,\ldots,n_X} h_{\{i\}}(x_i)
         \quad + \sum_{1\leq i < j \leq n_X} h_{\{i,j\}}(x_i,x_j) \nonumber \\
      & \quad+ \ldots +
             h_{\{1,2,\ldots,n_X\}}(x_1,x_2,\ldots,x_p),

where :math:`h_0` is a constant and the functions of the decomposition satisfy the equalities:

.. math::
    \int_0^1 h_{\{i_1,\ldots,i_s\}}(x_{i_1},\ldots,x_{i_s})dx_{i_k} = 0,

for any :math:`k=1,2,\ldots,s` and any indices :math:`1\leq i_1< i_2< \ldots< i_s\leq n_X` and
:math:`s=1,2,\ldots,n_X`.

Extension to any input distribution with independent marginals
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this section, we extend the previous definitions to an input random vector
that is not necessarily defined on the input unit cube :math:`[0,1]^{n_X}`.
To do this, we define the functions :math:`h_\bdu` using conditional
expectations.

The functions :math:`h_\bdu` satisfy the equality:

.. math::
    \int_{[0,1]^{|\overline{\bdu}|}} \operatorname{g}(\bdx) d\bdx_{\overline{\bdu}}
    = \sum_{\bdv \subseteq \bdu} h_\bdv(\bdx_\bdv),

for any group of variables :math:`\bdu \subseteq \{1,2,\ldots,n_X\}` with
size lower or equal to :math:`n_X`, where :math:`|\overline{\bdu}|` is the
cardinal of the subset :math:`\overline{\bdu}`.
The functions :math:`h_\bdu` can be defined recursively, using groups of
variables of lower dimensionality:

.. math::
    h_\bdu(\bdx_\bdu)
    = \int_{[0,1]^{|\overline{\bdu}|}} \operatorname{g}(\bdx_\bdu,\bdx_{\overline{\bdu}}) d\bdx_{\overline{\bdu}}
    -  \sum_{\bdv \subsetneq \bdu} h_\bdv(\bdx_\bdv)

where :math:`\subsetneq` denotes a proper subset. 
Let :math:`\boldsymbol{x} \in [0,1]^{n_X}` be a point and
let :math:`\bdu \subseteq \{1, \ldots, n_X\}` be a group of variables.
Therefore:

.. math::
    \Expect{Y|\bdX_\bdu=\bdx_\bdu}
    = \sum_{\bdv \subseteq \bdu} h_\bdv(\bdx_\bdv).

The Möbius inversion formula implies (see [daveiga2022]_ Theorem 3.3 page 49):
.. math::
    h_\bdu(\bdx_\bdu)
    = \sum_{\bdv \subseteq \bdu} (-1)^{|\bdu| - |\bdv|} \Expect{Y|\bdX_\bdv=\bdx_\bdv}.

The previous equation is a consequence of the Möbius inversion formula [rota1964]_
(also called the exclusion-inclusion principle).

Decomposition of the variance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The variance of the function :math:`\operatorname{g}` can be
decomposed into:

.. math::
    \Var{Y}=\sum_{i=1}^{n_X} V_{i}
    + \sum_{1\leq i < j\leq n_X} V_{\{i,j\}} + \ldots + V_{\{1,2,\ldots,n_X\}}

where the interaction variances are:

.. math::
    V_{i}        &= \Var{h_{\{i\}}(X_i)}, \label{eq-sde-varvi1-2} \\
    V_{\{i,j\}}  &= \Var{h_{\{i, j\}}(X_i,X_j)}, \\
    V_{\{i,j,\}} &= \Var{h_{i,j,k}(X_i,X_j,k)}, \\
    \ldots       & \\
    V_{\{1,2,\ldots,n_X\}} &= \Var{h_{\{1,2,\ldots,n_X\}}(X_1,X_2,\ldots,X_p)}.

More generally, the interaction variance of a group of variables is:

.. math::
    V_\bdu = \Var{h_\bdu(\bdx_\bdu)},

for any :math:`\bdu \subseteq \{1,2,\ldots,n_X\}`.
Using the Hoeffding decomposition, we get:

.. math::

   \Var{Y} = \sum_{ \bdu \subseteq \{1, \ldots, n_X\} } V_\bdu.

The Möbius inversion formula implies (see [daveiga2022]_ corrollary 3.5 page 52):

.. math::

    V_\bdu = \sum_{\bdv \subseteq \bdu} (-1)^{ |\bdu| - |\bdv| } \Var{\Expect{ Y \vert \mat{X}_\bdv} }.

Interaction sensitivity indices of a variable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first order interaction sensitivity indices are equal to:

.. math::

    S_i           &= \frac{V_{i}}{\Var{Y}} , \\
    S_{\{i,j\}}   &= \frac{V_{\{i,j\}}}{\Var{Y}} , \\
    S_{\{i,j,k\}} &= \frac{V_{\{i,j,k\}}}{\Var{Y}} , \\
    \ldots & \\
    S_{\{i_1,i_2,\ldots,i_s\}} &= \frac{V_{\{i_1,i_2,\ldots,i_s\}}}{\Var{Y}}, \\
    \ldots & \\
    S_{\{1,2,\ldots,p\}} &= \frac{V_{\{1,2,\ldots,p\}}}{\Var{Y}}.

The first order Sobol' index :math:`S_i` measures the part of the variance of :math:`Y` explained by :math:`X_i` alone.
The second order Sobol' index :math:`S_{i,j}`  measures the part of the variance of :math:`Y` explained by the interaction of :math:`X_i` and :math:`X_j`.

More generally, the first order interaction Sobol' index of a group
of variables :math:`\bdu` is:

.. math::
    S_\bdu = \frac{V_\bdu}{\Var{Y}} = \frac{\Var{h_\bdu(\bdX_\bdu)}}{\Var{Y}}.

where :math:`h_\bdu` is the function of the input variables in the group :math:`\bdu`
of the functional Sobol'-Hoeffding ANOVA decomposition of the physical model.
This index measures the sensitivity of the variance of the output explained
by interactions within the group.

The total interaction sensitivity index of the group :math:`\bdu` 
is (see (Liu & Owen, 2006)):

.. math::
    S^{T,i}_\bdu = \sum_{\bdv \supseteq \bdu} S_{\bdv}

This index measures the sensitivity of the variance of the output explained
by interactions within the group and groups of variables containing it.

First order and total sensitivity indices of a variable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first order Sobol' sensitivity index is equal to the corresponding
interaction index of the group :math:`\{i\}`:

.. math::
    S_i &= S_{\{i\}}

for :math:`i=1,\ldots, n_X`.
The first order Sobol' index :math:`S_i` measures the sensitivity of the 
output variance explained by the effect of :math:`X_i` alone.
We can alternatively define the first order Sobol' sensitivity index using
the variance of a conditional expectation.
The first order Sobol' sensitivity index satisfies the equation:

.. math::
    S_i &= \frac{\Var{\Expect{Y|X_i}}}{\Var{Y}}

for :math:`i=1,\ldots, n_X`.

The total  Sobol' sensitivity index is:

.. math::
    S^T_i &= \frac{V_{i} + \sum_{\substack{j\in\{1,\ldots, n_X\}\\j\neq i}} V_{\{i,j\}} + \ldots
    V_{1, 2,\ldots, n_X}}{\Var{Y}}

for :math:`i=1,\ldots, n_X`.
The total Sobol' sensitivity index can be equivalently defined in terms
of the variance of a conditional expectation.
The total  Sobol' sensitivity index satisfies the equation:

.. math::
    S^T_i &= 1 - \frac{\Var{\Expect{Y|X_{\overline{\{i\}}}}}}{\Var{Y}}

for :math:`i=1,\ldots, n_X`.
For any :math:`i=1,\ldots,n_X`, let us define

.. math::

    V_i^T   & = \sum_{\bdu \ni i} V_\bdu \\
    V_{-i} & = \Var{ \Expect{Y \vert X_1, \ldots, X_{i-1}, X_{i+1}, \ldots X_{n_X}} }.

Total order Sobol' indices satisfy the equality:

.. math::

    S_i^T = \frac{V_i^T}{\Var{Y}} = 1 - \frac{V_{-i}}{\Var{Y}}

for :math:`i=1,\ldots,n_X`.

The total order Sobol' index :math:`S_i^T` measures the part of the variance
of :math:`Y` explained by :math:`X_i`
and its interactions with other input variables.
It can also be viewed as the part of the variance of :math:`Y` that cannot
be explained without :math:`X_i`.


Sensitivity indices of a group of variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`\bdu \subseteq \{1, \ldots, n_X\}` be a group of input variables.
The first order (closed) Sobol' index of a group of input variables :math:`\bdu`
is:

.. math::
    S_{\bdu}^{\operatorname{cl}}
    = \frac{\Var{\Expect{Y|\vect{X}_{\bdu}}}}{\Var{Y}}

The first order closed Sobol' index of a group of input variables :math:`\bdu`
measures the sensitivity of the variance of :math:`Y` explained by the
variables within the group.
This index is useful when the group contains random variables parameterizing 
a single uncertainty source (see (Knio & Le Maitre, 2006) page 139).

The total Sobol' index of a group of variables :math:`\bdu`
is:

.. math::
    S^T_\bdu
    = \frac{\sum_{\bdv\cap\bdu\neq\emptyset} \Var{h_\bdv(\bdX_\bdv)}}{\Var{Y}}

where :math:`h_\bdv` is the function of the variables in the group :math:`\bdv`
of the functional Sobol'-Hoeffding ANOVA decomposition of the physical model.
The total Sobol' index of a group of input variables :math:`\bdu`
measures the sensitivity of the variance of :math:`Y` explained by the
variables within the group and any group of variables containing any variable in the 
group.
It can also be viewed as the part of the variance of :math:`Y` that cannot
be explained without :math:`X_\bdu`.


For any group of variables :math:`\bdu`,
the total and first order (closed) Sobol' indices are related by the equation:

.. math::
    S^T_\bdu + S_{\overline{\bdu}}^{\operatorname{cl}} = 1

where :math:`\overline{\bdu}` is the complementary group of :math:`\bdu`.

Summary of Sobol' indices
~~~~~~~~~~~~~~~~~~~~~~~~~

The next table presents a summary of the 6 different Sobol' indices that
we have presented.

+--------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
| Single variable or group             | Sensitivity Index    | Formula                                                                                                                  |
+======================================+======================+==========================================================================================================================+
| One single variable :math:`i`        | First order          | :math:`S_i = \frac{\Var{\Expect{Y|X_i}}}{\Var{Y}}= \frac{V_i}{\Var{Y}}`                                                  |
+--------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
|                                      | Total                | :math:`S^T_i = \sum_{\bdu \ni i} S_\bdu = 1 - S_{\overline{\{i\}}}^{\operatorname{cl}}`                                  |
+--------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
| Interaction of a group :math:`\bdu`  | First order          | :math:`S_\bdu = \frac{V_\bdu}{\Var{Y}}`                                                                                  |
+--------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
|                                      | Total interaction    | :math:`S^{T,i}_\bdu = \sum_{\bdv \supseteq \bdu} S_{\bdv}`                                                               |
+--------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
| Group (closed) :math:`\bdu`          | First order closed   | :math:`S_\bdu^{\operatorname{cl}} = \frac{\Var{\Expect{Y|\bdX_\bdu}}}{\Var{Y}} = \sum_{\bdv \subseteq \bdu} S_\bdv`      |
+--------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+
|                                      | Total                | :math:`S^T_\bdu = \frac{\sum_{\bdv\cap\bdu\neq\emptyset} V_\bdv}{\Var{Y}} = 1 - S_{\overline{\bdu}}^{\operatorname{cl}}` |
+--------------------------------------+----------------------+--------------------------------------------------------------------------------------------------------------------------+

**Table 1.** First order and total Sobol' indices of a single variable :math:`i` or a group :math:`\bdu`.

Let us summarize the properties of the Sobol' indices.

- All these indices are in the :math:`[0, 1]` interval.

- The sum of interaction first order Sobol' indices is equal to 1:

.. math::
    \sum_{\bdu \subseteq \{1,2,\ldots,n_X\}} S_\bdu = 1.

- Each first order index is lower than its total counterpart:

.. math::
    S_\bdu & \leq S^{T,i}_\bdu \\
    S_i & \leq S^T_i \\
    S_\bdu^{\operatorname{cl}} & \leq S^T_\bdu

- If :math:`S_i < S^T_i`, there are interactions between the variable :math:`X_i` and other variables.

- If :math:`S_i = S^T_i` for :math:`i = 1, \ldots, n_X`, then the function is additive, i.e.
  the function :math:`g` is the sum of functions :math:`g_1, \ldots, g_{n_X}` of input dimension 1:

.. math::
    Y = \sum_{i = 1}^{n_X} g_i(X_i).

Example
~~~~~~~

Let us consider a function :math:`g` which has :math:`n_X = 3` inputs
:math:`(X_1, X_2, X_3)`.
The full set of interaction indices is:

.. math::
    S_1, \; S_2, \; S_3, \; S_{\{1, 2\}}, \; S_{\{1, 3\}}, \; S_{\{2, 3\}},
    \; S_{\{1, 2, 3\}}.

Each Sobol' index combines a subset of the previous interaction indices.
For example, the first and total Sobol' indices are presented in the next
table.

+-----------------------+-----------------------+-----------------------------------------------------+
| Variable              | First order           | Total                                               |
+=======================+=======================+=====================================================+
| :math:`X_1`           | :math:`S_1`           | :math:`S_1^T = S_1 + S_{1,2} + S_{1,3} + S_{1,2,3}` |
+-----------------------+-----------------------+-----------------------------------------------------+
| :math:`X_2`           | :math:`S_2`           | :math:`S_2^T = S_2 + S_{1,2} + S_{2,3} + S_{1,2,3}` |
+-----------------------+-----------------------+-----------------------------------------------------+
| :math:`X_3`           | :math:`S_3`           | :math:`S_3^T = S_3 + S_{1,3} + S_{2,3} + S_{1,2,3}` |
+-----------------------+-----------------------+-----------------------------------------------------+

**Table 2.** First order and total Sobol' indices of the variables :math:`X_1`, :math:`X_2` and :math:`X_3`.

The list of possible groups is :math:`\{1,2\}`, :math:`\{1,3\}`, :math:`\{2,3\}` and  :math:`\{1,2,3\}`.
The next table presents the Sobol' indices of the group :math:`\bdu = \{1, 2\}`.

+-----------------------------------------------+------------------------------------------------------------------------+
| Sobol' index of group :math:`\bdu = \{1, 2\}` | Value                                                                  |
+===============================================+========================================================================+
| Group interaction                             | :math:`S_{\{1,2\}}`                                                    |
+-----------------------------------------------+------------------------------------------------------------------------+
| Group total interaction                       | :math:`S_{\{1,2\}}^{T,i} = S_{\{1,2\}} + S_{\{1, 2, 3\}}`              |
+-----------------------------------------------+------------------------------------------------------------------------+
| Group first order (closed)                    | :math:`S_{\{1,2\}}^{\operatorname{cl}} = S_{1} + S_{2} + S_{\{1, 2\}}` |
+-----------------------------------------------+------------------------------------------------------------------------+
| Group total                                   | :math:`S_{\{1,2\}}^T = S_1 + S_2 + S_{\{1, 2\}} + S_{\{1, 2, 3\}}`     |
+-----------------------------------------------+------------------------------------------------------------------------+

**Table 3.** Sobol' indices of the group :math:`\bdu = \{1, 2\}`.

Aggregated Sobol' indices
~~~~~~~~~~~~~~~~~~~~~~~~~

For multivariate outputs i.e. when :math:`n_Y>1`, the Sobol'
indices can be aggregated [gamboa2013]_.
Let :math:`V_i^{(k)}` be the (first order) variance of the conditional
expectation of the k-th output :math:`Y^{(k)}`:

.. math::

    V_i^{(k)} & = \Var{ \Expect{Y^{(k)} \vert X_i} }

for :math:`i=1,\ldots,n_X` and :math:`k=1,\ldots,n_Y`.
Similarly, let :math:`V_i^{(T, k)}` be the total variance of the conditional expectation
of :math:`Y^{(k)}` for :math:`i = 1, \ldots, n_X` and :math:`k = 1, \ldots, n_Y`.

The indices can be aggregated with the following formulas:

.. math::

    S_i^{(a)}  & =  \frac{ \sum_{k=1}^{n_Y} V_{i}^{(k)} }{ \sum_{k=1}^{n_Y} \Var{Y_k} }  \\
    S_i^{(T, a)} & =  \frac{ \sum_{k=1}^{n_Y} VT_{i}^{(k)} }{ \sum_{k=1}^{n_Y} \Var{Y_k} }

for :math:`i=1,\ldots,n_X`.

Estimators
~~~~~~~~~~
To estimate these quantities,
Sobol' proposes to use numerical methods that rely on two independent realizations of the random vector :math:`\vect{X}`.
This is known as the pick-freeze estimator.

Let :math:`N \in \Nset` be the size of each sample.
Let :math:`\mat{A}` and :math:`\mat{B}` be two independent samples of size :math:`N`
of :math:`\vect{X}`:

.. math::

   \mat{A} = \left(
   \begin{array}{cccc}
   a_{1,1} & a_{1,2} & \cdots & a_{1, n_X} \\
   a_{2,1} & a_{2,2} & \cdots & a_{2, n_X} \\
   \vdots  & \vdots  & \ddots  & \vdots \\
   a_{N,1} & a_{1,2} & \cdots & a_{N, n_X}
   \end{array}
   \right), \  \mat{B} = \left(
   \begin{array}{cccc}
   b_{1,1} & b_{1,2} & \cdots & b_{1, n_X} \\
   b_{2,1} & b_{2,2} & \cdots & b_{2, n_X} \\
   \vdots  & \vdots  & \vdots  & \vdots \\
   b_{N,1} & b_{1,2} & \cdots & b_{N, n_X}
   \end{array}
   \right)

Each line is a realization of the random vector :math:`\vect{X}`.

We are now going to mix these two samples to get an estimate of the sensitivity indices.

.. math::

   \mat{E}^i = \left(
   \begin{array}{cccccc}
   a_{1,1} & a_{1,2} & \cdots & b_{1,i} & \cdots & a_{1, n_X} \\
   a_{2,1} & a_{2,2} & \cdots & b_{2,i} & \cdots & a_{2, n_X} \\
   \vdots  & \vdots  &        & \vdots  & \ddots & \vdots \\
   a_{N,1} & a_{1,2} & \cdots & b_{N,i} & \cdots & a_{N, n_X}
   \end{array}
   \right), \;
   \mat{C}^i = \left(
   \begin{array}{cccccc}
   b_{1,1} & b_{1,2} & \cdots & a_{1,i} & \cdots & b_{1, n_X} \\
   b_{2,1} & b_{2,2} & \cdots & a_{2,i} & \cdots & b_{2, n_X} \\
   \vdots  & \vdots  &        & \vdots  & \ddots  & \vdots \\
   b_{N,1} & b_{1,2} & \cdots & a_{N,i} & \cdots & b_{N, n_X}
   \end{array}
   \right)

Several estimators of :math:`V_i`, :math:`V_i^T` and :math:`V_{-i}` are provided by the :class:`SobolIndicesAlgorithm` implementations:

- :class:`~openturns.SaltelliSensitivityAlgorithm` for the `Saltelli` method,
- :class:`~openturns.JansenSensitivityAlgorithm` for the `Jansen` method,
- :class:`~openturns.MauntzKucherenkoSensitivityAlgorithm` for the `Mauntz-Kucherenko` method,
- :class:`~openturns.MartinezSensitivityAlgorithm` for the `Martinez` method.

Specific formulas for :math:`\widehat{V}_i`, :math:`\widehat{VT}_i` and :math:`\widehat{V}_{-i}` are given in the corresponding documentation pages.

The estimator :math:`\widehat{V}_{i,j}` of :math:`V_{i,j}` is the same for all these classes:

.. math::

    \widehat{V}_{i,j} = \frac{1}{N-1} \sum_{k=1}^{N} \tilde{g}(\vect{E}_k^i) \tilde{g}(\vect{C}_k^j) - \frac{1}{N} \sum_{k=1}^{N} \tilde{g}(\vect{A}_k) \tilde{g}(\vect{B}_k) - \widehat{V}_i - \widehat{V}_j.

Notice that the value of the second order conditional variance depends on the estimators :math:`\widehat{V}_i` and :math:`\widehat{V}_j` which are method-dependent. This implies that the value of the second order indices may depend on the specific Sobol' estimator we use.

Centering the output
~~~~~~~~~~~~~~~~~~~~

For the sake of stability, computations are performed with centered output.
Let :math:`\overline{\vect{g}}` be the mean of the combined samples
:math:`\vect{g}(\mat{A})` and :math:`\vect{g}(\mat{B})`.
Let :math:`\tilde{\vect{g}}` be the empirically centered function defined,
for any :math:`\vect{x} \in \Rset^{n_X}`, by:

.. math::

   \tilde{\vect{g}}(\vect{x}) = \vect{g}(\vect{x}) - \overline{\vect{g}}.

To estimate the total variance :math:`\Var{Y}`,
we use the :meth:`~openturns.Sample.computeVariance` method of the :class:`~openturns.Sample` :math:`\tilde{g}(\mat{A})`.

.. topic:: API:

    - See :class:`~openturns.SobolIndicesAlgorithm` for indices based on sampling
    - See :class:`~openturns.FunctionalChaosSobolIndices` for indices based on chaos expansion

.. topic:: Examples:

    - See :doc:`/auto_reliability_sensitivity/sensitivity_analysis/plot_sensitivity_sobol`
    - See :doc:`/auto_reliability_sensitivity/sensitivity_analysis/plot_functional_chaos_sensitivity`

.. topic:: References:

    - [saltelli2002]_
    - [daveiga2022]_
    - [sobol1993]_
    - Knio, O. M., & Le Maitre, O. P. (2006). Uncertainty propagation in CFD using polynomial chaos decomposition. *Fluid dynamics research*, *38* (9), 616.
    - Liu, R., & Owen, A. B. (2006). Estimating mean dimensionality of analysis of variance decompositions. *Journal of the American Statistical Association*, 101(474), 712-721.
    - Knio, O. M., & Le Maitre, O. P. (2006). Uncertainty propagation in CFD using polynomial chaos decomposition. *Fluid dynamics research*, *38* (9), 616.
