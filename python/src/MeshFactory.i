// SWIG file CovarianceModelFactory.i

%{
#include "openturns/MeshFactory.hxx"
%}

OTTypedInterfaceObjectHelper(MeshFactory)

%include MeshFactory_doc.i

%include openturns/MeshFactory.hxx

namespace OT { 

  %extend MeshFactory {

     MeshFactory(const MeshFactory & other) { return new OT::MeshFactory(other); }

   }
 }
