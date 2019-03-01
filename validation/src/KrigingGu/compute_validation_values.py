#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Test and validate OpenTURNS Kriging Algorithm with Gu methods. 
"""
import openturns as ot
import sys
sys.path.insert(0,'.')
from Gu import Gu

print(ot.__version__)

sampleSize = 6
dimension = 1

f = ot.SymbolicFunction(['x0'], ['x0 * sin(x0)'])
log = ot.SymbolicFunction(['x'], ['log(x)'])

X = ot.Sample([1.0, 3.0, 5.0, 6.0, 7.0, 8.0], dimension)
Y = f(X)

basis = ot.ConstantBasisFactory(dimension).build()

scale = [1.0]
amplitude = [4.123456]
nu = 0.5

def FitKrigingOpenTURNS(scale, amplitude, nu, 
                        prior, parametrization):
    """
    Fit a Kriging with OpenTURNS.
    """
    covarianceModel = ot.MaternModel(scale, amplitude, nu)
    covarianceModel.setScaleParametrization(parametrization)
    algo = ot.KrigingAlgorithm(X, Y, covarianceModel, basis, False)
    algo.setScalePrior(prior)
    algo.run()
    result = algo.getResult()
    print('OpenTURNS')
    scaleOTGU = result.getCovarianceModel().getParameter()
    print('parameters=', scaleOTGU)
    rllfunction = algo.getReducedLogLikelihoodFunction()
    objective_value = rllfunction(scaleOTGU)
    print("Objectif value =", objective_value[0])
    print("")
    return

def ComputeParametrizationName(parametrization):
    if parametrization == ot.CovarianceModelImplementation.STANDARD:
        parametrizationname = 'standard'
    elif parametrization == ot.CovarianceModelImplementation.INVERSE:
        parametrizationname = 'inverse'
    elif parametrization == ot.CovarianceModelImplementation.LOGINVERSE:
        parametrizationname = 'log-inverse'
    else:
        raise ValueError("Unexpected parametrization value %d" % (parametrization))
    return parametrizationname

def ComputePriorName(prior):
    if prior == ot.GeneralLinearModelAlgorithm.REFERENCE:
        priorname = 'reference'
    elif prior == ot.GeneralLinearModelAlgorithm.JOINTLYROBUST:
        priorname = 'jointly-robust'
    else:
        raise ValueError("Unexpected prior value %d" % (prior))
    return priorname

def FitKrigingGu(scale, amplitude, nu, 
                 prior, parametrization):
    """
    Fit a Kriging with Gu class.
    """
    priorname = ComputePriorName(prior)
    parametrizationname = ComputeParametrizationName(parametrization)
        
    print("")
    print('Gu class')
    covarianceModel = ot.MaternModel(scale, amplitude, nu)
    gu = Gu(X, Y, basis, covarianceModel, priorname, parametrizationname)
    result = gu.optimize_scale()
    scaleJOGU = result.x
    
    
    if parametrization == ot.CovarianceModelImplementation.STANDARD:
        print("parameter=", scaleJOGU[0])
    elif parametrization == ot.CovarianceModelImplementation.INVERSE:
        print("parameter=", 1.0 / scaleJOGU[0])
    elif parametrization == ot.CovarianceModelImplementation.LOGINVERSE:
        print("parameter=", log(1.0 / scaleJOGU)[0])
    else:
        raise ValueError("Unexpected parametrization value %d" % (parametrization))
        
    objective_value = gu.set_scale(scaleJOGU)
    print("Objectif value =", - objective_value)
    return

####################################################################
# Test A : prior = REFERENCE, parametrization = STANDARD

prior = ot.GeneralLinearModelAlgorithm.REFERENCE
parametrization = ot.CovarianceModelImplementation.STANDARD
priorname = ComputePriorName(prior)
parametrizationname = ComputeParametrizationName(parametrization)
print("")
print("+ prior", priorname, ", parametrization", parametrizationname)
FitKrigingOpenTURNS(scale, amplitude, nu, prior, parametrization)
FitKrigingGu(scale, amplitude, nu, prior, parametrization)

####################################################################
# Test B : prior = REFERENCE, parametrization = INVERSE

prior = ot.GeneralLinearModelAlgorithm.REFERENCE
parametrization = ot.CovarianceModelImplementation.INVERSE
priorname = ComputePriorName(prior)
parametrizationname = ComputeParametrizationName(parametrization)
print("")
print("+ prior", priorname, ", parametrization", parametrizationname)
FitKrigingOpenTURNS(scale, amplitude, nu, prior, parametrization)
FitKrigingGu(scale, amplitude, nu, prior, parametrization)

####################################################################
# Test C : prior = REFERENCE, parametrization = LOGINVERSE

prior = ot.GeneralLinearModelAlgorithm.REFERENCE
parametrization = ot.CovarianceModelImplementation.LOGINVERSE
priorname = ComputePriorName(prior)
parametrizationname = ComputeParametrizationName(parametrization)
print("")
print("+ prior", priorname, ", parametrization", parametrizationname)
FitKrigingOpenTURNS(scale, amplitude, nu, prior, parametrization)
FitKrigingGu(scale, amplitude, nu, prior, parametrization)

####################################################################
# Test D : prior = JOINTLYROBUST, parametrization = STANDARD

prior = ot.GeneralLinearModelAlgorithm.JOINTLYROBUST
parametrization = ot.CovarianceModelImplementation.STANDARD
priorname = ComputePriorName(prior)
parametrizationname = ComputeParametrizationName(parametrization)
print("")
print("+ prior", priorname, ", parametrization", parametrizationname)
FitKrigingOpenTURNS(scale, amplitude, nu, prior, parametrization)
FitKrigingGu(scale, amplitude, nu, prior, parametrization)

####################################################################
# Test E : prior = JOINTLYROBUST, parametrization = INVERSE

prior = ot.GeneralLinearModelAlgorithm.JOINTLYROBUST
parametrization = ot.CovarianceModelImplementation.INVERSE
priorname = ComputePriorName(prior)
parametrizationname = ComputeParametrizationName(parametrization)
print("")
print("+ prior", priorname, ", parametrization", parametrizationname)
FitKrigingOpenTURNS(scale, amplitude, nu, prior, parametrization)
FitKrigingGu(scale, amplitude, nu, prior, parametrization)

####################################################################
# Test F : prior = JOINTLYROBUST, parametrization = LOGINVERSE

prior = ot.GeneralLinearModelAlgorithm.JOINTLYROBUST
parametrization = ot.CovarianceModelImplementation.LOGINVERSE
priorname = ComputePriorName(prior)
parametrizationname = ComputeParametrizationName(parametrization)
print("")
print("+ prior", priorname, ", parametrization", parametrizationname)
FitKrigingOpenTURNS(scale, amplitude, nu, prior, parametrization)
FitKrigingGu(scale, amplitude, nu, prior, parametrization)

