// SWIG file Bonmin.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/Bonmin.hxx"

namespace OT {
template <>
struct traitsPythonType< OT::OptimizationProblemImplementation::VariableType >
{
  typedef _PyLong_ Type;
};

template <>
  inline
  OT::OptimizationProblemImplementation::VariableType
  convert< _PyLong_, OT::OptimizationProblemImplementation::VariableType >(PyObject * pyObj)
  {
    return (OT::OptimizationProblemImplementation::VariableType) convert< _PyLong_, UnsignedInteger >(pyObj);
  }


}
%}

%template(_VariableTypeCollection) OT::Collection<OT::OptimizationProblemImplementation::VariableType>;

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const VariableTypeCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL)) || OT::isAPythonSequenceOf<OT::_PyLong_>($input);
}

%typemap(in) const VariableTypeCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    try {
      temp = OT::convert<OT::_PySequence_, OT::Collection<OT::UnsignedInteger> >($input);
      $1 = &temp;
    } catch (const OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of VariableType");
    }
  }
}

%apply const VariableTypeCollection & {const OT::OptimizationProblem::VariableTypeCollection};

%include Bonmin_doc.i

%copyctor OT::Bonmin;

%include openturns/Bonmin.hxx
