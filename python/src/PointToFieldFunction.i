// SWIG file PointToFieldFunction.i

%{
#include "openturns/PointToFieldFunction.hxx"
#include "openturns/PythonPointToFieldFunction.hxx"

namespace OT {

  template <>
  struct traitsPythonType< OT::PointToFieldFunction >
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  bool
  canConvert< _PyObject_, OT::PointToFieldFunction >(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__PointToFieldFunction, 0 ))) {
      OT::PointToFieldFunction * p_nmf = reinterpret_cast< OT::PointToFieldFunction * >( ptr );
      return p_nmf != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__PointToFieldFunctionImplementation, 0 ))) {
      OT::PointToFieldFunctionImplementation * p_impl = reinterpret_cast< OT::PointToFieldFunctionImplementation * >( ptr );
      return p_impl != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::PointToFieldFunctionImplementation> *"), 0))) {
      OT::Pointer<OT::PointToFieldFunctionImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::PointToFieldFunctionImplementation> * >( ptr );
      return p_impl != NULL && !p_impl->isNull();
    } else {
      return PyCallable_Check( pyObj );
    }
  }

  template <>
  inline
  OT::PointToFieldFunction
  convert< _PyObject_, OT::PointToFieldFunction >(PyObject * pyObj)
  {
    void * ptr = 0;
    if ( SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__PointToFieldFunction, 0 ) ) ) {
      OT::PointToFieldFunction * p_nmf = reinterpret_cast< OT::PointToFieldFunction * >( ptr );
      return *p_nmf;
    } else if ( SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIGTYPE_p_OT__PointToFieldFunctionImplementation, 0 ) ) ) {
      OT::PointToFieldFunctionImplementation * p_impl = reinterpret_cast< OT::PointToFieldFunctionImplementation * >( ptr );
      return *p_impl;
    } else if (SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::Pointer<OT::PointToFieldFunctionImplementation> *"), 0))) {
      OT::Pointer<OT::PointToFieldFunctionImplementation> * p_impl = reinterpret_cast< OT::Pointer<OT::PointToFieldFunctionImplementation> * >( ptr );
      return **p_impl;
    } else if (!PyCallable_Check( pyObj )) {
      throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object (function or class) - can not be convertible to a PointToFieldFunction";
    }
    OT::PointToFieldFunction pythonFunction(new OT::PythonPointToFieldFunction(pyObj));
    return pythonFunction;
  }

} /* namespace OT */

%}

%include PointToFieldFunction_doc.i

OTTypedInterfaceObjectHelper(PointToFieldFunction)

%include openturns/PointToFieldFunction.hxx

namespace OT { 
%extend PointToFieldFunction {

PointToFieldFunction(PyObject * pyObj)
{
  void * ptr = 0;
  if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::Object *"), 0)))
  {
    throw OT::InvalidArgumentException(HERE) << "Argument should be a pure python object";
  }
  return new OT::PointToFieldFunction(OT::convert<OT::_PyObject_, OT::PointToFieldFunction>(pyObj));
}

PointToFieldFunction(const PointToFieldFunction & other) { return new OT::PointToFieldFunction( other ); }

}}

%pythoncode %{
# We have to make sure the submodule is loaded with absolute path
import openturns.typ

class OpenTURNSPythonPointToFieldFunction(object):
    """
    Override PointToFieldFunction from Python.

    Parameters
    ----------
    inputDim : positive int
        Dimension of the input field values d
    outputDim : positive int
        Dimension of the output vector d'
    spatialDim : positive integer
        Dimension of the input domain n

    Notes
    -----
    You have to overload the function:
        _exec(X): single evaluation, X is a :class:`~openturns.Field`,
        returns a :class:`~openturns.Field`

    Examples
    --------
    >>> import openturns as ot
    >>> class FUNC(ot.OpenTURNSPythonPointToFieldFunction):
    ...     def __init__(self):
    ...         super(FUNC, self).__init__(2, 2)
    ...         self.setInputDescription(['R', 'S'])
    ...         self.setOutputDescription(['T', 'U'])
    ...         self.mesh_ = ot.RegularGrid(0.0, 0.1, 11) 
    ...     def _exec(self, X):
    ...         size = self.mesh_.getVerticesNumber()
    ...         values = [ot.Point(X)*i for i in range(size)]
    ...         Y = ot.Field(self.mesh_, values)
    ...         return Y
    >>> F = FUNC()
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
        self.__descIn = ['x' + str(i) for i in range(n)]
        self.__descOut = ['y' + str(i) for i in range(p)]

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
        return 'OpenTURNSPythonPointToFieldFunction( %s #%d ) -> %s #%d' % (self.__descIn, self.__n, self.__descOut, self.__p)

    def __repr__(self):
        return self.__str__()

    def __call__(self, X):
        Y = None
        try:
            pt = openturns.typ.Point(X)
        except:
            try:
                sp = openturns.typ.Sample(X)
            except:
                raise TypeError('Expect a Point or a Sample as argument')
            else:
                Y = self._exec_sample(sp)
        else:
            Y = self._exec(pt)
        return Y

    def _exec(self, X):
        raise RuntimeError('You must define a method _exec(X) -> Y, where X is a Point object and Y a Field objects')

    def _exec_sample(self, X):
        if len(X) == 0:
            return FieldSample(Mesh(), 0, self.getOutputDimension())
        res = FieldSample(1, self._exec(X[0]))
        for i in range(1, len(X)):
            res.add(self._exec(X[i]))
        return res

    def _exec_point_on_exec_sample(self, X):
        """Implement exec from exec_sample."""
        return self._exec_sample([X])[0]

class PythonPointToFieldFunction(PointToFieldFunction):
    """
    Override PointToFieldFunction from Python.

    Parameters
    ----------
    inputDim : positive int
        Dimension of the input field values d
    outputDim : positive int
        Dimension of the output vector d'
    spatialDim : positive integer
        Dimension of the input domain n
    func : a callable python object
        called on a :class:`~openturns.Field` object.
        Returns a :class:`~openturns.Field`.
        Default is None.

    Examples
    --------
    >>> import openturns as ot
    >>> def  myPyFunc(X):
    ...     mesh = ot.RegularGrid(0.0, 0.1, 11)
    ...     size = 11
    ...     values = [ot.Point(X)*i for i in range(size)]
    ...     Y = ot.Field(mesh, values)
    ...     return Y
    >>> inputDim = 2
    >>> outputDim = 2
    >>> spatialDim = 1
    >>> myFunc = ot.PythonPointToFieldFunction(inputDim, outputDim, spatialDim, myPyFunc)

    Evaluation on a vector:

    >>> Yfield = myFunc([1.1, 2.2])
    """
    def __new__(self, n, p, s, func=None):
        if func == None:
            raise RuntimeError('func not provided.')
        instance = OpenTURNSPythonPointToFieldFunction(n, p, s)
        import collections
        if func != None:
            if not isinstance(func, collections.Callable):
                raise RuntimeError('func argument is not callable.')
            instance._exec = func
        return PointToFieldFunction(instance)


%}
