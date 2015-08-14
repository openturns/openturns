// SWIG file MediumSafe.i

%{
#include "MediumSafe.hxx"
%}

%include MediumSafe_doc.i

%include MediumSafe.hxx
namespace OT{ %extend MediumSafe { MediumSafe(const MediumSafe & other) { return new OT::MediumSafe(other); } } }
