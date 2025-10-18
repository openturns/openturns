---
title: 'OpenTURNS: A Python package for uncertainty quantification'
tags:
  - Python
  - uncertainty quantification
  - probabilistic modeling
  - statistics
  - meta-modeling
  - calibration
  - stochastic processes
  - Bayesian modeling
  - machine learning
  - supervised machine learning
  - unsupervised machine learning
  - surrogate
authors:
  - name: Julien Schueller
    orcid: 0009-0006-2452-7861
    affiliation: 1
  - name: Michaël Baudin
    orcid: 0000-0002-0450-7947
    affiliation: 2
  - name: Anne Dutfoy
    affiliation: 2
  - name: Joseph Muré
    affiliation: 2
  - name: Julien Pelamatti
    affiliation: 2
  - name: Régis Lebrun
    affiliation: 3
  - name: Sofiane Haddad
    affiliation: 3
  - name: Loïc Brevault
    affiliation: 4
  - name: Mathieu Balesdent
    affiliation: 4
affiliations:
 - name: Phimeca, France
   index: 1
 - name: EDF R&D, France
   index: 2
 - name: Airbus, France
   index: 3
 - name: ONERA, France
   index: 4
date: 30 July 2025
bibliography: paper.bib
---

# Summary

OpenTURNS (www.openturns.org) is an open-source Python module built on a 
C++ core library for probabilistic modeling, uncertainty quantification
(UQ), and machine learning.
OpenTURNS (Open source Treatment of Uncertainty, Risk ’N Statistics)
is a general-purpose library for probabilistic modeling, uncertainty
quantification, and machine learning, released under the LGPL license.
It provides a unified interface through a Python package, granting
users access to a comprehensive set of state-of-the-art algorithms and
modeling tools.
The library is supported by extensive, regularly updated
documentation, including examples (in particular, a Getting Started
example), tutorials, and user guides.
The documentation makes the library very user-friendly.
OpenTURNS integrates seamlessly with NumPy, Pandas, and other
scientific Python packages, allowing users to develop UQ
workflows and generate reproducible results. Its modular architecture
enables both newcomers and advanced users to access reliable,
well-tested methods for uncertainty propagation, sensitivity analysis,
and reliability assessment.
Its active community provides additional support and resources through 
a Discourse forum and its website.

## Statement of need

UQ plays a critical role in robust decision-making during the design
and operation of complex systems.
It encompasses probabilistic modeling (multivariate distributions,
stochastic processes), sampling, functional algebra, uncertainty
propagation, sensitivity analysis, reliability, optimization, inverse
problems, and surrogate modeling.
Validated, well-tested and well-documented methods are essential for teaching, academic research, and industrial applications.
However, many probabilistic modeling libraries are domain-specific, 
focusing on fields such as mechanical engineering, computational fluid
dynamics, or hydrology, which limits the sharing of best practices.
This underscores the need for a general-purpose, cross-disciplinary
framework for probabilistic modeling.

Of course, other Python packages exist for Monte-Carlo
sampling and other numerical methods [@virtanen2020scipy],
statistical modeling [@seabold2010statsmodels], 
machine learning [@pedregosa2011scikit], for providing UQ test 
functions [@Wicaksono2023], and for quasi-Monte Carlo methods [@roy2023quasi]
(among others).
One of the goals of OpenTURNS is to provide a consistent interface to 
these tasks in the UQ context.

# Features

OpenTURNS provides a wide range of multivariate probabilistic modeling
features: more than 60 continuous and discrete distributions, 
including extreme value distributions, copula-based models (more than 
15 copulas), and distributions defined by conditioning.
It is also possible to determine exactly any univariate push-forward
distribution (for example, the exact distribution of the cosine of a
distribution).
User-defined distributions can be easily implemented and integrated
natively.

Multivariate distributions can be sampled using general sampling algorithms
(Monte Carlo, LHS, and optimized LHS).
OpenTURNS also provides quasi–Monte Carlo low-discrepancy sequences 
(such as Sobol’ or Halton) for integration and model analysis.

In addition, OpenTURNS offers modeling features for stationary 
and non-stationary stochastic processes (ARMA processes, Gaussian 
processes in arbitrary dimensions) associated with several covariance 
kernels (exponential, Matèrn, squared exponential, etc.).
The covariance kernels can be combined to produce a multivariate
covariance kernel.
It is also possible to implement a user-defined stationary or
non-stationary covariance kernel.

OpenTURNS offers a wide variety of unsupervised algorithms that are
essential for exploratory data analysis and machine learning.
For example, OpenTURNS provides numerous possibilities for multivariate data analysis: data can be visualized via pair plots or parallel
coordinate plots, which reveal dependence structures or outliers.
Dependence indicators can also be computed: Pearson and Spearman 
correlations, Kendall’s tau, and tail dependence coefficients.
Furthermore, OpenTURNS makes it possible to fit a multivariate 
distribution to a sample using parametric techniques based on several estimators (maximum likelihood, method of moments, etc.).
In addition to parametric estimation techniques, it provides
several non-parametric estimation techniques such as kernel
density estimation (with an automatically computed bandwidth).
Copula estimation can also be performed using a copula model or the
non-parametric Bernstein copula.
OpenTURNS provides many tests to validate the estimation: quantitative
tests such as the Kolmogorov–Smirnov or Lilliefors tests, the
chi-squared test, and useful graphical tests such as the
quantile–quantile plot and the Kendall plot for copula goodness of fit.

OpenTURNS implements function algebra up to second order, enabling the
exact calculation of gradients and Hessians of functions defined
as compositions of functions providing their own gradients and
Hessians.
Functions can be defined in Python.
For example, for functions that can be executed from the operating
system's command line, OpenTURNS provides coupling tools that wrap
the executable as a Python function using text files for input and output.
OpenTURNS functions are designed to manage vector as well as field inputs. Their outputs can be vectors as well as fields.
The documentation provides extensive examples of how to build and
compose these functions.

Several sensitivity and reliability indices can be used to quantify
how influential a variable is to another variable.
Notable examples include Standardized Regression Coefficients (SRC),
Sobol’ indices, and Hilbert–Schmidt Independence Criterion (HSIC) indices. For the latter two, OpenTURNS provides the asymptotic distribution of
the estimators, which allows the computation of confidence intervals.

OpenTURNS implements numerous reliability algorithms that enable the
efficient calculation of probabilities for rare events: classic
algorithms such as the First-Order Reliability Method,
Second-Order Reliability Method, subset sampling, and other
variance reduction algorithms, as well as more recent ones such as
Nonparametric Adaptive Importance Sampling (NAIS) and Cross-Entropy.
An event can be a single event (for example, threshold exceedance), a
system event, or an event based on a stochastic process.

For supervised learning algorithms, OpenTURNS enables the construction
of surrogate models from data, such as linear regression models,
polynomial chaos expansion, and Gaussian process regression.
The polynomial chaos expansion implemented in OpenTURNS can be based on
an arbitrary polynomial orthogonal basis, in addition to the
usual Legendre, Hermite, Jacobi, and Laguerre bases.
OpenTURNS also provides functional orthogonal bases such as Haar wavelets
or Fourier series.
This surrogate model can also be built for models with functional inputs.
The optimization of the hyperparameters of Gaussian process regression
can be performed on an arbitrary subset of the hyperparameters, including,
for example, the shape and amplitude parameters.

In many cases, probabilistic analyses use optimization algorithms (e.g.,
for supervised learning or calibration).
OpenTURNS can be used to define a wide variety of optimization problems
in a unified manner and provides users with solution algorithms from
numerous existing libraries: NLOpt, Ipopt, PAGMO, Bonmin, Dlib,
Ceres, Cminpack, Cobyla, and more.
It makes it possible to solve optimization problems with single
or multiple objectives, with equality or inequality constraints, and
with continuous or discrete variables.

Finally, since many analyses require the computation of integrals,
OpenTURNS provides powerful integration methods based on sampling
or quadrature methods: multivariate Gaussian quadrature (e.g.,
Gauss–Legendre quadrature), Gauss–Kronrod, Smolyak, simplicial cubature,
and other algorithms including those from the Cuba library.

# Usages

OpenTURNS is widely used in research, industrial engineering, and
academic education.
It has been integrated into university curricula (e.g., DTU Denmark) and
engineering schools (e.g., ENSTA, ENPC, Centrale Nantes, Sigma Clermont,
Sup Galilée, École Polytechnique, INSA Rennes, and ISAE-SupAéro)
for courses on reliability analysis, risk assessment, and UQ.
Two stable releases are published annually, and the library is
distributed through Conda, pip, and Debian repositories.

The library has been introduced in several publications (e.g.,
[@baudin:2016]).
Its calibration features, for example, are introduced in [@baudin:2021].
The library is also used in several external packages.
The benchmarking module otbenchmark, for example, is introduced in
[@fekhari:2021].
Numerous application articles are based on OpenTURNS (e.g., [@Dutfoy:2023]
on seismic risk assessment using the Gutenberg–Richter law).

The Persalys graphical user interface [@persalys], built on top of OpenTURNS,
enables users to perform UQ analyses with no (or minimal) Python
programming.

An annual User’s Day has been held since 2008, fostering collaboration
and knowledge exchange among researchers, engineers, and educators.

# Conclusion

OpenTURNS provides a robust, extensible, and well-documented platform
for uncertainty quantification and probabilistic modeling.
Its modular design, broad methodological coverage, and open-source nature
make it suitable for both academic research and industrial applications.
By bridging probabilistic modeling, statistical inference, and
reliability methods, OpenTURNS contributes to the development of
reproducible and transparent UQ workflows.

# References
