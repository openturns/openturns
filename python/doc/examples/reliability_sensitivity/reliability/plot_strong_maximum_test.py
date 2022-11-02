"""
Test the design point with the Strong Maximum Test
==================================================
"""
# %%
# In this example we are going to validate a FORM estimation using the Strong Maximum Test.
#
# The Strong Maximum Test helps to evaluate the quality of the design point
# resulting from the optimization algorithm. It checks whether the design point computed is:
#
# - the **true** design point, which means a global maximum point,
# - a **strong** design point, which means that there is no other local maximum
#   located on the event boundary and which likelihood is slightly inferior to the design point one.
#
# This verification is very important in order to give sense to the FORM and SORM approximations.
#
# We briefly recall here the main principles of the test.
# The objective is to detect all the points :math:`\tilde{P}^*` in the ball of radius
# :math:`R_{\varepsilon} = \beta(1+\delta_{\varepsilon})` which are potentially the
# real design point (case of :math:`\tilde{P}_2^*`) or which contribution to :math:`P_f` is
# not negligeable as regards the approximations Form and Sorm (case of :math:`\tilde{P}_1^*`).
# The contribution of a point is considered as negligeable when its likelihood in
# the :math:`U`-space is more than :math:`\varepsilon`-times lesser than the design point one.
# The radius :math:`R_{\varepsilon}` is the distance to the :math:`U`-space center upon which
# points are considered as negligeable in the evaluation of :math:`P_f`.
#

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)

# %%
# Create the model Y = x1 + 2*x2
model = ot.SymbolicFunction(["x1", "x2"], ["x1+2*x2"])

# Create the input distribution and random vector X
inputDist = ot.Normal(2)
inputDist.setDescription(["X1", "X2"])

inputVector = ot.RandomVector(inputDist)

# Create the output random vector Y=model(X)
output = ot.CompositeRandomVector(model, inputVector)
output.setName("MyOutputY")

# %%
# Create the physical event Y > 4
threshold = 4
myEvent = ot.ThresholdEvent(output, ot.Greater(), threshold)

# Create the associated standard event in the standard space
myStandardEvent = ot.StandardEvent(myEvent)

# %%
# First : FORM analyses to get the design point
myCobyla = ot.Cobyla()
myStartingPoint = inputDist.getMean()
myAlgoFORM = ot.FORM(myCobyla, myEvent, myStartingPoint)
myAlgoFORM.run()
FORMResult = myAlgoFORM.getResult()
standardSpaceDesignPoint = FORMResult.getStandardSpaceDesignPoint()

# %%
# Fix the importance level epsilon of the test
# Care : 0<epsilon<1
importanceLevel = 0.15

# Fix the accuracy level tau of the test
# Care : tau >0
# It is recommended that tau <4
accuracyLevel = 3.0

# Fix the confidence level (1-q) of the test
confidenceLevel = 0.99

# Create the Strong Maximum Test
# CARE : the event must be declared in the standard space
# 1. From the confidenceLevel parameter
mySMT_CL = ot.StrongMaximumTest(
    myStandardEvent,
    standardSpaceDesignPoint,
    importanceLevel,
    accuracyLevel,
    confidenceLevel,
)

# 2. Or from the  maximum number of points sampling the sphere
pointsNumber = 1000
mySMT_PN = ot.StrongMaximumTest(
    myStandardEvent,
    standardSpaceDesignPoint,
    importanceLevel,
    accuracyLevel,
    pointsNumber,
)

# Perform the test
mySMT_CL.run()
mySMT_PN.run()

# Get (or evaluate) the confidence level
# associated to the number of points used to sample the sphere
print("Confidence level = ", mySMT_CL.getConfidenceLevel())

# Get (or evaluate) the number of points used to sample the sphere
# associated the confidence level used
print("Points Number = ", mySMT_CL.getPointNumber())

# %%

# Get all the points verifying the event  and outside the design point vicinity
# Get also the values of limit state function at these points
potentialDesignPoints = mySMT_CL.getFarDesignPointVerifyingEventPoints()
values = mySMT_CL.getFarDesignPointVerifyingEventValues()
print("Potential design points = ", potentialDesignPoints)
print("Model values = ", values)

# Get all the points verifying the event and inside the design point vicinity
# Get also the values of limit state function at these points
vicinityDesignPoint = mySMT_CL.getNearDesignPointVerifyingEventPoints()
values = mySMT_CL.getNearDesignPointVerifyingEventValues()
print(
    "Points verifying the Event in the vicinity of the design points = ",
    vicinityDesignPoint,
)
print("Model values = ", values)

# Get all the points not verifying the event and outside the design point vicinity
# Get also the values of limit state function at these points
farSecurityPoints = mySMT_CL.getFarDesignPointViolatingEventPoints()
values = mySMT_CL.getFarDesignPointViolatingEventValues()
print(
    "Points NOT verifying the Event outside the vicinity of the design points = ",
    farSecurityPoints,
)
print("Model values = ", values)

# Get  all the points not verifying the event and inside the design point vicinity
# Get also the values of limit state function at these points
vicinitySecurityPoints = mySMT_CL.getNearDesignPointViolatingEventPoints()
values = mySMT_CL.getNearDesignPointViolatingEventValues()
print(
    "Points NOT verifying the Event outside the vicinity of the design points = ",
    vicinitySecurityPoints,
)
print("Model values = ", values)
