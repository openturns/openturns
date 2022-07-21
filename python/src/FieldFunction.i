// SWIG file FieldFunction.i

%{
#include "openturns/FieldFunction.hxx"
#include "openturns/PythonFieldFunction.hxx"

namespace OT {

  template <>
  struct traitsPythonType< OT::FieldFunction >
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::FieldFunction >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__FieldFunction, SWIG_POINTER_NO_NULL))) {
      OT::FieldFunction * p_nmf = reinterpret_cast< OT::FieldFunction * >(ptr);
      return p_nmf != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__FieldFunctionImplementation, SWIG_POINTER_NO_NULL))) {
      OT::FieldFunctionImplementation * p_impl = reinterpret_cast< OT::FieldFunctionImplementation * >( ptr );
      return p_impl != NULL;
    } else {
      return PyCallable_Check(pyObj);
    }
  }

  template <>
  inline
  OT::FieldFunction
  convert< _PyObject_, OT::FieldFunction >(PyObject * pyObj)
  {
    void * ptr = 0;
    if ( SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FieldFunction, 0 ) ) ) {
      OT::FieldFunction * p_nmf = reinterpret_cast< OT::FieldFunction * >( ptr );
      return *p_nmf;
    } else if ( SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__FieldFunctionImplementation, SWIG_POINTER_NO_NULL))) {
      OT::FieldFunctionImplementation * p_impl = reinterpret_cast< OT::FieldFunctionImplementation * >(ptr);
      return *p_impl;
    } else if (!PyCallable_Check(pyObj)) {
      throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object (function or class) - can not be convertible to a FieldFunction";
    }
    OT::FieldFunction pythonFunction(new OT::PythonFieldFunction(pyObj));
    return pythonFunction;
  }

} /* namespace OT */

%}

%include FieldFunction_doc.i

OTTypedInterfaceObjectHelper(FieldFunction)

%include openturns/FieldFunction.hxx

namespace OT { 
%extend FieldFunction {

FieldFunction(PyObject * pyObj)
{
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Object, 0)))
  {
    throw OT::InvalidArgumentException(HERE) << "Argument should be a pure python object";
  }
  return new OT::FieldFunction(OT::convert<OT::_PyObject_, OT::FieldFunction>(pyObj));
}

FieldFunction(const FieldFunction & other) { return new OT::FieldFunction( other ); }

}}

%pythoncode %{
# We have to make sure the submodule is loaded with absolute path
import openturns.geom

class OpenTURNSPythonFieldFunction(object):
    r"""
    Override FieldFunction from Python.

    Parameters
    ----------
    inputMesh : :class:`~openturns.Mesh`
        The input mesh
    inputDim : int, :math:`\geq 1`
        Dimension :math:`d` of the values of the input field
    outputMesh : :class:`~openturns.Mesh`
        The output mesh
    outputDim : int, :math:`\geq 1`
        Dimension :math:`d'` of the values of the output field

    Notes
    -----
    You have to overload the function:
        _exec(X): single evaluation, X is a :class:`~openturns.Field`,
        returns a :class:`~openturns.Field`

    Examples
    --------
    >>> import openturns as ot
    >>> mesh = ot.Mesh(1)
    >>> class FUNC(ot.OpenTURNSPythonFieldFunction):
    ...     def __init__(self):
    ...         # first argument:
    ...         super(FUNC, self).__init__(mesh, 2, mesh, 2)
    ...         self.setInputDescription(['R', 'S'])
    ...         self.setOutputDescription(['T', 'U'])
    ...     def _exec(self, X):
    ...         Y = ot.Field(self.getOutputMesh(), X.getValues() * ([2.0]*X.getValues().getDimension()))
    ...         return Y
    >>> F = FUNC()

    Create the associated FieldFunction: 

    >>> myFunc = ot.FieldFunction(F)
    """
    def __init__(self, inputMesh, inputDim, outputMesh, outputDim):
        if not isinstance(inputMesh, openturns.geom.Mesh):
            raise TypeError('inputMesh argument is not a Mesh.')
        self.__inputMesh = inputMesh
        try:
            self.__inputDim = int(inputDim)
        except:
            raise TypeError('inputDim argument is not an integer.')
        if not isinstance(outputMesh, openturns.geom.Mesh):
            raise TypeError('outputMesh argument is not a Mesh.')
        self.__outputMesh = outputMesh
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

    def getOutputMesh(self):
        return self.__outputMesh

    def __str__(self):
        return 'OpenTURNSPythonFieldFunction( %s #%d ) -> %s #%d' % (self.__descIn, self.__inputDim, self.__descOut, self.__outputDim)

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
        raise RuntimeError('You must define a method _exec(X) -> Y, where X and Y are Fields objects')

    def _exec_sample(self, X):
        res = ProcessSample(self.getOutputMesh(), 0, self.getOutputDimension())
        for i in range(len(X)):
            res.add(self._exec(X[i]))
        return res

    def _exec_point_on_exec_sample(self, X):
        """Implement exec from exec_sample."""
        return self._exec_sample([X])[0]

class PythonFieldFunction(FieldFunction):
    r"""
    Override FieldFunction from Python.

    Parameters
    ----------
    inputMesh : :class:`~openturns.Mesh`
        The input mesh
    inputDim : int, :math:`\geq 1`
        Dimension :math:`d` of the values of the input field
    outputMesh : :class:`~openturns.Mesh`
        The output mesh
    outputDim : int, :math:`\geq 1`
        Dimension :math:`d'` of the values of the output field 
    func : a callable python object
        called on a :class:`~openturns.Field` object.
        Returns a :class:`~openturns.Field`.
        Default is None.

    Examples
    --------
    >>> import openturns as ot
    >>> mesh = ot.Mesh(1)
    >>> def myPyFunc(X):
    ...     mesh = X.getMesh()
    ...     values = X.getValues() * ([2.0]*X.getValues().getDimension())
    ...     values.setDescription(ot.Description.BuildDefault(values.getDimension(), 'Y'))
    ...     Y = ot.Field(mesh, values)
    ...     return Y
    >>> inputDim = 2
    >>> outputDim = 2
    >>> myFunc = ot.PythonFieldFunction(mesh, inputDim, mesh, outputDim, myPyFunc)
    """
    def __new__(self, inputMesh, inputDim, outputMesh, outputDim, func=None):
        if func is None:
            raise RuntimeError('func not provided.')
        instance = OpenTURNSPythonFieldFunction(inputMesh, inputDim, outputMesh, outputDim)
        if func is not None:
            if not callable(func):
                raise RuntimeError('func argument is not callable.')
            instance._exec = func
        return FieldFunction(instance)


%}
