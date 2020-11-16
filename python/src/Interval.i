// SWIG file Interval.i

%{
#include "openturns/PythonWrappingFunctions.hxx"
#include "openturns/Interval.hxx"
%}

%include Interval_doc.i

// we check for boolean type but they are stored as integers
%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const BoolCollection & {
  $1 = ($input != Py_None) && (SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) || OT::isAPythonSequenceOf<OT::_PyBool_>($input));
}

%typemap(in) const BoolCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    try {
      temp = OT::convert<OT::_PySequence_,OT::Collection<OT::UnsignedInteger> >( $input );
      $1 = &temp;
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Bool");
    }
  }
}

%apply const BoolCollection & { OT::Interval::BoolCollection & };

%rename(__eq__) OT::Interval::operator==(const Interval & rhs) const;

%include openturns/Interval.hxx

namespace OT { %extend Interval { Interval(const Interval & other) { return new OT::Interval(other); } } }
