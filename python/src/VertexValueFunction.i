// SWIG file VertexValueFunction.i

%{
#include "openturns/VertexValueFunction.hxx"
%}

%include VertexValueFunction_doc.i

%include openturns/VertexValueFunction.hxx
namespace OT { %extend VertexValueFunction { VertexValueFunction(const VertexValueFunction & other) { return new OT::VertexValueFunction(other); } } }

%pythoncode %{
# deprecated
class TemporalFunction(VertexValueFunction):
    def __init__(self, *args):
        super(TemporalFunction, self).__init__(*args)
        openturns.common.Log.Warn('class TemporalFunction is deprecated in favor of VertexValueFunction')
%}

