.. _process_definitions:

Stochastic process definitions
==============================

In this document, we note:

-  :math:`X: \Omega \times\cD \rightarrow \Rset^d` a multivariate
   stochastic process of dimension :math:`d`, where
   :math:`\omega \in \Omega` is an event, :math:`\cD` is a domain of
   :math:`\Rset^n`, :math:`\vect{t}\in \cD` is a multivariate index and
   :math:`X(\omega, \vect{t}) \in \Rset^d`;

-  :math:`X_{\vect{t}}: \Omega \rightarrow \Rset^d` the random variable
   at index :math:`\vect{t} \in \cD` defined by
   :math:`X_{\vect{t}}(\omega)=X(\omega, \vect{t})`;

-  :math:`X(\omega): \cD  \rightarrow \Rset^d` a realization of the
   process :math:`X`, for a given :math:`\omega \in \Omega` defined by
   :math:`X(\omega)(\vect{t})=X(\omega, \vect{t})`.

| If :math:`n=1`, :math:`t` may be interpreted as a time stamp to
  recover the classical notation of a stochastic process.
| If the process is a second order process, we note:

-  :math:`m : \cD \rightarrow  \Rset^d` its *mean function*, defined by
   :math:`m(\vect{t})=\Expect{X_{\vect{t}}}`,

-  :math:`C : \cD \times \cD \rightarrow  \cM_{d \times d}(\Rset)` its
   *covariance function*, defined by
   :math:`C(\vect{s}, \vect{t})=\Expect{(X_{\vect{s}}-m(\vect{s}))(X_{\vect{t}}-m(\vect{t}))^t}`,

-  :math:`R : \cD \times \cD \rightarrow  \mathcal{M}_{d \times d}(\Rset)`
   its *correlation function*, defined for all
   :math:`(\vect{s}, \vect{t})`, by :math:`R(\vect{s}, \vect{t})` such
   that for all :math:`(i,j)`,
   :math:`R_{ij}(\vect{s}, \vect{t})=C_{ij}(\vect{s}, \vect{t})/\sqrt{C_{ii}(\vect{s}, \vect{t})C_{jj}(\vect{s}, \vect{t})}`.

We recall here some useful definitions.

**Spatial (temporal) and Stochastic Mean**

The *spatial mean* of the process :math:`X` is the function
:math:`m: \Omega \rightarrow \Rset^d` defined by:

.. math::
  :label: spatMean

    \displaystyle m(\omega)=\frac{1}{|\cD|} \int_{\cD} X(\omega)(\vect{t})\, d\vect{t}

If :math:`n=1` and if the mesh is a regular grid
:math:`(t_0, \dots, t_{N-1})`, then the spatial mean corresponds to the
*temporal mean* defined by:

.. math::
  :label: tempMean

    m(\omega) =  \frac{1}{t_{N-1} - t_0} \int_{t_0}^{t_{N-1}}X(\omega)(t) \, dt

| The spatial mean is estimated from one realization of the process (see
  the use case on Field or Time series).
| The *stochastic mean* of the process :math:`X` is the function
  :math:`g: \cD \rightarrow \Rset^d` defined by:

.. math::
  :label: stocMean

    \displaystyle g(\vect{t}) = \Expect{X_{\vect{t}}}

| The stochastic mean is estimated from a sample of realizations of the
  process (see the use case on the Process sample).
| For an *ergodic process*, the stochastic mean and the spatial mean are
  equal and constant (equal to the constant vector noted
  :math:`\vect{c}`):

.. math::
  :label: ergodic

    \forall \omega\in \Omega, \, \forall \vect{t} \in \cM, \, m(\omega)=  g(\vect{t})  = \vect{c}

**Normal process**

A stochastic process is *normal* if all its finite
dimensional joint distributions are normal, which means that for all
:math:`k  \in  \Nset` and :math:`I_k \in \Nset^*`, with
:math:`\mathrm{card} I_k = k`, there exist
:math:`\vect{m}_1,\dots,\vect{m}_k\in\Rset^d` and
:math:`\mat{C}_{1,\dots,k}\in\mathcal{M}_{kd,kd}(\Rset)` such that:

.. math::

     \Expect{\exp\left\{i\vect{X}_{I_k}^t \vect{U}_{k}  \right\}} =
     \exp{\left\{i\vect{U}_{k}^t\vect{M}_{k}-\frac{1}{2}\vect{U}_{k}^t\mat{C}_{1,\dots,k}\vect{U}_{k}\right\}}

where
:math:`\vect{X}_{I_k}^t = (X_{\vect{t}_1}^t, \hdots, X_{\vect{t}_k}^t)`,
:math:`\vect{U}_{k}^t = (\vect{u}_{1}^t, \hdots, \vect{u}_{k}^t)` and
:math:`\vect{M}_{k}^t = (\vect{m}_{1}^t, \hdots, \vect{m}_{k}^t)` and
:math:`\mat{C}_{1,\dots,k}` is the symmetric matrix:

.. math::
  :label: covMatrix

     \mat{C}_{1,\dots,k} = \left(
     \begin{array}{cccc}
       C(\vect{t}_1, \vect{t}_1) &C(\vect{t}_1, \vect{t}_2) & \hdots & C(\vect{t}_1, \vect{t}_{k}) \\
       \hdots & C(\vect{t}_2, \vect{t}_2)  & \hdots & C(\vect{t}_2, \vect{t}_{k}) \\
       \hdots & \hdots & \hdots & \hdots \\
       \hdots & \hdots & \hdots & C(\vect{t}_{k}, \vect{t}_{k})
     \end{array}
     \right)

A normal process is entirely defined by its mean function :math:`m`
and its covariance function :math:`C` (or correlation function
:math:`R`).

**Weak stationarity (second order stationarity)**

A process
:math:`X` is *weakly stationary* or *stationary of second order* if
its mean function is constant and its covariance function is invariant
by translation:

.. math::
  :label: stat2order

    \forall  (\vect{s},\vect{t}) \in \cD, &   \, m(\vect{t})   =  m(\vect{s}) \\
    \forall (\vect{s},\vect{t},\vect{h}) \in \cD,  &  \, C(\vect{s}, \vect{s}+\vect{h})  =C(\vect{t}, \vect{t}+\vect{h})

We note :math:`C^{stat}(\vect{\tau})` for
:math:`C(\vect{s}, \vect{s}+\vect{\tau})` as this quantity does not
depend on :math:`\vect{s}`.
In the continuous case, :math:`\cD` must be equal to
:math:`\Rset^n`\ as it is invariant by any translation. In the
discrete case, :math:`\cD` is a lattice
:math:`\mathcal{L}=(\delta_1 \Zset \times \dots \times \delta_n \Zset)`
where :math:`\forall i, \delta_i >0`.

**Stationarity**

A process :math:`X` is *stationary* if its
distribution is invariant by translation: :math:`\forall k \in \Nset`,
:math:`\forall (\vect{t}_1, \dots, \vect{t}_k) \in \cD`,
:math:`\forall \vect{h}\in \Rset^n`, we have:

.. math::
  :label: statGen

    \forall k \in \Nset, \, \forall (\vect{t}_1, \dots, \vect{t}_k) \in \cD, \, \forall \vect{h}\in \Rset^n, \, (X_{\vect{t}_1}, \dots, X_{\vect{t}_k}) \stackrel{\mathcal{D}}{=} (X_{\vect{t}_1+\vect{h}}, \dots, X_{\vect{t}_k+\vect{h}})

**Spectral density function**

If :math:`X` is a zero-mean weakly
stationary continuous process and if for all :math:`(i,j)`,
:math:`C^{stat}_{i,j} : \Rset^n \rightarrow \Rset^n` is
:math:`\cL^1(\Rset^n)` (ie
:math:`\int_{\Rset^n} |C^{stat}_{i,j}(\vect{\tau})|\, d\vect{\tau}\, < +\infty`),
we define the *bilateral spectral density function*
:math:`S : \Rset^n \rightarrow \cH^+(d)` where
:math:`\mathcal{H}^+(d) \in \mathcal{M}_d(\Cset)` is the set of
:math:`d`-dimensional positive definite hermitian matrices, as the
Fourier transform of the covariance function :math:`C^{stat}`:

.. math::
  :label: specdensFunc

    \forall \vect{f} \in \Rset^n, \,S(\vect{f}) = \int_{\Rset^n}\exp\left\{  -2i\pi <\vect{f},\vect{\tau}> \right\} C^{stat}(\vect{\tau})\, d\vect{\tau}

Furthermore, if for all :math:`(i,j)`,
:math:`S_{i,j}: \Rset^n \rightarrow \Cset` is :math:`\cL^1(\Cset)` (ie
:math:`\int_{\Rset^n} |S_{i,j}(\vect{f})|\, d\vect{f}\, < +\infty`),
:math:`C^{stat}` may be evaluated from :math:`S` as follows:

.. math::
  :label: cspectransform

    C^{stat}(\vect{\tau})  = \int_{\Rset^n}\exp\left\{  2i\pi <\vect{f}, \vect{\tau}> \right\}S(\vect{f})\, d\vect{f}

In the discrete case, the spectral density is defined for a zero-mean
weakly stationary process, where
:math:`\cD=(\delta_1 \Zset \times \dots \times \delta_n \Zset)` with
:math:`\forall i, \delta_i >0` and where the previous integrals are
replaced by sums.
