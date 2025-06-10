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
  - name: Michaël Baudin
    orcid: 0000-0002-0450-7947
    affiliation: 1
  - name: Anne Dutfoy
    affiliation: 1
  - name: Joseph Muré
    affiliation: 1
  - name: Julien Pelamatti
    affiliation: 1
  - name: Régis Lebrun
    affiliation: 2
  - name: Sofiane Haddad
    affiliation: 2
  - name: Julien Schueller
    affiliation: 3
  - name: Loïc Brevault
    affiliation: 4
  - name: Mathieu Balesdent
    affiliation: 4
affiliations:
 - name: EDF R&D, France
   index: 1
 - name: Airbus, France
   index: 2
 - name: Phimeca, France
   index: 3
 - name: ONERA, France
   index: 4
date: 30 April 2025
bibliography: paper.bib
---

# Summary

``OpenTURNS`` is a C++ library for uncertainty propagation by probabilistic methods.
OpenTURNS is also available as a Python module. 

## Introduction
> A travailler pour éviter le plagiat
The needs to assess robust performances for complex systems and to answer tighter regulatory processes (security, safety, environmental control, health impacts, etc.) have led to the emergence of a new industrial simulation challenge: to take uncertainties into account when dealing with complex numerical simulation frameworks. Many attempts at treating uncertainty in large industrial applications have involved domain-specific approaches or standards: metrology, reliability,
differential-based approaches, variance decomposition, etc. However, facing the questioning of their certification authorities in an increasing number of different domains, these domain-specific approaches are no more appropriate. Therefore, a generic methodology has emerged from the joint effort of several industrial companies and academic institutions.

## Overview
OpenTURNS is an open-source software under the LGPL license based on
a C++ library and a Python. OpenTURNS provides the following features:
- Generic to the physical or industrial domains for treating of multi-physical problems
- Structured in a practitioner guidance methodological approach with advanced industrial computing capabilities, various engineering environments, large data models, etc.
- Includes algorithms in order to manage uncertainties in several situations
- in Python, plotting features are provided using Matplotlib.

## Features
The OpenTURNS library provides functions for defining a probabilistic model for a random vector or stochastic process. A multivariate distribution can be defined in a number of ways: directly, by decomposition into marginals and copulas, or by conditioning. For a one-dimensional distribution and a given function, we can determine the push-forward distribution. In addition, the library can be used to define functions acting on these objects, and to combine these functions. The calculation of the gradient or hessian of a function is taken into account. In the context of uncertainty quantification, the library provides services for sampling the law of the input random vector to the physical model and propagating uncertainties to the output. Sampling can be performed using Monte-Carlo, quasi Monte-Carlo and variance reduction methods (importance sampling, line sampling). Different types of experimental designs are implemented including Latin Hypercube Sampling (LHS) and optimized LHS.

The library can also be used to evaluate statistics from a sample. We can estimate the parameters of a probabilistic model including extreme value models (e.g. Generalized Extreme Value distribution and Generalized Pareto distribution) and validate them using fitting tests (e.g. Kolmogorov-Smirnov, Anderson-Darling, etc.) or using graphical tests (e.g. QQ-plot, Kendall plot). Non parametric models can be used based on a sample including kernel density estimation or Bernstein copula. For a given model, variables can be prioritized using sensitivity analysis methods (e.g. SRC, Sobol' and HSIC indices). A function can also be approximated by a meta-model, including linear model, polynomial chaos expansion, Gaussian process regression. A unified interface of optimization algorithms (NLOpt, Ipopt, PAGMO, Bonmin, Dlib, Ceres, Cminpack, Cobyla, etc.) is available and used in particular in the context of parametric model calibration with Bayesian or non-Bayesian methods. Since many probabilistic methods require the evaluation of an integral, numerical integration methods are available by sampling or quadrature (if necessary, adaptive). The provided quadrature methods are the multivariate Gaussian quadrature (e.g. Gauss Legendre quadrature), Gauss-Kronrod, Smolyak quadrature and other algorithms including algorithms from Cuba's library.  The library provides full or compressed linear algebra capabilities (in particular for covariance matrices). Numerous reliability algorithms are implemented for probability calculation and quantification of rare events (e.g. FORM-SORM, subset sampling). Other reliability algorithms such as Non parametric Adaptive Importance Sampling (NAIS) and Cross-Entropy are available.

## Usages
The OpenTURNS library is used for research and engineering studies. OpenTURNS is used for higher education at universities (DTU Denmark) and engineering schools (ENSTA, ENPC, Centrale Nantes, Sigma Clermont, Sup Galilée).

The library is presented in several publications, e.g. `@Baudin:2016`. For example, calibration features of the library are introduced in `@Baudin:2021`. External modules such as the benchmarking module `otbenchmark` is introduced in `@Fekhari:2021`. Numerous application articles are based on OpenTURNS (e.g. `@Dutfoy:2023` on seismic risk assessment using the Gutenberg-Richter law).

The Persalys graphical user interface provides uncertainty quantification features using OpenTURNS (see www.persalys.fr) . Other Persalys-derived graphical tools are also based on OpenTURNS (see `@Bryla:2022`). 

A User's day meeting is organized each year since 2008. The goal of this meeting is to present the use of OpenTURNS in engineering or research uncertainty quantification studies.

# Acknowledgements

We acknowledge contributions from Ivan Dutka-Malen, Sandrine Buriel, Denis Barbier during the genesis of this project.

# References
