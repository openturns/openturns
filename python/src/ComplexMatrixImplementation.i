// SWIG file ComplexMatrixImplementation.i

%{
#include "openturns/ComplexMatrixImplementation.hxx"
%}

%include ComplexMatrixImplementation_doc.i

%rename(ComplexMatrixImplementation_operator___eq__) OT::operator ==(const ComplexMatrixImplementation & lhs, const ComplexMatrixImplementation & rhs);

%template(ComplexCollection)                   OT::Collection<OT::Complex>;
%template(ComplexPersistenCollection)          OT::PersistentCollection<OT::Complex>;

%typemap(in) const ComplexCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    try {
      temp = OT::convert<OT::_PySequence_,OT::Collection<OT::Complex> >( $input );
      $1 = &temp;
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Complex");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const ComplexCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL)) ||
       OT::isAPythonBufferOf<OT::Complex, 2>( $input ) || OT::isAPythonSequenceOf<OT::_PyComplex_>($input);
}

%include openturns/ComplexMatrixImplementation.hxx
namespace OT{ %extend ComplexMatrixImplementation { ComplexMatrixImplementation(const ComplexMatrixImplementation & other) { return new OT::ComplexMatrixImplementation(other); } } }

