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

``OpenTURNS`` is a C++ library for uncertainty propagation by probabilistic methods.
OpenTURNS is also available as a Python module. 

## Introduction
Uncertainties in numerical simulations play a crucial role in enabling robust decision-making for the design and operation of complex systems. Supervisory authorities in safety-critical industrial sectors (e.g. nuclear power or civil aviation) require the use of validated methods, supported by well-tested, documented, and auditable implementations. Consequently, there has been significant growth in the development of uncertainty quantification (UQ) libraries in recent decades. However, many of these tools are tailored to specific disciplines, such as mechanical engineering, computational fluid dynamics or hydrology, which limits the sharing of best practices. This emphasises the need for a generic, cross-disciplinary approach to UQ.

## Overview
OpenTURNS is a generic library for probabilistic modelling and UQ, released under the LGPL license. The software is built on a C++ library and provides a Python package. The library provides comprehensive documentation, including the programming interface, examples, and the underlying probabilistic theory. In the Python environment, plotting features are available through the Matplotlib module.

## Features
The OpenTURNS library provides functions for defining a probabilistic model for a random vector or stochastic process. A multivariate distribution can be defined using existing multivariate distributions, through decomposition into marginals and copulas, or by conditioning. In the context of a one-dimensional distribution and a given function, it is possible to determine the push-forward distribution. Furthermore, the library provides support for the definition and composition of functions. It should be noted that the calculation of the gradient or Hessian of a function is taken into account. Within the UQ framework, the library plays a pivotal role in facilitating the sampling of the input random vector from the probabilistic model. This is followed by the propagation of uncertainties through the physical model to the output. There are a number of sampling methods that can be employed, including Monte Carlo, quasi Monte Carlo and variance reduction methods (such as importance sampling and line sampling). A variety of experimental designs are employed, encompassing Latin Hypercube Sampling (LHS), Optimized LHS and low-discrepancy sequences (e.g., Sobol', Faure, Halton).

When the physical model is available as a Python function, using the library is straightforward. However, evaluating the model directly in Python is not always easy. For physical models that can be executed from the operating system's command line, the library provides coupling tools that wrap the executable as a Python function, using text files for input and output.

It is possible to estimate to evaluate statistics from a sample. It is possible to estimate the parameters of a probabilistic model including extreme value models (e.g., Generalized Extreme Value distribution and Generalized Pareto distribution) and validate them using fitting tests (e.g., Kolmogorov-Smirnov, Anderson-Darling) or using graphical tests (e.g., QQ-plot, Kendall plot). Nonparametric models can be used based on a sample including kernel density estimation or Bernstein copula. For a given model, input variables can be ranked using sensitivity analysis methods e.g., Standardized Regression Coefficients (SRC), Sobol' and Hilbert-Schmidt Independence Criterion (HSIC) indices. A function can also be approximated by a meta-model, including linear model, polynomial chaos expansion, Gaussian process regression. A unified interface to optimization algorithms (NLOpt, Ipopt, PAGMO, Bonmin, Dlib, Ceres, Cminpack, Cobyla, etc.) is available and used in particular in the context of parametric model calibration with Bayesian or non-Bayesian methods. Since many probabilistic methods require the evaluation of an integral, numerical integration methods are available by sampling or quadrature (if necessary, adaptive). The provided quadrature methods include multivariate Gaussian quadrature (e.g., Gauss–Legendre quadrature), Gauss–Kronrod, Smolyak quadrature, and other algorithms including those from the Cuba library. The OpenTURNS library provides full or compressed linear algebra capabilities (in particular for covariance matrices). Numerous reliability algorithms are implemented for probability calculation and quantification of rare events (e.g., First-Order Reliability Method, Second-Order Reliability Method, subset sampling). Other reliability algorithms such as Non parametric Adaptive Importance Sampling (NAIS) and Cross-Entropy are also available.

## Usages
The OpenTURNS library is used for research and engineering studies. OpenTURNS is employed in higher education at universities (DTU Denmark) and engineering schools (e.g., ENSTA, ENPC, Centrale Nantes, Sigma Clermont, Sup Galilée, Ecole Polytechnique, INSA Rennes and ISAE SupAéro).

Two releases are published each year. The library is packaged and distributed via Conda, pip, and Debian.

The library is presented in several publications e.g., `@Baudin:2016`. For example, calibration features of the library are introduced in `@Baudin:2021`. The library is used in several external packages. The benchmarking module `otbenchmark`, for example, is introduced in `@Fekhari:2021`. Numerous application articles are based on OpenTURNS (e.g. `@Dutfoy:2023` on seismic risk assessment using the Gutenberg-Richter law).

The Persalys graphical user interface provides UQ features using OpenTURNS (see www.persalys.fr). Other Persalys-derived graphical tools are also based on OpenTURNS (see `@Bryla:2022`). 

A User's day meeting has been organized each year since 2008. The goal of this meeting is to present the use of OpenTURNS in engineering or research UQ studies. It's also an opportunity to present the new features of each version of the library, as well as the roadmap. A forum is available for discussions about the library (see `@openturns_forum`).

# Acknowledgements

We acknowledge contributions from Ivan Dutka-Malen, Sandrine Buriel, Denis Barbier during the genesis of this project.

# References
