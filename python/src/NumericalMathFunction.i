// SWIG file NumericalMathFunction.i

%{
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/PythonNumericalMathEvaluationImplementation.hxx"
%}

%include BaseFuncCollection.i

OTTypedInterfaceObjectHelper(NumericalMathFunction)
//OTTypedCollectionInterfaceObjectHelper(NumericalMathFunction)



%typemap(in) const NumericalMathFunctionCollection & {
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    // From interface class, ok
  } else if (SWIG_IsOK(SWIG_ConvertPtr($input, &ptr, SWIG_TypeQuery("OT::Basis *"), 0))) {
    // From Implementation*
    OT::Basis * p_impl = reinterpret_cast< OT::Basis * >( ptr );
    $1 = new OT::Collection<OT::NumericalMathFunction>(*p_impl);
  } else {
    try {
      $1 = OT::buildCollectionFromPySequence< OT::NumericalMathFunction >( $input );
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of NumericalMathFunction");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const NumericalMathFunctionCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0))
    || OT::canConvertCollectionObjectFromPySequence< OT::NumericalMathFunction >( $input )
    || SWIG_IsOK(SWIG_ConvertPtr($input, NULL, SWIG_TypeQuery("OT::Basis *"), 0));
}

%apply const NumericalMathFunctionCollection & { const OT::Collection<OT::NumericalMathFunction> & };

%template(NumericalMathFunctionCollection) OT::Collection<OT::NumericalMathFunction>;
%template(NumericalMathFunctionPersistentCollection) OT::PersistentCollection<OT::NumericalMathFunction>;


%include NumericalMathFunction_doc.i

%ignore OT::NumericalMathFunction::getUseDefaultGradientImplementation;
%ignore OT::NumericalMathFunction::setUseDefaultGradientImplementation;
%ignore OT::NumericalMathFunction::getUseDefaultHessianImplementation;
%ignore OT::NumericalMathFunction::setUseDefaultHessianImplementation;

%include openturns/NumericalMathFunction.hxx

namespace OT {  

%extend NumericalMathFunction {

NumericalMathFunction(PyObject * pyObj)
{
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::Object *"), 0)))
  {
    throw OT::InvalidArgumentException(HERE) << "Argument should be a pure python object";
  }
  return new OT::NumericalMathFunction(OT::convert<OT::_PyObject_, OT::NumericalMathFunction>(pyObj));
}

NumericalMathFunction(const NumericalMathFunction & other)
{
  return new OT::NumericalMathFunction( other );
}

}

}

%pythoncode %{
# We have to make sure the submodule is loaded with absolute path
import openturns.typ

class OpenTURNSPythonFunction(object):
    """
    Override NumericalMathFunction from Python.

    Parameters
    ----------
    n : positive int
        the input dimension
    p : positive int
        the output dimension

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
    """
    def __init__(self, n=0, p=0):
        try:
            self.__n = int(n)
        except:
            raise TypeError('n argument is not an integer.')
        try:
            self.__p = int(p)
        except:
            raise TypeError('p argument is not an integer.')
        self.__descIn = list(map(lambda i: 'x' + str(i), range(n)))
        self.__descOut = list(map(lambda i: 'y' + str(i), range(p)))
        self.__param = []
        self.__paramDesc = []

    def setInputDescription(self, descIn):
        if (len(descIn) != self.__n):
            raise ValueError('Input description size does NOT match input dimension')
        self.__descIn = descIn

    def getInputDescription(self):
        return self.__descIn

    def setOutputDescription(self, descOut):
        if (len(descOut) != self.__p):
            raise ValueError('Output description size does NOT match output dimension')
        self.__descOut = descOut

    def getOutputDescription(self):
        return self.__descOut

    def getInputDimension(self):
        return self.__n

    def getOutputDimension(self):
        return self.__p

    def __str__(self):
        return 'OpenTURNSPythonFunction( %s #%d ) -> %s #%d' % (self.__descIn, self.__n, self.__descOut, self.__p)

    def __repr__(self):
        return self.__str__()

    def __call__(self, X):
        Y = None
        try:
            pt = openturns.typ.NumericalPoint(X)
        except TypeError:
            try:
                ns = openturns.typ.NumericalSample(X)
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
        res = list()
        for point in X:
            res.append(self._exec(point))
        return res

    def _exec_point_on_exec_sample(self, X):
        """Implement exec from exec_sample."""
        return self._exec_sample([X])[0]

    def setParameter(self, param):
        self.__param = param

    def getParameter(self):
        return self.__param

    def setParameterDescription(self, paramDesc):
        self.__paramDesc = paramDesc

    def getParameterDescription(self):
        return self.__paramDesc

def _exec_sample_multiprocessing(func, n_cpus):
    """Return a distributed function using multiprocessing

    Parameters
    ----------
    func : Function or calable
        A callable python object, usually a function. The function should take
        an input vector as argument and return an output vector.

    n_cpus : int
        Number of CPUs on which to distribute the function calls.

    Returns
    -------
    _exec_sample : Function or callable
        The parallelized funtion.
    """

    def _exec_sample(X):
        from multiprocessing import Pool
        p = Pool(processes=n_cpus)
        rs = p.map_async(func, X)
        p.close()
        return rs.get()
    return _exec_sample

class PythonFunction(NumericalMathFunction):
    """
    Override NumericalMathFunction from Python.

    Parameters
    ----------
    n : positive int
        the input dimension
    p : positive int
        the output dimension
    func : a callable python object
        called on a single point.
        Default is None.
    func_sample : a callable python object
        called on multiple points at once.
        Default is None.
    gradient : a callable python objects
        returns the gradient as a 2-d sequence of float.
        Default is None (uses finite-difference).
    hessian : a callable python object
        returns the hessian as a 3-d sequence of float.
        Default is None (uses finite-difference).
    n_cpus : integer
        Number of cpus on which func should be distributed using multiprocessing.
        If -1, it uses all the cpus available. If 1, it does nothing. If n_cpus
        and func_sample are both given as arguments, n_cpus will be ignored and
        samples will be handled by func_sample.
        Default is None.

    Notes
    -----
    You must provide at least func or func_sample arguments. Notice that if
    func_sample is provided, n_cpus is ignored. Note also that if PythonFunction
    is distributed (n_cpus > 1), the traceback of a raised exception by a func
    call is lost due to the way multiprocessing dispatches and handles func
    calls. This can be solved by temporarily deactivating n_cpus during the
    development of the wrapper or by manually handling the distribution of the
    wrapper with external libraries like joblib that keep track of a raised
    exception and shows the traceback to the user.

    Examples
    --------
    >>> import openturns as ot
    >>> def a_exec(X):
    ...     Y = [3.*X[0] - X[1]]
    ...     return Y
    >>> def a_grad(X):
    ...     dY = [[3.], [-1.]]
    ...     return dY
    >>> f = ot.PythonFunction(2, 1, a_exec, gradient=a_grad)
    >>> X = [100., 100.]
    >>> Y = f(X)
    >>> print(Y)
    [200]
    >>> dY = f.gradient(X)
    >>> print(dY)
    [[  3 ]
     [ -1 ]]
    """
    def __new__(self, n, p, func=None, func_sample=None, gradient=None, hessian=None, n_cpus=None):
        if func == None and func_sample == None:
            raise RuntimeError('no func nor func_sample given.')
        instance = OpenTURNSPythonFunction(n, p)
        import collections
        if func != None:
            if not isinstance(func, collections.Callable):
                raise RuntimeError('func argument is not callable.')
            instance._exec = func
        if func_sample != None:
            if not isinstance(func_sample, collections.Callable):
                raise RuntimeError('func_sample argument is not callable.')
            instance._exec_sample = func_sample
            if func == None:
                instance._exec = instance._exec_point_on_exec_sample
        elif n_cpus != None and n_cpus != 1 and func != None:
            if not isinstance(n_cpus, int):
                raise RuntimeError('n_cpus is not an integer')
            if n_cpus == -1:
                import multiprocessing
                n_cpus = multiprocessing.cpu_count()
            instance._exec_sample = _exec_sample_multiprocessing(func, n_cpus)
        if gradient != None:
            if not isinstance(gradient, collections.Callable):
                raise RuntimeError('gradient argument is not callable.')
            instance._gradient = gradient
        if hessian != None:
            if not isinstance(hessian, collections.Callable):
                raise RuntimeError('hessian argument is not callable.')
            instance._hessian = hessian 
        return NumericalMathFunction(instance)
%}
