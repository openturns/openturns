---
title: 'OpenTURNS: A Python package for uncertainty quantification'
tags:
  - Python
  - uncertainty quantification
  - probabilistic modeling
  - statistics
  - meta-modeling
  - calibration
  - inverse problems
  - stochastic processes
  - Bayesian modeling
  - machine learning
  - supervised machine learning
  - unsupervised machine learning
  - surrogate modelling
  - polynomial chaos expansion
  - gaussian process regression
  - sensitivity analysis
  - regression
  - function approximation
  - Quasi-Monte Carlo methods
  - optimization
  - Reliability Analysis
  - Markov chain Monte Carlo
  - copula
  - dimensionality reduction
  - maximum likelihood estimation
  - importance sampling
authors:
  - name: Michaël Baudin
    orcid: 0000-0002-0450-7947
    affiliation: 1
  - name: Anne Dutfoy
    affiliation: 1
  - name: Régis Lebrun
    affiliation: 2
  - name: Julien Schueller
    orcid: 0009-0006-2452-7861
    affiliation: 3
  - name: Sofiane Haddad
    affiliation: 2
  - name: Loïc Brevault
    affiliation: 4
  - name: Mathieu Balesdent
    affiliation: 4
  - name: Julien Pelamatti
    affiliation: 1
  - name: Joseph Muré
    affiliation: 1
affiliations:
 - name: EDF R&D, France
   index: 1
 - name: Airbus, France
   index: 2
 - name: Phimeca, France
   index: 3
 - name: ONERA, France
   index: 4
date: 30 July 2025
bibliography: paper.bib
---

# Summary

OpenTURNS (www.openturns.org) is an open-source Python package for 
probabilistic modeling, uncertainty quantification (UQ), and machine learning.
It provides a unified interface, granting users access to a comprehensive 
set of state-of-the-art algorithms and modeling tools.
The library is supported by extensive, regularly updated
documentation, including introductory examples, various use cases, 
tutorials, and user guides, enhancing its accessibility.
OpenTURNS integrates seamlessly with NumPy, Pandas, and other
scientific Python packages, allowing users to develop UQ
workflows and generate reproducible results.
Its modular architecture enables both newcomers and advanced users to
access reliable, well-tested methods for uncertainty propagation,
sensitivity analysis, and reliability assessment.
Its active community provides additional support and resources through 
a Discourse forum [@openturns_forum] and a website.
Stable releases are published on a semiannual basis, and the library is
distributed through Conda, pip, and Debian repositories and available on 
Linux, Windows, and macOS.

# Statement of need

UQ plays a critical role in robust decision-making during the design
and operation of complex systems.
It encompasses probabilistic modeling (multivariate distributions,
stochastic processes), sampling, functional algebra, uncertainty
propagation, sensitivity analysis, reliability, optimization, inverse
problems, and surrogate modeling.
Validated and well-documented methods are essential for teaching, academic 
research, and industrial applications.
However, many probabilistic modeling libraries are
domain-specific—focusing on fields such as mechanical engineering, computational
fluid dynamics, or hydrology—which limits the sharing of best practices.
This underscores the need for a general-purpose, cross-disciplinary
framework for probabilistic modeling.

# State of the field

Other Python packages exist for Monte Carlo
sampling and other numerical methods [@virtanen2020scipy],
statistical modeling [@seabold2010statsmodels], 
machine learning [@pedregosa2011scikit], UQ test 
functions [@Wicaksono2023], and quasi-Monte Carlo methods [@roy2023quasi], 
among others.
One of the goals of OpenTURNS is to provide a consistent interface to 
these tasks in the UQ context.

Developing OpenTURNS as a standalone framework was necessary because
integrating its specialized, tightly coupled C++ architecture for advanced
reliability analysis and stochastic modeling into general-purpose libraries
like SciPy or scikit-learn would have been architecturally incompatible and
misaligned with their primary scopes.
OpenTURNS was created in 2005 at a time where SciPy was
a relatively new library (the development of SciPy was started in 2001)
and scikit-learn did not exist (the first public release was in 2010).
Today, this choice is still relevant as OpenTURNS is integrated in other
C++ libraries like Persalys, Salome (see [@salome_platform]) 
or pSeven (see [@pseven]) for example, where the C++ implementation of
OpenTURNS is mandatory.

# Software design

OpenTURNS relies on a dual-language architecture to balance computational 
efficiency with user accessibility.
The core library is implemented in C++, which handles the memory management and
heavy computational workloads required by Monte Carlo simulations, 
high-dimensional integration, and optimization algorithms.
This C++ core is then exposed through a Python interface using SWIG. 

The primary trade-off in this design is the increased complexity of 
maintaining a mixed-language codebase and managing bindings. 
However, this cost is outweighed by the performance gains in intensive 
numerical tasks. 

From an architectural perspective, OpenTURNS is strictly object-oriented, 
mirroring mathematical concepts directly in the code.
Users manipulate objects such as `Distribution`, `Process`, and `Function`. 
The `Function` class, for instance, provides an abstract representation of 
mathematical models, allowing for caching, composition, and automatic or 
finite-difference differentiation. 
This abstraction ensures that reliability and sensitivity algorithms can operate 
on complex industrial black-box models exactly as they would on simple 
analytical equations.

# Features

OpenTURNS provides a wide range of multivariate probabilistic modeling
features: more than 60 continuous and discrete distributions,
including extreme-value distributions, copula-based models (more than
15 copulas), and distribution transformations (such as truncation,
conditioning, mixture, push-forward, and distribution algebra).

Multivariate distributions can be sampled using general sampling algorithms
(Monte Carlo, Latin Hypercube Sampling (LHS), and optimized LHS) and
quasi-Monte Carlo low-discrepancy sequences (such as Sobol’ or Halton) for
integration and model analysis.

OpenTURNS offers modeling features for stationary 
and non-stationary stochastic processes (e.g., ARMA processes and Gaussian 
processes in arbitrary dimensions) associated with several covariance 
kernels (e.g., exponential, Matérn, and squared exponential).

The library provides extensive data analysis capabilities.
Dependency indicators can be computed (Pearson and Spearman correlations).
Inference of parametric and non-parametric univariate or multivariate
probabilistic models, including copula models, can be performed, along with 
several statistical tests to validate the estimation (quantitative tests such as the 
Kolmogorov–Smirnov or Lilliefors tests, and graphical tests such as the
quantile–quantile plot).

The library provides an abstract representation of vector-valued or field-based
mathematical functions that allows for composition, aggregation, symbolic or
finite-difference derivatives, and caching.
It also includes tools for wrapping black-box code that communicates
through input/output file exchange.

Several sensitivity and reliability indices can be used to quantify
the influence of one variable on another.
Notable examples include Standardized Regression Coefficients (SRC),
Sobol’ indices, and Hilbert–Schmidt Independence Criterion (HSIC) indices.
For the latter two, OpenTURNS provides the asymptotic distribution of
the estimators, which allows the computation of confidence intervals.

OpenTURNS implements numerous reliability algorithms for rare-event
probabilities. 
These include classic algorithms such as the First-Order Reliability
Method, Second-Order Reliability Method, subset simulation, and other
variance reduction methods.
It also provides more recent methods such as
Nonparametric Adaptive Importance Sampling (NAIS) and Importance Sampling by
Cross-Entropy.
An event can be a single occurrence (e.g., a threshold exceedance), a
system event, or a condition based on a stochastic process.

For supervised learning, OpenTURNS enables the construction
of surrogate models from data, such as linear regression models,
polynomial chaos expansions, and Gaussian process regression.
These surrogate models can also be built for models with functional inputs.
The optimization of the Gaussian process regression hyperparameters can be
performed on an arbitrary subset thereof.

In many cases, probabilistic analyses use optimization algorithms (e.g.,
for supervised learning or calibration).
OpenTURNS can be used to define a wide variety of optimization problems
in a unified manner and provides users with solution algorithms from
numerous existing libraries: NLopt, IPOPT, pagmo, BONMIN, dlib,
Ceres, cminpack, and COBYLA, among others.
It can solve optimization problems with single
or multiple objectives, with equality or inequality constraints, and
with continuous or discrete variables.

Since many analyses require the computation of integrals,
OpenTURNS provides integration methods based on sampling
or quadrature: multivariate Gaussian quadrature (e.g.,
Gauss–Legendre quadrature), Gauss–Kronrod, Smolyak, simplicial cubature,
and other algorithms, including those from the Cuba library.

# Research impact statement

OpenTURNS is widely used in research, industrial engineering, and
academic education.
It has been integrated into university curricula (e.g., DTU Denmark) and
engineering schools (e.g., ENSTA, ENPC, Centrale Nantes, among others)
for courses on reliability analysis, risk assessment, and UQ.

The library has been introduced in several publications [e.g., @baudin2016] 
and used in a broad range of UQ studies across
multiple science and engineering domains
[see @dutfoy2023; @espoeys2025multifidelity; @faure2024impact].

The Persalys graphical user interface [@persalys], built on top of OpenTURNS,
enables users to perform UQ analyses with no (or minimal) Python
programming.

An annual Users’ Day has been held since 2008, fostering collaboration
and knowledge exchange among researchers, engineers, and educators.

# AI usage disclosure

Generative AI was used to verify the grammar and spelling of this paper,
check the choices of the words and check that the paper comply to the
requirements.
No generative AI was used in the creation of the software, the technical 
documentation, or the algorithm design.

# References
