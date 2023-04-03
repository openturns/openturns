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
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    try {
      temp = OT::convert<OT::_PySequence_, OT::Sample>($input);
      $1 = &temp;
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a Sample");
    }
  }
}

%typemap(typecheck,precedence=OT_TYPECHECK_NUMERICALSAMPLE) const Sample & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || OT::isAPythonBufferOf<OT::Scalar, 2>($input)
    || OT::isAPythonSequenceOf<OT::_PySequence_>($input);
}

%apply const Sample & { const OT::Sample & };

%typemap(in) const UnsignedIntegerCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    try {
      temp = OT::convert<OT::_PySequence_, OT::Collection<OT::UnsignedInteger> >($input);
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


%typemap(in) const SampleCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    try {
      temp = OT::convert<OT::_PySequence_, OT::Collection<OT::Sample> >($input);
      $1 = &temp;
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Sample");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const SampleCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL))
    || OT::canConvertCollectionObjectFromPySequence< OT::Sample >($input);
}

%apply const SampleCollection & { const OT::ProcessSample::SampleCollection &};

%pythoncode %{
# This code has been added to conform to Numpy ndarray interface
# that tries to reuse the data stored in the Sample (zero copy)
# see http://docs.scipy.org/doc/numpy/reference/arrays.interface.html#arrays-interface
# for details.
# See python doc http://docs.python.org/reference/datamodel.html?highlight=getattribute#object.__getattribute__
# for details on how to write such a method.
def __Sample_getattribute(self, name):
    """Implement attribute accesses."""
    if name == '__array_interface__':
        self.__dict__['__array_interface__'] = {'shape': (self.getSize(), self.getDimension()),
                                                'typestr': "|f" + str(self.__elementsize__()),
                                                'data': (int(self.__baseaddress__() or 1), True),
                                                'version': 3, 
                                                }
    return super(Sample, self).__getattribute__(name)
Sample.__getattribute__ = __Sample_getattribute

def __Sample_asDataFrame(self):
    """
    Convert to pandas DataFrame.

    Returns
    -------
    df : pandas DataFrame
        The converted data
    """
    from pandas import DataFrame
    df = DataFrame.from_records(self, columns=list(self.getDescription()))
    return df
Sample.asDataFrame = __Sample_asDataFrame

def __Sample_BuildFromDataFrame(df):
    """
    Convert a pandas DataFrame to Sample.

    Parameters
    ----------
    df : pandas DataFrame
        The data to convert

    Returns
    -------
    sample : :class:`~openturns.Sample`
        The converted sample
    """
    sample = openturns.Sample(df.values)
    sample.setDescription(df.columns)
    return sample
Sample.BuildFromDataFrame = __Sample_BuildFromDataFrame



def __Sample_repr_html(self):
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

Sample._repr_html_ = __Sample_repr_html
%}

namespace OT {
%extend Sample {

Point __getitem__(SignedInteger index) const
{
  OT::UnsignedInteger size = self->getSize();
  if (index < 0)
    index += self->getSize();
  if (index < 0)
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  return self->at(index);
}

void __setitem__ (SignedInteger index,
                  const Point & val)
{
  OT::UnsignedInteger size = self->getSize();
  if (index < 0)
    index += self->getSize();
  if (index < 0)
    throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
  // CopyOnWrite only if index is ok
  self->copyOnWrite();
  self->at(index) = val;
}

UnsignedInteger __len__() const
{
  return self->getSize();
}


PyObject * __getitem__(PyObject * args) const
{
  if (!PyTuple_Check(args))
  {
    if (PySlice_Check(args))
    {
      // case 0.1: [slice] => Sample
      Py_ssize_t start = 0;
      Py_ssize_t stop = 0;
      Py_ssize_t step = 0;
      Py_ssize_t size = 0;

      if (PySlice_Unpack(args, &start, &stop, &step) < 0)
        throw OT::InvalidArgumentException(HERE) << "Sample.__getitem__: PySlice_Unpack failed";
      size = PySlice_AdjustIndices(self->getSize(), &start, &stop, step);

      OT::Sample result(size, self->getDimension());
      for (Py_ssize_t i = 0; i < size; ++ i)
        result.at(i) = self->at(start + i * step);
      result.setDescription(self->getDescription());
      return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN | 0);
    }
    else if (PySequence_Check(args))
    {
      // case 0.2: [sequence] => Sample
      OT::ScopedPyObjectPointer seq(PySequence_Fast(args, ""));
      const Py_ssize_t size = PySequence_Fast_GET_SIZE(seq.get());
      OT::Sample result(size, self->getDimension());
      for (Py_ssize_t i = 0; i < size; ++ i)
      {
        PyObject * elt = PySequence_Fast_GET_ITEM(seq.get(), i);
        long index = 0;
        if (PyInt_Check(elt))
          index = PyInt_AsLong(elt);
        else if (PyObject_HasAttrString(elt, "__int__"))
        {
          OT::ScopedPyObjectPointer intValue(PyObject_CallMethod(elt, const_cast<char *>("__int__"), const_cast<char *>("()")));
          if (intValue.isNull())
            OT::handleException();
          index = PyInt_AsLong(intValue.get());
        }
        else
          throw OT::InvalidArgumentException(HERE) << "Indexing list expects int type";
        if (index < 0)
          index += self->getSize();
        if (index < 0)
          throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
        result.at(i) = self->at(index);
      }
      result.setDescription(self->getDescription());
      return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN | 0);
    }
    else if (PyObject_HasAttrString(args, "__int__"))
    {
      // case 0.3: [numpy.int64] => Point
      OT::ScopedPyObjectPointer intValue(PyObject_CallMethod(args, const_cast<char *>("__int__"), const_cast<char *>("()")));
      if (intValue.isNull())
        OT::handleException();
      long index = PyInt_AsLong(intValue.get());
      if (index < 0)
        index += self->getSize();
      if (index < 0)
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;
      OT::Point result(self->at(index));
      return SWIG_NewPointerObj((new OT::Point(static_cast< const OT::Point& >(result))), SWIGTYPE_p_OT__Point, SWIG_POINTER_OWN | 0);
    }
  }

  PyObject * obj1 = 0;
  PyObject * obj2 = 0;
  if (!PyArg_ParseTuple(args,(char *)"OO:Sample___getitem__", &obj1, &obj2)) SWIG_fail;

  if (OT::isAPython< OT::_PyInt_ >(obj1))
  {
    long index1 = 0;
    int ecode1 = SWIG_AsVal_long(obj1, &index1);
    if (!SWIG_IsOK(ecode1))
      SWIG_exception_fail(SWIG_ArgError(ecode1), "in method '" "Sample___getitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
    if (index1 < 0)
      index1 += self->getSize();
    if (index1 < 0)
      throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;

    if (OT::isAPython< OT::_PyInt_ >(obj2))
    {
      // case 1.1: [int/int] => float
      long index2 = 0;
      int ecode2 = SWIG_AsVal_long(obj2, &index2);
      if (!SWIG_IsOK(ecode2)) {
        SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Sample___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
      }
      if (index2 < 0) {
        index2 += self->getDimension();
      }
      if (index2 < 0) {
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;
      }
      return OT::convert< OT::Scalar, OT::_PyFloat_>(self->at(index1, index2));
    }
    else if (PySlice_Check(obj2))
    {
      // case 1.2: [int/slice] => Point
      Py_ssize_t start2 = 0;
      Py_ssize_t stop2 = 0;
      Py_ssize_t step2 = 0;
      Py_ssize_t size2 = 0;

      if (PySlice_Unpack(obj2, &start2, &stop2, &step2) < 0)
        throw OT::InvalidArgumentException(HERE) << "Sample.__getitem__: PySlice_Unpack failed";
      size2 = PySlice_AdjustIndices(self->getDimension(), &start2, &stop2, step2);

      OT::Point result(size2);
      for (Py_ssize_t j = 0; j < size2; ++ j)
        result.at(j) = self->at(index1, start2 + j * step2);
      return SWIG_NewPointerObj((new OT::Point(static_cast< const OT::Point& >(result))), SWIGTYPE_p_OT__Point, SWIG_POINTER_OWN | 0);
    }
    else if (PySequence_Check(obj2))
    {
      // case 1.3: [int/sequence] => Point
      OT::ScopedPyObjectPointer seq2(PySequence_Fast(obj2, ""));
      const Py_ssize_t size2 = PySequence_Fast_GET_SIZE(seq2.get());
      OT::Point result(size2);
      for (Py_ssize_t j = 0; j < size2; ++ j)
      {
        PyObject * elt = PySequence_Fast_GET_ITEM(seq2.get(), j);
        if (PyInt_Check(elt))
        {
          long index2 = PyInt_AsLong(elt);
          if (index2 < 0)
            index2 += self->getDimension();
          if (index2 < 0)
            throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;
          result[j] = self->at(index1, index2);
        }
        else
          SWIG_exception(SWIG_TypeError, "Indexing list expects int type");
      }
      return SWIG_NewPointerObj((new OT::Point(static_cast< const OT::Point& >(result))), SWIGTYPE_p_OT__Point, SWIG_POINTER_OWN | 0);
    }
  }
  else if (PySlice_Check(obj1))
  {
    Py_ssize_t start1 = 0;
    Py_ssize_t stop1 = 0;
    Py_ssize_t step1 = 0;
    Py_ssize_t size1 = 0;

    if (PySlice_Unpack(obj1, &start1, &stop1, &step1) < 0)
      throw OT::InvalidArgumentException(HERE) << "Sample.__setitem__: PySlice_Unpack failed";
    size1 = PySlice_AdjustIndices(self->getSize(), &start1, &stop1, step1);

    if (OT::isAPython< OT::_PyInt_ >(obj2))
    {
      // case 2.1: [slice/int] => Sample
      long index2 = 0;
      int ecode2 = SWIG_AsVal_long(obj2, &index2);
      if (!SWIG_IsOK(ecode2))
        SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Sample___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
      if (index2 < 0)
        index2 += self->getDimension();
      if (index2 < 0)
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;

      OT::Sample result(size1, 1);
      for (Py_ssize_t i = 0; i < size1; ++ i)
        result.at(i, 0) = self->at(start1 + i * step1, index2);
      result.setDescription(OT::Description(1, self->getDescription()[index2]));
      return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN | 0);
    }
    else if (PySlice_Check(obj2))
    {
      // case 2.2: [slice/slice] => Sample
      Py_ssize_t start2 = 0;
      Py_ssize_t stop2 = 0;
      Py_ssize_t step2 = 0;
      Py_ssize_t size2 = 0;

      if (PySlice_Unpack(obj2, &start2, &stop2, &step2) < 0)
        throw OT::InvalidArgumentException(HERE) << "Sample.__getitem__: PySlice_Unpack failed";
      size2 = PySlice_AdjustIndices(self->getDimension(), &start2, &stop2, step2);

      OT::Sample result(size1, size2);
      for (Py_ssize_t i = 0; i < size1; ++ i)
        for (Py_ssize_t j = 0; j < size2; ++ j)
          result.at(i, j) = self->at(start1 + i * step1, start2 + j * step2);
      OT::Description entireDescription(self->getDescription());
      OT::Description description(size2);
      for (Py_ssize_t j = 0; j < size2; ++ j)
        description[j] = entireDescription[start2 + j*step2];
      result.setDescription(description);
      return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN | 0);
    }
    else if (PySequence_Check(obj2))
    {
      // case 2.3: [slice/sequence] => Sample
      OT::ScopedPyObjectPointer seq2(PySequence_Fast(obj2, ""));
      Py_ssize_t size2 = PySequence_Fast_GET_SIZE(seq2.get());
      OT::Sample result(size1, size2);
      OT::Indices indices2(size2);
      for (Py_ssize_t j = 0; j < size2; ++ j)
      {
        PyObject * elt = PySequence_Fast_GET_ITEM(seq2.get(), j);
        if (PyInt_Check(elt))
        {
          long index2 = PyInt_AsLong(elt);
          if (index2 < 0)
            index2 += self->getDimension();
          if (index2 < 0)
            throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;
          indices2[j] = index2;
        }
        else
          SWIG_exception(SWIG_TypeError, "Indexing list expects int type");
      }
      for (Py_ssize_t i = 0; i < size1; ++ i)
        for (Py_ssize_t j = 0; j < size2; ++ j)
          result.at(i, j) = self->at(start1 + i * step1, indices2[j]);
      OT::Description description(self->getDescription());
      OT::Description marginalDescription(size2);
      for (Py_ssize_t j = 0; j < size2; ++ j)
        marginalDescription[j] = description[indices2[j]];
      result.setDescription(marginalDescription);
      return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN | 0);
    }
  }
  else if (PySequence_Check(obj1))
  {
    OT::ScopedPyObjectPointer seq1(PySequence_Fast(obj1, ""));
    const Py_ssize_t size1 = PySequence_Fast_GET_SIZE(seq1.get());
    OT::Indices indices1(size1);
    for (Py_ssize_t i = 0; i < size1; ++ i)
    {
      PyObject * elt = PySequence_Fast_GET_ITEM(seq1.get(), i);
      if (PyInt_Check(elt))
      {
        long index1 = PyInt_AsLong(elt);
        if (index1 < 0)
          index1 += self->getSize();
        if (index1 < 0)
          throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;
        indices1[i] = index1;
      }
      else
        SWIG_exception(SWIG_TypeError, "Indexing list expects int type");
    }

    if (OT::isAPython< OT::_PyInt_ >(obj2))
    {
      // case 3.1: [sequence/int] => Sample
      long index2 = 0;
      int ecode2 = SWIG_AsVal_long(obj2, &index2);
      if (!SWIG_IsOK(ecode2))
        SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Sample___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
      if (index2 < 0)
        index2 += self->getDimension();
      if (index2 < 0)
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;
      OT::Sample result(size1, 1);
      for (Py_ssize_t i = 0; i < size1; ++ i)
        result.at(i, 0) = self->at(indices1[i], index2);
      result.setDescription(OT::Description(1, self->getDescription()[index2]));
      return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN | 0);
    }
    else if (PySlice_Check(obj2))
    {
      // case 3.2: [sequence/slice] => Sample
      Py_ssize_t start2 = 0;
      Py_ssize_t stop2 = 0;
      Py_ssize_t step2 = 0;
      Py_ssize_t size2 = 0;

      if (PySlice_Unpack(obj2, &start2, &stop2, &step2) < 0)
        throw OT::InvalidArgumentException(HERE) << "Sample.__getitem__: PySlice_Unpack failed";
      size2 = PySlice_AdjustIndices(self->getDimension(), &start2, &stop2, step2);

      OT::Sample result(size1, size2);
      for (Py_ssize_t i = 0; i < size1; ++ i)
        for (Py_ssize_t j = 0; j < size2; ++ j)
          result.at(i, j) = self->at(indices1[i], start2 + j * step2);
      OT::Description description(self->getDescription());
      OT::Description marginalDescription(size2);
      for (Py_ssize_t j = 0; j < size2; ++ j)
        marginalDescription[j] = description[start2 + j*step2];
      result.setDescription(marginalDescription);
      return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN | 0);
    }
    else if (PySequence_Check(obj2))
    {
      // case 3.3: [sequence/sequence] => Sample
      OT::ScopedPyObjectPointer seq2(PySequence_Fast(obj2, ""));
      const Py_ssize_t size2 = PySequence_Fast_GET_SIZE(seq2.get());
      OT::Indices indices2(size2);
      for (Py_ssize_t j = 0; j < size2; ++ j)
      {
        PyObject * elt = PySequence_Fast_GET_ITEM(seq2.get(), j);
        if (PyInt_Check(elt))
        {
          long index2 = PyInt_AsLong(elt);
          if (index2 < 0)
            index2 += self->getDimension();
          if (index2 < 0)
            throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;
          indices2[j] = index2;
        }
        else
          SWIG_exception(SWIG_TypeError, "Indexing list expects int type");
      }
      OT::Sample result(size1, size2);
      for (Py_ssize_t i = 0; i < size1; ++ i)
        for (Py_ssize_t j = 0; j < size2; ++ j)
          result.at(i, j) = self->at(indices1[i], indices2[j]);
      OT::Description description(self->getDescription());
      OT::Description marginalDescription(size2);
      for (Py_ssize_t j = 0; j < size2; ++ j)
        marginalDescription[j] = description[indices2[j]];
      result.setDescription(marginalDescription);
      return SWIG_NewPointerObj((new OT::Sample(static_cast< const OT::Sample& >(result))), SWIGTYPE_p_OT__Sample, SWIG_POINTER_OWN | 0);
    }
  }
  else
    SWIG_exception(SWIG_TypeError, "Sample.__getitem__ expects int, slice or sequence arguments");
fail:
  return NULL;
}



void __setitem__(PyObject * args, PyObject * valObj)
{
  if (!PyTuple_Check(args))
  {
    if (PySlice_Check(args))
    {
      // case 0.1: [slice] <= Sample
      Py_ssize_t start = 0;
      Py_ssize_t stop = 0;
      Py_ssize_t step = 0;
      Py_ssize_t size = 0;

      if (PySlice_Unpack(args, &start, &stop, &step) < 0)
        throw OT::InvalidArgumentException(HERE) << "Sample.__setitem__: PySlice_Unpack failed";
      size = PySlice_AdjustIndices(self->getSize(), &start, &stop, step);

      OT::Sample temp;
      OT::Sample *val = 0;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val, SWIGTYPE_p_OT__Sample, SWIG_POINTER_NO_NULL))) {
        temp = OT::convert< OT::_PySequence_, OT::Sample >(valObj);
        val = &temp;
      }
      assert(val);
      OT::Sample result(size, self->getDimension());
      for (Py_ssize_t i = 0; i < size; ++ i)
        self->at(start + i*step) = val->at(i);
    }
    else if (PySequence_Check(args))
    {
      // case 0.2: [sequence] <= Sample
      OT::ScopedPyObjectPointer seq(PySequence_Fast(args, ""));
      const Py_ssize_t size = PySequence_Fast_GET_SIZE(seq.get());
      OT::Sample temp;
      OT::Sample *val = 0;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val, SWIGTYPE_p_OT__Sample, SWIG_POINTER_NO_NULL))) {
        temp = OT::convert< OT::_PySequence_, OT::Sample >(valObj);
        val = &temp;
      }
      assert(val);
      OT::Sample result(size, self->getDimension());
      for (Py_ssize_t i = 0; i < size; ++ i)
      {
        PyObject * elt = PySequence_Fast_GET_ITEM(seq.get(), i);
        long index = 0;
        if (PyInt_Check(elt))
          index = PyInt_AsLong(elt);
        else if (PyObject_HasAttrString(elt, "__int__"))
        {
          OT::ScopedPyObjectPointer intValue(PyObject_CallMethod(elt, const_cast<char *>("__int__"), const_cast<char *>("()")));
          if (intValue.isNull())
            OT::handleException();
          index = PyInt_AsLong(intValue.get());
        }
        else
          throw OT::InvalidArgumentException(HERE) << "Indexing list expects int type";
        if (index < 0)
          index += self->getSize();
        if (index < 0)
          throw OT::OutOfBoundException(HERE) << "index should be in [-" << size << ", " << size - 1 << "]." ;
        self->at(index) = val->at(i);
      }
    }
    else if (PyObject_HasAttrString(args, "__int__"))
    {
      // case 0.3: [numpy.int64] <= Point
      OT::ScopedPyObjectPointer intValue(PyObject_CallMethod(args, const_cast<char *>("__int__"), const_cast<char *>("()")));
      if (intValue.isNull())
        OT::handleException();
      long index = PyInt_AsLong(intValue.get());
      if (index < 0)
        index += self->getSize();
      if (index < 0)
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;
      OT::Point temp;
      OT::Point *val = 0;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val, SWIGTYPE_p_OT__Point, SWIG_POINTER_NO_NULL))) {
        temp = OT::convert<OT::_PySequence_, OT::Point>(valObj);
        val = &temp;
      }
      assert(val);
      self->at(index) = temp;
    }
    return;
  }

  PyObject * obj1 = 0;
  PyObject * obj2 = 0;
  if (!PyArg_ParseTuple(args,(char *)"OO:Sample___getitem__", &obj1, &obj2)) SWIG_fail;

  if (OT::isAPython< OT::_PyInt_ >(obj1))
  {
    long index1 = 0;
    int ecode1 = SWIG_AsVal_long(obj1, &index1);
    if (!SWIG_IsOK(ecode1))
      SWIG_exception_fail(SWIG_ArgError(ecode1), "in method '" "Sample___getitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
    if (index1 < 0)
      index1 += self->getSize();
    if (index1 < 0)
      throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;

    if (OT::isAPython< OT::_PyInt_ >(obj2))
    {
      // case 1.1: [int/int] <= float
      long index2 = 0;
      int ecode2 = SWIG_AsVal_long(obj2, &index2);
      if (!SWIG_IsOK(ecode2)) {
        SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Sample___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
      }
      if (index2 < 0) {
        index2 += self->getDimension();
      }
      if (index2 < 0) {
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;
      }

      const OT::Scalar val = OT::checkAndConvert<OT::_PyFloat_, OT::Scalar>(valObj);
      self->at(index1, index2) = val;
    }
    else if (PySlice_Check(obj2))
    {
      // case 1.2: [int/slice] <= Point
      Py_ssize_t start2 = 0;
      Py_ssize_t stop2 = 0;
      Py_ssize_t step2 = 0;
      Py_ssize_t size2 = 0;

      if (PySlice_Unpack(obj2, &start2, &stop2, &step2) < 0)
        throw OT::InvalidArgumentException(HERE) << "Sample.__setitem__: PySlice_Unpack failed";
      size2 = PySlice_AdjustIndices(self->getDimension(), &start2, &stop2, step2);

      OT::Point temp;
      OT::Point *val = 0;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val, SWIGTYPE_p_OT__Point, SWIG_POINTER_NO_NULL))) {
        temp = OT::convert<OT::_PySequence_, OT::Point>(valObj);
        val = &temp;
      }

      for (Py_ssize_t j = 0; j < size2; ++ j)
        self->at(index1, start2 + j * step2) = val->at(j);
    }
    else if (PySequence_Check(obj2))
    {
      // case 1.3: [int/sequence] <= Point
      OT::ScopedPyObjectPointer seq2(PySequence_Fast(obj2, ""));
      const Py_ssize_t size2 = PySequence_Fast_GET_SIZE(seq2.get());

      OT::Indices indices2(size2);
      for (Py_ssize_t j = 0; j < size2; ++ j)
      {
        PyObject * elt = PySequence_Fast_GET_ITEM(seq2.get(), j);
        if (PyInt_Check(elt))
        {
          long index2 = PyInt_AsLong(elt);
          if (index2 < 0)
            index2 += self->getDimension();
          if (index2 < 0)
            throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;
          indices2[j] = index2;
        }
        else
          SWIG_exception(SWIG_TypeError, "Indexing list expects int type");
      }

      OT::Point temp;
      OT::Point *val = 0;
      if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val, SWIGTYPE_p_OT__Point, SWIG_POINTER_NO_NULL))) {
        temp = OT::convert<OT::_PySequence_, OT::Point>(valObj);
        val = &temp;
      }

      for (Py_ssize_t j = 0; j < size2; ++ j)
        self->at(index1, indices2[j]) = val->at(j);
    }
  }
  else if (PySlice_Check(obj1))
  {
    Py_ssize_t start1 = 0;
    Py_ssize_t stop1 = 0;
    Py_ssize_t step1 = 0;
    Py_ssize_t size1 = 0;

    if (PySlice_Unpack(obj1, &start1, &stop1, &step1) < 0)
      throw OT::InvalidArgumentException(HERE) << "Sample.__setitem__: PySlice_Unpack failed";
    size1 = PySlice_AdjustIndices(self->getSize(), &start1, &stop1, step1);

    OT::Sample temp;
    OT::Sample *val = 0;
    if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val, SWIGTYPE_p_OT__Sample, SWIG_POINTER_NO_NULL))) {
      temp = OT::convert<OT::_PySequence_, OT::Sample>(valObj);
      val = &temp;
    }
    assert(val);

    if (OT::isAPython< OT::_PyInt_ >(obj2))
    {
      // case 2.1: [slice/int] <= Sample
      long index2 = 0;
      int ecode2 = SWIG_AsVal_long(obj2, &index2);
      if (!SWIG_IsOK(ecode2))
        SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Sample___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
      if (index2 < 0)
        index2 += self->getDimension();
      if (index2 < 0)
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;

      for (Py_ssize_t i = 0; i < size1; ++ i)
        self->at(start1 + i * step1, index2) = val->at(i, 0);
    }
    else if (PySlice_Check(obj2))
    {
      // case 2.2: [slice/slice] <= Sample
      Py_ssize_t start2 = 0;
      Py_ssize_t stop2 = 0;
      Py_ssize_t step2 = 0;
      Py_ssize_t size2 = 0;

      if (PySlice_Unpack(obj2, &start2, &stop2, &step2) < 0)
        throw OT::InvalidArgumentException(HERE) << "Sample.__setitem__: PySlice_Unpack failed";
      size2 = PySlice_AdjustIndices(self->getDimension(), &start2, &stop2, step2);

      for (Py_ssize_t i = 0; i < size1; ++ i)
        for (Py_ssize_t j = 0; j < size2; ++ j)
          self->at(start1 + i * step1, start2 + j * step2) = val->at(i, j);
    }
    else if (PySequence_Check(obj2))
    {
      // case 2.3: [slice/sequence] <= Sample
      OT::ScopedPyObjectPointer seq2(PySequence_Fast(obj2, ""));
      Py_ssize_t size2 = PySequence_Fast_GET_SIZE(seq2.get());
      OT::Sample result(size1, size2);
      OT::Indices indices2(size2);
      for (Py_ssize_t j = 0; j < size2; ++ j)
      {
        PyObject * elt = PySequence_Fast_GET_ITEM(seq2.get(), j);
        if (PyInt_Check(elt))
        {
          long index2 = PyInt_AsLong(elt);
          if (index2 < 0)
            index2 += self->getDimension();
          if (index2 < 0)
            throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;
          indices2[j] = index2;
        }
        else
          SWIG_exception(SWIG_TypeError, "Indexing list expects int type");
      }
      for (Py_ssize_t i = 0; i < size1; ++ i)
        for (Py_ssize_t j = 0; j < size2; ++ j)
          self->at(start1 + i * step1, indices2[j]) = val->at(i, j);
    }
  }
  else if (PySequence_Check(obj1))
  {
    OT::ScopedPyObjectPointer seq1(PySequence_Fast(obj1, ""));
    const Py_ssize_t size1 = PySequence_Fast_GET_SIZE(seq1.get());
    OT::Indices indices1(size1);
    for (Py_ssize_t i = 0; i < size1; ++ i)
    {
      PyObject * elt = PySequence_Fast_GET_ITEM(seq1.get(), i);
      if (PyInt_Check(elt))
      {
        long index1 = PyInt_AsLong(elt);
        if (index1 < 0)
          index1 += self->getSize();
        if (index1 < 0)
          throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getSize() << ", " << self->getSize() - 1 << "]." ;
        indices1[i] = index1;
      }
      else
        SWIG_exception(SWIG_TypeError, "Indexing list expects int type");
    }

    OT::Sample temp;
    OT::Sample *val = 0;
    if (! SWIG_IsOK(SWIG_ConvertPtr(valObj, (void **) &val, SWIGTYPE_p_OT__Sample, SWIG_POINTER_NO_NULL))) {
      temp = OT::convert<OT::_PySequence_, OT::Sample>(valObj);
      val = &temp;
    }
    assert(val);

    if (OT::isAPython< OT::_PyInt_ >(obj2))
    {
      // case 3.1: [sequence/int] <= Sample
      long index2 = 0;
      int ecode2 = SWIG_AsVal_long(obj2, &index2);
      if (!SWIG_IsOK(ecode2))
        SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" "Sample___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
      if (index2 < 0)
        index2 += self->getDimension();
      if (index2 < 0)
        throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;
      for (Py_ssize_t i = 0; i < size1; ++ i)
        self->at(indices1[i], index2) = val->at(i, 0);
    }
    else if (PySlice_Check(obj2))
    {
      // case 3.2: [sequence/slice] <= Sample
      Py_ssize_t start2 = 0;
      Py_ssize_t stop2 = 0;
      Py_ssize_t step2 = 0;
      Py_ssize_t size2 = 0;

      if (PySlice_Unpack(obj2, &start2, &stop2, &step2) < 0)
        throw OT::InvalidArgumentException(HERE) << "Sample.__setitem__: PySlice_Unpack failed";
      size2 = PySlice_AdjustIndices(self->getDimension(), &start2, &stop2, step2);

      for (Py_ssize_t i = 0; i < size1; ++ i)
        for (Py_ssize_t j = 0; j < size2; ++ j)
          self->at(indices1[i], start2 + j * step2) = val->at(i, j);
    }
    else if (PySequence_Check(obj2))
    {
      // case 3.3: [sequence/sequence] <= Sample
      OT::ScopedPyObjectPointer seq2(PySequence_Fast(obj2, ""));
      const Py_ssize_t size2 = PySequence_Fast_GET_SIZE(seq2.get());
      OT::Indices indices2(size2);
      for (Py_ssize_t j = 0; j < size2; ++ j)
      {
        PyObject * elt = PySequence_Fast_GET_ITEM(seq2.get(), j);
        if (PyInt_Check(elt))
        {
          long index2 = PyInt_AsLong(elt);
          if (index2 < 0)
            index2 += self->getDimension();
          if (index2 < 0)
            throw OT::OutOfBoundException(HERE) << "index should be in [-" << self->getDimension() << ", " << self->getDimension() - 1 << "]." ;
          indices2[j] = index2;
        }
        else
          SWIG_exception(SWIG_TypeError, "Indexing list expects int type");
      }
      for (Py_ssize_t i = 0; i < size1; ++ i)
        for (Py_ssize_t j = 0; j < size2; ++ j)
          self->at(indices1[i], indices2[j]) = val->at(i, j);
    }
  }
  else
    SWIG_exception(SWIG_TypeError, "Sample.__setitem__ expects int, slice or sequence arguments");
fail:
  return;
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

