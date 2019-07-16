// SWIG file Function.i

%{
#include "openturns/Function.hxx"
#include "openturns/PythonEvaluation.hxx"
%}

%include BaseFuncCollection.i

OTTypedInterfaceObjectHelper(Function)
//OTTypedCollectionInterfaceObjectHelper(Function)



%typemap(in) const FunctionCollection & {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIG_TypeQuery("OT::Basis *"), 0))) {
    // From Implementation*
    OT::Basis * p_impl = reinterpret_cast< OT::Basis * >( ptr );
    $1 = new OT::Collection<OT::Function>(*p_impl);
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence< OT::Function >( $input );
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Function");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const FunctionCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< OT::Function >( $input )
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIG_TypeQuery("OT::Basis *"), 0));
}

%apply const FunctionCollection & { const OT::Collection<OT::Function> & };

%template(FunctionCollection) OT::Collection<OT::Function>;
%template(FunctionPersistentCollection) OT::PersistentCollection<OT::Function>;


%include Function_doc.i

%ignore OT::Function::getUseDefaultGradientImplementation;
%ignore OT::Function::setUseDefaultGradientImplementation;
%ignore OT::Function::getUseDefaultHessianImplementation;
%ignore OT::Function::setUseDefaultHessianImplementation;

%include openturns/Function.hxx

namespace OT {

%extend Function {

Function(PyObject * pyObj)
{
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Object, 0)))
  {
    throw OT::InvalidArgumentException(HERE) << "Argument should be a pure python object";
  }
  return new OT::Function(OT::convert<OT::_PyObject_, OT::Function>(pyObj));
}

Function(const Function & other)
{
  return new OT::Function( other );
}


}

}

%pythoncode %{
# We have to make sure the submodule is loaded with absolute path
import openturns.typ

class OpenTURNSPythonFunction(object):
    """
    Override Function from Python.

    Parameters
    ----------
    inputDim : positive int
        Dimension of the input vector
    outputDim : positive int
        Dimension of the output vector

    Notes
    -----
    You have to overload the function:
        _exec(X): single evaluation, X is a sequence of float,
        returns a sequence of float

    You can also optionally override these functions:
        _exec_sample(X): multiple evaluations, X is a 2-d sequence of float,
        returns a 2-d sequence of float

        _gradient(X): gradient, X is a sequence of float,
        returns a 2-d sequence of float

        _hessian(X): hessian, X is a sequence of float,
        returns a 3-d sequence of float

    Examples
    --------
    >>> import openturns as ot
    >>> class FUNC(OpenTURNSPythonFunction):
    ...     def __init__(self):
    ...         super(FUNC, self).__init__(2, 1)
    ...         self.setInputDescription(['R', 'S'])
    ...         self.setOutputDescription(['T'])
    ...     def _exec(self, X):
    ...         Y = [X[0] + X[1]]
    ...         return Y
    >>> F = FUNC()

    Create the associated Function:

    >>> myFunc = Function(F)
    """
    def __init__(self, n=0, p=0):
        try:
            self.__n = int(n)
        except:
            raise TypeError('inputDim argument is not an integer.')
        try:
            self.__p = int(p)
        except:
            raise TypeError('outputDim argument is not an integer.')
        self.__descIn = ['x' + str(i) for i in range(n)]
        self.__descOut = ['y' + str(i) for i in range(p)]

    def setInputDescription(self, descIn):
        """Input description accessor."""
        if (len(descIn) != self.__n):
            raise ValueError('Input description size does NOT match input dimension')
        self.__descIn = descIn

    def getInputDescription(self):
        """Input description accessor."""
        return self.__descIn

    def setOutputDescription(self, descOut):
        """Output description accessor."""
        if (len(descOut) != self.__p):
            raise ValueError('Output description size does NOT match output dimension')
        self.__descOut = descOut

    def getOutputDescription(self):
        """Output description accessor."""
        return self.__descOut

    def getInputDimension(self):
        """Input dimension accessor."""
        return self.__n

    def getOutputDimension(self):
        """Output dimension accessor."""
        return self.__p

    def __str__(self):
        return 'OpenTURNSPythonFunction( %s #%d ) -> %s #%d' % (self.__descIn, self.__n, self.__descOut, self.__p)

    def __repr__(self):
        return self.__str__()

    def __call__(self, X):
        Y = None
        try:
            pt = openturns.typ.Point(X)
        except TypeError:
            try:
                ns = openturns.typ.Sample(X)
            except TypeError:
                raise TypeError('Expect a 1-d or 2-d sequence of float as argument')
            else:
                Y = self._exec_sample(ns)
        else:
            Y = self._exec(pt)
        return Y

    def _exec(self, X):
        raise RuntimeError('You must define a method _exec(X) -> Y, where X and Y are 1-d sequence of float')

    def _exec_sample(self, X):
        """Implement exec_sample from exec."""
        return [self._exec(point) for point in X]

    def _exec_point_on_exec_sample(self, X):
        """Implement exec from exec_sample."""
        return self._exec_sample([X])[0]

def _exec_point_on_func_sample(func_sample):
    """Return a _exec function.

    Parameters
    ----------
    func_sample : Function or callable
        A callable python object. Called when evaluated on multiple points at once.

    Returns
    -------
    _exec : Function or callable
        The exec point function.
    """
    def _exec(X):
        return func_sample([X])[0]
    return _exec

def _exec_sample_multiprocessing_func(func, n_cpus):
    """Return a distributed function using multiprocessing.

    Parameters
    ----------
    func : Function or callable
        A callable python object, usually a function. The function should take
        an input vector as argument and return an output vector.

    n_cpus : int
        Number of CPUs on which to distribute the function calls.

    Returns
    -------
    _exec_sample : Function or callable
        The parallelized function.
    """
    def _exec_sample(X):
        try:
            from multiprocessing import Pool
            p = Pool(processes=n_cpus)
        except:
            # multiprocessing is not working on this platform,
            # fallback to sequential computations.
            return [func(point) for point in X]
        rs = p.map_async(func, X)
        p.close()
        return rs.get()
    return _exec_sample

def _exec_sample_multiprocessing_func_sample(func_sample, n_cpus):
    """Return a distributed function using multiprocessing.

    Parameters
    ----------
    func_sample : Function or callable
        A callable python object. Called when evaluated on multiple points at once.

    n_cpus : int
        Number of CPUs on which to distribute the function calls.

    Returns
    -------
    _exec_sample : Function or callable
        The parallelized function.
    """
    def _exec_sample(X):
        import warnings
        try:
            from multiprocessing import Pool
            p = Pool(processes=n_cpus)
        except:
            # multiprocessing is not working on this platform,
            # fallback to sequential computations.
            warnings.warn("-- Multiprocessing is not working on this platform"
                  " fallback to sequential computations.")
            return func_sample(X)
        try:
            import numpy as np
        except:
            warnings.warn("-- Without numpy multiprocessing is not efficient")
            p.close()
            return func_sample(X)

        nsim = len(X)
        if nsim < n_cpus:
            return func_sample(X)

        a = np.array(X, copy=False)
        rs = p.map(func_sample, [a[r:r+(nsim//n_cpus), :] for r in range(0, nsim, nsim//n_cpus)])
        p.close()
        p.join()
        return np.vstack(rs)
    return _exec_sample

class PythonFunction(Function):
    """
    Override Function from Python.

    Parameters
    ----------
    inputDim : positive int
        Dimension of the input vector
    outputDim : positive int
        Dimension of the output vector
    func : a callable python object, optional
        Called when evaluated on a single point.
        Default is None.
    func_sample : a callable python object, optional
        Called when evaluated on multiple points at once.
        Default is None.
    gradient : a callable python objects, optional
        Returns the gradient as a 2-d sequence of float.
        Default is None (uses finite-difference).
    hessian : a callable python object, optional
        Returns the hessian as a 3-d sequence of float.
        Default is None (uses finite-difference).
    n_cpus : integer
        Number of cpus on which func should be distributed using multiprocessing.
        If -1, it uses all the cpus available. If 1, it does nothing.
        Default is None.
    copy : bool, optional
        If True, input sample is converted into a Python 2-d sequence before calling
        func_sample.  Otherwise, it is passed directy to func_sample.
        Default is False.
        
    //TODO: doc isLinear

    You must provide at least func or func_sample arguments.  For efficiency
    reasons, these functions do not receive a :class:`~openturns.Point` or
    :class:`~openturns.Sample` as arguments, but a proxy object which gives
    access to internal object data.  This object supports indexing, but nothing
    more.  It must be wrapped into anoter object, for instance
    :class:`~openturns.Point` in func and :class:`~openturns.Sample` in
    func_sample, or in a Numpy array, for vectorized operations.

    Notes
    -----
    Note that if PythonFunction is distributed (n_cpus > 1), the traceback of a raised
    exception by a func call is lost due to the way multiprocessing dispatches
    and handles func calls. This can be solved by temporarily deactivating
    n_cpus during the development of the wrapper or by manually handling the
    distribution of the wrapper with external libraries like joblib that keep
    track of a raised exception and shows the traceback to the user.

    Examples
    --------
    >>> import openturns as ot
    >>> def a_exec(X):
    ...     Y = [3.0 * X[0] - X[1]]
    ...     return Y
    >>> def a_grad(X):
    ...     dY = [[3.0], [-1.0]]
    ...     return dY
    >>> f = ot.PythonFunction(2, 1, a_exec, gradient=a_grad)
    >>> X = [100.0, 100.0]
    >>> Y = f(X)
    >>> print(Y)
    [200]
    >>> dY = f.gradient(X)
    >>> print(dY)
    [[  3 ]
     [ -1 ]]

    Same example, but optimized for best performance with Numpy when function
    is going to be evaluated on large samples.

    >>> import openturns as ot
    >>> import numpy as np
    >>> def a_exec_sample(X):
    ...     Xarray = np.array(X, copy=False)
    ...     Y = 3.0 * Xarray[:,0] - Xarray[:,1]
    ...     return np.expand_dims(Y, axis=1)
    >>> def a_grad(X):
    ...     dY = [[3.0], [-1.0]]
    ...     return dY
    >>> f = ot.PythonFunction(2, 1, func_sample=a_exec_sample, gradient=a_grad)
    >>> X = [100.0, 100.0]
    >>> Y = f(X)
    >>> print(Y)
    [200]
    >>> dY = f.gradient(X)
    >>> print(dY)
    [[  3 ]
     [ -1 ]]
    """
    def __new__(self, n, p, func=None, func_sample=None, gradient=None, hessian=None, n_cpus=None, copy=False, functionLinearity=None, variablesLinearity=None):
        if func is None and func_sample is None:
            raise RuntimeError('no func nor func_sample given.')
        instance = OpenTURNSPythonFunction(n, p)
        if copy:
            instance._discard_openturns_memoryview = True
        import collections

        if n_cpus is not None:
            if not isinstance(n_cpus, int):
                raise RuntimeError('n_cpus is not an integer')
            if n_cpus == -1:
                import multiprocessing
                n_cpus = multiprocessing.cpu_count()
            if n_cpus <= 1:
                n_cpus = None

        if func is not None:
            if not isinstance(func, collections.Callable):
                raise RuntimeError('func argument is not callable.')
            instance._exec = func
        else:
            instance._exec = _exec_point_on_func_sample(func_sample)

        instance._has_exec = True

        if func_sample is not None:
            if n_cpus is not None:
                instance._exec_sample = _exec_sample_multiprocessing_func_sample(func_sample, n_cpus)
            else:
                instance._exec_sample = func_sample
            instance._has_exec_sample = True
        else:
            if n_cpus is not None:
                instance._exec_sample = _exec_sample_multiprocessing_func(func, n_cpus)
                instance._has_exec_sample = True

        if gradient is not None:
            if not isinstance(gradient, collections.Callable):
                raise RuntimeError('gradient argument is not callable.')
            instance._gradient = gradient
        if hessian is not None:
            if not isinstance(hessian, collections.Callable):
                raise RuntimeError('hessian argument is not callable.')
            instance._hessian = hessian 
            
        if functionLinearity is not None:
            instance.isLinear = functionLinearity
            
        if variablesLinearity is not None:
            instance.isVariableLinear = variablesLinearity
                          
        return Function(instance)
%}
