// SWIG file CovarianceMatrix.i

%{
#include "CovarianceMatrix.hxx"

namespace OT { 

  template <>
  struct traitsPythonType<OT::CovarianceMatrix>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  OT::CovarianceMatrix
  convert<_PyObject_,OT::CovarianceMatrix>(PyObject * pyObj)
  {
    void * ptr = 0;
    if (! SWIG_IsOK(SWIG_ConvertPtr( pyObj, &ptr, SWIG_TypeQuery("OT::CovarianceMatrix *"), 0 |  0 )))
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is not a CovarianceMatrix";
    OT::CovarianceMatrix * p_sm = reinterpret_cast< OT::CovarianceMatrix * >( ptr );
    return *p_sm;
  }
}

%}

%include CovarianceMatrix_doc.i

%apply const NumericalScalarCollection & { const OT::CovarianceMatrix::NumericalScalarCollection & };
%template(CovarianceMatrixCollection) OT::Collection<OT::CovarianceMatrix>;
%template(CovarianceMatrixPersistentCollection) OT::PersistentCollection<OT::CovarianceMatrix>;

%typemap(in) const CovarianceMatrix & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    temp = OT::convert<OT::_PySequence_,OT::CovarianceMatrix>( $input );
    $1 = &temp;
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const CovarianceMatrix & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) ||
       OT::isAPythonSequenceOf<OT::_PySequence_>( $input );
}

%include CovarianceMatrix.hxx

namespace OT {  

%extend CovarianceMatrix {

  CovarianceMatrix(const CovarianceMatrix & other) { return new OT::CovarianceMatrix(other); }
  
  CovarianceMatrix(PyObject * pyObj) { return new OT::CovarianceMatrix( OT::convert<OT::_PySequence_,OT::CovarianceMatrix>(pyObj) ); }
  
  OTMatrixAccessors()
  
} // CovarianceMatrix
} // OT
