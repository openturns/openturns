// SWIG file NumericalSample.i

%ignore OT::NumericalSample::storeToTemporaryFile;
%ignore OT::NumericalSample::streamToRFormat;

%{
#include "openturns/NumericalSampleImplementation.hxx"
#include "openturns/NumericalSample.hxx"
%}

%include NumericalSample_doc.i

%template(NumericalSampleImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::NumericalSampleImplementation>;

#define OT_TYPECHECK_NUMERICALSAMPLE 5

%typemap(in) const NumericalSample & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    try {
      temp = OT::convert<OT::_PySequence_,OT::NumericalSample>( $input );
      $1 = &temp;
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a NumericalSample");
    }
  }
}

%typemap(typecheck,precedence=OT_TYPECHECK_NUMERICALSAMPLE) const NumericalSample & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) || OT::isAPythonSequenceOf<OT::_PySequence_>( $input );
}

%apply const NumericalSample & { const OT::NumericalSample & };

%rename(__contains__) OT::NumericalSample::contains;

%include openturns/NumericalSampleImplementation.hxx
%include openturns/NumericalSample.hxx

%pythoncode %{
# This code has been added to conform to Numpy ndarray interface
# that tries to reuse the data stored in the NumericalSample (zero copy)
# see http://docs.scipy.org/doc/numpy/reference/arrays.interface.html#arrays-interface
# for details.
# See python doc http://docs.python.org/reference/datamodel.html?highlight=getattribute#object.__getattribute__
# for details on how to write such a method.
def NumericalSample___getattribute__(self, name):
    """Implement attribute accesses."""
    if (name == '__array_interface__'):
        self.__dict__['__array_interface__'] = {'shape': (self.getSize(), self.getDimension()),
                                                'typestr': "|f" + str(self.__elementsize__()),
                                                'data': (int(self.__baseaddress__()), True),
                                                'version': 3, 
                                                }
    return object.__getattribute__(self, name)
NumericalSample.__getattribute__ = NumericalSample___getattribute__


def NumericalSample__repr_html_(self):
    """Get HTML representation."""
    html = '<TABLE>'
    desc = self.getDescription()
    if not desc.isBlank():
        html += '<TR>'
        html += '<TD></TD>'
        for j in range(self.getDimension()):
            html += '<TH>' + desc[j] + '</TH>'
        html += '</TR>'
    for i in range(self.getSize()):
        html += '<TR>'
        html += '<TH>' + str(i) + '</TH>'
        for j in range(self.getDimension()):
            html += '<TD>' + str(self[i, j]) + '</TD>'
        html += '</TR>'
    html += '</TABLE>'
    return html

NumericalSample._repr_html_ = NumericalSample__repr_html_
%}

namespace OT {
%extend NumericalSample {

NumericalPoint __getitem__(SignedInteger index) const {
  if (index < 0) {
    index += self->getSize();
  }
  return self->at(index);
}

void __setitem__ (SignedInteger index,
                  const NumericalPoint & val) {
  self->copyOnWrite();
  if (index < 0) {
    index += self->getSize();
  }
  self->at(index) = val;
}

UnsignedInteger __len__() const
{
  return self->getSize();
}


PyObject * __getitem__(PyObject * args) const {

  Py_ssize_t start1;
  Py_ssize_t stop1;
  Py_ssize_t step1;
  Py_ssize_t slicelength1;

  // case #0: [slice] => NumericalSample
  if ( PySlice_Check( args ) )
  { 
    PySlice_GetIndicesEx( OT::SliceCast( args ), self->getSize(), &start1, &stop1, &step1, &slicelength1 );
    OT::NumericalSample result( slicelength1, self->getDimension() );
    for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
    {
      result.at(i) = self->at( start1 + i*step1 );
    }
    result.setDescription(self->getDescription());
    return SWIG_NewPointerObj((new OT::NumericalSample(static_cast< const OT::NumericalSample& >(result))), SWIG_TypeQuery("OT::NumericalSample *"), SWIG_POINTER_OWN |  0 );
  }

  PyObject * obj1 = 0 ;
  PyObject * obj2 = 0 ;

  // argument values
  OT::UnsignedInteger arg2 = 0;
  OT::UnsignedInteger arg3 = 0;

  if (!PyArg_ParseTuple(args,(char *)"OO:NumericalSample___getitem__",&obj1,&obj2)) SWIG_fail;

  Py_ssize_t start2;
  Py_ssize_t stop2;
  Py_ssize_t step2;
  Py_ssize_t slicelength2;

  // convert first list argument 
  if ( PySlice_Check( obj1 ) )
  { 
    PySlice_GetIndicesEx( OT::SliceCast( obj1 ), self->getSize(), &start1, &stop1, &step1, &slicelength1 );
  }
  else
  {
    long val2 ;
    int ecode2 = 0 ;
    ecode2 = SWIG_AsVal_long(obj1, &val2);
    if (!SWIG_IsOK(ecode2)) {
      SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "NumericalSample___getitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
    }
    if (val2 < 0) {
      val2 += self->getSize();
    }
    arg2 = static_cast< OT::UnsignedInteger >(val2);
  }

  // convert second list argument
  if ( PySlice_Check( obj2 ) )
  {
    PySlice_GetIndicesEx( OT::SliceCast( obj2 ), self->getDimension(), &start2, &stop2, &step2, &slicelength2 );
  }
  else
  {
    long val3 ;
    int ecode3 = 0 ;
    ecode3 = SWIG_AsVal_long(obj2, &val3);
    if (!SWIG_IsOK(ecode3)) {
      SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" "NumericalSample___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
    }
    if (val3 < 0) {
      val3 += self->getDimension();
    }
    arg3 = static_cast< OT::UnsignedInteger >(val3);
  }

  // handle arguments
  if ( PySlice_Check( obj1 ) )
  {

    if ( PySlice_Check( obj2 ) )
    {
      // case #1: [slice/slice] => NumericalSample
      OT::NumericalSample result( slicelength1, slicelength2 );
      for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
      {
        for ( Py_ssize_t j = 0; j < slicelength2; ++ j )
        {
          result.at(i, j) = self->at( start1 + i*step1, start2 + j*step2 );
        }
      }
      OT::Description entireDescription(self->getDescription());
      OT::Description description(slicelength2);
      for ( Py_ssize_t j = 0; j < slicelength2; ++ j ) {
        description[j] = entireDescription[start2 + j*step2];
      }
      result.setDescription(description);
      return SWIG_NewPointerObj((new OT::NumericalSample(static_cast< const OT::NumericalSample& >(result))), SWIG_TypeQuery("OT::NumericalSample *"), SWIG_POINTER_OWN |  0 );
    }
    else
    {
      // case #2: [slice/index] => NumericalSample
      OT::NumericalSample result( slicelength1, 1 );
      for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
      {
        result.at(i, 0) = self->at( start1 + i*step1, arg3 );
      }
      result.setDescription(OT::Description(1, self->getDescription()[arg3]));
      return SWIG_NewPointerObj((new OT::NumericalSample(static_cast< const OT::NumericalSample& >(result))), SWIG_TypeQuery("OT::NumericalSample *"), SWIG_POINTER_OWN |  0 );
    }

  }
  else
  {
    if ( PySlice_Check( obj2 ) )
    {
      // case #3: [index/slice] => NumericalPoint
      OT::NumericalPoint result( slicelength2 );
      for ( Py_ssize_t j = 0; j < slicelength2; ++ j )
      {
        result.at(j) = self->at( arg2, start2 + j*step2 );
      }
      return SWIG_NewPointerObj((new OT::NumericalPoint(static_cast< const OT::NumericalPoint& >(result))), SWIG_TypeQuery("OT::NumericalPoint *"), SWIG_POINTER_OWN |  0 );
    }
    else
    {
      // case #4: [index/index] => float
      return OT::convert< OT::NumericalScalar, OT::_PyFloat_>( self->at(arg2, arg3) );
    }
  }

fail:
  return NULL;
}



PyObject * __setitem__(PyObject * args, PyObject * valObj) {

  Py_ssize_t start1;
  Py_ssize_t stop1;
  Py_ssize_t step1;
  Py_ssize_t slicelength1;

  // case #0: [slice] <= NumericalSample
  if ( PySlice_Check( args ) )
  {
    PySlice_GetIndicesEx( OT::SliceCast( args ), self->getSize(), &start1, &stop1, &step1, &slicelength1 );
    OT::NumericalSample temp2 ;
    OT::NumericalSample *val2 = 0 ;
    if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIG_TypeQuery("OT::NumericalSample *"), 0))) {
      temp2 = OT::convert< OT::_PySequence_, OT::NumericalSample >( valObj );
      val2 = &temp2;
    }
    assert( val2 );
    for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
    {
      self->at( start1 + i*step1 ) = val2->at(i);
    }
    return SWIG_Py_Void();
  }

  PyObject * obj1 = 0 ;
  PyObject * obj2 = 0 ;

  // argument values
  OT::UnsignedInteger arg2 = 0;
  OT::UnsignedInteger arg3 = 0;

  if (!PyArg_ParseTuple(args,(char *)"OO:NumericalSample___getitem__",&obj1,&obj2)) SWIG_fail;

  Py_ssize_t start2;
  Py_ssize_t stop2;
  Py_ssize_t step2;
  Py_ssize_t slicelength2;

  // convert first list argument 
  if ( PySlice_Check( obj1 ) )
  {
    PySlice_GetIndicesEx( OT::SliceCast( obj1 ), self->getSize(), &start1, &stop1, &step1, &slicelength1 );
  }
  else
  {
    long val2 ;
    int ecode2 = 0 ;
    ecode2 = SWIG_AsVal_long(obj1, &val2);
    if (!SWIG_IsOK(ecode2)) {
      SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "NumericalSample___setitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
    }
    if (val2 < 0) {
      val2 += self->getSize();
    }
    arg2 = static_cast< OT::UnsignedInteger >(val2);
  }

  // convert second list argument
  if ( PySlice_Check( obj2 ) )
  {
    PySlice_GetIndicesEx( OT::SliceCast( obj2 ), self->getDimension(), &start2, &stop2, &step2, &slicelength2 );
  }
  else
  {
    long val3 ;
    int ecode3 = 0 ;
    ecode3 = SWIG_AsVal_long(obj2, &val3);
    if (!SWIG_IsOK(ecode3)) {
      SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" "NumericalSample___setitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
    }
    if (val3 < 0) {
      val3 += self->getDimension();
    }
    arg3 = static_cast< OT::UnsignedInteger >(val3);
  }

  // handle arguments
  if ( PySlice_Check( obj1 ) )
  {

    if ( PySlice_Check( obj2 ) )
    {
      // case #1: [slice/slice] <= NumericalSample
      OT::NumericalSample temp2 ;
      OT::NumericalSample *val2 = 0 ;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIG_TypeQuery("OT::NumericalSample *"), 0))) {
        temp2 = OT::convert<OT::_PySequence_,OT::NumericalSample>( valObj );
        val2 = &temp2;
      }
      for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
      {
        for ( Py_ssize_t j = 0; j < slicelength2; ++ j )
        {
          self->at( start1 + i*step1, start2 + j*step2 ) = val2->at(i, j);
        }
      }
    }
    else
    {
      // case #2: [slice/index] <= NumericalSample
      OT::NumericalSample temp2 ;
      OT::NumericalSample *val2 = 0 ;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIG_TypeQuery("OT::NumericalSample *"), 0))) {
        temp2 = OT::convert<OT::_PySequence_,OT::NumericalSample>( valObj );
        val2 = &temp2;
      }
      for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
      {
        self->at( start1 + i*step1, arg3 ) = val2->at(i, 0);
      }
    }

  }
  else
  {
    if ( PySlice_Check( obj2 ) )
    {
      // case #3: [index/slice] <= NumericalPoint
      OT::NumericalPoint temp2 ;
      OT::NumericalPoint *val2 = 0 ;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIG_TypeQuery("OT::NumericalPoint *"), 0))) {
        temp2 = OT::convert<OT::_PySequence_,OT::NumericalPoint>( valObj );
        val2 = &temp2;
      }
      for ( Py_ssize_t j = 0; j < slicelength2; ++ j )
      {
        self->at( arg2, start2 + j*step2 ) = val2->at(j);
      }
    }
    else
    {  
      // case #4: [index/index] <= float
      double val = 0.;
      int ecode2 = SWIG_AsVal_double(valObj, &val );
      if (!SWIG_IsOK(ecode2)) {
        SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "NumericalSample___setitem__" "', argument " "2"" of type '" "OT::NumericalScalar""'");
      }
      self->at(arg2, arg3) = val;
    }
  }

  return SWIG_Py_Void();
fail:
  return NULL;
}

NumericalSample(const NumericalSample & other)
{
  return new OT::NumericalSample( other );
}

NumericalSample(PyObject * pyObj)
{
  return new OT::NumericalSample( OT::convert< OT::_PySequence_, OT::NumericalSample>(pyObj) );  
}

NumericalSample(PyObject * pyObj, UnsignedInteger dimension)
{
  OT::NumericalPoint point( OT::convert< OT::_PySequence_, OT::NumericalPoint >(pyObj) );
  OT::UnsignedInteger pointSize = point.getDimension();
  OT::UnsignedInteger size = (pointSize + dimension - 1) / dimension;
  OT::NumericalSample sample(size, dimension);
  OT::UnsignedInteger k = 0;
  for ( OT::UnsignedInteger i = 0; i < size; ++ i ) {
    for ( OT::UnsignedInteger j = 0; j < dimension; ++ j ) {
      if ( k < pointSize ) {
        sample[i][j] = point[k];
        ++ k;
      }
    }
  }
  return new OT::NumericalSample( sample );
}

Bool __eq__(const NumericalSample & other) { return (*self) == other; }

NumericalSample __truediv__(const NumericalScalar & u) { return (*self) / u; }

NumericalSample __truediv__(const NumericalPoint & v) { return (*self) / v; }

NumericalSample __truediv__(const SquareMatrix & m) { return (*self) / m; }

} // %extend
} // namespace OT
