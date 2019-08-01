// SWIG file RandomVector.i

%{
#include "openturns/RandomVector.hxx"
#include "openturns/PythonRandomVector.hxx"
%}

%include RandomVector_doc.i

%pythoncode %{
class PythonRandomVector(object):
    """
    Allow to overload RandomVector from Python.

    Parameters
    ----------
    dim : positive int
        Vector dimension.
        Default is 0.

    See also
    --------
    RandomVector

    Examples
    --------
    >>> import openturns as ot
    >>> ot.RandomGenerator.SetSeed(0)

    Overload RandomVector from Python:

    >>> class RVEC(ot.PythonRandomVector):
    ...    def __init__(self):
    ...        super(RVEC, self).__init__(2)
    ...        self.setDescription(['R', 'S'])
    ...
    ...    def getRealization(self):
    ...        X = [ot.RandomGenerator.Generate(), 2 + ot.RandomGenerator.Generate()]
    ...        return X
    ...
    ...    def getSample(self, size):
    ...        X = []
    ...        for i in range(size):
    ...            X.append([ot.RandomGenerator.Generate(), 2 + ot.RandomGenerator.Generate()])
    ...        return X
    ...
    ...    def getMean(self):
    ...        return [0.5, 2.5]
    ...
    ...    def getCovariance(self):
    ...        return [[0.0833333, 0.], [0., 0.0833333]]

    Use the overloaded class:

    >>> R = RVEC()
    >>> # Instance creation
    >>> myRV = ot.RandomVector(R)
    >>> # Realization
    >>> print(myRV.getRealization())
    [0.629877,2.88281]
    >>> # Sample
    >>> print(myRV.getSample(5))
    0 : [ 0.135276  2.0325    ]
    1 : [ 0.347057  2.96942   ]
    2 : [ 0.92068   2.50304   ]
    3 : [ 0.0632061 2.29276   ]
    4 : [ 0.714382  2.38336   ]
    >>> # Mean
    >>> print(myRV.getMean())
    [0.5,2.5]
    >>> # Covariance
    >>> print(myRV.getCovariance())
    [[ 0.0833333 0         ]
     [ 0         0.0833333 ]]

    """
    def __init__(self, dim=0):
        # Warning: these names are used in PythonRandomVector class. Synchronize the files if changed
        self.__dim = dim
        self.__desc = ['x' + str(i) for i in range(dim)]

    def __str__(self):
        return 'PythonRandomVector -> %s #%d' % (self.__desc, self.__dim)

    def __repr__(self):
        return self.__str__()

    def getDimension(self):
        """
        Get the dimension.

        Returns
        -------
        dim : positive int
            Dimension of the RandomVector.
        """
        return self.__dim

    def setDescription(self, desc):
        """
        Set the description.

        Parameters
        ----------
        desc : sequence of str
            *desc* describes the components of the RandomVector.
            Its size must be equal to the dimension of the RandomVector.
        """
        if (len(desc) != self.__dim):
            raise ValueError('Description size does NOT match dimension')
        self.__desc = desc

    def getDescription(self):
        """
        Get the description.

        Returns
        -------
        desc : :class:`~openturns.Description`
            *desc* describes the components of the RandomVector.
        """
        return self.__desc

    def getRealization(self):
        """
        Get a realization of the random vector.

        Returns
        -------
        realization : :class:`~openturns.Point`
            Sequence of values randomly determined from the RandomVector definition.
        """
        raise RuntimeError('You must define a method getRealization() -> X, where X is a Point')

    def getMean(self):
        """
        Get the mean.

        Returns
        -------
        mean : :class:`~openturns.Point`
            Mean of the RandomVector.
        """
        raise RuntimeError('You must define a method mean -> X, where X is a Point')

    def getCovariance(self):
        """
        Get the covariance.

        Returns
        -------
        covariance : :class:`~openturns.CovarianceMatrix`
            Covariance of the RandomVector.
        """
        raise RuntimeError('You must define a method var -> M, where M is a CovarianceMatrix')

%}

OTTypedInterfaceObjectHelper(RandomVector)

%include openturns/RandomVector.hxx
namespace OT { %extend RandomVector { 

RandomVector(const RandomVector & other)
{
  return new OT::RandomVector(other);
} 

RandomVector(PyObject * pyObj)
{
  return new OT::RandomVector( new OT::PythonRandomVector(pyObj) );
} 

} // class RandomVector
} // namespace OT
