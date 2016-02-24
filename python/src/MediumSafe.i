// SWIG file MediumSafe.i

%{
#include "openturns/MediumSafe.hxx"
%}

%include MediumSafe_doc.i

%include openturns/MediumSafe.hxx
namespace OT{ %extend MediumSafe { MediumSafe(const MediumSafe & other) { return new OT::MediumSafe(other); } } }
