// SWIG file PointToPointConnection.i

%{
#include "openturns/PointToPointConnection.hxx"
%}

%include PointToPointConnection_doc.i

%include openturns/PointToPointConnection.hxx
namespace OT { %extend PointToPointConnection { PointToPointConnection(const PointToPointConnection & other) { return new OT::PointToPointConnection(other); } } }
