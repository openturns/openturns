// SWIG file Distribution.i

%{
#include "Distribution.hxx"
#include "PythonDistributionImplementation.hxx"
%}

%include Distribution_doc.i

%pythoncode %{
from openturns.typ import Interval

class PythonDistribution(object):
    """
    Allow to override Distribution from Python.

    Parameters
    ----------
    dim : positive int
        the distribution dimension
    """
    def __init__(self, dim=0):
        self.__dim = dim

    def __str__(self):
        return 'PythonDistribution -> #%d' % self.__dim

    def __repr__(self):
        return self.__str__()

    def getDimension(self):
        return self.__dim

    def computeCDF(self, X):
        raise RuntimeError('You must define a method computePDF(x) -> cdf, where cdf is a float')


class SciPyDistribution(PythonDistribution):
    """
    Allow to override Distribution from a scipy distribution.

    Parameters
    ----------
    dist : a scipy.stats distribution
        the distribution to wrap
    """
    def __init__(self, dist):
        super(SciPyDistribution, self).__init__(1)
        if dist.__class__.__name__ != 'rv_frozen':
            raise TypeError('Argument is not a scipy distribution')
        self._dist = dist

        # compute range
        lb = dist.ppf(0.)
        ub = dist.ppf(1.)
        flb = lb != float('-inf')
        fub = ub != float('+inf')
        self.__range = Interval([lb], [ub])
        self.__range.setFiniteLowerBound([int(flb)])
        self.__range.setFiniteUpperBound([int(fub)])

    def getRange(self):
        return self.__range

    def getRealization(self):
        rvs = self._dist.rvs()
        return [rvs]

    def getSample(self, size):
        rvs = self._dist.rvs(size)
        return rvs.reshape(size, 1)

    def computePDF(self, X):
        pdf = self._dist.pdf(X[0])
        return pdf

    def computeCDF(self, X):
        cdf = self._dist.cdf(X[0])
        return cdf

    def getMean(self):
        mean = float(self._dist.stats('m'))
        return [mean]

    def getStandardDeviation(self):
        var = float(self._dist.stats('v'))
        std = var ** 0.5
        return [std]

    def getSkewness(self):
        skewness = float(self._dist.stats('s'))
        return [skewness]

    def getKurtosis(self):
        kurtosis = float(self._dist.stats('k'))
        return [kurtosis]

    def getMoment(self, n):
        moment = self._dist.moment(n)
        return [moment]
%}

%include UncertaintyModelCopulaCollection.i

OTTypedInterfaceObjectHelper(Distribution)
OTTypedCollectionInterfaceObjectHelper(Distribution)

%ignore OT::Distribution::pow;

%include Distribution.hxx

namespace OT {  

%extend Distribution {

Distribution(const Distribution & other)
{
  return new OT::Distribution(other);
}

Distribution(PyObject * pyObj)
{
  return new OT::Distribution( new OT::PythonDistributionImplementation( pyObj ) );
} 

Distribution __add__ (NumericalScalar s)
{
 return *self + s;
}

Distribution __radd__ (NumericalScalar s)
{
 return *self + s;
}

Distribution __sub__(NumericalScalar s)
{
 return *self - s;
}

Distribution __rsub__(NumericalScalar s)
{
  return (*self * (-1.0)) + s;
}

Distribution __mul__(NumericalScalar s)
{
 return (*self) * s;
}

Distribution __rmul__(NumericalScalar s)
{
 return (*self) * s;
}

Distribution __div__(NumericalScalar s)
{
 return (*self) / s;
}

Distribution __truediv__(NumericalScalar s) { return (*self) / s; }

} // class Distribution
} // namespace OT
