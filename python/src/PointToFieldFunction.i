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
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::PointToFieldFunction *"), SWIG_POINTER_NO_NULL))) {
      OT::PointToFieldFunction * p_nmf = reinterpret_cast< OT::PointToFieldFunction * >(ptr);
      return p_nmf != NULL;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::PointToFieldFunctionImplementation *"), SWIG_POINTER_NO_NULL))) {
      OT::PointToFieldFunctionImplementation * p_impl = reinterpret_cast< OT::PointToFieldFunctionImplementation * >(ptr);
      return p_impl != NULL;
    } else {
      return PyCallable_Check(pyObj);
    }
  }

  template <>
  inline
  OT::PointToFieldFunction
  convert< _PyObject_, OT::PointToFieldFunction >(PyObject * pyObj)
  {
    void * ptr = 0;
    if ( SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::PointToFieldFunction *"), SWIG_POINTER_NO_NULL))) {
      OT::PointToFieldFunction * p_nmf = reinterpret_cast< OT::PointToFieldFunction * >(ptr);
      return *p_nmf;
    } else if ( SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIG_TypeQuery("OT::PointToFieldFunctionImplementation *"), SWIG_POINTER_NO_NULL))) {
      OT::PointToFieldFunctionImplementation * p_impl = reinterpret_cast< OT::PointToFieldFunctionImplementation * >(ptr);
      return *p_impl;
    } else if (!PyCallable_Check(pyObj)) {
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

class OpenTURNSPythonPointToFieldFunction:
    r"""
    Override PointToFieldFunction from Python.

    Parameters
    ----------
    inputDim : positive int
        Dimension :math:`\inputDim` of the input vector
    outputMesh : :class:`~openturns.Mesh`
        The output mesh
    outputDim : positive int
        Dimension of the output field values d'

    Notes
    -----
    A OpenTURNSPythonPointToFieldFunction acts on points to produce fields:

    .. math::

        f: \left| \begin{array}{rcl}
                   \Rset^{d} &  \rightarrow & \cM_{N'} \times (\Rset^{d'})^{N'}  \\
                    \vect{v} & \mapsto  & F' 
                  \end{array} \right.

    with  :math:`\cM_{N'}` a mesh of :math:`\cD' \subset \Rset^{n'}`.

    A field is represented by a collection :math:`(\vect{t}'_i, \vect{v}'_i)_{1 \leq i \leq N''}` of elements of :math:`\cM_{N'} \times (\Rset^{d'})^{N'}` where :math:`\vect{t}'_i` is a vertex of :math:`\cM_{N'}` and :math:`\vect{v}'_i` the associated value in :math:`\Rset^{d'}`.
    
    Using the class OpenTURNSPythonPointToFieldFunction allows one to define a persistent state between the evaluations of the function.
    
    At least, you have to overload the function:
        _exec(X): a single evaluation, where X is a :class:`~openturns.Point`. It returns
        a :class:`~openturns.Field`.

    Examples
    --------    
    For example, we create the function which maps the point :math:`\vect{x}\in \Rset^2` into the field where the output values are :math:`(\vect{O}, \vect{x}, 2\vect{x}, \dots, 10\vect{x})` on the regular grid :math:`(0, 0.1, \dots, 1.0)`.
    
    >>> import openturns as ot
    >>> class FUNC(ot.OpenTURNSPythonPointToFieldFunction):
    ...     def __init__(self):
    ...         mesh = ot.RegularGrid(0.0, 0.1, 11)
    ...         super(FUNC, self).__init__(2, mesh, 2)
    ...         self.setInputDescription(['R', 'S'])
    ...         self.setOutputDescription(['T', 'U'])
    ...     def _exec(self, X):
    ...         size = self.getOutputMesh().getVerticesNumber()
    ...         Y = [ot.Point(X)*i for i in range(size)]
    ...         return Y
    >>> F = FUNC()
    """
    def __init__(self, inputDim, outputMesh, outputDim):
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
        """
        Accessor to the description of the input values of the function.

        Parameters
        ----------
        descIn : sequence of str
            The description of the input values of the function.    
        """
        if (len(descIn) != self.__inputDim):
            raise ValueError('Input description size does NOT match input dimension')
        self.__descIn = descIn

    def getInputDescription(self):
        """
        Accessor to the description of the input values of the function.

        Returns
        -------
        descIn : sequence of str
            The description of the input values of the function.    
        """
        return self.__descIn

    def setOutputDescription(self, descOut):
        """
        Accessor to the description of the output field values.

        Parameters
        ----------
        descOut : sequence of str
            The description of theof the output field values of the function.    
        """
        if (len(descOut) != self.__outputDim):
            raise ValueError('Output description size does NOT match output dimension')
        self.__descOut = descOut

    def getOutputDescription(self):
        """
        Accessor to the description of the output field values.

        Returns
        -------
        descOut : sequence of str
            The description of the output field values of the function.    
        """
        return self.__descOut

    def getInputDimension(self):
        r"""
        Accessor to the dimension of the input point of the function.

        Returns
        -------
        inputPointDim : int
            The dimension of the input point of the function :math:`\inputDim`.
        """
        return self.__inputDim

    def getOutputDimension(self):
        r"""
        Accessor to the dimension of the output field values.

        Returns
        -------
        outputFieldDim : int
            The dimension of the output field values :math:`d'`.
        """
        return self.__outputDim

    def getOutputMesh(self):
        """
        Accessor to the mesh of the output field of the function.

        Returns
        -------
        outputMesh : int
            The mesh of the output field of the function.
        """
        return self.__outputMesh

    def __str__(self):
        return f"OpenTURNSPythonPointToFieldFunction( {self.__descIn} #{self.__inputDim} ) -> {self.__descOut} #{self.__outputDim}"

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
        raise RuntimeError('You must define a method _exec(X) -> Y, where X is a Point object and Y a Field object')

    def _exec_sample(self, X):
        res = ProcessSample(self.getOutputMesh(), 0, self.getOutputDimension())
        for i in range(len(X)):
            res.add(self._exec(X[i]))
        return res

    def _exec_point_on_exec_sample(self, X):
        """Implement exec from exec_sample."""
        return self._exec_sample([X])[0]

class PythonPointToFieldFunction(PointToFieldFunction):
    r"""
    Override PointToFieldFunction from Python.

    Parameters
    ----------
    inputDim : positive int
        Dimension of the input vector d
    outputMesh : :class:`~openturns.Mesh`
        The output mesh
    outputDim : positive int
        Dimension of the output field values d'
    func : a callable python object
        called on a :class:`~openturns.Field` object.
        Returns a :class:`~openturns.Field`.
        Default is None.

    Notes
    -----
    A PythonPointToFieldFunction acts on points to produce fields:

    .. math::

        f: \left| \begin{array}{rcl}
                   \Rset^{d} &  \rightarrow & \cM_{N'} \times (\Rset^{d'})^{N'}  \\
                    \vect{v} & \mapsto  & F' 
                  \end{array} \right.

    with  :math:`\cM_{N'}` a mesh of :math:`\cD' \subset \Rset^{n'}`.

    A field is represented by a collection :math:`(\vect{t}'_i, \vect{v}'_i)_{1 \leq i \leq N'}` of elements of :math:`\cM_{N'} \times (\Rset^{d'})^{N'}` where :math:`\vect{t}'_i` is a vertex of :math:`\cM_{N'}` and :math:`\vect{v}'_i` the associated value in :math:`\Rset^{d'}`.

    Examples
    --------
    For example, we create the function which maps the point :math:`\vect{x}\in \Rset^2` into the field where the output values are :math:`(\vect{O}, \vect{x}, 2\vect{x}, \dots, 10\vect{x})` on the regular grid :math:`(0, 0.1, \dots, 1.0)`.
    
    >>> import openturns as ot
    >>> mesh = ot.RegularGrid(0.0, 0.1, 11)
    >>> def  myPyFunc(X):
    ...     size = 11
    ...     Y = [ot.Point(X)*i for i in range(size)]
    ...     return Y
    >>> inputDim = 2
    >>> outputDim = 2
    >>> myFunc = ot.PythonPointToFieldFunction(inputDim, mesh, outputDim, myPyFunc)

    Evaluation on a vector:

    >>> Yfield = myFunc([1.1, 2.2])
    """
    def __new__(self, inputDim, outputMesh, outputDim, func=None):
        if func is None:
            raise RuntimeError('func not provided.')
        instance = OpenTURNSPythonPointToFieldFunction(inputDim, outputMesh, outputDim)
        if func is not None:
            if not callable(func):
                raise RuntimeError('func argument is not callable.')
            instance._exec = func
        return PointToFieldFunction(instance)


%}
