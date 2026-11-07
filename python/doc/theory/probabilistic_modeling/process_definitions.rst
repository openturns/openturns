.. _process_definitions:

Stochastic process definitions
------------------------------

Notations
~~~~~~~~~

In this document, we note:

-  :math:`X: \Omega \times\cD \rightarrow \Rset^{\processDim}` a multivariate
   stochastic process of dimension :math:`\processDim`, where
   :math:`\omega \in \Omega` is an event, :math:`\cD` is a domain of
   :math:`\Rset^{\domainDim}`, :math:`\vect{t}\in \cD` is a multivariate
   index and :math:`X(\omega, \vect{t}) \in \Rset^{\processDim}`;

-  :math:`X_{\vect{t}}: \Omega \rightarrow \Rset^{\processDim}` the random
   variable at index :math:`\vect{t} \in \cD` defined by
   :math:`X_{\vect{t}}(\omega)=X(\omega, \vect{t})`;

-  :math:`X(\omega): \cD  \rightarrow \Rset^{\processDim}` a realization of
   the process :math:`X`, for a given :math:`\omega \in \Omega` defined by
   :math:`X(\omega)(\vect{t})=X(\omega, \vect{t})`.

If :math:`\domainDim=1`, :math:`t` may be interpreted as a time stamp to
recover the classical notation of a stochastic process.
If the process is a second order process, we note:

-  :math:`m : \cD \rightarrow  \Rset^{\processDim}` its *mean function*,
   defined by :math:`m(\vect{t})=\Expect{X_{\vect{t}}}`,

-  :math:`\mat{C} : \cD \times \cD \rightarrow
   \cM_{\processDim \times \processDim}(\Rset)` its *covariance function*,
   defined by:

.. math::
   \begin{aligned}
   \mat{C}(\vect{s}, \vect{t})
   & := \Cov{X_{\vect{s}}, X_{\vect{t}}} \\
   & \; = \Expect{\left(X_{\vect{s}} - m(\vect{s})\right)
     \Tr{\left(X_{\vect{t}} - m(\vect{t})\right)}},
   \end{aligned}

-  :math:`\mat{R} : \cD \times \cD \rightarrow
   \cM_{\processDim \times \processDim}(\Rset)` its *correlation function*,
   defined for all :math:`(\vect{s}, \vect{t})`, by
   :math:`\mat{R}(\vect{s}, \vect{t})` such that for all
   :math:`i, j \in \{1, \dots, \processDim\}`:

.. math::
   \begin{aligned}
   \mat{R}_{ij}(\vect{s}, \vect{t})
   & := \Cor{X_{\vect{s}}, X_{\vect{t}}} \\
   & \; = \frac{\mat{C}_{ij}(\vect{s}, \vect{t})}
     {\sqrt{\mat{C}_{ii}(\vect{s}, \vect{s}) \mat{C}_{jj}(\vect{t}, \vect{t})}}.
   \end{aligned}

We recall here some useful definitions.

Spatial (temporal) and Stochastic Mean
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The *spatial mean* of the process :math:`X` is the random variable
:math:`\overline{X}: \Omega \rightarrow \Rset^{\processDim}` defined by:

.. math::
  :label: spatMean

    \displaystyle \overline{X}(\omega)=\frac{1}{|\cD|}
    \int_{\cD} X(\omega)(\vect{t})\, \di{\vect{t}}

If :math:`\domainDim=1` and if the mesh is a regular grid
:math:`(t_0, \dots, t_{N-1})`, then the spatial mean corresponds to the
*temporal mean* defined by:

.. math::
  :label: tempMean

    \overline{X}(\omega) =  \frac{1}{t_{N-1} - t_0}
    \int_{t_0}^{t_{N-1}}X(\omega)(t) \, \di{t}

The spatial mean is estimated from one realization of the process (see
the use case on Field or Time series).
The *stochastic mean* of the process :math:`X` is its mean function
:math:`m : \cD \rightarrow \Rset^{\processDim}` introduced in the
notations section:

.. math::
  :label: stocMean

    \displaystyle m(\vect{t}) = \Expect{X_{\vect{t}}}

The stochastic mean is estimated from a sample of realizations of the
process (see the use case on the Process sample).
For an *ergodic process*, the stochastic mean and the spatial mean are
equal and constant (equal to the constant vector noted
:math:`\vect{c}`):

.. math::
  :label: ergodic

    \forall \omega\in \Omega, \, \forall \vect{t} \in \cD, \,
    \overline{X}(\omega)=  m(\vect{t})  = \vect{c}

These definitions follow the classical framework of second order
processes, see for example A. Papoulis and S. U. Pillai, *Probability,
Random Variables and Stochastic Processes*, 4th edition, McGraw-Hill
(2002).

Normal process
~~~~~~~~~~~~~~

A stochastic process is *normal* if all its finite
dimensional joint distributions are normal, which means that for all
:math:`k  \in  \Nset` and :math:`I_k \in \Nset^*`, with
:math:`\mathrm{card} I_k = k`, there exist
:math:`\vect{m}_1,\dots,\vect{m}_k\in\Rset^{\processDim}` and
:math:`\mat{C}_{1,\dots,k}\in
\cM_{k\processDim \times k\processDim}(\Rset)` such that:

.. math::

     \Expect{\exp\left\{i\Tr{\vect{X}_{I_k}} \vect{U}_{k}\right\}} =
     \exp\left\{i\Tr{\vect{U}_{k}}\vect{M}_{k}
     -\frac{1}{2}\Tr{\vect{U}_{k}}\mat{C}_{1,\dots,k}\vect{U}_{k}\right\}

where
:math:`\vect{X}_{I_k} = (X_{\vect{t}_1}, \dots, X_{\vect{t}_k})`,
:math:`\vect{U}_{k} = (\vect{u}_{1}, \dots, \vect{u}_{k})` and
:math:`\vect{M}_{k} = (\vect{m}_{1}, \dots, \vect{m}_{k})` are the stacked
vectors, and :math:`\mat{C}_{1,\dots,k}` is the symmetric matrix:

.. math::
  :label: covMatrix

     \mat{C}_{1,\dots,k} =
     \begin{pmatrix}
       \mat{C}(\vect{t}_1, \vect{t}_1) &
       \mat{C}(\vect{t}_1, \vect{t}_2) & \dots &
       \mat{C}(\vect{t}_1, \vect{t}_{k}) \\
       \vdots & \ddots & \ddots & \vdots \\
       \mat{C}(\vect{t}_{k}, \vect{t}_1) & \dots &
       \mat{C}(\vect{t}_{k}, \vect{t}_{k-1}) &
       \mat{C}(\vect{t}_{k}, \vect{t}_{k})
     \end{pmatrix}

A normal process is entirely defined by its mean function :math:`m`
and its covariance function :math:`\mat{C}` (or correlation function
:math:`\mat{R}`).

Weak stationarity (second order stationarity)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A process
:math:`X` is *weakly stationary* or *stationary of second order* if
its mean function is constant and its covariance function is invariant
by translation:

.. math::
  :label: stat2order

    \begin{aligned}
    \forall (\vect{s},\vect{t}) \in \cD \times \cD, \quad
    & m(\vect{t})   =  m(\vect{s}), \\
    \forall (\vect{s},\vect{t}) \in \cD \times \cD, \,
    \forall \vect{h} \in \Rset^{\domainDim}
    \text{ such that }
    \vect{s}+\vect{h} \in \cD \text{ and } \vect{t}+\vect{h} \in \cD,
    \quad
    & \mat{C}(\vect{s}, \vect{s}+\vect{h})
      = \mat{C}(\vect{t}, \vect{t}+\vect{h}).
    \end{aligned}

We note :math:`\mat{C}^{stat}(\vect{\tau})` for
:math:`\mat{C}(\vect{s}, \vect{s}+\vect{\tau})` as this quantity does not
depend on :math:`\vect{s}`.
The translations :math:`\vect{h}` are restricted to those for which all
the shifted indices remain in :math:`\cD`. When :math:`\cD` is invariant
under every translation, for example when
:math:`\cD = \Rset^{\domainDim}`, this restriction is void; otherwise,
:math:`\vect{h}` must be such that both :math:`\vect{s}+\vect{h}` and
:math:`\vect{t}+\vect{h}` remain in :math:`\cD`. In the discrete case,
:math:`\cD` is a lattice
:math:`\cL=(\delta_1 \Zset \times \dots \times
\delta_{\domainDim} \Zset)` where :math:`\forall i, \delta_i >0`.

Stationarity
~~~~~~~~~~~~

A process :math:`X` is *stationary* if its
distribution is invariant by translation: :math:`\forall k \in \Nset`,
:math:`\forall (\vect{t}_1, \dots, \vect{t}_k) \in \cD^k`,
:math:`\forall \vect{h}\in \Rset^{\domainDim}` such that
:math:`\vect{t}_i+\vect{h} \in \cD` for all :math:`i`, we have:

.. math::
  :label: statGen

    \forall k \in \Nset, \,
    \forall (\vect{t}_1, \dots, \vect{t}_k) \in \cD^k, \,
    \forall \vect{h}\in \Rset^{\domainDim}
    \text{ with }
    \vect{t}_1+\vect{h}, \dots, \vect{t}_k+\vect{h} \in \cD, \,
    (X_{\vect{t}_1}, \dots, X_{\vect{t}_k})
    \stackrel{\mathcal{D}}{=}
    (X_{\vect{t}_1+\vect{h}}, \dots, X_{\vect{t}_k+\vect{h}})

Spectral density function
~~~~~~~~~~~~~~~~~~~~~~~~~

In this section, the index domain is assumed to be invariant under every
translation, i.e. :math:`\cD = \Rset^{\domainDim}`, so that the covariance
function is defined on the whole lag domain :math:`\Rset^{\domainDim}`.

If :math:`X` is a zero-mean weakly
stationary continuous process and if for all :math:`(i,j)`,
:math:`\mat{C}^{stat}_{i,j} : \Rset^{\domainDim} \rightarrow \Rset` is
:math:`\cL^1(\Rset^{\domainDim})` (i.e.
:math:`\int_{\Rset^{\domainDim}} |\mat{C}^{stat}_{i,j}(\vect{\tau})|\,
\di{\vect{\tau}} < +\infty`),
we define the *bilateral spectral density function*
:math:`\mat{S} : \Rset^{\domainDim} \rightarrow \cH^+(\processDim)`
where :math:`\cH^+(\processDim) \subset \cM_{\processDim}(\Cset)` is the
set of :math:`\processDim`-dimensional positive definite hermitian
matrices, as the Fourier transform of the covariance function
:math:`\mat{C}^{stat}`:

.. math::
  :label: specdensFunc

    \forall \vect{f} \in \Rset^{\domainDim}, \,
    \mat{S}(\vect{f}) = \int_{\Rset^{\domainDim}}
    \exp\left\{-2i\pi\scalarproduct{\vect{f}}{\vect{\tau}}\right\}
    \mat{C}^{stat}(\vect{\tau})\, \di{\vect{\tau}}

Furthermore, if for all :math:`(i,j)`,
:math:`\mat{S}_{i,j}: \Rset^{\domainDim} \rightarrow \Cset` is
:math:`\cL^1(\Rset^{\domainDim})` (i.e.
:math:`\int_{\Rset^{\domainDim}} |\mat{S}_{i,j}(\vect{f})|\,
\di{\vect{f}} < +\infty`),
:math:`\mat{C}^{stat}` may be evaluated from :math:`\mat{S}` as follows:

.. math::
  :label: cspectransform

    \mat{C}^{stat}(\vect{\tau})  = \int_{\Rset^{\domainDim}}
    \exp\left\{2i\pi\scalarproduct{\vect{f}}{\vect{\tau}}\right\}
    \mat{S}(\vect{f})\, \di{\vect{f}}

In the discrete case, the spectral density is defined for a zero-mean
weakly stationary process, where
:math:`\cD=(\delta_1 \Zset \times \dots \times
\delta_{\domainDim} \Zset)` with
:math:`\forall i, \delta_i >0` and where the previous integrals are
replaced by sums.
