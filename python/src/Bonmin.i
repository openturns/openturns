// SWIG file Bonmin.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/Bonmin.hxx"

namespace OT {
template <>
struct traitsPythonType< OT::OptimizationProblemImplementation::VariableType >
{
  typedef _PyInt_ Type;
};

template <>
  inline
  OT::OptimizationProblemImplementation::VariableType
  convert< _PyInt_, OT::OptimizationProblemImplementation::VariableType >(PyObject * pyObj)
  {
    return (OT::OptimizationProblemImplementation::VariableType) convert< _PyInt_, UnsignedInteger >(pyObj);
  }


}
%}

%template(VariableTypeCollection) OT::Collection<OT::OptimizationProblemImplementation::VariableType>;

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const VariableTypeCollection & {
  $1 = ($input != Py_None) && (SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) || OT::isAPythonSequenceOf<OT::_PyInt_>($input));
}

%typemap(in) const VariableTypeCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    try {
      temp = OT::convert<OT::_PySequence_,OT::Collection<OT::UnsignedInteger> >( $input );
      $1 = &temp;
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of VariableType");
    }
  }
}

%apply const VariableTypeCollection & {const OT::OptimizationProblem::VariableTypeCollection};

%include Bonmin_doc.i

%include openturns/Bonmin.hxx

namespace OT { %extend Bonmin { Bonmin(const Bonmin & other) { return new OT::Bonmin(other); } } }
