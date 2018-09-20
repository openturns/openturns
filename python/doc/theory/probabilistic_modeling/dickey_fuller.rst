.. _dickey_fuller:

Dickey-Fuller stationarity test
===============================

The Dickey-Fuller test checks the stationarity of a scalar time series using one time series. It assumes that the :math:`X: \Omega \times \cD \rightarrow \Rset` process with :math:`\cD \in \Rset`, discretized on the time grid :math:`(t_0, \dots, t_{N-1})` writes:

.. math::
    :label: DFmodel

    X_t = a + bt + \rho X_{t-1} + \varepsilon_{t}

where :math:`\rho > 0` and where :math:`a` or :math:`b` or both :math:`(a,b)` can be assumed to be equal to 0.

The Dickey-Fuller test checks whether the random perturbation at time :math:`t` vanishes with time.

When :math:`a \neq 0` and :math:`b=0`, the model :eq:`DFmodel` is said to have a *drift*. When :math:`a = 0` and :math:`b \neq 0`, the model :eq:`DFmodel` is said to have a *linear trend*.

In the model :eq:`DFmodel`, the only way to have stochastic non stationarity is to have :math:`\rho = 1` (if :math:`\rho > 1`, then the process diverges with time which is readily seen in the data). In the general case, the Dickey-Fuller test is a unit root test to detect whether :math:`\rho=1` against :math:`\rho < 1`:

The test statistics and its limit distribution depend on the a priori knowledge we have on :math:`a` and :math:`b`. In case of absence of a priori knowledge on the structure of the model, several authors have proposed a global strategy to cover all the subcases of the model :eq:`DFmodel`, depending on the possible values on :math:`a` and :math:`b`. 

The strategy implemented is recommended by Enders (*Applied Econometric Times Series*, Enders, W., second edition, John Wiley \& sons editions, 2004.).



We note :math:`(X_1, \hdots, X_n)` the data, by :math:`W(r)` the Wiener process, and :math:`W^{a}(r) = W(r) - \int_{0}^{1} W(r)\di{r}`, :math:`W^{b}(r) = W^{a}(r) - 12 \left(r - \frac{1}{2} \right) \int_{0}^{1} \left(s - \frac{1}{2} \right) W(s)\di{s}`.


**1.** We assume the model :eq:`Model1`:

.. math::
    :label: Model1

    \boldsymbol{X_t = a + bt + \rho X_{t-1} + \varepsilon_{t}}

The coefficients :math:`(a,b,\rho)` are estimated by :math:`(\Hat{a}_n, \Hat{b}_n, \Hat{\rho}_n)` using ordinary least-squares fitting, which leads to:

.. math::
    :label: Model1Estim

    \underbrace{\left(
       \begin{array}{lll}
         \displaystyle n-1 &\sum_{i=1}^n t_{i} &\sum_{i=2}^n y_{i-1}\\
         \displaystyle \sum_{i=1}^n t_{i} &\sum_{i=1}^n t_{i}^2 &\sum_{i=2}^n t_{i} y_{i-1}\\
         \displaystyle \sum_{i=2}^n y_{i-1}& \sum_{i=2}^n t_{i}y_{i-1} &\sum_{i=2}^n y_{i-1}^2
       \end{array}
       \right)}_{\mat{M}}
     \left(
       \begin{array}{c}
        \hat{a}_n\\
        \hat{b}_n\\
        \hat{\rho}_n
       \end{array}
     \right)=
     \left(
     \begin{array}{l}
       \displaystyle \sum_{i=1}^n y_{i} \\
       \displaystyle \sum_{i=1}^n t_{i} y_{i}\\
       \displaystyle \sum_{i=2}^n y_{i-1} y_{i}
     \end{array}
     \right)


We first test:

.. math::
    :label: TestModel1

    \left\{
    \begin{array}{lr}
      \cH_0: & \rho = 1 \\
      \cH_1: & \rho < 1
    \end{array}
    \right.

thanks to the Student statistics:

.. math::

    t_{\rho=1} = \frac{\rho_n-1}{\hat{\sigma}_{\rho_n}}

where :math:`\sigma_{\rho_n}` is the least square estimate of the standard deviation of :math:`\Hat{\rho}_n`, given by:

.. math::

    \sigma_{\rho_n}=\mat{M}^{-1}_{33}\sqrt{\frac{1}{n-1}\sum_{i=2}^n\left(y_{i}-(\hat{a}_n+\hat{b}_nt_i+\hat{\rho}_ny_{i-1})\right)^2}


which converges in distribution to the Dickey-Fuller distribution associated to the model with drift and trend:

.. math::

    t_{\rho = 1} \stackrel{\mathcal{L}}{\longrightarrow} \frac{\int_{0}^{1}W^{b}(r) \di{W(r)}}{\int_{1}^{0} W^{b}(r)^2 \di{r}}

The null hypothesis :math:`\cH_0` from :eq:`TestModel1` is accepted when :math:`t_{\rho=1} > C_{\alpha}` where :math:`C_{\alpha}` is the test threshold of level :math:`\alpha`.

The quantiles of the Dickey-Fuller statistics for the model with drift and linear trend are:

.. math::
    
    \left\{
    \begin{array}{ll}
        \alpha = 0.01, & C_{\alpha} = -3.96 \\
        \alpha = 0.05, & C_{\alpha} = -3.41 \\
        \alpha = 0.10, & C_{\alpha} = -3.13
    \end{array}
    \right.


**1.1. Case 1:** The null hypothesis :math:`\cH_0` from :eq:`TestModel1` is rejected

We test whether :math:`b=0`:

.. math::
    :label: TestSousModele1_1

    \left\{
    \begin{array}{lr}
      \cH_0: & b = 0 \\
      \cH_1: & b \neq 0
    \end{array}
    \right.

where the statistics :math:`t_n = \frac{|\hat{b}_n|}{\sigma_{b_n}}` converges in distribution to the Student distribution :class:`~openturns.Student` with :math:`\nu=n-4`, where :math:`\sigma_{b_n}` is the least square estimate of the standard deviation of :math:`\Hat{b}_n`, given by:

.. math::

    \sigma_{b_n}=\mat{M}^{-1}_{22}\sqrt{\frac{1}{n-1}\sum_{i=2}^n\left(y_{i}-(\hat{a}_n+\hat{b}_nt_i+\hat{\rho}_ny_{i-1})\right)^2}

The decision to be taken is:
    - If :math:`\cH_0` from :eq:`TestSousModele1_1` is rejected, then the model 1 :eq:`Model1` is confirmed. And the test :eq:`TestModel1` proved that the unit root is rejected : :math:`\rho < 1`. We then conclude that the final model is : :math:`\boldsymbol{X_t = a + bt + \rho X_{t-1} + \varepsilon_{t}}` with :math:`\boldsymbol{\rho < 1}` which is a **trend stationary model**.

    - If :math:`\cH_0` from :eq:`TestSousModele1_1` is accepted, then the model 1 :eq:`Model1` is not confirmed, since the trend presence is rejected and the test :eq:`TestModel1` is not conclusive (since based on a wrong model). **We then have to test the second model** :eq:`Model2`.


**1.2. Case 2:** The null hypothesis :math:`\cH_0` from :eq:`TestModel1` is accepted

We test whether :math:`(\rho, b) = (1,0)`:

.. math::
    :label: TestSousModele1_2

    \left\{
    \begin{array}{lr}
      \cH_0: & (\rho, b) = (1,0) \\
      \cH_1: & (\rho, b) \neq (1,0)
    \end{array}
    \right.

with the Fisher statistics:

.. math::

    \displaystyle \hat{F}_1 = \frac{(S_{1,0} - S_{1,b})/2}{S_{1,b}/(n-3)}

where :math:`S_{1,0}=\sum_{i=2}^n\left(y_i-(\hat{a}_n+y_{i-1})\right)^2` is the sum of the square errors of the model 1 :eq:`Model1` assuming :math:`\cH_0` from :eq:`TestSousModele1_2` and :math:`S_{1,b}=\sum_{i=2}^n\left(y_i-(\hat{a}_n+\hat{b}_nt_i+\hat{\rho}_ny_{i-1})\right)^2` is the same sum when we make no assumption on :math:`\rho` and :math:`b`.

The statistics :math:`\hat{F}_1` converges in distribution to the Fisher-Snedecor distribution :class:`~openturns.FisherSnedecor` with :math:`d_1=2, d_2=n-3`. The null hypothesis :math:`\cH_0` from :eq:`TestModel1` is accepted when :math:`\hat{F}_1 < \Phi_{\alpha}` where :math:`\Phi_{\alpha}` is the test threshold of level :math:`\alpha`.

The decision to be taken is:
    - If :math:`\cH_0` from :eq:`TestSousModele1_2` is rejected, then the model 1 :eq:`Model1` is confirmed since the presence of linear trend is confirmed. And the test :eq:`TestModel1` proved that the unit root is accepted: :math:`\rho = 1`. We then conclude that the model is: :math:`\boldsymbol{X_t = a + bt + X_{t-1} + \varepsilon_{t}}` which is a **non stationary model**.
    
    - If :math:`\cH_0` from :eq:`TestSousModele1_2` is accepted, then the model 1 :eq:`Model1` is not confirmed, since the presence of the linear trend is rejected and the test :eq:`TestModel1` is not conclusive (since based on a wrong model). **We then have to test the second model** :eq:`Model2`.


**2.** We assume the model :eq:`Model2`:

.. math::
    :label: Model2

    \boldsymbol{X_t = a + \rho X_{t-1} + \varepsilon_{t}}


The coefficients :math:`(a,\rho)` are estimated as follows:

.. math::
    :label: Model2Estim

    \underbrace{\left(\begin{array}{lll}
       \displaystyle n-1 &\sum_{i=2}^n y_{i-1}\\
       \displaystyle \sum_{i=2}^n y_{i-1} &\sum_{i=2}^n y_{i-1}^2
                      \end{array}
     \right)}_{\mat{N}}
     \left(
      \begin{array}{c}
        \hat{a}_n\\
        \hat{\rho}_n
      \end{array}
     \right)=
     \left(
      \begin{array}{l}
        \displaystyle \sum_{i=1}^n y_{i} \\
        \displaystyle \sum_{i=2}^n y_{i-1} y_{i}
       \end{array}
     \right)
   

We first test:

.. math::
    :label: TestModel2

    \left\{
     \begin{array}{lr}
       \mathcal{H}_0: & \rho = 1 \\
       \mathcal{H}_1: & \rho < 1
     \end{array}
     \right.

thanks to the Student statistics:

.. math::

    t_{\rho=1} = \frac{\rho_n-1}{\sigma_{\rho_n}}

where :math:`\sigma_{\rho_n}` is the least square estimate of the standard deviation of :math:`\Hat{\rho}_n`, given by:

.. math::

    \sigma_{\rho_n}=\mat{N}^{-1}_{22}\sqrt{\frac{1}{n-1}\sum_{i=2}^n\left(y_{i}-(\hat{a}_n+\hat{\rho}_ny_{i-1})\right)^2}

which converges in distribution to the Dickey-Fuller distribution associated to the model with drift and no linear trend:

.. math::

    t_{\rho = 1} \stackrel{\mathcal{L}}{\longrightarrow} \frac{\int_{0}^{1}W^{a}(r) \di{W(r)}}{\int_{1}^{0} W^{a}(r)^2 \di{r}}

The null hypothesis :math:`\cH_0` from :eq:`TestModel2` is accepted when :math:`t_{\rho=1} > C_{\alpha}` where :math:`C_{\alpha}` is the test threshold of level :math:`\alpha`.

The quantiles of the Dickey-Fuller statistics for the model with drift are:

.. math::
    
    \left\{
    \begin{array}{ll}
        \alpha = 0.01, & C_{\alpha} = -3.43 \\
        \alpha = 0.05, & C_{\alpha} = -2.86 \\
        \alpha = 0.10, & C_{\alpha} = -2.57
    \end{array}
    \right.


**2.1. Case 1:** The null hypothesis :math:`\cH_0` from :eq:`TestModel2` is rejected

We test whether :math:`a=0`:

.. math::
    :label: TestSousModele2_1
    
    \left\{
    \begin{array}{lr}
      \mathcal{H}_0: & a = 0 \\
      \mathcal{H}_1: & a \neq 0
    \end{array}
    \right.

where the statistics :math:`t_n = \frac{|\hat{a}_n|}{\sigma_{a_n}}` converges in distribution to the Student distribution :class:`~openturns.Student` with :math:`\nu=n-3`, where :math:`\sigma_{a_n}` is the least square estimate of the standard deviation of :math:`\Hat{a}_n`, given by:

.. math::

    \sigma_{a_n}=\mat{N}^{-1}_{11}\sqrt{\frac{1}{n-1}\sum_{i=2}^n\left(y_{i}-(\hat{a}_n+\hat{\rho}_ny_{i-1})\right)^2}

The decision to be taken is:
    - If :math:`\cH_0` from :eq:`TestSousModele2_1` is rejected, then the model 2 :eq:`Model2` is confirmed. And the test :eq:`TestModel2` proved that the unit root is rejected: :math:`\rho < 1`. We then conclude that the final model is: :math:`\boldsymbol{X_t = a + \rho X_{t-1} + \varepsilon_{t}}` with :math:`\boldsymbol{\rho < 1}` which is a **stationary model**.

    - If :math:`\cH_0` from :eq:`TestSousModele2_1` is accepted, then the model 2 :eq:`Model2` is not confirmed, since the drift presence is rejected and the test :eq:`TestModel1` is not conclusive (since based on a wrong model). **We then have to test the third model** :eq:`Model3`.


**2.2. Case 2:** The null hypothesis :math:`\cH_0` from :eq:`TestModel2` is accepted

We test whether :math:`(\rho, a) = (1,0)`:

.. math::
    :label: TestSousModele2_2

    \left\{
    \begin{array}{lr}
      \mathcal{H}_0: & (\rho, a) = (1,0) \\
      \mathcal{H}_1: & (\rho, a) \neq (1,0)
    \end{array}
    \right.

with a Fisher test. The statistics is:

.. math::

    \displaystyle \hat{F}_2 = \frac{(SCR_{2,c} - SCR_{2})/2}{SCR_{2}/(n-2)}

where :math:`SCR_{2,c}` is the sum of the square errors of the model 2 :eq:`Model2` assuming :math:`\cH_0` from :eq:`TestSousModele2_2` and :math:`SCR_{2}` is the same sum when we make no assumption on :math:`\rho` and :math:`a`.

The statistics :math:`\hat{F}_2` converges in distribution to the Fisher-Snedecor distribution :class:`~openturns.FisherSnedecor` with :math:`d_1=2, d_2=n-2`. The null hypothesis :math:`\cH_0` from :eq:`TestModel1` is accepted if when :math:`\hat{F}_2 < \Phi_{\alpha}` where :math:`\Phi_{\alpha}` is the test threshold of level :math:`\alpha`.

The decision to be taken is:
    - If :math:`\cH_0` from :eq:`TestSousModele2_2` is rejected, then the model 2 :eq:`Model2` is confirmed since the presence of the drift is confirmed. And the test :eq:`TestModel2` proved that the unit root is accepted: :math:`\rho =1`. We then conclude that the model is: :math:`\boldsymbol{X_t = a + X_{t-1} + \varepsilon_{t}}` which is a **non stationary model**.

    - If :math:`\cH_0` from :eq:`TestSousModele2_2` is accepted, then the model 2 :eq:`Model2` is not confirmed, since the drift presence is rejected and the test :eq:`TestModel2` is not conclusive (since based on a wrong model). **We then have to test the third model** :eq:`Model3`.



**3.** We assume the model :eq:`Model3`:

.. math::
    :label: Model3

    \boldsymbol{X_t = \rho X_{t-1} + \varepsilon_{t}}

The coefficients :math:`\rho` are estimated as follows:

.. math::
    :label: Model3Estim

    \hat{\rho}_n=\frac{\sum_{i=2}^ny_{i-1}y_i}{\sum_{i=2}^ny_{i-1}^2}

We first test:

.. math::
    :label: TestModel3
  
    \left\{
    \begin{array}{lr}
      \mathcal{H}_0: & \rho = 1 \\
      \mathcal{H}_1: & \rho < 1
    \end{array}
    \right.

thanks to the Student statistics:

.. math::

    t_{\rho=1} = \frac{\hat{\rho}_n-1}{\sigma_{\rho_n}}

where :math:`\sigma_{\rho_n}` is the least square estimate of the standard deviation of :math:`\Hat{\rho}_n`, given by:

.. math::

    \sigma_{\rho_n}=\sqrt{\frac{1}{n-1}\sum_{i=2}^n\left(y_{i}-\hat{\rho}_ny_{i-1}\right)^2}/\sqrt{\sum_{i=2}^ny_{i-1}^2}

which converges in distribution to the Dickey-Fuller distribution associated to the random walk model:

.. math::

    t_{\rho = 1} \stackrel{\mathcal{L}}{\longrightarrow} \frac{\int_{0}^{1}W(r) \di{W(r)}}{\int_{1}^{0} W(r)^2 \di{r}}

The null hypothesis :math:`\cH_0` from :eq:`TestModel3` is accepted when :math:`t_{\rho=1} > C_{\alpha}` where :math:`C_{\alpha}` is the test threshold of level :math:`\alpha`.

The quantiles of the Dickey-Fuller statistics for the random walk model are:

.. math::
    
    \left\{
    \begin{array}{ll}
        \alpha = 0.01, & C_{\alpha} = -2.57 \\
        \alpha = 0.05, & C_{\alpha} = -1.94 \\
        \alpha = 0.10, & C_{\alpha} = -1.62
    \end{array}
    \right.

The decision to be taken is:
    - If :math:`\cH_0` from :eq:`TestModel3` is rejected, we then conclude that the model is : :math:`\boldsymbol{X_t = \rho X_{t-1} + \varepsilon_{t}}` where :math:`\rho < 1` which is a **stationary model**.

    - If :math:`\cH_0` from :eq:`TestModel3` is accepted, we then conclude that the model is: :math:`\boldsymbol{X_t = X_{t-1} + \varepsilon_{t}}` which is a **non stationary model**.

.. topic:: API:

    - See :class:`~openturns.DickeyFullerTest`
