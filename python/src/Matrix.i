// SWIG file Matrix.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Matrix & ROut ($*ltype temp) %{ temp = OT::Matrix(); $1 = &temp; %}
%typemap(argout) OT::Matrix & ROut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Matrix(*$1), SWIG_TypeQuery("OT::Matrix *"), SWIG_POINTER_OWN |  0 )); %}

%apply OT::Matrix & ROut { OT::Matrix & uOut, OT::Matrix & vTOut, OT::Matrix & vOut };

%typemap(in, numinputs=0) OT::ComplexMatrix & vOut ($*ltype temp) %{ temp = OT::ComplexMatrix(); $1 = &temp; %}
%typemap(argout) OT::ComplexMatrix & vOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::ComplexMatrix(*$1), SWIG_TypeQuery("OT::ComplexMatrix *"), SWIG_POINTER_OWN |  0 )); %}

%{
#include "openturns/Matrix.hxx"
%}

%include Matrix_doc.i

%ignore OT::Matrix::getRow;
%ignore OT::Matrix::getColumn;
%rename(__baseaddress__) OT::Matrix::data;
%rename(__elementsize__) OT::Matrix::elementSize;
%rename(__stride__) OT::Matrix::stride;

%template(_MatrixImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::MatrixImplementation>;
%apply const ScalarCollection & { const OT::Matrix::ScalarCollection & };

%define OTMatrixGetAccessor(baseType, elementType, pythonElementType)
PyObject * __getitem__(PyObject * args) const {

  Py_ssize_t start1 = 0;
  Py_ssize_t stop1 = 0;
  Py_ssize_t step1 = 0;
  Py_ssize_t slicelength1 = 0;

  // case #0: [slice] => Matrix
  if (PySlice_Check(args))
  { 
    if (PySlice_Unpack(args, &start1, &stop1, &step1) < 0)
      throw OT::InvalidArgumentException(HERE) << "Matrix.__getitem__: PySlice_Unpack failed";
    slicelength1 = PySlice_AdjustIndices(self->getNbRows(), &start1, &stop1, step1);

    OT::baseType result(slicelength1, self->getNbColumns());
    for( OT::UnsignedInteger j = 0; j < self->getNbColumns(); ++ j )
    {
      for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
      {
        result.operator()(i, j) = self->operator()( start1 + i*step1, j );
      }
    }
    return SWIG_NewPointerObj((new OT::baseType(static_cast< const OT::baseType& >(result))), SWIG_TypeQuery("OT::" #baseType " *"), SWIG_POINTER_OWN |  0 );
  }

  PyObject * obj1 = 0 ;
  PyObject * obj2 = 0 ;
  
  // argument values
  OT::UnsignedInteger arg2 = 0;
  OT::UnsignedInteger arg3 = 0;
  
  Py_ssize_t start2 = 0;
  Py_ssize_t stop2 = 0;
  Py_ssize_t step2 = 0;
  Py_ssize_t slicelength2 = 0;

  if (!PyArg_ParseTuple(args,(char *)"OO:" #baseType "___getitem__",&obj1,&obj2)) SWIG_fail;

  // convert first list argument 
  if (PySlice_Check(obj1))
  { 
    if (PySlice_Unpack(obj1, &start1, &stop1, &step1) < 0)
      throw OT::InvalidArgumentException(HERE) << "Matrix.__getitem__: PySlice_Unpack failed";
    slicelength1 = PySlice_AdjustIndices(self->getNbRows(), &start1, &stop1, step1);
  }
  else
  {
    long val2 ;
    int ecode2 = 0 ;
    ecode2 = SWIG_AsVal_long(obj1, &val2);
    if (!SWIG_IsOK(ecode2)) {
      SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" #baseType "___getitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
    }
    if (val2 < 0) {
      val2 += self->getNbRows();
    }
    arg2 = static_cast< OT::UnsignedInteger >(val2);
  }

  // convert second list argument
  if (PySlice_Check(obj2))
  {
    if (PySlice_Unpack(obj2, &start2, &stop2, &step2) < 0)
      throw OT::InvalidArgumentException(HERE) << "Matrix.__getitem__: PySlice_Unpack failed";
    slicelength2 = PySlice_AdjustIndices(self->getNbColumns(), &start2, &stop2, step2);
  }
  else
  {
    long val3 ;
    int ecode3 = 0 ;
    ecode3 = SWIG_AsVal_long(obj2, &val3);
    if (!SWIG_IsOK(ecode3)) {
      SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" #baseType "___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
    }
    if (val3 < 0) {
      val3 += self->getNbColumns();
    }
    arg3 = static_cast< OT::UnsignedInteger >(val3);
  }

  // handle arguments
  if ( PySlice_Check( obj1 ) )
  {

    if ( PySlice_Check( obj2 ) )
    {
      // case #1: [slice/slice] => baseType
      OT::baseType result( slicelength1, slicelength2 );
      for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
      {
        for ( Py_ssize_t j = 0; j < slicelength2; ++ j )
        {
          result.operator()(i, j) = self->operator()( start1 + i*step1, start2 + j*step2 );
        }
      }
      return SWIG_NewPointerObj((new OT::baseType(static_cast< const OT::baseType& >(result))), SWIG_TypeQuery("OT::" #baseType " *"), SWIG_POINTER_OWN |  0 );
    }
    else
    {
      // case #2: [slice/index] => baseType
      OT::baseType result( slicelength1, 1 );
      for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
      {
        result.operator()(i, 0) = self->operator()( start1 + i*step1, arg3 );
      }
      return SWIG_NewPointerObj((new OT::baseType(static_cast< const OT::baseType& >(result))), SWIG_TypeQuery("OT::" #baseType " *"), SWIG_POINTER_OWN |  0 );
    }

  }
  else
  {
    if ( PySlice_Check( obj2 ) )
    {
      // case #3: [index/slice] => baseType
      OT::baseType result( 1, slicelength2 );
      for ( Py_ssize_t j = 0; j < slicelength2; ++ j )
      {
        result.operator()(0, j) = self->operator()( arg2, start2 + j*step2 );
      }
      return SWIG_NewPointerObj((new OT::baseType(static_cast< const OT::baseType& >(result))), SWIG_TypeQuery("OT::" #baseType " *"), SWIG_POINTER_OWN |  0 );
    }
    else
    {  
      // case #4: [index/index] => elementType
      return OT::convert< OT::elementType, OT::pythonElementType>( self->operator()(arg2, arg3) );
    }
  }
fail:
  return NULL;
}
%enddef

%define OTMatrixSetAccessor(baseType, elementType, pythonElementType)
PyObject * __setitem__(PyObject * args, PyObject * valObj) {

  Py_ssize_t start1 = 0;
  Py_ssize_t stop1 = 0;
  Py_ssize_t step1 = 0;
  Py_ssize_t slicelength1 = 0;

  // case #0: [slice] <= baseType
  if (PySlice_Check(args))
  {
    if (PySlice_Unpack(args, &start1, &stop1, &step1) < 0)
      throw OT::InvalidArgumentException(HERE) << "Matrix.__setitem__: PySlice_Unpack failed";
    slicelength1 = PySlice_AdjustIndices(self->getNbRows(), &start1, &stop1, step1);

    OT::baseType temp2 ;
    OT::baseType *val2 = 0 ;
    if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIG_TypeQuery("OT::" #baseType " *"), SWIG_POINTER_NO_NULL))) {
      temp2 = OT::convert< OT::_PySequence_, OT::baseType >(valObj);
      val2 = &temp2;
    }
    assert( val2 );
    for ( OT::UnsignedInteger j = 0; j < val2->getNbColumns(); ++ j)
    {
      for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
      {
        self->operator()( start1 + i*step1, j ) = val2->operator()(i, j);
      }
    }
    return SWIG_Py_Void();
  }

  PyObject * obj1 = 0 ;
  PyObject * obj2 = 0 ;

  // argument values
  OT::UnsignedInteger arg2 = 0;
  OT::UnsignedInteger arg3 = 0;
  
  Py_ssize_t start2 = 0;
  Py_ssize_t stop2 = 0;
  Py_ssize_t step2 = 0;
  Py_ssize_t slicelength2 = 0;

  if (!PyArg_ParseTuple(args,(char *)"OO:" #baseType "___getitem__",&obj1,&obj2)) SWIG_fail;

  // convert first list argument 
  if (PySlice_Check(obj1))
  { 
    if (PySlice_Unpack(obj1, &start1, &stop1, &step1) < 0)
      throw OT::InvalidArgumentException(HERE) << "Matrix.__setitem__: PySlice_Unpack failed";
    slicelength1 = PySlice_AdjustIndices(self->getNbRows(), &start1, &stop1, step1);
  }
  else
  {
    long val2 ;
    int ecode2 = 0 ;
    ecode2 = SWIG_AsVal_long(obj1, &val2);
    if (!SWIG_IsOK(ecode2)) {
      SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" #baseType "___setitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
    }
    if (val2 < 0) {
      val2 += self->getNbRows();
    }
    arg2 = static_cast< OT::UnsignedInteger >(val2);
  }

  // convert second list argument
  if (PySlice_Check(obj2))
  {
    if (PySlice_Unpack(obj2, &start2, &stop2, &step2) < 0)
      throw OT::InvalidArgumentException(HERE) << "Matrix.__setitem__: PySlice_Unpack failed";
    slicelength2 = PySlice_AdjustIndices(self->getNbColumns(), &start2, &stop2, step2);
  }
  else
  {
    long val3 ;
    int ecode3 = 0 ;
    ecode3 = SWIG_AsVal_long(obj2, &val3);
    if (!SWIG_IsOK(ecode3)) {
      SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" #baseType "___setitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
    }
    if (val3 < 0) {
      val3 += self->getNbColumns();
    }
    arg3 = static_cast< OT::UnsignedInteger >(val3);
  }

  // handle arguments
  if ( PySlice_Check( obj1 ) )
  {

    if ( PySlice_Check( obj2 ) )
    {
      // case #1: [slice/slice] <= baseType
      OT::baseType temp2 ;
      OT::baseType *val2 = 0 ;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIG_TypeQuery("OT::" #baseType " *"), SWIG_POINTER_NO_NULL))) {
        temp2 = OT::convert<OT::_PySequence_,OT::baseType>( valObj );
        val2 = &temp2;
      }
      for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
      {
        for ( Py_ssize_t j = 0; j < slicelength2; ++ j )
        {
          self->operator()( start1 + i*step1, start2 + j*step2 ) = val2->operator()(i, j);
        }
      }
    }
    else
    {
      // case #2: [slice/index] <= baseType
      OT::baseType temp2 ;
      OT::baseType *val2 = 0 ;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIG_TypeQuery("OT::" #baseType " *"), SWIG_POINTER_NO_NULL))) {
        temp2 = OT::convert<OT::_PySequence_,OT::baseType>( valObj );
        val2 = &temp2;
      }
      for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
      {
        self->operator()( start1 + i*step1, arg3 ) = val2->operator()(i, 0);
      }
    }

  }
  else
  {
    if (PySlice_Check(obj2))
    {
      // case #3: [index/slice] <= baseType
      OT::baseType temp2 ;
      OT::baseType *val2 = 0 ;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIG_TypeQuery("OT::" #baseType " *"), SWIG_POINTER_NO_NULL))) {
        temp2 = OT::convert<OT::_PySequence_,OT::baseType>( valObj );
        val2 = &temp2;
      }
      for ( Py_ssize_t j = 0; j < slicelength2; ++ j )
      {
        self->operator()( arg2, start2 + j*step2 ) = val2->operator()(0, j);
      }
    }
    else
    {  
      // case #4: [index/index] <= elementType
      self->operator()(arg2, arg3) = OT::convert< OT::pythonElementType, OT::elementType >(valObj);
    }
  }

  return SWIG_Py_Void();
fail:
  return NULL;
}
%enddef


%define OTMatrixAccessors()
  OTMatrixGetAccessor(Matrix, Scalar, _PyFloat_)
  OTMatrixSetAccessor(Matrix, Scalar, _PyFloat_)
%enddef

%include openturns/Matrix.hxx

%pythoncode %{
# This code has been added to conform to Numpy ndarray interface
# that tries to reuse the data stored in the Matrix (zero copy)
# see http://docs.scipy.org/doc/numpy/reference/arrays.interface.html#arrays-interface
# for details.
# See python doc http://docs.python.org/reference/datamodel.html?highlight=getattribute#object.__getattribute__
# for details on how to write such a method.
def Matrix___getattribute__(self, name):
    """Implement attribute accesses."""
    if name == '__array_interface__':
        self.__dict__['__array_interface__'] = {'shape': (self.getNbRows(), self.getNbColumns()),
                                                'typestr': "|f" + str(self.__elementsize__()),
                                                'data': (int(self.__baseaddress__() or 1), True),
                                                'strides': (self.__stride__(0), self.__stride__(1)),
                                                'version': 3,
                                                }
    return super(Matrix, self).__getattribute__(name)
Matrix.__getattribute__ = Matrix___getattribute__
%}

namespace OT {

%extend Matrix {

  Matrix(const Matrix & other) { return new OT::Matrix(other); }

  Matrix(PyObject * pyObj) { return new OT::Matrix( OT::convert<OT::_PySequence_,OT::Matrix>(pyObj) ); }

  OTMatrixAccessors()

  Matrix __rmul__(const Scalar s) { return s * (*self); }
  Matrix __matmul__(const Matrix & other) { return *self * other; }

  Matrix __neg__() { return -1.0 * (*self); }

} // Matrix
} // OT
