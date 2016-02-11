// SWIG file MeshFactoryImplementation.i

%{
#include "openturns/MeshFactoryImplementation.hxx"
%}

%include MeshFactoryImplementation_doc.i

%include openturns/MeshFactoryImplementation.hxx
namespace OT {%extend MeshFactoryImplementation {MeshFactoryImplementation(const MeshFactoryImplementation & other){return new OT::MeshFactoryImplementation(other);}}}
