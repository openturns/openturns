// SWIG file NumericalMathFunction.i

%{
#include "NumericalMathFunction.hxx"
#include "PythonNumericalMathEvaluationImplementation.hxx"
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
    $1 = OT::buildCollectionFromPySequence< OT::NumericalMathFunction >( $input );
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

%include NumericalMathFunction.hxx

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

    Notes
    -----
    You may provide either one of func or func_sample arguments

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
    def __new__(self, n, p, func=None, func_sample=None, gradient=None, hessian=None):
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
