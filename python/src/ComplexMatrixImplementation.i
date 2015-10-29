// SWIG file ComplexMatrixImplementation.i

%{
#include "ComplexMatrixImplementation.hxx"
%}

%include ComplexMatrixImplementation_doc.i

%rename(ComplexMatrixImplementation_operator___eq__) OT::operator ==(const ComplexMatrixImplementation & lhs, const ComplexMatrixImplementation & rhs);

%template(NumericalComplexCollection)                   OT::Collection<OT::NumericalComplex>;
%template(NumericalComplexPersistenCollection)          OT::PersistentCollection<OT::NumericalComplex>;

%typemap(in) const NumericalComplexCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    try {
      temp = OT::convert<OT::_PySequence_,OT::Collection<OT::NumericalComplex> >( $input );
      $1 = &temp;
    } catch (OT::InvalidArgumentException & ex) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of NumericalComplex");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const NumericalComplexCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) ||
       OT::isAPythonSequenceOf<OT::_PyComplex_>( $input );
}

%include ComplexMatrixImplementation.hxx
namespace OT{ %extend ComplexMatrixImplementation { ComplexMatrixImplementation(const ComplexMatrixImplementation & other) { return new OT::ComplexMatrixImplementation(other); } } }
