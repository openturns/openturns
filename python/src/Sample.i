// SWIG file Sample.i

%ignore OT::Sample::storeToTemporaryFile;
%ignore OT::Sample::streamToRFormat;

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/SampleImplementation.hxx"
#include "openturns/Sample.hxx"
%}

%include Sample_doc.i

%template(SampleImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::SampleImplementation>;
%template(SampleCollection)            OT::Collection<OT::Sample>;

#define OT_TYPECHECK_NUMERICALSAMPLE 5

%typemap(in) const Sample & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    try {
      temp = OT::convert<OT::_PySequence_,OT::Sample>( $input );
      $1 = &temp;
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a Sample");
    }
  }
}

%typemap(typecheck,precedence=OT_TYPECHECK_NUMERICALSAMPLE) const Sample & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) || OT::isAPythonBufferOf<OT::Scalar, 2>($input) || OT::isAPythonSequenceOf<OT::_PySequence_>( $input );
}

%apply const Sample & { const OT::Sample & };

%typemap(in) const UnsignedIntegerCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    try {
      temp = OT::convert<OT::_PySequence_,OT::Collection<OT::UnsignedInteger> >( $input );
      $1 = &temp;
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of UnsignedInteger");
    }
  }
}

%apply const UnsignedIntegerCollection & { const OT::Sample::UnsignedIntegerCollection &, const OT::SampleImplementation::UnsignedIntegerCollection & };

%rename(__contains__) OT::Sample::contains;
%rename(__baseaddress__) OT::Sample::data;
%rename(__elementsize__) OT::Sample::elementSize;

%include openturns/SampleImplementation.hxx
%include openturns/Sample.hxx

%pythoncode %{
# This code has been added to conform to Numpy ndarray interface
# that tries to reuse the data stored in the Sample (zero copy)
# see http://docs.scipy.org/doc/numpy/reference/arrays.interface.html#arrays-interface
# for details.
# See python doc http://docs.python.org/reference/datamodel.html?highlight=getattribute#object.__getattribute__
# for details on how to write such a method.
def Sample___getattribute__(self, name):
    """Implement attribute accesses."""
    if name == '__array_interface__':
        self.__dict__['__array_interface__'] = {'shape': (self.getSize(), self.getDimension()),
                                                'typestr': "|f" + str(self.__elementsize__()),
                                                'data': (int(self.__baseaddress__()), True),
                                                'version': 3, 
                                                }
    return super(Sample, self).__getattribute__(name)
Sample.__getattribute__ = Sample___getattribute__


def Sample__repr_html_(self):
    """Get HTML representation."""
    html = '<TABLE>'
    desc = self.getDescription()
    ell_threshold = openturns.common.ResourceMap.GetAsUnsignedInteger("Sample-PrintEllipsisThreshold")
    ell_size = openturns.common.ResourceMap.GetAsUnsignedInteger("Sample-PrintEllipsisSize")
    size = self.getSize()
    dim = self.getDimension()
    ellipsis = size * dim > ell_threshold
    if not desc.isBlank():
        if ellipsis and dim > 2 * ell_size:
            html += '<TR><TH></TH><TH>'
            html += '</TH><TH>'.join(desc[0:ell_size])
            html += '</TH><TH COLSPAN="%d">...</TH><TH>' % (dim - 2 * ell_size)
            html += '</TH><TH>'.join(desc[-ell_size:])
            html += '</TH></TR>\n'
        else:
            html += '<TR><TD></TD><TH>' + '</TH><TH>'.join(desc) + '</TH></TR>\n'
    for i in range(size):
        if ellipsis and size > 2 * ell_size:
            if i == ell_size:
                html += '<TR><TD COLSPAN="%d">...</TD></TR>\n' % (dim + 1)
                continue
            else:
                if i > ell_size and i < size - ell_size:
                    continue
        # Write row
        fmt = "%.7g"
        if ellipsis and dim > 2 * ell_size:
            html += '<TR><TH>' + str(i)
            if dim > 0:
                html += '</TH><TD>'
            html += '</TD><TD>'.join([fmt % x for x in self[i, 0:ell_size]])
            html += '<TD COLSPAN="%d">...</TD><TD>' % (dim - 2 * ell_size)
            html += '</TD><TD>'.join([fmt % x for x in self[i, -ell_size:]])
            html += '</TD></TR>\n'
        else:
            html += '<TR><TH>' + str(i)
            if dim > 0:
                html += '</TH><TD>' + '</TD><TD>'.join([fmt % x for x in self[i]])
            html += '</TD></TR>\n'
    html += '</TABLE>'
    return html

Sample._repr_html_ = Sample__repr_html_
%}

namespace OT {
%extend Sample {

Point __getitem__(SignedInteger index) const {
  OT::UnsignedInteger size = self->getSize();
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  return self->at(index);
}

void __setitem__ (SignedInteger index,
                  const Point & val) {
  OT::UnsignedInteger size = self->getSize();
  if (index < 0) {
    index += self->getSize();
  }
  if (index < 0) {
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  }
  // CopyOnWrite only if index is ok
  self->copyOnWrite();
  self->at(index) = val;
}

UnsignedInteger __len__() const
{
  return self->getSize();
}


PyObject * __getitem__(PyObject * args) const {

  Py_ssize_t start1 = 0;
  Py_ssize_t stop1 = 0;
  Py_ssize_t step1 = 0;
  Py_ssize_t slicelength1 = 0;

  // case #0: [slice] => Sample
  if ( PySlice_Check( args ) )
  { 
    PySlice_GetIndicesEx( OT::SliceCast( args ), self->getSize(), &start1, &stop1, &step1, &slicelength1 );
    OT::Sample result( slicelength1, self->getDimension() );
    for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
    {
      result.at(i) = self->at( start1 + i*step1 );
    }
    result.setDescription(self->getDescription());
    return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN |  0 );
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

  if (!PyArg_ParseTuple(args,(char *)"OO:Sample___getitem__",&obj1,&obj2)) SWIG_fail;  

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
      SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Sample___getitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
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
      SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" "Sample___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
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
      // case #1: [slice/slice] => Sample
      OT::Sample result( slicelength1, slicelength2 );
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
      return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN |  0 );
    }
    else
    {
      // case #2: [slice/index] => Sample
      OT::Sample result( slicelength1, 1 );
      for ( Py_ssize_t i = 0; i < slicelength1; ++ i )
      {
        result.at(i, 0) = self->at( start1 + i*step1, arg3 );
      }
      result.setDescription(OT::Description(1, self->getDescription()[arg3]));
      return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN |  0 );
    }

  }
  else
  {
    if ( PySlice_Check( obj2 ) )
    {
      // case #3: [index/slice] => Point
      OT::Point result( slicelength2 );
      for ( Py_ssize_t j = 0; j < slicelength2; ++ j )
      {
        result.at(j) = self->at( arg2, start2 + j*step2 );
      }
      return SWIG_NewPointerObj((new OT::Point(static_cast< const OT::Point& >(result))), SWIGTYPE_p_OT__Point, SWIG_POINTER_OWN |  0 );
    }
    else
    {
      // case #4: [index/index] => float
      return OT::convert< OT::Scalar, OT::_PyFloat_>( self->at(arg2, arg3) );
    }
  }

fail:
  return NULL;
}



PyObject * __setitem__(PyObject * args, PyObject * valObj) {

  Py_ssize_t start1 = 0;
  Py_ssize_t stop1 = 0;
  Py_ssize_t step1 = 0;
  Py_ssize_t slicelength1 = 0;

  // case #0: [slice] <= Sample
  if ( PySlice_Check( args ) )
  {
    PySlice_GetIndicesEx( OT::SliceCast( args ), self->getSize(), &start1, &stop1, &step1, &slicelength1 );
    OT::Sample temp2 ;
    OT::Sample *val2 = 0 ;
    if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIGTYPE_p_OT__Sample, 0))) {
      temp2 = OT::convert< OT::_PySequence_, OT::Sample >( valObj );
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

  Py_ssize_t start2 = 0;
  Py_ssize_t stop2 = 0;
  Py_ssize_t step2 = 0;
  Py_ssize_t slicelength2 = 0;

  if (!PyArg_ParseTuple(args,(char *)"OO:Sample___getitem__",&obj1,&obj2)) SWIG_fail;

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
      SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Sample___setitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
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
      SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" "Sample___setitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
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
      // case #1: [slice/slice] <= Sample
      OT::Sample temp2 ;
      OT::Sample *val2 = 0 ;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIGTYPE_p_OT__Sample, 0))) {
        temp2 = OT::convert<OT::_PySequence_,OT::Sample>( valObj );
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
      // case #2: [slice/index] <= Sample
      OT::Sample temp2 ;
      OT::Sample *val2 = 0 ;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIGTYPE_p_OT__Sample, 0))) {
        temp2 = OT::convert<OT::_PySequence_,OT::Sample>( valObj );
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
      // case #3: [index/slice] <= Point
      OT::Point temp2 ;
      OT::Point *val2 = 0 ;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val2, SWIGTYPE_p_OT__Point, 0))) {
        temp2 = OT::convert<OT::_PySequence_,OT::Point>( valObj );
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
        SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Sample___setitem__" "', argument " "2"" of type '" "OT::Scalar""'");
      }
      self->at(arg2, arg3) = val;
    }
  }

  return SWIG_Py_Void();
fail:
  return NULL;
}

Sample(const Sample & other)
{
  return new OT::Sample( other );
}

Sample(PyObject * pyObj)
{
  return new OT::Sample( OT::convert< OT::_PySequence_, OT::Sample>(pyObj) );  
}

Bool __eq__(const Sample & other) { return (*self) == other; }

#if SWIG_VERSION < 0x030011
Sample __truediv__(const Scalar & u) { return (*self) / u; }

Sample __truediv__(const Point & v) { return (*self) / v; }
#endif

Sample __rmul__(Scalar s)
{
  return s * (*self);
}

} // %extend
} // namespace OT

