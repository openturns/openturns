#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# 1d
mesh = ot.IntervalMesher([10]).build(ot.Interval(-2.0, 2.0))
graph = mesh.draw()

# 2d
mesh = ot.IntervalMesher([10] * 2).build(ot.Interval([-2.0] * 2, [2.0] * 2))
graph = mesh.draw()

# 3d
mesh = ot.IntervalMesher([10] * 3).build(ot.Interval([-2.0] * 3, [2.0] * 3))
graph = mesh.draw3D()

for drawEdge in [True, False]:
    for shading in [True, False]:
        for rho in [1.0, 0.8]:
            for alpha in [1.0, 0.5]:
                faceColor = ot.Drawable.ConvertToRGBA(
                    ot.Drawable.ConvertFromName(
                        ot.ResourceMap.GetAsString("Mesh-FaceColor")
                    )
                )
                faceColor[3] = int(255 * alpha)
                ot.ResourceMap.SetAsString(
                    "Mesh-FaceColor",
                    ot.Drawable.ConvertFromRGBA(*faceColor),
                )
                edgeColor = ot.Drawable.ConvertToRGBA(
                    ot.Drawable.ConvertFromName(
                        ot.ResourceMap.GetAsString("Mesh-EdgeColor")
                    )
                )
                edgeColor[3] = int(255 * alpha)
                ot.ResourceMap.SetAsString(
                    "Mesh-EdgeColor",
                    ot.Drawable.ConvertFromRGBA(*edgeColor),
                )

                graph = mesh.draw3D(drawEdge, 0.2, 0.5, 0.9, shading, rho)
                graph.setTitle(f"{drawEdge=}, {shading=}, {rho=}, {alpha=}")
