// SWIG file GridLayout.i

%{
#include "openturns/GridLayout.hxx"
%}

%include GridLayout_doc.i

%include openturns/GridLayout.hxx

%copyctor OT::GridLayout;

%pythoncode %{
def GridLayout__repr_png_(self):
    """Get the PNG representation."""
    if openturns.common.ResourceMap.Get('View-ImageFormat') != 'png':
        raise NotImplementedError
    from .viewer import _ToImageString
    return _ToImageString(self)
GridLayout._repr_png_ = Drawable__repr_png_

def GridLayout__repr_svg_(self):
    """Get the SVG representation."""
    if openturns.common.ResourceMap.Get('View-ImageFormat') != 'svg':
        raise NotImplementedError
    from .viewer import _ToImageString
    return _ToImageString(self)
GridLayout._repr_svg_ = Drawable__repr_svg_

def GridLayout__repr_html_(self):
    """Get the HTML representation."""
    raise NotImplementedError
GridLayout._repr_html_ = Drawable__repr_html_
%}
