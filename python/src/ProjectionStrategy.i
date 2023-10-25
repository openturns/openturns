// SWIG file ProjectionStrategy.i

%{
#include "openturns/ProjectionStrategy.hxx"
%}

%include ProjectionStrategy_doc.i

OTTypedInterfaceObjectHelper(ProjectionStrategy)

%include openturns/ProjectionStrategy.hxx
namespace OT{ %extend ProjectionStrategy { ProjectionStrategy(const ProjectionStrategy & other) { return new OT::ProjectionStrategy(other); } } }

%pythoncode %{
def __ProjectionStrategy_repr_html(self):
    """Get HTML representation."""
    html = ""
    html += f"{self.getClassName()}\n"
    html += "<ul>\n"
    html += f"  <li>implementation: {self.getImplementation().getClassName()}</li>\n"
    html += f"  <li>coefficients: dimension= {self.getCoefficients().getDimension()}</li>\n"
    html += f"  <li>residual: {self.getResidual()}</li>\n"
    html += (
        f"  <li>relative error: {self.getRelativeError()}</li>\n"
    )
    html += f"  <li>measure: {self.getMeasure().getImplementation().getClassName()}</li>\n"
    html += f"  <li>experiment: {self.getExperiment().getClassName()}</li>\n"
    html += f"  <li>input sample: size= {self.getInputSample().getSize()} " \
        f"x dimension= {self.getInputSample().getDimension()}</li>\n"
    html += f"  <li>output sample: size= {self.getOutputSample().getSize()} " \
        f"x dimension= {self.getOutputSample().getDimension()}</li>\n"
    html += f"  <li>weights: dimension= {self.getWeights().getDimension()}</li>\n"
    html += f"  <li>design: size= {self.getDesignProxy().getSampleSize()}</li>\n"
    html += "</ul>\n"
    return html

ProjectionStrategy._repr_html_ = __ProjectionStrategy_repr_html
%}
