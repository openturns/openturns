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
  - surrogate modeling
  - polynomial chaos expansion
  - gaussian process regression
  - sensitivity analysis
  - regression
  - function approximation
  - Quasi-Monte Carlo methods
  - optimization
  - reliability analysis
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
- name: Anne Dutfoy
  orcid: 0000-0002-5139-106X
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
- name: Mathieu Balesdent
  orcid: 0000-0003-4064-3361
  affiliation: 4
  - name: Julien Pelamatti
- name: Julien Pelamatti
  orcid: 0000-0002-1769-2715
  affiliation: 1
  - name: Joseph Muré
- name: Joseph Muré
  orcid: 0000-0003-4147-1143
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
The library is supported by extensive, regularly updated,
easy to read documentation, including introductory examples, various use cases, 
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

There are many software packages and libraries for Uncertainty Quantification.
Although the landscape is highly fragmented with significant specialization,
these tools fall into two broad categories.
First, there are general-purpose libraries, which aim to provide
most of the features used in research and engineering to perform UQ studies.
In this class of tools, we find Dakota [@adams2025dakota], Raven [@RAVENuserMan],
Sparse Grid++ [@pflueger10spatially], PSUADE [@tong2024psuade],
URANIE [@blanchard2019uranie], UQLab [@marelli2014uqlab],
UQTk [@Debusschere2017UQTk], OpenCOSSAN [@Patelli2016Cossan],
UQPy [@olivier2020uqpy], EasyVVUQ [@SULEIMENOVA2021101402],
SmartUQ [@absil2016smart], and NESSUS [@McFarland2017], among others.
These tools and libraries are provided in a variety of languages including
MATLAB (e.g., UQLab), Python (e.g., UQPy), and ROOT/C++ (e.g., URANIE).
Some of these tools are open-source (e.g., EasyVVUQ), while others are
commercial products (e.g., NESSUS).
Other libraries have become open-source over time (e.g., UQLab became
open-source in 2022).
Many of these libraries are massive software
projects (e.g., Dakota has more than 300,000 lines of code).
Some of these tools have a graphical user interface (e.g., SmartUQ), but not all.
The second category comprises a large number of specialized libraries.
In this class of tools, we find, for example, GPy [@gpy2014] for Gaussian
Processes, QUESO [@McDougall2017] for Bayesian inference, SALib [@Herman2017]
and sensitivity [@iooss2026sensitivity] for sensitivity analysis and
ChaosPy [@feinberg2015chaospy] for polynomial chaos expansion, among
many others.
Most of these tools are relatively small software projects (e.g.,
ChaosPy has approximately 30,000 lines of code).
These specialized libraries are sometimes maintained by a small number of
core developers, although some are based on many contributions (e.g.,
SALib has more than 40 contributors on GitHub).
Furthermore, they are often easier to adapt for research purposes.
This is the case, for example, for Python projects, which are easier for many
users to extend compared to a C++ library.
On the other hand, maintaining these libraries over time can be
challenging.

Other Python packages exist for Monte Carlo
sampling and other numerical methods [@virtanen2020scipy],
statistical modeling [@seabold2010statsmodels], 
machine learning [@pedregosa2011scikit], UQ test 
functions [@Wicaksono2023], and quasi-Monte Carlo methods [@roy2023quasi], 
among others.
One of the goals of OpenTURNS is to provide a consistent interface for all
these tasks, serving as a general-purpose tool for UQ.

Developing OpenTURNS as a standalone framework was necessary because
integrating its specialized, tightly coupled C++ architecture for advanced
reliability analysis and stochastic modeling into general-purpose libraries
like SciPy or scikit-learn would have been architecturally incompatible and
misaligned with their primary scopes.
OpenTURNS was created in 2005 at a time when SciPy was
a relatively new library (the development of SciPy was started in 2001)
and scikit-learn did not exist (the first public release was in 2010).
Today, this choice remains relevant as OpenTURNS is integrated into other C++
libraries such as Persalys, SALOME (see [@salome_platform]) and pSeven
(see [@pseven]), where the C++ implementation of OpenTURNS is
mandatory.

# Software design

OpenTURNS relies on a dual-language architecture to balance computational 
efficiency with user accessibility.
The core library is implemented in C++, which handles memory management and
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

OpenTURNS provides a wide range of easy to build multivariate probabilistic models:
more than 60 continuous and discrete distributions,
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
Dependency indicators can be computed (e.g., Pearson and Spearman correlations).
Inference of parametric and non-parametric univariate or multivariate
probabilistic models, including copula models, can be performed, along with 
several statistical tests to validate the estimation (quantitative tests such as the 
Kolmogorov-Smirnov or Lilliefors tests, and graphical tests such as the
quantile-quantile plot).

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
The optimization can be performed on an arbitrary subset of the Gaussian
process regression hyperparameters.

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
OpenTURNS is a requirement for several other tools, including [@gemseo] 
and [@Roy2018], for example.

An annual Users’ Day has been held since 2008, fostering collaboration
and knowledge exchange among researchers, engineers, and educators.

# AI usage disclosure

Generative AI (Gemini 3.1 Pro) was used to verify the grammar and spelling of this paper,
check word choices, and check that the paper complies with the
requirements.
No generative AI was used in the creation of the software, the technical 
documentation, or the algorithm design.

# Acknowledgements

We acknowledge support from the OpenTURNS consortium.
Additional funding was provided by EDF and Airbus through external development
contracts.
The project has received funding from the European Commission within the
LOCOMACHS (grant agreement No. 314003) and the UMRIDA (grant agreement No.
605036) projects.
It has also received funding from the French National Research Agency (ANR)
within the OPUS (ANR-07-TLOG-0015) and CHORUS (ANR-13-MONU-0005) projects.

# References
