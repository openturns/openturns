// SWIG file Distribution.i

%{
#include "openturns/Distribution.hxx"
#include "openturns/PythonDistribution.hxx"
%}

%include Distribution_doc.i

%include UncertaintyModelCopulaCollection.i

OTTypedInterfaceObjectHelper(Distribution)
OTTypedCollectionInterfaceObjectHelper(Distribution)

%ignore OT::Distribution::pow;
%ignore OT::Distribution::setWeight;
%ignore OT::Distribution::getWeight;

%include openturns/Distribution.hxx

namespace OT {

%extend Distribution {

Distribution(const Distribution & other)
{
  return new OT::Distribution(other);
}

Distribution(PyObject * pyObj)
{
  return new OT::Distribution( new OT::PythonDistribution( pyObj ) );
}

Distribution __add__ (Scalar s)
{
 return *self + s;
}

Distribution __radd__ (Scalar s)
{
 return *self + s;
}

Distribution __sub__(Scalar s)
{
 return *self - s;
}

Distribution __rsub__(Scalar s)
{
  return (*self * (-1.0)) + s;
}

Distribution __neg__()
{
  return *self * (-1.0);
}

Distribution __mul__(Scalar s)
{
 return (*self) * s;
}

Distribution __rmul__(Scalar s)
{
 return (*self) * s;
}

Distribution __div__(Scalar s)
{
 return (*self) / s;
}

Distribution __pow__(const Scalar s) { return self->pow(s); }

Distribution __rdiv__(Scalar s) { return self->inverse() * s; }

Distribution __rtruediv__(Scalar s) { return self->inverse() * s; }

#if SWIG_VERSION < 0x030011
  Distribution __truediv__(Scalar s) { return (*self) / s; }
#endif

} // class Distribution
} // namespace OT

%pythoncode %{
from openturns.typ import Interval
import math

class PythonDistribution:
    """
    Allow one to override Distribution from Python.

    Parameters
    ----------
    dim : positive int
        the distribution dimension

    Examples
    --------
    Not useful on its own, see the examples section on how to inherit from it.
    """
    def __init__(self, dim=0):
        """
        Constructor.
        """
        self.__dim = dim

    def __str__(self):
        return 'PythonDistribution -> #%d' % self.__dim

    def __repr__(self):
        return self.__str__()

    def getDimension(self):
        """
        Dimension accessor.
        """
        return self.__dim

    def computeCDF(self, X):
        """
        CDF accessor.
        """
        raise RuntimeError('You must define a method computeCDF(x) -> cdf, where cdf is a float')


class SciPyDistribution(PythonDistribution):
    """
    Allow one to override Distribution from a scipy distribution.

    Parameters
    ----------
    dist : a scipy.stats distribution
        The distribution to wrap

    Examples
    --------
    >>> import openturns as ot
    >>> import scipy.stats as st  # doctest: +SKIP
    >>> scipy_dist = st.johnsonsu(2.55, 2.25)  # doctest: +SKIP
    >>> distribution = ot.Distribution(ot.SciPyDistribution(scipy_dist))  # doctest: +SKIP
    >>> distribution.getRealization()  # doctest: +SKIP
    """
    def __init__(self, dist):
        super(SciPyDistribution, self).__init__(1)
        if dist.__class__.__name__ not in ('rv_frozen', 'rv_continuous_frozen', 'rv_histogram', 'rv_discrete_frozen'):
            raise TypeError('Argument is not a scipy 1D distribution')
        self._dist = dist

        # compute range
        from openturns import ResourceMap
        cdf_epsilon = ResourceMap.GetAsScalar('Distribution-DefaultCDFEpsilon')
        lb, ub = dist.support()
        flb = lb != float('-inf')
        fub = ub != float('+inf')
        if not flb:
            lb = dist.ppf(cdf_epsilon)
        if not fub:
            ub = dist.ppf(0.5 + (0.5 - cdf_epsilon))
        self.__range = Interval([lb], [ub])
        self.__range.setFiniteLowerBound([int(flb)])
        self.__range.setFiniteUpperBound([int(fub)])

    def getRange(self):
        return self.__range

    def getSupport(self, interval):
        # assume discrete distributions are integral
        intersection = self.getRange().intersect(interval)
        kMin = math.ceil(intersection.getLowerBound()[0])
        kMax = math.floor(intersection.getUpperBound()[0])
        return [[k] for k in range(kMin, kMax + 1)]

    def getRealization(self):
        rvs = self._dist.rvs()
        return [rvs]

    def getSample(self, size):
        rvs = self._dist.rvs(size)
        return rvs.reshape(size, 1)

    def computePDF(self, X):
        x0 = float(X[0])
        if self.isContinuous():
            pdf = self._dist.pdf(x0)
        else:
            if x0.is_integer():
                pdf = self._dist.pmf(x0)
            else:
                # newer scipy pmf is a continuous function
                pdf = 0.0
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
        kurtosis = float(self._dist.stats('k')) + 3.0
        return [kurtosis]

    def getMoment(self, n):
        moment = self._dist.moment(n)
        return [moment]

    def isContinuous(self):
        return hasattr(self._dist.dist, 'pdf')

    def isDiscrete(self):
        return not self.isContinuous()

    def isIntegral(self):
        # assume discrete distributions are integral
        return self.isDiscrete()

    def computeScalarQuantile(self, p, tail=False):
        q = self._dist.ppf(1.0 - p if tail else p)
        return q

    def computeQuantile(self, prob, tail=False):
        p = 1.0 - prob if tail else prob
        q = self._dist.ppf(p)
        return [q]

    def getParameter(self):
        return self._dist.args

    def setParameter(self, params):
        size = len(self._dist.args)
        if len(params) != size:
            raise ValueError('Parameter must be of size %d' % size)
        self._dist.args = params

    def getParameterDescription(self):
        size = len(self._dist.args)
        return ['parameter' + str(i + 1) for i in range(size)]


class ChaospyDistribution(PythonDistribution):
    """
    Allow one to override Distribution from a chaospy distribution.

    Parameters
    ----------
    dist : a chaospy distribution
        The distribution to wrap. It is currently limited to stochastically
        independent distributions as chaopy distributions doesn't implement CDF
        computation for dependencies.

    Examples
    --------
    >>> import openturns as ot
    >>> import chaospy as cp  # doctest: +SKIP
    >>> chaospy_dist = cp.J(cp.Triangular(1.0, 2.0, 3.0), cp.F(4.0, 5.0))  # doctest: +SKIP
    >>> distribution = ot.Distribution(ot.ChaospyDistribution(chaospy_dist))  # doctest: +SKIP
    >>> distribution.getRealization()  # doctest: +SKIP
    """
    def __init__(self, dist):
        super(ChaospyDistribution, self).__init__(len(dist))
        from chaospy import Iid, J, __version__
        independent = len(dist) == 1
        independent |= isinstance(dist, Iid)
        if __version__ < '4.0.0':
            from chaospy import get_dependencies
            independent |= isinstance(dist, J) and not get_dependencies(*dist)
        else:
            independent |= not dist.stochastic_dependent
        if not independent:
            raise Exception(
                "Dependent chaospy distributions doesn't implement CDF computation")
        self._dist = dist
        self.__range = Interval(dist.lower, dist.upper)

    def getRange(self):
        return self.__range

    def getRealization(self):
        rvs = self._dist.sample(1).flatten()
        return rvs

    def getSample(self, size):
        rvs = self._dist.sample(size).T
        return rvs.reshape(size, len(self._dist))

    def computePDF(self, X):
        pdf = self._dist.pdf(X)
        return pdf

    def computeCDF(self, X):
        cdf = self._dist.cdf(X)
        return cdf

    def getMean(self):
        import chaospy as cp
        mean = cp.E(self._dist).reshape([len(self._dist)])
        return mean

    def getStandardDeviation(self):
        import chaospy as cp
        std = cp.Std(self._dist).reshape([len(self._dist)])
        return std

    def getSkewness(self):
        import chaospy as cp
        skewness = cp.Skew(self._dist).reshape([len(self._dist)])
        return skewness

    def getKurtosis(self):
        import chaospy as cp
        kurtosis = cp.Kurt(self._dist).reshape([len(self._dist)]) + 3.0
        return kurtosis

    def getMoment(self, n):
        moment = self._dist.mom([n] * len(self._dist))
        return moment

    def computeScalarQuantile(self, p, tail=False):
        if len(self._dist) > 1:
            raise Exception(
                "Multivariate distribution doesn't implement scalar quantile")
        q = self._dist.inv(1 - p if tail else p)
        return float(q)

    def computeQuantile(self, prob, tail=False):
        p = 1.0 - prob if tail else prob
        q = self._dist.inv(p).flatten()
        return q
%}
