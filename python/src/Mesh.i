// SWIG file Mesh.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & coordinates ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & coordinates %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIG_TypeQuery("OT::Point *"), SWIG_POINTER_OWN |  0 )); %}
OT::Bool OT::Mesh::checkPointInSimplexWithCoordinates(const OT::Point & point,
						      const OT::UnsignedInteger index,
						      OT::Point & coordinates) const;

OT::Indices OT::Mesh::getNearestVertexAndSimplexIndicesWithCoordinates(const OT::Point & point,
						      OT::Point & coordinates) const;

%{
#include "openturns/Mesh.hxx"
%}

%include Mesh_doc.i

%include openturns/Mesh.hxx

namespace OT { %extend Mesh { Mesh(const Mesh & other) { return new OT::Mesh(other); } } }
