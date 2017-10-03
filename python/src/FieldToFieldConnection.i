// SWIG file FieldToFieldConnection.i

%{
#include "openturns/FieldToFieldConnection.hxx"
%}

%include FieldToFieldConnection_doc.i

%include openturns/FieldToFieldConnection.hxx
namespace OT { %extend FieldToFieldConnection { FieldToFieldConnection(const FieldToFieldConnection & other) { return new OT::FieldToFieldConnection(other); } } }
