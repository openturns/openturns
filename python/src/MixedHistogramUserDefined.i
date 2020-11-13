// SWIG file MixedHistogramUserDefined.i

%{
#include "openturns/MixedHistogramUserDefined.hxx"
%}

%include MixedHistogramUserDefined_doc.i

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const OT::DistributionImplementation::PointCollection & {
  $1 = ($input != Py_None) && (SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, 0)) || OT::isAPythonSequenceOf<OT::_PySequence_>($input));
}

%typemap(in) const OT::DistributionImplementation::PointCollection & {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0))) {
    try {
      $1 = OT::buildCollectionFromPySequence< OT::Point >($input);
    } catch (OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Point");
    }
  }
}

%include openturns/MixedHistogramUserDefined.hxx
namespace OT { %extend MixedHistogramUserDefined { MixedHistogramUserDefined(const MixedHistogramUserDefined & other) { return new OT::MixedHistogramUserDefined(other); } } }
