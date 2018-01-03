Sparse least squares polynomial metamodel
-----------------------------------------

| The response of the model under consideration may be globally
  approximated by a multivariate polynomial. In this setup, the response
  is characterized by a finite number of coefficients which have to be
  estimated. This may be achieved by least squares. However, this
  method cannot be applied if the number of unknown coefficients is of
  similar size to the number of model evaluations, or even possibly
  greater. Indeed, the resulting underdetermined least squares problem
  would be ill-posed.
| To overcome this difficulty, *sparse least squares* schemes may be
  employed (they are also known as *variable selection* techniques in
  statistics). These methods are aimed at identifying a small set of
  significant coefficients in the model response approximation.
  Eventually, a *sparse* polynomial response surface, i.e. a polynomial
  which only contains a low number of non-zero coefficients, is obtained
  by means of a reduced number of possibly costly model evaluations. In
  the sequel, we focus on sparse regression schemes based on
  :math:`L_1`-penalization.
| Actually the proposed approaches do not provide only one
  approximation, but a *collection* of less and less sparse
  approximations. Eventually an optimal approximation may be retained
  with regard to a given criterion.

**Context**

Consider the mathematical model :math:`h` of a physical system depending
on :math:`n_X` input parameters
:math:`\underline{x} = (x_{1},\dots,x_{n_X})^{\textsf{T}}`. Note that
these input variables are assumed to be deterministic in this section.
The model response may be approximated by a polynomial as follows:

.. math::
  :label: 5-2.1

    h(\underline{X}) \, \, \approx \, \, \widehat{h}(\underline{x}) \, \, = \, \, \sum_{j=0}^{P-1} \; a_{j} \; \psi_{j}(\underline{x})

| Let us consider a set of values taken by the input vector (i.e. an
  experimental design)
  :math:`\underline{\underline{\cX}} = (\underline{x}^{(1)},\dots,\underline{x}^{(N)})^{\textsf{T}}`
  as well as the vector
  :math:`\underline{\cY} = (h(\underline{x}^{(1)}),\dots,h(\underline{x}^{(N)}))^{\textsf{T}} =  (y^{(1)},\dots,y^{(N)})^{\textsf{T}}`
  of the corresponding model evaluations. It is assumed that the number
  of terms :math:`P` in the polynomial basis is of similar size to
  :math:`N`, or even possibly significantly larger than :math:`N`. In
  such a situation it is not possible to compute the polynomial
  coefficients by ordinary least squares, since the corresponding system
  is ill-posed. Methods that may be employed as an alternative are
  outlined in the sequel.
| **LASSO**

| The so-called *LASSO* (for *Least absolute shrinkage and selection
  operator*) method is based on a :math:`\cL^{1}`-penalized regression
  as follows:

  .. math::
    :label: 5-2.5

      \textrm{Minimize} \quad \quad \sum_{i=1}^{N} \; \left( h(\underline{x}^{(i)}) \; - \; \underline{a}^{\textsf{T}} \underline{\psi}(\underline{x}^{(i)})  \right)^{2}
         \, + \,  C \; \|\underline{a}\|_{1}^{2}

  where :math:`\|\underline{a}\|_{1} = \sum_{j=0}^{P-1} |a_{j}|` and
  :math:`C` is a non negative constant. A nice feature of LASSO is that
  it provides a *sparse* metamodel, i.e. it discards insignificant
  variables from the set of predictors. The obtained response surface is
  all the sparser since the value of the tuning parameter :math:`C` is
  high.

| For a given :math:`C\geq 0`, the solving procedure may be implemented
  via quadratic programming. Obtaining the whole set of coefficient
  estimates for :math:`C` varying from 0 to a maximum value may be
  computationally expensive though since it requires solving the
  optimization problem for a fine grid of values of :math:`C`.
| **Forward stagewise regression**

Another procedure, known as *forward stagewise regression*, appears to
be different from LASSO, but turns out to provide similar results. The
procedure first picks the predictor that is most correlated with the
vector of observations. However, the value of the corresponding
coefficient is only increased by a small amount. Then the predictor with
largest correlation with the current residual (possible the same term as
in the previous step) is picked, and so on. Let us introduce the vector
notation:

.. math:: \underline{\psi}_{j} \, \, = \, \, (\psi_{j}(\underline{x}^{(1)}) , \dots, \psi_{j}(\underline{x}^{(N)}) )^{\textsf{T}}

The forward stagewise algorithm is outlined below:

#. Start with :math:`\underline{R} = \cY` and
   :math:`a_{0} = \dots = a_{P-1} = 0`.

#. Find the predictor :math:`\underline{\psi}_{j}` that is most
   correlated with :math:`\underline{R}`.

#. Update :math:`\hat{a}_{j} = \hat{a}_{j} + \delta_{j}`, where
   :math:`\delta_{j} = \varepsilon \cdot \mbox{sign}(\underline{\psi}_{j}^{\textsf{T}} \underline{R} )`.

#. Update
   :math:`\underline{R} =  \underline{R} - \delta_{j} \underline{\psi}_{j}`
   and repeats Steps 2 and 3 until no predictor has any correlation with
   :math:`\underline{R}`.

| Note that parameter :math:`\varepsilon` has to be set equal to a small
  value in practice, say :math:`\varepsilon=0.01`. This procedure is
  known to be more stable than traditional stepwise regression.
| **Least Angle Regression**

*Least Angle Regression* (LAR) may be viewed as a version of forward
stagewise that uses mathematical derivations to speed up the
computations. Indeed, instead of taking many small steps with the basis
term most correlated with current residual :math:`\underline{R}`, the
related coefficient is directly increased up to the point where some
other basis predictor has as much correlation with
:math:`\underline{R}`. Then the new predictor is entered, and the
procedure is continued. The LAR algorithm is detailed below:

#. Initialize the coefficients to :math:`a_{0},\dots,a_{P-1} = 0`. Set
   the initial residual equal to the vector of observations :math:`\cY`.

#. Find the vector :math:`\underline{\psi}_{j}` which is most correlated
   with the current residual.

#. Move :math:`a_{j}` from 0 toward the least-square coefficient of the
   current residual on :math:`\underline{\psi}_{j}`, until some other
   predictor :math:`\underline{\psi}_{k}` has as much correlation with
   the current residual as does :math:`\underline{\psi}_{j}`.

#. Move jointly :math:`(a_{j} , a_{k})^{\textsf{T}}` in the direction
   defined by their joint least-square coefficient of the current
   residual on :math:`(\underline{\psi}_{j},\underline{\psi}_{k})`,
   until some other predictor :math:`\underline{\psi}_{l}` has as much
   correlation with the current residual.

#. Continue this way until :math:`m = \min(P,N-1)` predictors have been
   entered.

| Steps 2 and 3 correspond to a “move” of the *active* coefficients
  toward their least-square value. It corresponds to an updating of the
  form
  :math:`\hat{\underline{a}}^{(k+1)} = \hat{\underline{a}}^{(k)} + \gamma^{(k)} \tilde{\underline{w}}^{(k)}`.
  Vector :math:`\tilde{\underline{w}}^{(k)}` and coefficient
  :math:`\gamma^{(k)}` are referred to as the LAR *descent direction*
  and *step*, respectively. Both quantities may be derived
  algebraically.
| Note that if :math:`N \geq P`, then the last step of LAR provides the
  ordinary least-square solution. It is shown that LAR is noticeably
  efficient since it only requires the computational cost of ordinary
  least-square regression on :math:`P` predictors for producing a
  *collection* of :math:`m` metamodels.
| **LASSO and Forward Stagewise as variants of LAR**

It has been shown that with only one modification, the LAR procedure
provides in one shot the entire paths of LASSO solution coefficients as
the tuning parameter :math:`C` in Eq.([eq:5-2.5]) is increased from 0 up
to a maximum value. The modified algorithm is as follows:

-  Run the LAR procedure from Steps 1 to 4.

-  If a non zero coefficient hits zero, discard it from the current
   metamodel and recompute the current joint least-square direction.

-  Continue this way until :math:`m = \min(P,N-1)` predictors have been
   entered.

| Note that the LAR-based LASSO procedure may take more than :math:`m`
  steps since the predictors are allowed to be discarded and introduced
  later again into the metamodel. In a similar fashion, a limiting
  version of the forward stagewise method when :math:`\varepsilon \to 0`
  may be obtained by slightly modifying the original LAR algorithm. In
  the literature, one commonly uses the label LARS when referring to all
  these LAR-based algorithms (with *S* referring to *Stagewise* and
  *LASSO*).
| **Selection of the optimal LAR solution**

The LAR-based approaches (i.e. original LAR, LASSO and forward
stagewise) provide a *collection* of less and less sparse PC
approximations. The accuracy of each PC metamodel may be assessed by
cross validation. Eventually the PC representation associated with the
lowest error estimate is retained.


.. topic:: API:

    - See :class:`~openturns.LARS`
    - See :class:`~openturns.FunctionalChaosAlgorithm`

.. topic:: Examples:

    - See :doc:`/examples/meta_modeling/fieldfunction_metamodel`


.. topic:: References:

    - B. Efron, T. Hastie, I. Johnstone, and R. Tibshirani, 2004, "Least angle regression", Annals of Statistics 32, 407--499.
    - T. Hastie, J. Taylor, R. Tibshirani, and G. Walther, 2007, "Forward stagewise regression and the monotone Lasso", Electronic J. Stat. 1, 1--29.

