// SWIG file DrawableImplementation.i

%{
#include "openturns/DrawableImplementation.hxx"
%}

%include DrawableImplementation_doc.i

%include openturns/DrawableImplementation.hxx

%copyctor OT::DrawableImplementation;

%pythoncode %{
def _DrawableImplementation__repr_png_(self):
    """Get the PNG representation."""
    if openturns.common.ResourceMap.Get('View-ImageFormat') != 'png':
        raise NotImplementedError
    from .viewer import _ToImageString
    return _ToImageString(self)
DrawableImplementation._repr_png_ = _DrawableImplementation__repr_png_

def _DrawableImplementation__repr_svg_(self):
    """Get the SVG representation."""
    if openturns.common.ResourceMap.Get('View-ImageFormat') != 'svg':
        raise NotImplementedError
    from .viewer import _ToImageString
    return _ToImageString(self)
DrawableImplementation._repr_svg_ = _DrawableImplementation__repr_svg_

def _DrawableImplementation__repr_html_(self):
    """Get the HTML representation."""
    raise NotImplementedError
DrawableImplementation._repr_html_ = _DrawableImplementation__repr_html_
%}

