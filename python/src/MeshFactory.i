// SWIG file CovarianceModelFactory.i

%{
#include "MeshFactory.hxx"
%}

OTTypedInterfaceObjectHelper(MeshFactory)

%include MeshFactory_doc.i

%include MeshFactory.hxx

namespace OT { 

  %extend MeshFactory {

     MeshFactory(const MeshFactory & other) { return new OT::MeshFactory(other); }

   }
 }
