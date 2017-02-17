// SWIG file DrawableImplementation.i

%{
#include "openturns/DrawableImplementation.hxx"
%}

%include DrawableImplementation_doc.i

%include openturns/DrawableImplementation.hxx

%pythoncode %{
def DrawableImplementation__repr_svg_(self):
    """Get the SVG representation."""
    from .viewer import ToSVGString
    return ToSVGString(self)

DrawableImplementation._repr_svg_ = DrawableImplementation__repr_svg_


def DrawableImplementation__repr_html_(self):
    """Get the HTML representation."""
    raise NotImplementedError

DrawableImplementation._repr_html_ = DrawableImplementation__repr_html_
%}

namespace OT { %extend DrawableImplementation { DrawableImplementation(const DrawableImplementation & other) { return new OT::DrawableImplementation(other); } } }
