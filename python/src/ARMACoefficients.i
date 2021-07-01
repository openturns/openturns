// SWIG file ARMACoefficients.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/ARMACoefficients.hxx"
#include "openturns/SquareMatrix.hxx"


namespace OT { 

  template <>
  struct traitsPythonType<OT::SquareMatrix>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  OT::SquareMatrix
  convert<_PyObject_, OT::SquareMatrix>(PyObject * pyObj)
  {
    void * ptr = 0;
    if (! SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::SquareMatrix *"), SWIG_POINTER_NO_NULL)))
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not a SquareMatrix";
    OT::SquareMatrix * p_sm = reinterpret_cast< OT::SquareMatrix * >( ptr );
    return *p_sm;
  }
}
%}

%include ARMACoefficients_doc.i

%template(SquareMatrixCollection) OT::Collection<OT::SquareMatrix>;
%template(SquareMatrixPersistentCollection) OT::PersistentCollection<OT::SquareMatrix>;

%include openturns/ARMACoefficients.hxx


%extend OT::Collection<OT::SquareMatrix>
{

OT::Collection<OT::SquareMatrix> (PyObject * pyObj)
{
  return OT::buildCollectionFromPySequence<OT::SquareMatrix>( pyObj );
}

}

namespace OT {  
%extend ARMACoefficients {

ARMACoefficients(PyObject * pyObj)
{
  OT::Pointer<OT::Collection<OT::SquareMatrix> > p_coll =  OT::buildCollectionFromPySequence<OT::SquareMatrix>( pyObj );
  return new OT::ARMACoefficients( *p_coll );
}

ARMACoefficients(const ARMACoefficients & other)
{
  return new OT::ARMACoefficients(other);
}

}

}

