// SWIG file FieldToPointFunction.i

%{
#include "openturns/FieldToPointFunction.hxx"
#include "openturns/PythonFieldToPointFunction.hxx"

namespace OT {

  template <>
  struct traitsPythonType< OT::FieldToPointFunction >
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::FieldToPointFunction >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__FieldToPointFunction, SWIG_POINTER_NO_NULL))) {
      OT::FieldToPointFunction * p_nmf = reinterpret_cast< OT::FieldToPointFunction * >(ptr);
      return p_nmf != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FieldToPointFunctionImplementation, 0 ))) {
      OT::FieldToPointFunctionImplementation * p_impl = reinterpret_cast< OT::FieldToPointFunctionImplementation * >(ptr);
      return p_impl != NULL;
    } else {
      return PyCallable_Check( pyObj );
    }
  }

  template <>
  inline
  OT::FieldToPointFunction
  convert< _PyObject_, OT::FieldToPointFunction >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__FieldToPointFunction, SWIG_POINTER_NO_NULL))) {
      OT::FieldToPointFunction * p_nmf = reinterpret_cast< OT::FieldToPointFunction * >( ptr );
      return *p_nmf;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FieldToPointFunctionImplementation, 0))) {
      OT::FieldToPointFunctionImplementation * p_impl = reinterpret_cast< OT::FieldToPointFunctionImplementation * >(ptr);
      return *p_impl;
    } else if (!PyCallable_Check(pyObj)) {
      throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object (function or class) - can not be convertible to a FieldToPointFunction";
    }
    OT::FieldToPointFunction pythonFunction(new OT::PythonFieldToPointFunction(pyObj));
    return pythonFunction;
  }

} /* namespace OT */

%}

%include FieldToPointFunction_doc.i

OTTypedInterfaceObjectHelper(FieldToPointFunction)

%include openturns/FieldToPointFunction.hxx

namespace OT { 
%extend FieldToPointFunction {

FieldToPointFunction(PyObject * pyObj)
{
  SWIG_PYTHON_THREAD_BEGIN_BLOCK;
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Object, 0)))
  {
    throw OT::InvalidArgumentException(HERE) << "Argument should be a pure python object";
  }
  return new OT::FieldToPointFunction(OT::convert<OT::_PyObject_, OT::FieldToPointFunction>(pyObj));
}

FieldToPointFunction(const FieldToPointFunction & other) { return new OT::FieldToPointFunction( other ); }

}}

%pythoncode %{

class OpenTURNSPythonFieldToPointFunction(object):
    """
    Override FieldToPointFunction from Python.

    Parameters
    ----------
    inputMesh : :class:`~openturns.Mesh`
        The input mesh
    inputDim : positive int
        Dimension of the input field values d
    outputDim : positive int
        Dimension of the output vector d'

    Notes
    -----
    You have to overload the function:
        _exec(X): single evaluation, X is a :class:`~openturns.Field`,
        returns a :class:`~openturns.Field`

    Examples
    --------
    >>> import openturns as ot
    >>> mesh = ot.Mesh(1)
    >>> class FUNC(ot.OpenTURNSPythonFieldToPointFunction):
    ...    def __init__(self):
    ...         # first argument:
    ...         super(FUNC, self).__init__(mesh, 2, 2)
    ...         self.setInputDescription(['R', 'S'])
    ...         self.setOutputDescription(['T', 'U'])
    ...    def _exec(self, X):
    ...         Y = ot.Sample(X).computeMean()
    ...         return Y
    >>> F = FUNC()
    """
    def __init__(self, inputMesh, inputDim, outputDim):
        if not isinstance(inputMesh, openturns.geom.Mesh):
            raise TypeError('inputMesh argument is not a Mesh.')
        self.__inputMesh = inputMesh
        try:
            self.__inputDim = int(inputDim)
        except:
            raise TypeError('inputDim argument is not an integer.')
        try:
            self.__outputDim = int(outputDim)
        except:
            raise TypeError('outputDim argument is not an integer.')
        self.__descIn = ['x' + str(i) for i in range(inputDim)]
        self.__descOut = ['y' + str(i) for i in range(outputDim)]

    def setInputDescription(self, descIn):
        if (len(descIn) != self.__inputDim):
            raise ValueError('Input description size does NOT match input dimension')
        self.__descIn = descIn

    def getInputDescription(self):
        return self.__descIn

    def setOutputDescription(self, descOut):
        if (len(descOut) != self.__outputDim):
            raise ValueError('Output description size does NOT match output dimension')
        self.__descOut = descOut

    def getOutputDescription(self):
        return self.__descOut

    def getInputDimension(self):
        return self.__inputDim

    def getOutputDimension(self):
        return self.__outputDim

    def getInputMesh(self):
        return self.__inputMesh

    def __str__(self):
        return 'OpenTURNSPythonFieldToPointFunction( %s #%d ) -> %s #%d' % (self.__descIn, self.__inputDim, self.__descOut, self.__outputDim)

    def __repr__(self):
        return self.__str__()

    def __call__(self, X):
        Y = None
        if isinstance(X, ProcessSample):
            Y = self._exec_sample(X)
        else:
            Y = self._exec(X)
        return Y

    def _exec(self, X):
        raise RuntimeError('You must define a method _exec(X) -> Y, where X is a Field object and Y a Point object')

    def _exec_sample(self, X):
        res = list()
        for i in range(len(X)):
            res.append(self._exec(X[i]))
        return res

    def _exec_point_on_exec_sample(self, X):
        """Implement exec from exec_sample."""
        return self._exec_sample([X])[0]

class PythonFieldToPointFunction(FieldToPointFunction):
    """
    Override FieldToPointFunction from Python.

    Parameters
    ----------
    inputMesh : :class:`~openturns.Mesh`
        The input mesh
    inputDim : positive int
        Dimension of the input field values d
    outputDim : positive int
        Dimension of the output vector d'
    func : a callable python object
        called on a :class:`~openturns.Field` object.
        Returns a :class:`~openturns.Field`.
        Default is None.

    Examples
    --------
    >>> import openturns as ot
    >>> mesh = ot.Mesh(1)
    >>> def myPyFunc(X):
    ...     Y = ot.Sample(X).computeMean()
    ...     return Y
    >>> inputDim = 2
    >>> outputDim = 2
    >>> myFunc = ot.PythonFieldToPointFunction(mesh, inputDim, outputDim, myPyFunc)
    """
    def __new__(self, inputMesh, inputDim, outputDim, func=None):
        if func is None:
            raise RuntimeError('func not provided.')
        instance = OpenTURNSPythonFieldToPointFunction(inputMesh, inputDim, outputDim)
        if func is not None:
            if not callable(func):
                raise RuntimeError('func argument is not callable.')
            instance._exec = func
        return FieldToPointFunction(instance)


%}
