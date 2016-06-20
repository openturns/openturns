//                                               -*- C++ -*-
/**
 * @brief This file provides functions to ease Python wrapping
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
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
  explicit ScopedPyObjectPointer( PyObject * pyObj = 0 )
    : pyObj_(pyObj)
  {
  }

  ~ScopedPyObjectPointer()
  {
    Py_XDECREF( pyObj_ );
  }

  ScopedPyObjectPointer & operator=(PyObject * pyObj)
  {
    Py_XDECREF( pyObj_ );
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
isAPython<_PyObject_>(PyObject * pyObj)
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

template <>
struct traitsPythonType< UnsignedInteger >
{
  typedef _PyInt_ Type;
};

template <>
inline
bool
canConvert< _PyInt_, UnsignedInteger >(PyObject * pyObj)
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
struct traitsPythonType< NumericalScalar >
{
  typedef _PyFloat_ Type;
};

template <>
inline
NumericalScalar
convert< _PyFloat_, NumericalScalar >(PyObject * pyObj)
{
  return PyFloat_AsDouble(pyObj);
}

template <>
inline
PyObject *
convert< NumericalScalar, _PyFloat_ >(NumericalScalar x)
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
struct traitsPythonType< NumericalComplex >
{
  typedef _PyComplex_ Type;
};

template <>
inline
NumericalComplex
convert< _PyComplex_, NumericalComplex >(PyObject * pyObj)
{
  return NumericalComplex(PyComplex_RealAsDouble(pyObj), PyComplex_ImagAsDouble(pyObj));
}

template <>
inline
PyObject *
convert< NumericalComplex, _PyComplex_ >(NumericalComplex x)
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
convert< String, _PyBytes_ >( String s )
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
convert< String, _PyString_ >( String s )
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




template <class PYTHON_Type>
static inline
int
isAPythonSequenceOf(PyObject * pyObj)
{
  int ok = isAPython<_PySequence_>(pyObj) && (! isAPython< _PyString_ >(pyObj));

  if ( ok )
  {
    const UnsignedInteger size = PySequence_Size(pyObj);
    for( UnsignedInteger i = 0; ok && (i < size); ++i )
    {
      ScopedPyObjectPointer elt(PySequence_ITEM( pyObj, i ));
      int elt_ok = isAPython<PYTHON_Type>(elt.get());
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
  catch( InvalidArgumentException & ex )
  {
    return false;
  }

  ScopedPyObjectPointer newPyObj(PySequence_Fast( pyObj, "" ));

  const UnsignedInteger size = PySequence_Fast_GET_SIZE( newPyObj.get() );
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    PyObject * elt = PySequence_Fast_GET_ITEM( newPyObj.get(), i );
    if (!canConvert< typename traitsPythonType< T >::Type, T >( elt ))
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
  ScopedPyObjectPointer newPyObj(PySequence_Fast( pyObj, "" ));
  if (!newPyObj.get()) throw InvalidArgumentException(HERE) << "Not a sequence object";
  const UnsignedInteger size = PySequence_Fast_GET_SIZE( newPyObj.get() );
  if ((sz != 0) && (sz != (int)size))
  {
    throw InvalidArgumentException(HERE) << "Sequence object has incorrect size " << size << ". Must be " << sz << ".";
  }
  Collection<T> * p_coll = new Collection< T >( size );

  for(UnsignedInteger i = 0; i < size; ++i)
  {
    PyObject * elt = PySequence_Fast_GET_ITEM( newPyObj.get(), i );
    try
    {
      check<typename traitsPythonType< T >::Type>( elt );
    }
    catch( InvalidArgumentException & ex )
    {
      delete p_coll;
      throw;
    }
    (*p_coll)[i] = convert< typename traitsPythonType< T >::Type, T >( elt );
  }

  return p_coll;
}






template <>
struct traitsPythonType< NumericalPoint >
{
  typedef _PySequence_ Type;
};

template <>
inline
NumericalPoint
convert< _PySequence_, NumericalPoint >(PyObject * pyObj)
{
  Pointer<Collection<NumericalScalar> > ptr = buildCollectionFromPySequence<NumericalScalar>(pyObj);
  return NumericalPoint( *ptr );
}

template <>
inline
PyObject *
convert< NumericalPoint, _PySequence_ >(NumericalPoint inP)
{
  UnsignedInteger dimension = inP.getDimension();
  PyObject * point = PyTuple_New( dimension );
  for ( UnsignedInteger i = 0; i < dimension; ++ i )
  {
    PyTuple_SetItem( point, i, convert< NumericalScalar, _PyFloat_ >( inP[i] ) );
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
struct traitsPythonType< Collection < NumericalComplex> >
{
  typedef _PySequence_ Type;
};


template <>
inline
Collection<NumericalComplex>
convert<_PySequence_, Collection<NumericalComplex> >(PyObject * pyObj)
{
  Pointer<Collection<NumericalComplex> > ptr = buildCollectionFromPySequence<NumericalComplex>(pyObj);
  return Collection<NumericalComplex>( *ptr );
}


template <>
struct traitsPythonType< NumericalSample >
{
  typedef _PySequence_ Type;
};

template <>
inline
NumericalSample
convert< _PySequence_, NumericalSample >(PyObject * pyObj)
{
  Pointer<Collection<NumericalPoint> > ptr = buildCollectionFromPySequence<NumericalPoint>(pyObj);
  return NumericalSample( *ptr );
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
  Pointer<Collection<UnsignedInteger> > ptr = buildCollectionFromPySequence<UnsignedInteger>(pyObj);
  return Collection<UnsignedInteger>( ptr->begin(), ptr->end() );
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
  return Indices( ptr->begin(), ptr->end() );
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
    PyTuple_SetItem( point, i, convert< UnsignedInteger, _PyInt_ >(inP[i]));
  }
  return point;
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
  return Description( *ptr );
}



template <>
struct traitsPythonType< Collection< NumericalScalar > >
{
  typedef _PySequence_ Type;
};

template <>
inline
Collection<NumericalScalar>
convert< _PySequence_, Collection<NumericalScalar> >(PyObject * pyObj)
{
  Pointer<Collection<NumericalScalar> > ptr = buildCollectionFromPySequence<NumericalScalar>(pyObj);
  return Collection<NumericalScalar>( *ptr );
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
  // use the same conversion function for numpy array/matrix, knowing numpy matrix is not a sequence
  if ( PyObject_HasAttrString(pyObj, const_cast<char *>("shape")) )
  {
    ScopedPyObjectPointer shapeObj(PyObject_GetAttrString( pyObj, "shape" ));
    if ( shapeObj.get() )
    {
      Indices shape( checkAndConvert< _PySequence_, Indices >( shapeObj.get() ) );
      if ( shape.getSize() == 2 )
      {
        UnsignedInteger nbRows = shape[0];
        UnsignedInteger nbColumns = shape[1];
        ScopedPyObjectPointer askObj(PyTuple_New(2));
        ScopedPyObjectPointer methodObj(convert< String, _PyString_ >("__getitem__"));
        p_implementation = new MatrixImplementation( nbRows, nbColumns );
        for ( UnsignedInteger i = 0; i < nbRows; ++ i )
        {
          PyTuple_SetItem( askObj.get(), 0, convert< UnsignedInteger, _PyInt_ >(i) );
          for ( UnsignedInteger j = 0; j < nbColumns; ++ j )
          {
            PyTuple_SetItem( askObj.get(), 1, convert< UnsignedInteger, _PyInt_ >(j) );
            ScopedPyObjectPointer elt(PyObject_CallMethodObjArgs( pyObj, methodObj.get(), askObj.get(), NULL));
            if (elt.get())
            {
              try
              {
                p_implementation->operator()( i, j ) = checkAndConvert<_PyFloat_, NumericalScalar>(elt.get());
              }
              catch( InvalidArgumentException & ex )
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
  else if ( PyObject_HasAttrString(pyObj, const_cast<char *>("getNbColumns")) )
  {
    // case of conversion from XMatrix to YMatrix
    // X could be Square,Triangular,Identity...
    // YMatrix might be Matrix of one of its inheritance types
    ScopedPyObjectPointer colunmsObj(PyObject_CallMethod ( pyObj,
                                     const_cast<char *>( "getNbColumns" ),
                                     const_cast<char *>( "()" ) ));
    ScopedPyObjectPointer rowsObj(PyObject_CallMethod ( pyObj,
                                  const_cast<char *>( "getNbRows" ),
                                  const_cast<char *>( "()" ) ));
    ScopedPyObjectPointer implObj(PyObject_CallMethod ( pyObj,
                                  const_cast<char *>( "getImplementation" ),
                                  const_cast<char *>( "()" ) ));
    Pointer< Collection< NumericalScalar > > ptr = buildCollectionFromPySequence< NumericalScalar >( implObj.get() );
    UnsignedInteger nbColumns( checkAndConvert< _PyInt_, UnsignedInteger >( colunmsObj.get() ) );
    UnsignedInteger nbRows( checkAndConvert< _PyInt_, UnsignedInteger >( rowsObj.get() ) );
    p_implementation = new MatrixImplementation( nbRows, nbColumns, *ptr );
  }
  else
  {
    // try to convert from a sequence of sequences
    Pointer< Collection< NumericalPoint > > ptr = buildCollectionFromPySequence< NumericalPoint >(pyObj);
    NumericalSample sample( *ptr );
    UnsignedInteger nbRows = sample.getSize();
    UnsignedInteger nbColumns = sample.getDimension();
    p_implementation = new MatrixImplementation( nbRows, nbColumns );
    for ( UnsignedInteger i = 0; i < nbRows; ++ i )
      for ( UnsignedInteger j = 0; j < nbColumns; ++ j )
        p_implementation->operator()( i, j ) = sample[i][j];
  }
  return p_implementation;
}



template <>
inline
Matrix
convert< _PySequence_, Matrix >(PyObject * pyObj)
{
  MatrixImplementation *p_implementation = convert< _PySequence_, MatrixImplementation* >(pyObj);
  return Matrix( p_implementation );
}



template <>
inline
SquareMatrix
convert< _PySequence_, SquareMatrix >(PyObject * pyObj)
{
  MatrixImplementation *p_implementation = convert< _PySequence_, MatrixImplementation* >(pyObj);
  if ( p_implementation->getNbRows() != p_implementation->getNbColumns() )
    throw InvalidArgumentException(HERE) << "The matrix is not square";
  return SquareMatrix( p_implementation );
}



template <>
inline
TriangularMatrix
convert< _PySequence_, TriangularMatrix >(PyObject * pyObj)
{
  MatrixImplementation *p_implementation = convert< _PySequence_, MatrixImplementation* >(pyObj);
  if (!(p_implementation->isTriangular(true) || p_implementation->isTriangular(false)))
    throw InvalidArgumentException(HERE) << "The matrix is not triangular";
  return TriangularMatrix( p_implementation, p_implementation->isTriangular(true) );
}



template <>
inline
SymmetricMatrix
convert< _PySequence_, SymmetricMatrix >(PyObject * pyObj)
{
  MatrixImplementation *p_implementation = convert< _PySequence_, MatrixImplementation* >(pyObj);
  if (!p_implementation->isSymmetric())
    throw InvalidArgumentException(HERE) << "The matrix is not symmetric";
  return SymmetricMatrix( p_implementation );
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
  return CovarianceMatrix( p_implementation );
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
  return CorrelationMatrix( p_implementation );
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
  Pointer< Collection< NumericalSample > > ptr = buildCollectionFromPySequence< NumericalSample >(pyObj);
  UnsignedInteger nbRows = ptr->getSize();
  UnsignedInteger nbColumns = ptr->getSize() > 0 ? (*ptr)[0].getSize() : 0;
  UnsignedInteger nbSheets = ptr->getSize() > 0 ? (*ptr)[0].getDimension() : 0;
  TensorImplementation *p_implementation = new TensorImplementation( nbRows, nbColumns, nbSheets );
  for ( UnsignedInteger i = 0; i < nbRows; ++ i )
    for ( UnsignedInteger j = 0; j < nbColumns; ++ j )
      for ( UnsignedInteger k = 0; k < nbSheets; ++ k )
        p_implementation->operator()( i, j, k ) = (*ptr)[i][j][k];
  return p_implementation;
}



template <>
inline
Tensor
convert< _PySequence_, Tensor >(PyObject * pyObj)
{
  TensorImplementation *p_implementation = convert<_PySequence_, TensorImplementation*>(pyObj);
  return Tensor( p_implementation );
}



template <>
inline
SymmetricTensor
convert< _PySequence_, SymmetricTensor >(PyObject * pyObj)
{
  TensorImplementation *p_implementation = convert< _PySequence_, TensorImplementation* >(pyObj);
  if (!p_implementation->isSymmetric())
    throw InvalidArgumentException(HERE) << "The tensor is not symmetric";
  return SymmetricTensor( p_implementation );
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
  // use the same conversion function for numpy array/matrix, knowing numpy matrix is not a sequence
  if ( PyObject_HasAttrString(pyObj, const_cast<char *>("shape")) )
  {
    ScopedPyObjectPointer shapeObj(PyObject_GetAttrString( pyObj, "shape" ));
    if (shapeObj.get())
    {
      Indices shape( checkAndConvert< _PySequence_, Indices >( shapeObj.get() ) );
      if ( shape.getSize() == 2 )
      {
        UnsignedInteger nbRows = shape[0];
        UnsignedInteger nbColumns = shape[1];
        ScopedPyObjectPointer askObj(PyTuple_New(2));
        ScopedPyObjectPointer methodObj(convert< String, _PyString_ >("__getitem__"));
        ComplexMatrixImplementation *p_implementation = new ComplexMatrixImplementation( nbRows, nbColumns );
        for ( UnsignedInteger i = 0; i < nbRows; ++ i )
        {
          PyTuple_SetItem( askObj.get(), 0, convert< UnsignedInteger, _PyInt_ >(i) );
          for ( UnsignedInteger j = 0; j < nbColumns; ++ j )
          {
            PyTuple_SetItem( askObj.get(), 1, convert< UnsignedInteger, _PyInt_ >(j) );
            ScopedPyObjectPointer elt(PyObject_CallMethodObjArgs( pyObj, methodObj.get(), askObj.get(), NULL));
            if (elt.get())
            {
              try
              {
                p_implementation->operator()( i, j ) = checkAndConvert<_PyComplex_, NumericalComplex>(elt.get());
              }
              catch( InvalidArgumentException & ex )
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
  if ( PyObject_HasAttrString(pyObj, const_cast<char *>("getNbColumns")) )
  {
    ScopedPyObjectPointer colunmsObj(PyObject_CallMethod ( pyObj,
                                     const_cast<char *>( "getNbColumns" ),
                                     const_cast<char *>( "()" ) ));
    ScopedPyObjectPointer rowsObj(PyObject_CallMethod ( pyObj,
                                  const_cast<char *>( "getNbRows" ),
                                  const_cast<char *>( "()" ) ));
    ScopedPyObjectPointer implObj(PyObject_CallMethod ( pyObj,
                                  const_cast<char *>( "getImplementation" ),
                                  const_cast<char *>( "()" ) ));
    Pointer< Collection< NumericalComplex > > ptr = buildCollectionFromPySequence< NumericalComplex >( implObj.get() );
    UnsignedInteger nbColumns( checkAndConvert< _PyInt_, UnsignedInteger >( colunmsObj.get() ) );
    UnsignedInteger nbRows( checkAndConvert< _PyInt_, UnsignedInteger >( rowsObj.get() ) );
    ComplexMatrixImplementation *p_implementation = new ComplexMatrixImplementation( nbRows, nbColumns, *ptr );
    return p_implementation;
  }

  // else try to convert from a sequence of sequences
  Pointer< Collection< Collection< NumericalComplex > > > ptr = buildCollectionFromPySequence< Collection< NumericalComplex > >(pyObj);
  UnsignedInteger nbRows = ptr->getSize();
  UnsignedInteger nbColumns = ptr->getSize() > 0 ? (*ptr)[0].getSize() : 0;
  ComplexMatrixImplementation *p_implementation = new ComplexMatrixImplementation( nbRows, nbColumns );
  for ( UnsignedInteger i = 0; i < nbRows; ++ i )
    for ( UnsignedInteger j = 0; j < nbColumns; ++ j )
      p_implementation->operator()( i, j ) = (*ptr)[i][j];
  return p_implementation;
}



template <>
inline
ComplexMatrix
convert< _PySequence_, ComplexMatrix >(PyObject * pyObj)
{
  ComplexMatrixImplementation *p_implementation = convert< _PySequence_, ComplexMatrixImplementation* >(pyObj);
  return ComplexMatrix( p_implementation );
}



template <>
inline
SquareComplexMatrix
convert< _PySequence_, SquareComplexMatrix >(PyObject * pyObj)
{
  ComplexMatrixImplementation *p_implementation = convert< _PySequence_, ComplexMatrixImplementation* >(pyObj);
  if ( p_implementation->getNbRows() != p_implementation->getNbColumns() )
    throw InvalidArgumentException(HERE) << "The matrix is not square";
  return SquareComplexMatrix( p_implementation );
}



template <>
inline
TriangularComplexMatrix
convert< _PySequence_, TriangularComplexMatrix >(PyObject * pyObj)
{
  ComplexMatrixImplementation *p_implementation = convert< _PySequence_, ComplexMatrixImplementation* >(pyObj);
  Bool lower = p_implementation->isTriangular(true);
  if ( !lower && !p_implementation->isTriangular(false) )
    throw InvalidArgumentException(HERE) << "The matrix is not triangular";
  return TriangularComplexMatrix( p_implementation, lower );
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
  if ( !p_implementation->isHermitian() )
    throw InvalidArgumentException(HERE) << "The matrix is not hermitian";
  return HermitianMatrix( p_implementation );
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

  // use the same conversion function for numpy array/matrix, knowing numpy matrix is not a sequence
  if ( PyObject_HasAttrString(pyObj, const_cast<char *>("shape")) )
  {
    ScopedPyObjectPointer shapeObj(PyObject_GetAttrString( pyObj, "shape" ));
    if (shapeObj.get())
    {
      Indices shape( checkAndConvert< _PySequence_, Indices >( shapeObj.get() ) );
      if ( shape.getSize() == 3 )
      {
        UnsignedInteger nbRows = shape[0];
        UnsignedInteger nbColumns = shape[1];
        UnsignedInteger nbSheets = shape[2];
        ScopedPyObjectPointer askObj(PyTuple_New(3));
        ScopedPyObjectPointer methodObj(convert< String, _PyString_ >("__getitem__"));
        p_implementation = new ComplexTensorImplementation( nbRows, nbColumns, nbSheets );
        for ( UnsignedInteger i = 0; i < nbRows; ++ i )
        {
          PyTuple_SetItem( askObj.get(), 0, convert< UnsignedInteger, _PyInt_ >(i) );
          for ( UnsignedInteger j = 0; j < nbColumns; ++ j )
          {
            PyTuple_SetItem( askObj.get(), 1, convert< UnsignedInteger, _PyInt_ >(j) );
            for ( UnsignedInteger k = 0; k < nbSheets; ++ k )
            {
              PyTuple_SetItem( askObj.get(), 2, convert< UnsignedInteger, _PyInt_ >(k) );
              ScopedPyObjectPointer elt(PyObject_CallMethodObjArgs( pyObj, methodObj.get(), askObj.get(), NULL));
              if (elt.get())
              {
                try
                {
                  p_implementation->operator()( i, j, k ) = checkAndConvert<_PyComplex_, NumericalComplex>(elt.get());
                }
                catch( InvalidArgumentException & ex )
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
  else if ( PyObject_HasAttrString(pyObj, const_cast<char *>("getNbSheets")) )
  {
    // case of conversion from XTensor to YTensor
    ScopedPyObjectPointer colunmsObj(PyObject_CallMethod ( pyObj,
                                     const_cast<char *>( "getNbColumns" ),
                                     const_cast<char *>( "()" ) ));
    ScopedPyObjectPointer rowsObj(PyObject_CallMethod ( pyObj,
                                  const_cast<char *>( "getNbRows" ),
                                  const_cast<char *>( "()" ) ));
    ScopedPyObjectPointer sheetsObj(PyObject_CallMethod ( pyObj,
                                    const_cast<char *>( "getNbSheets" ),
                                    const_cast<char *>( "()" ) ));
    ScopedPyObjectPointer implObj(PyObject_CallMethod ( pyObj,
                                  const_cast<char *>( "getImplementation" ),
                                  const_cast<char *>( "()" ) ));
    Pointer< Collection< NumericalComplex > > ptr = buildCollectionFromPySequence< NumericalComplex >( implObj.get() );
    UnsignedInteger nbColumns( checkAndConvert< _PyInt_, UnsignedInteger >( colunmsObj.get() ) );
    UnsignedInteger nbRows( checkAndConvert< _PyInt_, UnsignedInteger >( rowsObj.get() ) );
    UnsignedInteger nbSheets( checkAndConvert< _PyInt_, UnsignedInteger >( sheetsObj.get() ) );
    p_implementation = new ComplexTensorImplementation( nbRows, nbColumns, nbSheets, *ptr );
  }
  return p_implementation;
}



template <>
inline
ComplexTensor
convert< _PySequence_, ComplexTensor >(PyObject * pyObj)
{
  ComplexTensorImplementation *p_implementation = convert<_PySequence_, ComplexTensorImplementation*>(pyObj);
  return ComplexTensor( p_implementation );
}

template <>
struct traitsPythonType< WhittleFactoryState >
{
  typedef _PySequence_ Type;
};

template <>
inline
WhittleFactoryState
convert< _PySequence_, WhittleFactoryState >(PyObject * pyObj)
{
  return WhittleFactoryState();
}

template <>
struct traitsPythonType< Process >
{
  typedef _PySequence_ Type;
};

template <>
inline
Process
convert< _PySequence_, Process >(PyObject * pyObj)
{
  return Process();
}


inline
void handleException()
{
  PyObject * exception = PyErr_Occurred();

  if ( exception )
  {
    PyObject *type = NULL, *value = NULL, *traceback = NULL;
    PyErr_Fetch( &type, &value, &traceback );

    String exceptionMessage("Python exception");

    // get the name of the exception
    if ( type )
    {
      ScopedPyObjectPointer nameObj(PyObject_GetAttrString( type, "__name__" ));
      if ( nameObj.get() )
      {
        String typeString = checkAndConvert< _PyString_, String >(nameObj.get());
        exceptionMessage += ": " + typeString;
      }
    }

    // try to get error msg, value and traceback can be NULL
    if(value)
    {
      try
      {
        String valueString = checkAndConvert< _PyString_, String >(value);
        exceptionMessage += ": " + valueString;
      }
      catch( InvalidArgumentException & ex )
      {
        // could not get msg from strings
      }
    }

    PyErr_Restore( type, value, traceback );
    PyErr_Print();
    throw InternalException(HERE) << exceptionMessage;
  }
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
void pickleSave(Advocate & adv, PyObject * pyObj)
{
  ScopedPyObjectPointer pickleModule(PyImport_ImportModule( "pickle" )); // new reference
  assert( pickleModule.get() );

  PyObject * pickleDict = PyModule_GetDict( pickleModule.get() );
  assert(pickleDict);

  PyObject * dumpsMethod = PyDict_GetItemString( pickleDict, "dumps" );
  assert( dumpsMethod );
  if ( ! PyCallable_Check( dumpsMethod ) )
    throw InternalException(HERE) << "Python 'pickle' module has no 'dumps' method";

  assert(pyObj);
  ScopedPyObjectPointer rawDump(PyObject_CallFunctionObjArgs(dumpsMethod, pyObj, NULL)); // new reference

  handleException();
  assert( rawDump.get() );

  ScopedPyObjectPointer base64Module(PyImport_ImportModule( "base64" )); // new reference
  assert( base64Module.get() );

  PyObject * base64Dict = PyModule_GetDict( base64Module.get() );
  assert( base64Dict );

  PyObject * b64encodeMethod = PyDict_GetItemString( base64Dict, "standard_b64encode" );
  assert( b64encodeMethod );
  if ( ! PyCallable_Check( b64encodeMethod ) )
    throw InternalException(HERE) << "Python 'base64' module has no 'standard_b64encode' method";

  ScopedPyObjectPointer base64Dump(PyObject_CallFunctionObjArgs(b64encodeMethod, rawDump.get(), NULL)); // new reference
  handleException();
  assert( base64Dump.get() );

  String pyInstanceSt(convert< _PyBytes_, String >( base64Dump.get() ));
  adv.saveAttribute( "pyInstance_", pyInstanceSt );
}


inline
void pickleLoad(Advocate & adv, PyObject * & pyObj)
{
  String pyInstanceSt;
  adv.loadAttribute( "pyInstance_", pyInstanceSt );

  ScopedPyObjectPointer base64Dump(convert< String, _PyBytes_ >( pyInstanceSt )); // new reference
  assert( base64Dump.get() );

  ScopedPyObjectPointer base64Module(PyImport_ImportModule( "base64" )); // new reference
  assert( base64Module.get() );

  PyObject * base64Dict = PyModule_GetDict( base64Module.get() );
  assert( base64Dict );

  PyObject * b64decodeMethod = PyDict_GetItemString( base64Dict, "standard_b64decode" );
  assert( b64decodeMethod );
  if ( ! PyCallable_Check( b64decodeMethod ) )
    throw InternalException(HERE) << "Python 'base64' module has no 'standard_b64decode' method";

  ScopedPyObjectPointer rawDump(PyObject_CallFunctionObjArgs(b64decodeMethod, base64Dump.get(), NULL)); // new reference
  handleException();
  assert( rawDump.get() );

  ScopedPyObjectPointer pickleModule(PyImport_ImportModule( "pickle" )); // new reference
  assert( pickleModule.get() );

  PyObject * pickleDict = PyModule_GetDict( pickleModule.get() );
  assert( pickleDict );

  PyObject * loadsMethod = PyDict_GetItemString( pickleDict, "loads" );
  assert( loadsMethod );
  if ( ! PyCallable_Check( loadsMethod ) )
    throw InternalException(HERE) << "Python 'pickle' module has no 'loads' method";

  Py_XDECREF(pyObj);
  pyObj = PyObject_CallFunctionObjArgs(loadsMethod, rawDump.get(), NULL); // new reference
  handleException();
  assert(pyObj);
}


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PYTHONWRAPPINGFUNCTIONS_HXX */
