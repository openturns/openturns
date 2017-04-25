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
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FieldToPointFunction, 0 ))) {
      OT::FieldToPointFunction * p_nmf = reinterpret_cast< OT::FieldToPointFunction * >( ptr );
      return p_nmf != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FieldToPointFunctionImplementation, 0 ))) {
      OT::FieldToPointFunctionImplementation * p_impl = reinterpret_cast< OT::FieldToPointFunctionImplementation * >( ptr );
      return p_impl != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::FieldToPointFunctionImplementation> *"), 0))) {
      OT::Pointer<OT::FieldToPointFunctionImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::FieldToPointFunctionImplementation> * >( ptr );
      return p_impl != NULL && !p_impl->isNull();
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
    if ( SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FieldToPointFunction, 0 ) ) ) {
      OT::FieldToPointFunction * p_nmf = reinterpret_cast< OT::FieldToPointFunction * >( ptr );
      return *p_nmf;
    } else if ( SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__FieldToPointFunctionImplementation, 0 ) ) ) {
      OT::FieldToPointFunctionImplementation * p_impl = reinterpret_cast< OT::FieldToPointFunctionImplementation * >( ptr );
      return *p_impl;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::FieldToPointFunctionImplementation> *"), 0))) {
      OT::Pointer<OT::FieldToPointFunctionImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::FieldToPointFunctionImplementation> * >( ptr );
      return **p_impl;
    } else if (!PyCallable_Check( pyObj )) {
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
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::Object *"), 0)))
  {
    throw OT::InvalidArgumentException(HERE) << "Argument should be a pure python object";
  }
  return new OT::FieldToPointFunction(OT::convert<OT::_PyObject_, OT::FieldToPointFunction>(pyObj));
}

FieldToPointFunction(const FieldToPointFunction & other) { return new OT::FieldToPointFunction( other ); }

}}

%pythoncode %{
# We have to make sure the submodule is loaded with absolute path
import openturns.typ

class OpenTURNSPythonFieldToPointFunction(object):
    """
    Override FieldToPointFunction from Python.

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
        return 'OpenTURNSPythonFieldToPointFunction( %s #%d ) -> %s #%d' % (self.__descIn, self.__n, self.__descOut, self.__p)

    def __repr__(self):
        return self.__str__()

    def __call__(self, X):
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
        instance = OpenTURNSPythonFieldToPointFunction(n, p, s)
        import collections
        if func != None:
            if not isinstance(func, collections.Callable):
                raise RuntimeError('func argument is not callable.')
            instance._exec = func
        return FieldToPointFunction(instance)


%}
