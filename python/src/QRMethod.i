// SWIG file QRMethod.i

%{
#include "openturns/QRMethod.hxx"
%}

%include QRMethod_doc.i

%include openturns/QRMethod.hxx
namespace OT { %extend QRMethod { QRMethod(const QRMethod & other) { return new OT::QRMethod(other); } } }
