// SWIG file CovarianceModelImplementation.i

%{
#include "openturns/CovarianceModelImplementation.hxx"
%}

%include CovarianceModelImplementation_doc.i

%copyctor OT::CovarianceModelImplementation;

%include openturns/CovarianceModelImplementation.hxx

namespace OT {

%extend CovarianceModelImplementation {

CovarianceModel __rmul__(const Scalar scalar)
{
  return *self * scalar;
}

} // %extend CovarianceModelImplementation

} // namespace OT
