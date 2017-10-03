// SWIG file PointToFieldConnection.i

%{
#include "openturns/PointToFieldConnection.hxx"
%}

%include PointToFieldConnection_doc.i

%include openturns/PointToFieldConnection.hxx
namespace OT { %extend PointToFieldConnection { PointToFieldConnection(const PointToFieldConnection & other) { return new OT::PointToFieldConnection(other); } } }
