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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FieldFunction, 0 ))) {
      OT::FieldFunction * p_nmf = reinterpret_cast< OT::FieldFunction * >( ptr );
      return p_nmf != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FieldFunctionImplementation, 0 ))) {
      OT::FieldFunctionImplementation * p_impl = reinterpret_cast< OT::FieldFunctionImplementation * >( ptr );
      return p_impl != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::FieldFunctionImplementation> *"), 0))) {
      OT::Pointer<OT::FieldFunctionImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::FieldFunctionImplementation> * >( ptr );
      return p_impl != NULL && !p_impl->isNull();
    } else {
      return PyCallable_Check( pyObj );
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
    } else if ( SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FieldFunctionImplementation, 0 ) ) ) {
      OT::FieldFunctionImplementation * p_impl = reinterpret_cast< OT::FieldFunctionImplementation * >( ptr );
      return *p_impl;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::FieldFunctionImplementation> *"), 0))) {
      OT::Pointer<OT::FieldFunctionImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::FieldFunctionImplementation> * >( ptr );
      return **p_impl;
    } else if (!PyCallable_Check( pyObj )) {
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
  if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::Object *"), 0)))
  {
    throw OT::InvalidArgumentException(HERE) << "Argument should be a pure python object";
  }
  return new OT::FieldFunction(OT::convert<OT::_PyObject_, OT::FieldFunction>(pyObj));
}

FieldFunction(const FieldFunction & other) { return new OT::FieldFunction( other ); }

}}

%pythoncode %{
# We have to make sure the submodule is loaded with absolute path
import openturns.func

class OpenTURNSPythonFieldFunction(object):
    """
    Override FieldFunction from Python.

    Parameters
    ----------
    spatialDim : int, :math:`\geq 1`
        Dimension :math:`n` of the domain :math:`\cD`
    inputDim : int, :math:`\geq 1`
        Dimension :math:`d` of the values of the input field
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
    
    >>> class FUNC(ot.OpenTURNSPythonFieldFunction):
    ...     def __init__(self):
    ...         # first argument:
    ...         super(FUNC, self).__init__(2, 2, 1)
    ...         self.setInputDescription(['R', 'S'])
    ...         self.setOutputDescription(['T', 'U'])
    ...     def _exec(self, X):
    ...         Y = ot.Field(X.getMesh(), X.getValues() * ([2.0]*X.getValues().getDimension()))
    ...         return Y
    >>> F = FUNC()

    Create the associated FieldFunction: 

    >>> myFunc = ot.FieldFunction(F)
    """
    def __init__(self, n=0, p=0, s=0):
        try:
            self.__n = int(n)
        except:
            raise TypeError('inputDim argument is not an integer.')
        try:
            self.__p = int(p)
        except:
            raise TypeError('outputDim argument is not an integer.')
        try:
            self.__s = int(s)
        except:
            raise TypeError('spatialDim argument is not an integer.')
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

    def getSpatialDimension(self):
        return self.__s

    def __str__(self):
        return 'OpenTURNSPythonFieldFunction( %s #%d ) -> %s #%d' % (self.__descIn, self.__n, self.__descOut, self.__p)

    def __repr__(self):
        return self.__str__()

    def __call__(self, X):
        Y = None
        try:
            fld = Field(X)
        except:
            try:
                ps = ProcessSample(X)
            except:
                raise TypeError('Expect a Field or a ProcessSample as argument')
            else:
                Y = self._exec_sample(ps)
        else:
            Y = self._exec(fld)
        return Y

    def _exec(self, X):
        raise RuntimeError('You must define a method _exec(X) -> Y, where X and Y are Fields objects')

    def _exec_sample(self, X):
        if len(X) == 0:
            return ProcessSample(Mesh(), 0, self.getOutputDimension())
        res = ProcessSample(1, self._exec(X[0]))
        for i in range(1, len(X)):
            res.add(self._exec(X[i]))
        return res

    def _exec_point_on_exec_sample(self, X):
        """Implement exec from exec_sample."""
        return self._exec_sample([X])[0]

class PythonFieldFunction(FieldFunction):
    """
    Override FieldFunction from Python.

    Parameters
    ----------
    inputDim : int, :math:`\geq 1`
        Dimension :math:`d` of the values of the input field
    outputDim : int, :math:`\geq 1`
        Dimension :math:`d'` of the values of the output field 
    spatialDim : int, :math:`\geq 1`
        Dimension :math:`n` of the domain :math:`\cD`
    func : a callable python object
        called on a :class:`~openturns.Field` object.
        Returns a :class:`~openturns.Field`.
        Default is None.

    Examples
    --------
    >>> import openturns as ot
    >>> def myPyFunc(X):
    ...     mesh = X.getMesh()
    ...     values = X.getValues() * ([2.0]*X.getValues().getDimension())
    ...     values.setDescription(ot.Description.BuildDefault(values.getDimension(), 'Y'))
    ...     Y = ot.Field(mesh, values)
    ...     return Y
    >>> inputDim = 2
    >>> outputDim = 2
    >>> spatialDim = 1
    >>> myFunc = ot.PythonFieldFunction(inputDim, outputDim, spatialDim, myPyFunc)
    """
    def __new__(self, n, p, s, func=None):
        if func == None:
            raise RuntimeError('func not provided.')
        instance = OpenTURNSPythonFieldFunction(n, p, s)
        import collections
        if func != None:
            if not isinstance(func, collections.Callable):
                raise RuntimeError('func argument is not callable.')
            instance._exec = func
        return FieldFunction(instance)


%}
