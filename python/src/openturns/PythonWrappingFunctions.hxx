//                                               -*- C++ -*-
/**
 * @brief This file provides functions to ease Python wrapping
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef OPENTURNS_PYTHONWRAPPINGFUNCTIONS_HXX
#define OPENTURNS_PYTHONWRAPPINGFUNCTIONS_HXX

#include <Python.h>
#include "openturns/OT.hxx"

BEGIN_NAMESPACE_OPENTURNS


/** Scoped PyObject pointer: automatically triggers garbage collection */
class ScopedPyObjectPointer
{
public:
  explicit ScopedPyObjectPointer(PyObject * pyObj = 0)
    : pyObj_(pyObj)
  {
  }

  ~ScopedPyObjectPointer()
  {
    Py_XDECREF(pyObj_);
  }

  ScopedPyObjectPointer & operator=(PyObject * pyObj)
  {
    Py_XDECREF(pyObj_);
    pyObj_ = pyObj;
    return *this;
  }

  PyObject & operator*() const
  {
    return *pyObj_;
  }

  PyObject * get() const
  {
    return pyObj_;
  }

  bool isNull()
  {
    return !pyObj_;
  }

private:
  PyObject* pyObj_;
};


/** These templates are just declared, not defined. Only specializations are. */
template <class CPP_Type>                    struct traitsPythonType;
template <class PYTHON_Type>                 static inline int          isAPython(PyObject * pyObj);
template <class PYTHON_Type>                 static inline const char * namePython();
template <class PYTHON_Type, class CPP_Type> static inline CPP_Type     convert(PyObject * pyObj);
template <class CPP_Type, class PYTHON_Type> static inline PyObject *   convert(CPP_Type);
template <class PYTHON_Type, class CPP_Type> static inline bool         canConvert(PyObject * pyObj);
template <class PYTHON_Type>                 static inline void         check(PyObject * pyObj);
template <class PYTHON_Type, class CPP_Type> static inline CPP_Type     checkAndConvert(PyObject * pyObj);
template <class T>                           static inline T *          buildObjectFromPySequence(PyObject * pyObj);



/** Specializations */


/* PyObject */
struct _PyObject_ {};

template <>
inline
int
isAPython<_PyObject_>(PyObject *)
{
  return 1;
}

template <>
inline
const char *
namePython<_PyObject_>()
{
  return "object";
}




/* PyBool */
struct _PyBool_ {};

template <>
inline
int
isAPython<_PyBool_>(PyObject * pyObj)
{
  return PyBool_Check(pyObj);
}

template <>
inline
const char *
namePython<_PyBool_>()
{
  return "bool";
}

template <>
struct traitsPythonType<Bool>
{
  typedef _PyBool_ Type;
};

template <>
inline
Bool
convert< _PyBool_, Bool >(PyObject * pyObj)
{
  return pyObj == Py_True;
}

template <>
inline
PyObject *
convert< Bool, _PyBool_ >(Bool inB)
{
  return PyBool_FromLong(inB ? 1 : 0);
}


/* PyInt */
struct _PyInt_ {};

template <>
inline
int
isAPython< _PyInt_ >(PyObject * pyObj)
{
  // PyInt type is deprecated
#if PY_MAJOR_VERSION >= 3
  return PyLong_Check(pyObj);
#else
  return PyInt_Check(pyObj) || PyLong_Check(pyObj);
#endif
}

template <>
inline
const char *
namePython< _PyInt_ >()
{
  return "integer";
}

// In C++11, it is no more possible to define a static member which is a const char*.
// We could use constexpr instead, if we are sure that compiler support is okay.
// As only numerical types are concerned, it is unlikely that new types are added,
// so let us use an array instead and initialize an index into this array.
static const char* const pyBuf_formats [] =
{
  "l",
  "d",
  "Zd"
};

template <>
struct traitsPythonType< UnsignedInteger >
{
  typedef _PyInt_ Type;
  static const int buf_itemsize = sizeof(UnsignedInteger);
  static const int buf_format_idx = 0; // "l"
};

template <>
inline
bool
canConvert< _PyInt_, UnsignedInteger >(PyObject *)
{
  return true;
}

template <>
inline
UnsignedInteger
convert< _PyInt_, UnsignedInteger >(PyObject * pyObj)
{
  return PyLong_AsUnsignedLong(pyObj);
}

template <>
inline
PyObject *
convert< UnsignedInteger, _PyInt_ >(UnsignedInteger n)
{
  return PyLong_FromUnsignedLong(n);
}




/* PyFloat */
struct _PyFloat_ {};

template <>
inline
int
isAPython< _PyFloat_ >(PyObject * pyObj)
{
  // check also against sequence protocol as numpy array complies to several protocols
  return PyNumber_Check(pyObj) && !PyComplex_Check(pyObj) && !PySequence_Check(pyObj);
}

template <>
inline
const char *
namePython<_PyFloat_>()
{
  return "double";
}

template <>
struct traitsPythonType< Scalar >
{
  typedef _PyFloat_ Type;
  static const int buf_itemsize = sizeof(Scalar);
  static const int buf_format_idx = 1; // "d"
};

template <>
inline
Scalar
convert< _PyFloat_, Scalar >(PyObject * pyObj)
{
  return PyFloat_AsDouble(pyObj);
}

template <>
inline
PyObject *
convert< Scalar, _PyFloat_ >(Scalar x)
{
  return PyFloat_FromDouble(x);
}


/* PyComplex */
struct _PyComplex_ {};

template <>
inline
int
isAPython<_PyComplex_>(PyObject * pyObj)
{
  return PyNumber_Check(pyObj);
}

template <>
inline
const char *
namePython<_PyComplex_>()
{
  return "complex";
}

template <>
struct traitsPythonType< Complex >
{
  typedef _PyComplex_ Type;
  static const int buf_itemsize = sizeof(Complex);
  static const int buf_format_idx = 2; // "Zd"
};

template <>
inline
Complex
convert< _PyComplex_, Complex >(PyObject * pyObj)
{
  return Complex(PyComplex_RealAsDouble(pyObj), PyComplex_ImagAsDouble(pyObj));
}

template <>
inline
PyObject *
convert< Complex, _PyComplex_ >(Complex x)
{
  return PyComplex_FromDoubles(x.real(), x.imag());
}


/* PyBytes */
struct _PyBytes_ {};

template <>
inline
int
isAPython< _PyBytes_ >(PyObject * pyObj)
{
#if PY_MAJOR_VERSION >= 3
  return PyBytes_Check(pyObj);
#else
  return PyString_Check(pyObj);
#endif
}

template <>
inline
const char *
namePython< _PyBytes_ >()
{
  return "bytes";
}

template <>
inline
String
convert< _PyBytes_, String >(PyObject * pyObj)
{
#if PY_MAJOR_VERSION >= 3
  return PyBytes_AsString(pyObj);
#else
  return PyString_AsString(pyObj);
#endif
}

template <>
inline
PyObject *
convert< String, _PyBytes_ >(String s)
{
#if PY_MAJOR_VERSION >= 3
  return PyBytes_FromString(s.data());
#else
  return PyString_FromString(s.data());
#endif
}


/* PyUnicode */
struct _PyUnicode_ {};

template <>
inline
int
isAPython< _PyUnicode_ >(PyObject * pyObj)
{
  return PyUnicode_Check(pyObj);
}

template <>
inline
const char *
namePython< _PyUnicode_ >()
{
  return "unicode";
}

template <>
inline
String
convert< _PyUnicode_, String >(PyObject * pyObj)
{
  ScopedPyObjectPointer encodedBytes(PyUnicode_AsUTF8String(pyObj));
  assert(encodedBytes.get());
  return convert<_PyBytes_, String>(encodedBytes.get());
}

template <>
inline
PyObject *
convert< String, _PyUnicode_ >(String s)
{
  return PyUnicode_FromString(s.data());
}


/* PyString */
struct _PyString_ {};

template <>
inline
int
isAPython< _PyString_ >(PyObject * pyObj)
{
#if PY_MAJOR_VERSION >= 3
  return PyUnicode_Check(pyObj);
#else
  return PyString_Check(pyObj) || PyUnicode_Check(pyObj);
#endif
}

template <>
inline
const char *
namePython<_PyString_>()
{
  return "string";
}

template <>
struct traitsPythonType< String >
{
  typedef _PyString_ Type;
};

template <>
inline
String
convert< _PyString_, String >(PyObject * pyObj)
{
  String result;
#if PY_MAJOR_VERSION >= 3
  result = convert< _PyUnicode_, String >(pyObj);
#else
  if(isAPython<_PyBytes_>(pyObj))
  {
    result = convert<_PyBytes_, String>(pyObj);
  }
  else if (isAPython<_PyUnicode_>(pyObj))
  {
    result = convert<_PyUnicode_, String>(pyObj);
  }
#endif
  return result;
}

template <>
inline
PyObject *
convert< String, _PyString_ >(String s)
{
#if PY_MAJOR_VERSION >= 3
  return convert<String, _PyUnicode_>(s);
#else
  return convert<String, _PyBytes_>(s);
#endif
}


/* PySequence */
struct _PySequence_ {};

template <>
inline
int
isAPython< _PySequence_ >(PyObject * pyObj)
{
  return PySequence_Check(pyObj);
}

template <>
inline
const char *
namePython<_PySequence_>()
{
  return "sequence object";
}



/* Buffer protocol PEP 3118 */
template <class CPP_Type, int ndim>
static inline
bool
isAPythonBufferOf(PyObject * pyObj)
{
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      bool result = (view.ndim == ndim &&
                     view.itemsize == traitsPythonType<CPP_Type>::buf_itemsize &&
                     view.format != NULL &&
                     strcmp(view.format, pyBuf_formats[traitsPythonType<CPP_Type>::buf_format_idx]) == 0);
      PyBuffer_Release(&view);
      return result;
    }
    else
    {
      // When PyObject_GetBuffer fails, it sets PyErr_BufferException,
      // thus PyErr_Clear must be called.
      PyErr_Clear();
    }
  }
  return false;
}

template <class PYTHON_Type>
static inline
int
isAPythonSequenceOf(PyObject * pyObj)
{
  int ok = isAPython<_PySequence_>(pyObj) && (! isAPython< _PyString_ >(pyObj));

  if (ok)
  {
    const UnsignedInteger size = PySequence_Size(pyObj);
    for(UnsignedInteger i = 0; ok && (i < size); ++i)
    {
      ScopedPyObjectPointer elt(PySequence_ITEM(pyObj, i));
      int elt_ok = elt.get() && isAPython<PYTHON_Type>(elt.get());
      ok *= elt_ok;
    }
  }

  return ok;
}


template <class PYTHON_Type>
static inline
void
check(PyObject * pyObj)
{
  if (! isAPython<PYTHON_Type>(pyObj))
  {
    throw InvalidArgumentException(HERE) << "Object passed as argument is not a " << namePython<PYTHON_Type>();
  }
}


template <class PYTHON_Type, class CPP_Type>
static inline
CPP_Type
checkAndConvert(PyObject * pyObj)
{
  check<PYTHON_Type>(pyObj);
  return convert< PYTHON_Type, CPP_Type >(pyObj);
}





template <class T>
static inline
bool
canConvertCollectionObjectFromPySequence(PyObject * pyObj)
{
  try
  {
    check<_PySequence_>(pyObj);
  }
  catch (InvalidArgumentException &)
  {
    return false;
  }

  ScopedPyObjectPointer newPyObj(PySequence_Fast(pyObj, ""));

  const UnsignedInteger size = PySequence_Fast_GET_SIZE(newPyObj.get());
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    PyObject * elt = PySequence_Fast_GET_ITEM(newPyObj.get(), i);
    if (!canConvert< typename traitsPythonType< T >::Type, T >(elt))
    {
      return false;
    }
  }

  return true;
}




template <class T>
static inline
Collection<T> *
buildCollectionFromPySequence(PyObject * pyObj, int sz = 0)
{
  check<_PySequence_>(pyObj);
  ScopedPyObjectPointer newPyObj(PySequence_Fast(pyObj, ""));
  if (!newPyObj.get()) throw InvalidArgumentException(HERE) << "Not a sequence object";
  const UnsignedInteger size = PySequence_Fast_GET_SIZE(newPyObj.get());
  if ((sz != 0) && (sz != (int)size))
  {
    throw InvalidArgumentException(HERE) << "Sequence object has incorrect size " << size << ". Must be " << sz << ".";
  }
  Collection<T> * p_coll = new Collection< T >(size);

  for(UnsignedInteger i = 0; i < size; ++i)
  {
    PyObject * elt = PySequence_Fast_GET_ITEM(newPyObj.get(), i);
    try
    {
      check<typename traitsPythonType< T >::Type>(elt);
    }
    catch (InvalidArgumentException &)
    {
      delete p_coll;
      throw;
    }
    (*p_coll)[i] = convert< typename traitsPythonType< T >::Type, T >(elt);
  }

  return p_coll;
}






template <>
struct traitsPythonType< Point >
{
  typedef _PySequence_ Type;
};

template <>
inline
Point
convert< _PySequence_, Point >(PyObject * pyObj)
{
  // Check whether pyObj follows the buffer protocol
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      if (view.ndim == 1 &&
          view.itemsize == traitsPythonType<Scalar>::buf_itemsize &&
          view.format != NULL &&
          strcmp(view.format, pyBuf_formats[traitsPythonType<Scalar>::buf_format_idx]) == 0)
      {
        // 1-d contiguous array, we can directly copy memory chunk
        const Scalar* data = static_cast<const Scalar*>(view.buf);
        const UnsignedInteger size = view.shape[0];
        Point point(size);
        std::copy(data, data + size, (size > 0) ? &point[0] : 0);
        PyBuffer_Release(&view);
        return point;
      }
      PyBuffer_Release(&view);
    }
    else
      PyErr_Clear();
  }

  Pointer<Collection<Scalar> > ptr = buildCollectionFromPySequence<Scalar>(pyObj);
  return Point(*ptr);
}

template <>
inline
PyObject *
convert< Point, _PySequence_ >(Point inP)
{
  UnsignedInteger dimension = inP.getDimension();
  PyObject * point = PyTuple_New(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    PyTuple_SetItem(point, i, convert< Scalar, _PyFloat_ >(inP[i]));
  }
  return point;
}

template <>
inline
PyObject *
convert< Description, _PySequence_ >(Description inP)
{
  UnsignedInteger dimension = inP.getSize();
  PyObject * point = PyTuple_New(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    PyTuple_SetItem(point, i, convert< String, _PyString_ >(inP[i]));
  }
  return point;
}

template <>
struct traitsPythonType< Collection < Complex> >
{
  typedef _PySequence_ Type;
};


template <>
inline
Collection<Complex>
convert<_PySequence_, Collection<Complex> >(PyObject * pyObj)
{
  // Check whether pyObj follows the buffer protocol
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      if (view.ndim == 1 &&
          view.itemsize == traitsPythonType<Complex>::buf_itemsize &&
          view.format != NULL &&
          strcmp(view.format, pyBuf_formats[traitsPythonType<Complex>::buf_format_idx]) == 0)
      {
        // 1-d contiguous array, we can directly copy memory chunk
        const Complex* data = static_cast<const Complex*>(view.buf);
        const UnsignedInteger size = view.shape[0];
        Collection<Complex> result(size);
        std::copy(data, data + size, (size > 0) ? &result[0] : 0);
        PyBuffer_Release(&view);
        return result;
      }
      PyBuffer_Release(&view);
    }
    else
      PyErr_Clear();
  }

  Pointer<Collection<Complex> > ptr = buildCollectionFromPySequence<Complex>(pyObj);
  return Collection<Complex>(*ptr);
}

inline
void handleException()
{
  PyObject * exception = PyErr_Occurred();

  if (exception)
  {
    PyObject *type = NULL, *value = NULL, *traceback = NULL;
    PyErr_Fetch(&type, &value, &traceback);

    String exceptionMessage("Python exception");

    // get the name of the exception
    if (type)
    {
      ScopedPyObjectPointer nameObj(PyObject_GetAttrString(type, "__name__"));
      if (nameObj.get())
      {
        String typeString = checkAndConvert< _PyString_, String >(nameObj.get());
        exceptionMessage += ": " + typeString;
      }
    }

    // try to get error msg, value and traceback can be NULL
    if (value)
    {
      ScopedPyObjectPointer valueObj(PyObject_Str(value));
      if (valueObj.get())
      {
        String valueString = checkAndConvert< _PyString_, String >(valueObj.get());
        exceptionMessage += ": " + valueString;
      }
    }

    PyErr_Restore(type, value, traceback);
    PyErr_Print();
    throw InternalException(HERE) << exceptionMessage;
  }
}



template <>
struct traitsPythonType< Sample >
{
  typedef _PySequence_ Type;
};

template <>
inline
Sample
convert< _PySequence_, Sample >(PyObject * pyObj)
{
  // Check whether pyObj follows the buffer protocol
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      if (view.ndim == 2 &&
          view.itemsize == traitsPythonType<Scalar>::buf_itemsize &&
          view.format != NULL &&
          strcmp(view.format, pyBuf_formats[traitsPythonType<Scalar>::buf_format_idx]) == 0)
      {
        const Scalar* data = static_cast<const Scalar*>(view.buf);
        const UnsignedInteger size = view.shape[0];
        const UnsignedInteger dimension = view.shape[1];
        Sample sample(size, dimension);
        if (PyBuffer_IsContiguous(&view, 'C'))
        {
          // 2-d contiguous array in C notation, we can directly copy memory chunk
          std::copy(data, data + size * dimension, (Scalar *)sample.data());
        }
        else
        {
          for (UnsignedInteger j = 0; j < dimension; ++j)
            for(UnsignedInteger i = 0; i < size; ++i, ++data)
              sample(i, j) = *data;
        }
        PyBuffer_Release(&view);
        return sample;
      }
      PyBuffer_Release(&view);
    }
    else
      PyErr_Clear();
  }

  // use the same conversion function for numpy array/matrix, knowing numpy matrix is not a sequence
  if (PyObject_HasAttrString(pyObj, const_cast<char *>("shape")))
  {
    ScopedPyObjectPointer shapeObj(PyObject_GetAttrString(pyObj, "shape"));
    if (!shapeObj.get()) throw;

    Indices shape(checkAndConvert< _PySequence_, Indices >(shapeObj.get()));
    if (shape.getSize() == 2)
    {
      UnsignedInteger size = shape[0];
      UnsignedInteger dimension = shape[1];
      ScopedPyObjectPointer askObj(PyTuple_New(2));
      ScopedPyObjectPointer methodObj(convert< String, _PyString_ >("__getitem__"));
      Sample sample(size, dimension);
      for (UnsignedInteger i = 0; i < size; ++ i)
      {
        PyTuple_SetItem(askObj.get(), 0, convert< UnsignedInteger, _PyInt_ >(i));
        for (UnsignedInteger j = 0; j < dimension; ++ j)
        {
          PyTuple_SetItem(askObj.get(), 1, convert< UnsignedInteger, _PyInt_ >(j));
          ScopedPyObjectPointer elt(PyObject_CallMethodObjArgs(pyObj, methodObj.get(), askObj.get(), NULL));
          if (elt.get())
          {
            sample(i, j) = checkAndConvert<_PyFloat_, Scalar>(elt.get());
          }
        }
      }
      return sample;
    }
    else if (shape.getSize() == 1)
      throw InvalidArgumentException(HERE) << "Invalid array dimension 1 is ambiguous, please set the dimension explicitly";
    else
      throw InvalidArgumentException(HERE) << "Invalid array dimension: " << shape.getSize() << " is greater than 2";
  }
  check<_PySequence_>(pyObj);
  ScopedPyObjectPointer newPyObj(PySequence_Fast(pyObj, ""));
  if (!newPyObj.get()) throw InvalidArgumentException(HERE) << "Not a sequence object";
  const UnsignedInteger size = PySequence_Fast_GET_SIZE(newPyObj.get());
  if (size == 0) return Sample();

  // Get dimension of first point
  PyObject * firstPoint = PySequence_Fast_GET_ITEM(newPyObj.get(), 0);
  check<_PySequence_>(firstPoint);
  ScopedPyObjectPointer newPyFirstObj(PySequence_Fast(firstPoint, ""));
  const UnsignedInteger dimension = PySequence_Fast_GET_SIZE(newPyFirstObj.get());
  // Allocate result Sample
  Sample sample(size, dimension);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    PyObject * pointObj = PySequence_Fast_GET_ITEM(newPyObj.get(), i);
    ScopedPyObjectPointer newPyPointObj(PySequence_Fast(pointObj, ""));
    if (i > 0)
    {
      // Check that object is a sequence, and has the right size
      check<_PySequence_>(pointObj);
      const UnsignedInteger subDim = static_cast<UnsignedInteger>(PySequence_Fast_GET_SIZE(newPyPointObj.get()));
      if (subDim != dimension)
        throw InvalidArgumentException(HERE) << "Inner sequences must have the same dimension";
    }
    for(UnsignedInteger j = 0; j < dimension; ++j)
    {
      PyObject * value = PySequence_Fast_GET_ITEM(newPyPointObj.get(), j);
      sample(i, j) = checkAndConvert<_PyFloat_, Scalar>(value);
    }
  }
  return sample;
}

template <>
struct traitsPythonType< Collection< UnsignedInteger > >
{
  typedef _PySequence_ Type;
};

template <>
inline
Collection< UnsignedInteger >
convert< _PySequence_, Collection<UnsignedInteger> >(PyObject * pyObj)
{
  // Check whether pyObj follows the buffer protocol
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      if (view.ndim == 1 &&
          view.itemsize == traitsPythonType<UnsignedInteger>::buf_itemsize &&
          view.format != NULL &&
          strcmp(view.format, pyBuf_formats[traitsPythonType<UnsignedInteger>::buf_format_idx]) == 0)
      {
        // 1-d contiguous array, we can directly copy memory chunk
        const UnsignedInteger* data = static_cast<const UnsignedInteger*>(view.buf);
        const UnsignedInteger size = view.shape[0];
        Collection<UnsignedInteger> result(size);
        std::copy(data, data + size, &result[0]);
        PyBuffer_Release(&view);
        return result;
      }
      PyBuffer_Release(&view);
    }
    else
      PyErr_Clear();
  }

  Pointer<Collection<UnsignedInteger> > ptr = buildCollectionFromPySequence<UnsignedInteger>(pyObj);
  return Collection<UnsignedInteger>(ptr->begin(), ptr->end());
}


template <>
struct traitsPythonType< Indices >
{
  typedef _PySequence_ Type;
};

template <>
inline
Indices
convert< _PySequence_, Indices >(PyObject * pyObj)
{
  Pointer<Collection<UnsignedInteger> > ptr = buildCollectionFromPySequence<UnsignedInteger>(pyObj);
  return Indices(ptr->begin(), ptr->end());
}

template <>
inline
PyObject *
convert< Indices, _PySequence_ >(Indices inP)
{
  UnsignedInteger dimension = inP.getSize();
  PyObject * point = PyTuple_New(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    PyTuple_SetItem(point, i, convert< UnsignedInteger, _PyInt_ >(inP[i]));
  }
  return point;
}

template <>
struct traitsPythonType< IndicesCollection >
{
  typedef _PySequence_ Type;
};

template <>
inline
IndicesCollection
convert< _PySequence_, IndicesCollection >(PyObject * pyObj)
{
  // Check whether pyObj follows the buffer protocol
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      if (view.ndim == 2 &&
          view.itemsize == traitsPythonType<UnsignedInteger>::buf_itemsize &&
          view.format != NULL &&
          strcmp(view.format, pyBuf_formats[traitsPythonType<UnsignedInteger>::buf_format_idx]) == 0)
      {
        const UnsignedInteger* data = static_cast<const UnsignedInteger*>(view.buf);
        const UnsignedInteger size = view.shape[0];
        const UnsignedInteger dimension = view.shape[1];
        IndicesCollection indices(size, dimension);
        if (PyBuffer_IsContiguous(&view, 'C'))
        {
          // 2-d contiguous array in C notation, we can directly copy memory chunk
          std::copy(data, data + size * dimension, &indices(0, 0));
        }
        else
        {
          for (UnsignedInteger j = 0; j < dimension; ++j)
            for(UnsignedInteger i = 0; i < size; ++i, ++data)
              indices(i, j) = *data;
        }
        PyBuffer_Release(&view);
        return indices;
      }
      PyBuffer_Release(&view);
    }
    else
      PyErr_Clear();
  }

  // use the same conversion function for numpy array/matrix, knowing numpy matrix is not a sequence
  if (PyObject_HasAttrString(pyObj, const_cast<char *>("shape")))
  {
    ScopedPyObjectPointer shapeObj(PyObject_GetAttrString(pyObj, "shape"));
    if (shapeObj.get())
    {
      Indices shape(checkAndConvert< _PySequence_, Indices >(shapeObj.get()));
      if (shape.getSize() == 2)
      {
        UnsignedInteger size = shape[0];
        UnsignedInteger dimension = shape[1];
        ScopedPyObjectPointer askObj(PyTuple_New(2));
        ScopedPyObjectPointer methodObj(convert< String, _PyString_ >("__getitem__"));
        IndicesCollection indices(size, dimension);
        for (UnsignedInteger i = 0; i < size; ++ i)
        {
          PyTuple_SetItem(askObj.get(), 0, convert< UnsignedInteger, _PyInt_ >(i));
          for (UnsignedInteger j = 0; j < dimension; ++ j)
          {
            PyTuple_SetItem(askObj.get(), 1, convert< UnsignedInteger, _PyInt_ >(j));
            ScopedPyObjectPointer elt(PyObject_CallMethodObjArgs(pyObj, methodObj.get(), askObj.get(), NULL));
            if (elt.get())
            {
              indices(i, j) = checkAndConvert<_PyInt_, UnsignedInteger>(elt.get());
            }
          }
        }
        return indices;
      }
      else
        throw InvalidArgumentException(HERE) << "Invalid array dimension: " << shape.getSize();
    }
  }
  // This object is a sequence; unlike Matrix and Sample, dimension is not constant.
  check<_PySequence_>(pyObj);
  ScopedPyObjectPointer newPyObj(PySequence_Fast(pyObj, ""));
  if (!newPyObj.get()) throw InvalidArgumentException(HERE) << "Not a sequence object";
  const UnsignedInteger size = PySequence_Fast_GET_SIZE(newPyObj.get());
  if (size == 0) return IndicesCollection();
  // Allocate a Collection of Indices
  Collection<Indices> coll(size);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    PyObject * indicesObj = PySequence_Fast_GET_ITEM(newPyObj.get(), i);
    ScopedPyObjectPointer newPyIndicesObj(PySequence_Fast(indicesObj, ""));
    // Check that object is a sequence
    check<_PySequence_>(indicesObj);
    const UnsignedInteger dimension = PySequence_Fast_GET_SIZE(newPyIndicesObj.get());
    Indices newIndices(dimension);
    for(UnsignedInteger j = 0; j < dimension; ++j)
    {
      PyObject * value = PySequence_Fast_GET_ITEM(newPyIndicesObj.get(), j);
      newIndices[j] = checkAndConvert<_PyInt_, UnsignedInteger>(value);
    }
    coll[i] = newIndices;
  }
  return IndicesCollection(coll);
}


template <>
struct traitsPythonType< Description >
{
  typedef _PySequence_ Type;
};

template <>
inline
Description
convert<_PySequence_, Description>(PyObject * pyObj)
{
  Pointer<Collection<String> > ptr = buildCollectionFromPySequence<String>(pyObj);
  return Description(*ptr);
}



template <>
struct traitsPythonType< Collection< Scalar > >
{
  typedef _PySequence_ Type;
};

template <>
inline
Collection<Scalar>
convert< _PySequence_, Collection<Scalar> >(PyObject * pyObj)
{
  // Check whether pyObj follows the buffer protocol
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      if (view.ndim == 1 &&
          view.itemsize == traitsPythonType<Scalar>::buf_itemsize &&
          view.format != NULL &&
          strcmp(view.format, pyBuf_formats[traitsPythonType<Scalar>::buf_format_idx]) == 0)
      {
        // 1-d contiguous array, we can directly copy memory chunk
        const Scalar* data = static_cast<const Scalar*>(view.buf);
        const UnsignedInteger size = view.shape[0];
        Collection<Scalar> result(size);
        std::copy(data, data + size, &result[0]);
        PyBuffer_Release(&view);
        return result;
      }
      PyBuffer_Release(&view);
    }
    else
      PyErr_Clear();
  }
  Pointer<Collection<Scalar> > ptr = buildCollectionFromPySequence<Scalar>(pyObj);
  return Collection<Scalar>(*ptr);
}



template <>
struct traitsPythonType< MatrixImplementation >
{
  typedef _PySequence_ Type;
};



template <>
inline
MatrixImplementation*
convert< _PySequence_, MatrixImplementation* >(PyObject * pyObj)
{
  MatrixImplementation *p_implementation = 0;
  // Check whether pyObj follows the buffer protocol
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      if (view.ndim == 2 &&
          view.itemsize == traitsPythonType<Scalar>::buf_itemsize &&
          view.format != NULL &&
          strcmp(view.format, pyBuf_formats[traitsPythonType<Scalar>::buf_format_idx]) == 0)
      {
        const Scalar* data = static_cast<const Scalar*>(view.buf);
        const UnsignedInteger nbRows = view.shape[0];
        const UnsignedInteger nbColumns = view.shape[1];
        p_implementation = new MatrixImplementation(nbRows, nbColumns);
        if (PyBuffer_IsContiguous(&view, 'F'))
        {
          // 2-d contiguous array in Fortran notation, we can directly copy memory chunk
          std::copy(data, data + nbRows * nbColumns, &p_implementation->operator()(0, 0));
        }
        else
        {
          for(UnsignedInteger i = 0; i < nbRows; ++i)
            for (UnsignedInteger j = 0; j < nbColumns; ++j, ++data)
              p_implementation->operator()(i, j) = *data;
        }
        PyBuffer_Release(&view);
        return p_implementation;
      }
      PyBuffer_Release(&view);
    }
    else
      PyErr_Clear();
  }

  // use the same conversion function for numpy array/matrix, knowing numpy matrix is not a sequence
  if (PyObject_HasAttrString(pyObj, const_cast<char *>("shape")))
  {
    ScopedPyObjectPointer shapeObj(PyObject_GetAttrString(pyObj, "shape"));
    if (shapeObj.get())
    {
      Indices shape(checkAndConvert< _PySequence_, Indices >(shapeObj.get()));
      if (shape.getSize() == 2)
      {
        UnsignedInteger nbRows = shape[0];
        UnsignedInteger nbColumns = shape[1];
        ScopedPyObjectPointer askObj(PyTuple_New(2));
        ScopedPyObjectPointer methodObj(convert< String, _PyString_ >("__getitem__"));
        p_implementation = new MatrixImplementation(nbRows, nbColumns);
        for (UnsignedInteger i = 0; i < nbRows; ++ i)
        {
          PyTuple_SetItem(askObj.get(), 0, convert< UnsignedInteger, _PyInt_ >(i));
          for (UnsignedInteger j = 0; j < nbColumns; ++ j)
          {
            PyTuple_SetItem(askObj.get(), 1, convert< UnsignedInteger, _PyInt_ >(j));
            ScopedPyObjectPointer elt(PyObject_CallMethodObjArgs(pyObj, methodObj.get(), askObj.get(), NULL));
            if (elt.get())
            {
              try
              {
                p_implementation->operator()(i, j) = checkAndConvert<_PyFloat_, Scalar>(elt.get());
              }
              catch (InvalidArgumentException &)
              {
                delete p_implementation;
                throw;
              }
            }
          }
        }
      }
      else
        throw InvalidArgumentException(HERE) << "Invalid array dimension: " << shape.getSize();
    }
  }
  else if (PyObject_HasAttrString(pyObj, const_cast<char *>("getNbColumns")))
  {
    // case of conversion from XMatrix to YMatrix
    // X could be Square,Triangular,Identity...
    // YMatrix might be Matrix of one of its inheritance types
    ScopedPyObjectPointer colunmsObj(PyObject_CallMethod (pyObj,
                                     const_cast<char *>("getNbColumns"),
                                     const_cast<char *>("()")));
    ScopedPyObjectPointer rowsObj(PyObject_CallMethod (pyObj,
                                  const_cast<char *>("getNbRows"),
                                  const_cast<char *>("()")));
    ScopedPyObjectPointer implObj(PyObject_CallMethod (pyObj,
                                  const_cast<char *>("getImplementation"),
                                  const_cast<char *>("()")));
    Pointer< Collection< Scalar > > ptr = buildCollectionFromPySequence< Scalar >(implObj.get());
    UnsignedInteger nbColumns = checkAndConvert< _PyInt_, UnsignedInteger >(colunmsObj.get());
    UnsignedInteger nbRows = checkAndConvert< _PyInt_, UnsignedInteger >(rowsObj.get());
    p_implementation = new MatrixImplementation(nbRows, nbColumns, *ptr);
  }
  else
  {
    // try to convert from a sequence of sequences
    Pointer< Collection< Point > > ptr = buildCollectionFromPySequence< Point >(pyObj);
    Sample sample(*ptr);
    UnsignedInteger nbRows = sample.getSize();
    UnsignedInteger nbColumns = sample.getDimension();
    p_implementation = new MatrixImplementation(nbRows, nbColumns);
    for (UnsignedInteger i = 0; i < nbRows; ++ i)
      for (UnsignedInteger j = 0; j < nbColumns; ++ j)
        p_implementation->operator()(i, j) = sample(i, j);
  }
  return p_implementation;
}



template <>
inline
Matrix
convert< _PySequence_, Matrix >(PyObject * pyObj)
{
  MatrixImplementation *p_implementation = convert< _PySequence_, MatrixImplementation* >(pyObj);
  return Matrix(p_implementation);
}



template <>
inline
SquareMatrix
convert< _PySequence_, SquareMatrix >(PyObject * pyObj)
{
  MatrixImplementation *p_implementation = convert< _PySequence_, MatrixImplementation* >(pyObj);
  if (p_implementation->getNbRows() != p_implementation->getNbColumns())
    throw InvalidArgumentException(HERE) << "The matrix is not square";
  return SquareMatrix(p_implementation);
}



template <>
inline
TriangularMatrix
convert< _PySequence_, TriangularMatrix >(PyObject * pyObj)
{
  MatrixImplementation *p_implementation = convert< _PySequence_, MatrixImplementation* >(pyObj);
  if (!(p_implementation->isTriangular(true) || p_implementation->isTriangular(false)))
    throw InvalidArgumentException(HERE) << "The matrix is not triangular";
  return TriangularMatrix(p_implementation, p_implementation->isTriangular(true));
}



template <>
inline
SymmetricMatrix
convert< _PySequence_, SymmetricMatrix >(PyObject * pyObj)
{
  MatrixImplementation *p_implementation = convert< _PySequence_, MatrixImplementation* >(pyObj);
  if (!p_implementation->isSymmetric())
    throw InvalidArgumentException(HERE) << "The matrix is not symmetric";
  return SymmetricMatrix(p_implementation);
}



template <>
inline
CovarianceMatrix
convert< _PySequence_, CovarianceMatrix >(PyObject * pyObj)
{
  MatrixImplementation *p_implementation = convert< _PySequence_, MatrixImplementation* >(pyObj);
  if (!p_implementation->isSymmetric())
    throw InvalidArgumentException(HERE) << "The matrix is not symmetric";
  // SPD check is too expensive
  return CovarianceMatrix(p_implementation);
}



template <>
inline
CorrelationMatrix
convert< _PySequence_, CorrelationMatrix >(PyObject * pyObj)
{
  MatrixImplementation *p_implementation = convert< _PySequence_, MatrixImplementation* >(pyObj);
  if (!p_implementation->isSymmetric())
    throw InvalidArgumentException(HERE) << "The matrix is not symmetric";
  // SPD check is too expensive
  if (!p_implementation->hasUnitRange())
    throw InvalidArgumentException(HERE) << "The matrix range is not (-1;1)";
  return CorrelationMatrix(p_implementation);
}



template <>
struct traitsPythonType< TensorImplementation >
{
  typedef _PySequence_ Type;
};



template <>
inline
TensorImplementation*
convert< _PySequence_, TensorImplementation* >(PyObject * pyObj)
{
  // Check whether pyObj follows the buffer protocol
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      if (view.ndim == 3 &&
          view.itemsize == traitsPythonType<Scalar>::buf_itemsize &&
          view.format != NULL &&
          strcmp(view.format, pyBuf_formats[traitsPythonType<Scalar>::buf_format_idx]) == 0)
      {
        const Scalar* data = static_cast<const Scalar*>(view.buf);
        const UnsignedInteger nbRows = view.shape[0];
        const UnsignedInteger nbColumns = view.shape[1];
        const UnsignedInteger nbSheets = view.shape[2];
        TensorImplementation *p_implementation = new TensorImplementation(nbRows, nbColumns, nbSheets);
        if (PyBuffer_IsContiguous(&view, 'F'))
        {
          // 3-d contiguous array in Fortran notation, we can directly copy memory chunk
          std::copy(data, data + nbRows * nbColumns * nbSheets, &p_implementation->operator()(0, 0, 0));
        }
        else
        {
          for(UnsignedInteger i = 0; i < nbRows; ++i)
            for (UnsignedInteger j = 0; j < nbColumns; ++j)
              for (UnsignedInteger k = 0; k < nbSheets; ++k, ++data)
                p_implementation->operator()(i, j, k) = *data;
        }
        PyBuffer_Release(&view);
        return p_implementation;
      }
      PyBuffer_Release(&view);
    }
    else
      PyErr_Clear();
  }

  Pointer< Collection< Sample > > ptr = buildCollectionFromPySequence< Sample >(pyObj);
  UnsignedInteger nbRows = ptr->getSize();
  UnsignedInteger nbColumns = ptr->getSize() > 0 ? (*ptr)[0].getSize() : 0;
  UnsignedInteger nbSheets = ptr->getSize() > 0 ? (*ptr)[0].getDimension() : 0;
  TensorImplementation *p_implementation = new TensorImplementation(nbRows, nbColumns, nbSheets);
  for (UnsignedInteger i = 0; i < nbRows; ++ i)
    for (UnsignedInteger j = 0; j < nbColumns; ++ j)
      for (UnsignedInteger k = 0; k < nbSheets; ++ k)
        p_implementation->operator()(i, j, k) = (*ptr)[i](j, k);
  return p_implementation;
}



template <>
inline
Tensor
convert< _PySequence_, Tensor >(PyObject * pyObj)
{
  TensorImplementation *p_implementation = convert<_PySequence_, TensorImplementation*>(pyObj);
  return Tensor(p_implementation);
}



template <>
inline
SymmetricTensor
convert< _PySequence_, SymmetricTensor >(PyObject * pyObj)
{
  TensorImplementation *p_implementation = convert< _PySequence_, TensorImplementation* >(pyObj);
  if (!p_implementation->isSymmetric())
    throw InvalidArgumentException(HERE) << "The tensor is not symmetric";
  return SymmetricTensor(p_implementation);
}



template <>
struct traitsPythonType< ComplexMatrixImplementation* >
{
  typedef _PySequence_ Type;
};



template <>
inline
ComplexMatrixImplementation*
convert< _PySequence_, ComplexMatrixImplementation* >(PyObject * pyObj)
{
  // Check whether pyObj follows the buffer protocol
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      if (view.ndim == 2 &&
          view.itemsize == traitsPythonType<Complex>::buf_itemsize &&
          view.format != NULL &&
          strcmp(view.format, pyBuf_formats[traitsPythonType<Complex>::buf_format_idx]) == 0)
      {
        const Complex* data = static_cast<const Complex*>(view.buf);
        const UnsignedInteger nbRows = view.shape[0];
        const UnsignedInteger nbColumns = view.shape[1];
        ComplexMatrixImplementation *p_implementation = new ComplexMatrixImplementation(nbRows, nbColumns);
        if (PyBuffer_IsContiguous(&view, 'F'))
        {
          // 2-d contiguous array in Fortran notation, we can directly copy memory chunk
          std::copy(data, data + nbRows * nbColumns, &p_implementation->operator()(0, 0));
        }
        else
        {
          for(UnsignedInteger i = 0; i < nbRows; ++i)
            for (UnsignedInteger j = 0; j < nbColumns; ++j, ++data)
              p_implementation->operator()(i, j) = *data;
        }
        PyBuffer_Release(&view);
        return p_implementation;
      }
      PyBuffer_Release(&view);
    }
    else
      PyErr_Clear();
  }

  // use the same conversion function for numpy array/matrix, knowing numpy matrix is not a sequence
  if (PyObject_HasAttrString(pyObj, const_cast<char *>("shape")))
  {
    ScopedPyObjectPointer shapeObj(PyObject_GetAttrString(pyObj, "shape"));
    if (shapeObj.get())
    {
      Indices shape(checkAndConvert< _PySequence_, Indices >(shapeObj.get()));
      if (shape.getSize() == 2)
      {
        UnsignedInteger nbRows = shape[0];
        UnsignedInteger nbColumns = shape[1];
        ScopedPyObjectPointer askObj(PyTuple_New(2));
        ScopedPyObjectPointer methodObj(convert< String, _PyString_ >("__getitem__"));
        ComplexMatrixImplementation *p_implementation = new ComplexMatrixImplementation(nbRows, nbColumns);
        for (UnsignedInteger i = 0; i < nbRows; ++ i)
        {
          PyTuple_SetItem(askObj.get(), 0, convert< UnsignedInteger, _PyInt_ >(i));
          for (UnsignedInteger j = 0; j < nbColumns; ++ j)
          {
            PyTuple_SetItem(askObj.get(), 1, convert< UnsignedInteger, _PyInt_ >(j));
            ScopedPyObjectPointer elt(PyObject_CallMethodObjArgs(pyObj, methodObj.get(), askObj.get(), NULL));
            if (elt.get())
            {
              try
              {
                p_implementation->operator()(i, j) = checkAndConvert<_PyComplex_, Complex>(elt.get());
              }
              catch (InvalidArgumentException &)
              {
                delete p_implementation;
                throw;
              }
            }
          }
        }
        return p_implementation;
      }
      else
        throw InvalidArgumentException(HERE) << "Invalid array dimension: " << shape.getSize();
    }
  }

  // case of conversion from XMatrix to YMatrix
  // X could be Square,Triangular,Identity...
  // YMatrix might be Matrix of one of its inheritance types
  if (PyObject_HasAttrString(pyObj, const_cast<char *>("getNbColumns")))
  {
    ScopedPyObjectPointer colunmsObj(PyObject_CallMethod (pyObj,
                                     const_cast<char *>("getNbColumns"),
                                     const_cast<char *>("()")));
    ScopedPyObjectPointer rowsObj(PyObject_CallMethod (pyObj,
                                  const_cast<char *>("getNbRows"),
                                  const_cast<char *>("()")));
    ScopedPyObjectPointer implObj(PyObject_CallMethod (pyObj,
                                  const_cast<char *>("getImplementation"),
                                  const_cast<char *>("()")));
    Pointer< Collection< Complex > > ptr = buildCollectionFromPySequence< Complex >(implObj.get());
    UnsignedInteger nbColumns = checkAndConvert< _PyInt_, UnsignedInteger >(colunmsObj.get());
    UnsignedInteger nbRows = checkAndConvert< _PyInt_, UnsignedInteger >(rowsObj.get());
    ComplexMatrixImplementation *p_implementation = new ComplexMatrixImplementation(nbRows, nbColumns, *ptr);
    return p_implementation;
  }

  // else try to convert from a sequence of sequences
  Pointer< Collection< Collection< Complex > > > ptr = buildCollectionFromPySequence< Collection< Complex > >(pyObj);
  UnsignedInteger nbRows = ptr->getSize();
  UnsignedInteger nbColumns = ptr->getSize() > 0 ? (*ptr)[0].getSize() : 0;
  ComplexMatrixImplementation *p_implementation = new ComplexMatrixImplementation(nbRows, nbColumns);
  for (UnsignedInteger i = 0; i < nbRows; ++ i)
    for (UnsignedInteger j = 0; j < nbColumns; ++ j)
      p_implementation->operator()(i, j) = (*ptr)[i][j];
  return p_implementation;
}



template <>
inline
ComplexMatrix
convert< _PySequence_, ComplexMatrix >(PyObject * pyObj)
{
  ComplexMatrixImplementation *p_implementation = convert< _PySequence_, ComplexMatrixImplementation* >(pyObj);
  return ComplexMatrix(p_implementation);
}



template <>
inline
SquareComplexMatrix
convert< _PySequence_, SquareComplexMatrix >(PyObject * pyObj)
{
  ComplexMatrixImplementation *p_implementation = convert< _PySequence_, ComplexMatrixImplementation* >(pyObj);
  if (p_implementation->getNbRows() != p_implementation->getNbColumns())
    throw InvalidArgumentException(HERE) << "The matrix is not square";
  return SquareComplexMatrix(p_implementation);
}



template <>
inline
TriangularComplexMatrix
convert< _PySequence_, TriangularComplexMatrix >(PyObject * pyObj)
{
  ComplexMatrixImplementation *p_implementation = convert< _PySequence_, ComplexMatrixImplementation* >(pyObj);
  Bool lower = p_implementation->isTriangular(true);
  if (!lower && !p_implementation->isTriangular(false))
    throw InvalidArgumentException(HERE) << "The matrix is not triangular";
  return TriangularComplexMatrix(p_implementation, lower);
}


template <>
struct traitsPythonType< HermitianMatrix >
{
  typedef _PySequence_ Type;
};

template <>
inline
HermitianMatrix
convert< _PySequence_, HermitianMatrix >(PyObject * pyObj)
{
  ComplexMatrixImplementation *p_implementation = convert< _PySequence_, ComplexMatrixImplementation* >(pyObj);
  if (!p_implementation->isHermitian())
    throw InvalidArgumentException(HERE) << "The matrix is not hermitian";
  return HermitianMatrix(p_implementation);
}

template <>
struct traitsPythonType< ComplexTensorImplementation >
{
  typedef _PySequence_ Type;
};



template <>
inline
ComplexTensorImplementation*
convert< _PySequence_, ComplexTensorImplementation* >(PyObject * pyObj)
{
  ComplexTensorImplementation *p_implementation = 0;

  // Check whether pyObj follows the buffer protocol
  if (PyObject_CheckBuffer(pyObj))
  {
    Py_buffer view;
    if (PyObject_GetBuffer(pyObj, &view, PyBUF_FORMAT | PyBUF_ND | PyBUF_ANY_CONTIGUOUS) >= 0)
    {
      if (view.ndim == 3 &&
          view.itemsize == traitsPythonType<Complex>::buf_itemsize &&
          view.format != NULL &&
          strcmp(view.format, pyBuf_formats[traitsPythonType<Complex>::buf_format_idx]) == 0)
      {
        const Complex* data = static_cast<const Complex*>(view.buf);
        const UnsignedInteger nbRows = view.shape[0];
        const UnsignedInteger nbColumns = view.shape[1];
        const UnsignedInteger nbSheets = view.shape[2];
        p_implementation = new ComplexTensorImplementation(nbRows, nbColumns, nbSheets);
        if (PyBuffer_IsContiguous(&view, 'F'))
        {
          // 3-d contiguous array in Fortran notation, we can directly copy memory chunk
          std::copy(data, data + nbRows * nbColumns * nbSheets, &p_implementation->operator()(0, 0, 0));
        }
        else
        {
          for(UnsignedInteger i = 0; i < nbRows; ++i)
            for (UnsignedInteger j = 0; j < nbColumns; ++j)
              for (UnsignedInteger k = 0; k < nbSheets; ++k, ++data)
                p_implementation->operator()(i, j, k) = *data;
        }
        PyBuffer_Release(&view);
        return p_implementation;
      }
      PyBuffer_Release(&view);
    }
    else
      PyErr_Clear();
  }
  // use the same conversion function for numpy array/matrix, knowing numpy matrix is not a sequence
  if (PyObject_HasAttrString(pyObj, const_cast<char *>("shape")))
  {
    ScopedPyObjectPointer shapeObj(PyObject_GetAttrString(pyObj, "shape"));
    if (shapeObj.get())
    {
      Indices shape(checkAndConvert< _PySequence_, Indices >(shapeObj.get()));
      if (shape.getSize() == 3)
      {
        UnsignedInteger nbRows = shape[0];
        UnsignedInteger nbColumns = shape[1];
        UnsignedInteger nbSheets = shape[2];
        ScopedPyObjectPointer askObj(PyTuple_New(3));
        ScopedPyObjectPointer methodObj(convert< String, _PyString_ >("__getitem__"));
        p_implementation = new ComplexTensorImplementation(nbRows, nbColumns, nbSheets);
        for (UnsignedInteger i = 0; i < nbRows; ++ i)
        {
          PyTuple_SetItem(askObj.get(), 0, convert< UnsignedInteger, _PyInt_ >(i));
          for (UnsignedInteger j = 0; j < nbColumns; ++ j)
          {
            PyTuple_SetItem(askObj.get(), 1, convert< UnsignedInteger, _PyInt_ >(j));
            for (UnsignedInteger k = 0; k < nbSheets; ++ k)
            {
              PyTuple_SetItem(askObj.get(), 2, convert< UnsignedInteger, _PyInt_ >(k));
              ScopedPyObjectPointer elt(PyObject_CallMethodObjArgs(pyObj, methodObj.get(), askObj.get(), NULL));
              if (elt.get())
              {
                try
                {
                  p_implementation->operator()(i, j, k) = checkAndConvert<_PyComplex_, Complex>(elt.get());
                }
                catch (InvalidArgumentException &)
                {
                  delete p_implementation;
                  throw;
                }
              }
            }
          }
        }
      }
      else
        throw InvalidArgumentException(HERE) << "Invalid array dimension: " << shape.getSize();
    }
  }
  else if (PyObject_HasAttrString(pyObj, const_cast<char *>("getNbSheets")))
  {
    // case of conversion from XTensor to YTensor
    ScopedPyObjectPointer colunmsObj(PyObject_CallMethod (pyObj,
                                     const_cast<char *>("getNbColumns"),
                                     const_cast<char *>("()")));
    ScopedPyObjectPointer rowsObj(PyObject_CallMethod (pyObj,
                                  const_cast<char *>("getNbRows"),
                                  const_cast<char *>("()")));
    ScopedPyObjectPointer sheetsObj(PyObject_CallMethod (pyObj,
                                    const_cast<char *>("getNbSheets"),
                                    const_cast<char *>("()")));
    ScopedPyObjectPointer implObj(PyObject_CallMethod (pyObj,
                                  const_cast<char *>("getImplementation"),
                                  const_cast<char *>("()")));
    Pointer< Collection< Complex > > ptr = buildCollectionFromPySequence< Complex >(implObj.get());
    UnsignedInteger nbColumns = checkAndConvert< _PyInt_, UnsignedInteger >(colunmsObj.get());
    UnsignedInteger nbRows = checkAndConvert< _PyInt_, UnsignedInteger >(rowsObj.get());
    UnsignedInteger nbSheets = checkAndConvert< _PyInt_, UnsignedInteger >(sheetsObj.get());
    p_implementation = new ComplexTensorImplementation(nbRows, nbColumns, nbSheets, *ptr);
  }
  return p_implementation;
}



template <>
inline
ComplexTensor
convert< _PySequence_, ComplexTensor >(PyObject * pyObj)
{
  ComplexTensorImplementation *p_implementation = convert<_PySequence_, ComplexTensorImplementation*>(pyObj);
  return ComplexTensor(p_implementation);
}

template <>
struct traitsPythonType< WhittleFactoryState >
{
  typedef _PySequence_ Type;
};

template <>
inline
WhittleFactoryState
convert< _PySequence_, WhittleFactoryState >(PyObject *)
{
  return WhittleFactoryState();
}


// PySliceObject type is deprecated
#if PY_VERSION_HEX >= 0x03020000
inline PyObject* SliceCast(PyObject* pyObj)
{
  return pyObj;
}
#else
inline PySliceObject* SliceCast(PyObject* pyObj)
{
  return (PySliceObject*)pyObj;
}
#endif


inline
void pickleSave(Advocate & adv, PyObject * pyObj, const String attributName = "pyInstance_")
{
  ScopedPyObjectPointer pickleModule(PyImport_ImportModule("pickle")); // new reference
  assert(pickleModule.get());

  PyObject * pickleDict = PyModule_GetDict(pickleModule.get());
  assert(pickleDict);

  PyObject * dumpsMethod = PyDict_GetItemString(pickleDict, "dumps");
  assert(dumpsMethod);
  if (! PyCallable_Check(dumpsMethod))
    throw InternalException(HERE) << "Python 'pickle' module has no 'dumps' method";

  assert(pyObj);
  ScopedPyObjectPointer rawDump(PyObject_CallFunctionObjArgs(dumpsMethod, pyObj, NULL)); // new reference

  handleException();
  assert(rawDump.get());

  ScopedPyObjectPointer base64Module(PyImport_ImportModule("base64")); // new reference
  assert(base64Module.get());

  PyObject * base64Dict = PyModule_GetDict(base64Module.get());
  assert(base64Dict);

  PyObject * b64encodeMethod = PyDict_GetItemString(base64Dict, "standard_b64encode");
  assert(b64encodeMethod);
  if (! PyCallable_Check(b64encodeMethod))
    throw InternalException(HERE) << "Python 'base64' module has no 'standard_b64encode' method";

  ScopedPyObjectPointer base64Dump(PyObject_CallFunctionObjArgs(b64encodeMethod, rawDump.get(), NULL)); // new reference
  handleException();
  assert(base64Dump.get());

  String pyInstanceSt(convert< _PyBytes_, String >(base64Dump.get()));
  adv.saveAttribute(attributName, pyInstanceSt);
}


inline
void pickleLoad(Advocate & adv, PyObject * & pyObj, const String attributName = "pyInstance_")
{
  String pyInstanceSt;
  adv.loadAttribute(attributName, pyInstanceSt);

  ScopedPyObjectPointer base64Dump(convert< String, _PyBytes_ >(pyInstanceSt)); // new reference
  assert(base64Dump.get());

  ScopedPyObjectPointer base64Module(PyImport_ImportModule("base64")); // new reference
  assert(base64Module.get());

  PyObject * base64Dict = PyModule_GetDict(base64Module.get());
  assert(base64Dict);

  PyObject * b64decodeMethod = PyDict_GetItemString(base64Dict, "standard_b64decode");
  assert(b64decodeMethod);
  if (! PyCallable_Check(b64decodeMethod))
    throw InternalException(HERE) << "Python 'base64' module has no 'standard_b64decode' method";

  ScopedPyObjectPointer rawDump(PyObject_CallFunctionObjArgs(b64decodeMethod, base64Dump.get(), NULL)); // new reference
  handleException();
  assert(rawDump.get());

  ScopedPyObjectPointer pickleModule(PyImport_ImportModule("pickle")); // new reference
  assert(pickleModule.get());

  PyObject * pickleDict = PyModule_GetDict(pickleModule.get());
  assert(pickleDict);

  PyObject * loadsMethod = PyDict_GetItemString(pickleDict, "loads");
  assert(loadsMethod);
  if (! PyCallable_Check(loadsMethod))
    throw InternalException(HERE) << "Python 'pickle' module has no 'loads' method";

  Py_XDECREF(pyObj);
  pyObj = PyObject_CallFunctionObjArgs(loadsMethod, rawDump.get(), NULL); // new reference
  handleException();
  assert(pyObj);
}



inline
ScopedPyObjectPointer deepCopy(PyObject * pyObj)
{
  ScopedPyObjectPointer copyModule(PyImport_ImportModule("copy"));
  assert(copyModule.get());

  PyObject * copyDict = PyModule_GetDict(copyModule.get());
  assert(copyDict);

  PyObject * deepCopyMethod = PyDict_GetItemString(copyDict, "deepcopy");
  assert(deepCopyMethod );

  if (!PyCallable_Check(deepCopyMethod))
    throw InternalException(HERE) << "Python 'copy' module has no 'deepcopy' method";

  ScopedPyObjectPointer pyObjDeepCopy(PyObject_CallFunctionObjArgs(deepCopyMethod, pyObj, NULL));
  handleException();
  return pyObjDeepCopy;
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONWRAPPINGFUNCTIONS_HXX */
