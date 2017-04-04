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
import openturns.typ

class OpenTURNSPythonFieldFunction(object):
    """
    Override FieldFunction from Python.

    Parameters
    ----------
    n : positive int
        the input dimension
    p : positive int
        the output dimension
    s : positive integer
        the spatial dimension

    Notes
    -----
    You have to overload the function:
        _exec(X): single evaluation, X is a :class:`~openturns.Field`,
        returns a :class:`~openturns.Field`
    """
    def __init__(self, n=0, p=0, s=0):
        try:
            self.__n = int(n)
        except:
            raise TypeError('n argument is not an integer.')
        try:
            self.__p = int(p)
        except:
            raise TypeError('p argument is not an integer.')
        try:
            self.__s = int(s)
        except:
            raise TypeError('s argument is not an integer.')
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
        Y = self._exec(X)
        return Y

    def _exec(self, X):
        raise RuntimeError('You must define a method _exec(X) -> Y, where X and Y are Fields objects')

class PythonFieldFunction(FieldFunction):
    """
    Override FieldFunction from Python.

    Parameters
    ----------
    n : positive int
        the input dimension
    p : positive int
        the output dimension
    s : positive int
        the spatial dimension
    func : a callable python object
        called on a :class:`~openturns.Field` object.
        Returns a :class:`~openturns.Field`.
        Default is None.

    Notes
    -----
    func 
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


# deprecated
class DynamicalFunction(FieldFunction):
    def __init__(self, *args):
        super(DynamicalFunction, self).__init__(*args)
        openturns.common.Log.Warn('class DynamicalFunction is deprecated in favor of FieldFunction')

class OpenTURNSPythonDynamicalFunction(OpenTURNSPythonFieldFunction):
    def __init__(self, *args):
        super(OpenTURNSPythonDynamicalFunction, self).__init__(*args)
        openturns.common.Log.Warn('class OpenTURNSPythonDynamicalFunction is deprecated in favor of OpenTURNSPythonFieldFunction')

class PythonDynamicalFunction(PythonFieldFunction):
    def __new__(self, *args):
        common.Log.Warn('class PythonDynamicalFunction is deprecated in favor of PythonFieldFunction')
        return FieldFunction(*args)

%}
