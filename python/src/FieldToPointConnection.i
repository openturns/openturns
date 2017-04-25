// SWIG file FieldToPointConnection.i

%{
#include "openturns/FieldToPointConnection.hxx"
%}

%include FieldToPointConnection_doc.i

%include openturns/FieldToPointConnection.hxx
namespace OT { %extend FieldToPointConnection { FieldToPointConnection(const FieldToPointConnection & other) { return new OT::FieldToPointConnection(other); } } }
