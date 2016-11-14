// SWIG file

%{
#include "openturns/LinearProfile.hxx"
%}

%include LinearProfile_doc.i

%include openturns/LinearProfile.hxx
namespace OT { %extend LinearProfile { LinearProfile(const LinearProfile & other) { return new OT::LinearProfile(other); } } }
