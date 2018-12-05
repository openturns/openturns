import numpy as np
from rpy2.robjects import Formula, r
from rpy2.robjects.numpy2ri import numpy2ri, ri2py
from rpy2.robjects.packages import importr                                
import openturns as ot
lmtest = importr('lmtest')  

# to test the 3 hypothesis
hypothesis_list = [{'R':'two.sided', 'ot':'Equal'},
                   {'R':'greater', 'ot':'Less'},
                   {'R':'less', 'ot':'Greater'}]

for hypothesis in hypothesis_list:
    print('')
    print('-'*60)
    print('')
    print('Hypothesis H0: ', hypothesis['ot'])
    print('')

    ot.RandomGenerator.SetSeed(224)
    n_points = 30
    distribution = ot.Normal()
    sample = distribution.getSample(n_points)
    sample.stack(distribution.getSample(n_points))
    func = ot.SymbolicFunction(['x1', 'x2'], ['2*x1 - x2 + 1'])
    firstSample = sample
    epsilon = ot.Normal().getSample(n_points)
    secondSample = func(sample) + epsilon
    test_result = ot.LinearModelTest.LinearModelDurbinWatson(firstSample,
                                                             secondSample,
                                                             hypothesis['ot'])
    print('Result from OT :')
    print('p-value :', test_result.getPValue())
    print(test_result.getDescription()[0])
    print('')

    formula = Formula('y ~ x1 + x2') # take into account x1 and x2, not equal to the linear model
    x1 = numpy2ri(np.array(firstSample)[:,0])
    r.assign('x1', x1)

    x2 = numpy2ri(np.array(firstSample)[:,1])
    r.assign('x2', x2)

    y = numpy2ri(np.array(secondSample))
    r.assign('y', y)

    dw_test_1 = lmtest.dwtest(formula, alternative=hypothesis['R'], exact=True) # greater, less, two.sided
    print('Result from R :')
    print('p-value :', dw_test_1[3][0])
    print('Alternative hypothesis : ', dw_test_1[2][0])
    print('dw stat : ', dw_test_1[0][0])


    ################################################################################
    ################################################################################
    ################################################################################
    # code with sample from R objects : it is easier if we want to compare directly in R

    print('')
    print('-'*10)
    print('')
    rnorm = r['rnorm']
    set_seed = r['set.seed']

    ## generate regressor and dependent variable
    set_seed(0)
    X1 = rnorm(n_points)
    X2 = rnorm(n_points)
    err1 = rnorm(n_points)
    output = numpy2ri(ri2py(X1) + ri2py(X1) - ri2py(X2) + ri2py(err1) + 1)
    r.assign('X1', X1)
    r.assign('X2', X2)
    r.assign('output', output)

    ## perform Durbin-Watson test
    formula = Formula('output ~ X1 + X2')
    dw_test_1 = lmtest.dwtest(formula, alternative=hypothesis['R'], exact=True)
    print('Result from R :')
    print('p-value :', dw_test_1[3][0])
    print('Alternative hypothesis : ', dw_test_1[2][0])
    print('dw stat : ', dw_test_1[0][0])
    print('')

    # transformation into openturns objects
    firstSample = ot.Sample(np.column_stack((ri2py(X1), ri2py(X2))))
    secondSample = ot.Sample(np.vstack(ri2py(output)))
    test_result = ot.LinearModelTest.LinearModelDurbinWatson(firstSample,
                                                             secondSample,
                                                             hypothesis['ot'])
    print('Result from OT :')
    print('p-value :', test_result.getPValue())
    print(test_result.getDescription()[0])
