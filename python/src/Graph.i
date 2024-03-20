// SWIG file Graph.i

%{
#include "openturns/Graph.hxx"

namespace OT {

  template <>
  struct traitsPythonType<OT::Graph>
  {
    typedef _PyObject_ Type;
  };

  template <>
  inline
  OT::Graph
  convert<_PyObject_, OT::Graph>(PyObject * pyObj)
  {
    void * ptr = 0;
    if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__Graph, SWIG_POINTER_NO_NULL))) {
      OT::Graph * p_graph = reinterpret_cast< OT::Graph * >(ptr);
      return *p_graph;
    } else if (SWIG_IsOK(SWIG_ConvertPtr(pyObj, &ptr, SWIGTYPE_p_OT__GraphImplementation, SWIG_POINTER_NO_NULL))) {
      OT::GraphImplementation * p_graph = reinterpret_cast< OT::GraphImplementation * >(ptr);
      return *p_graph;
    } else {
      throw OT::InvalidArgumentException(HERE) << "Object passed as argument is neither a Graph nor an object convertible to a Graph";
    }
    return OT::Graph();
  }
}

%}

%include Graph_doc.i

OTTypedInterfaceObjectHelper(Graph)

%template(_GraphCollection) OT::Collection<OT::Graph>;

%include openturns/Graph.hxx

%pythoncode %{
def Graph__repr_png_(self):
    """Get the PNG representation."""
    if openturns.common.ResourceMap.Get('View-ImageFormat') != 'png':
        raise NotImplementedError
    from .viewer import _ToImageString
    return _ToImageString(self)
Graph._repr_png_ = Graph__repr_png_

def Graph__repr_svg_(self):
    """Get the SVG representation."""
    if openturns.common.ResourceMap.Get('View-ImageFormat') != 'svg':
        raise NotImplementedError
    from .viewer import _ToImageString
    return _ToImageString(self)
Graph._repr_svg_ = Graph__repr_svg_

def Graph__repr_html_(self):
    """Get the HTML representation."""
    raise NotImplementedError
Graph._repr_html_ = Graph__repr_html_
%}

namespace OT{  

%extend Graph {

Graph(const Graph & other) { return new OT::Graph(other); }

Graph(PyObject * pyObj)
{
  return new OT::Graph(OT::convert<OT::_PyObject_, OT::Graph>(pyObj));
}

}

}
