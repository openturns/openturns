#! /usr/bin/env python

import math
import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Sphere manifold S^2 in R^3
sphere = otexp.SphereManifold(3)
print("Manifold ", repr(sphere))
print("Manifold ", sphere)
assert sphere.getAmbientDimension() == 3
assert sphere.getIntrinsicDimension() == 2
assert sphere.getClassName() == "SphereManifold"

# Default tolerance comes from the Manifold-Epsilon ResourceMap key
ott.assert_almost_equal(sphere.getEpsilon(), 1e-10, 1e-15, 0.0)
sphere.setEpsilon(1e-8)
ott.assert_almost_equal(sphere.getEpsilon(), 1e-8, 1e-15, 0.0)
sphere.setEpsilon(1e-10)

# Total volume: 4 pi for the 2-sphere
ott.assert_almost_equal(sphere.getVolume(), 4.0 * math.pi, 1e-12, 1e-12)

# Unit circle: total volume 2 pi
circle = otexp.SphereManifold(2)
assert circle.getAmbientDimension() == 2
assert circle.getIntrinsicDimension() == 1
ott.assert_almost_equal(circle.getVolume(), 2.0 * math.pi, 1e-12, 1e-12)

# Geodesic distance between (1, 0, 0) and (0, 1, 0) is pi / 2
east = [1.0, 0.0, 0.0]
north2 = [0.0, 1.0, 0.0]
ott.assert_almost_equal(sphere.getGeodesicDistance(east, north2), 0.5 * math.pi, 1e-12, 1e-12)
ott.assert_almost_equal(sphere.getGeodesicDistance(east, east), 0.0, 1e-12, 1e-12)
ott.assert_almost_equal(sphere.getGeodesicDistance(east, [-1.0, 0.0, 0.0]), math.pi, 1e-12, 1e-12)

# Membership and projection
assert sphere.isOnManifold(east)
assert not sphere.isOnManifold([2.0, 0.0, 0.0])
assert not sphere.isOnManifold([1.0, 0.0])
projected = sphere.projectToManifold([2.0, 0.0, 0.0])
print("projected=", projected)
ott.assert_almost_equal(projected, east, 1e-12, 1e-12)

# Tangent projection is orthogonal to the base point
tangent = sphere.projectToTangentSpace(east, [1.0, 1.0, 0.0])
print("tangent=", tangent)
ott.assert_almost_equal(tangent, [0.0, 1.0, 0.0], 1e-12, 1e-12)

# Exponential map: unit tangent vector at east along y gives north2 after pi / 2
image = sphere.expMap(east, [0.0, 0.5 * math.pi, 0.0])
print("image=", image)
ott.assert_almost_equal(image, north2, 1e-12, 1e-12)

# Exponential map of a null vector is the base point
ott.assert_almost_equal(sphere.expMap(east, [0.0, 0.0, 0.0]), east, 1e-12, 1e-12)

# Logarithmic map inverts the exponential map
preimage = sphere.logMap(east, north2)
print("preimage=", preimage)
ott.assert_almost_equal(preimage, [0.0, 0.5 * math.pi, 0.0], 1e-12, 1e-12)
ott.assert_almost_equal(sphere.expMap(east, preimage), north2, 1e-12, 1e-12)

# Logarithmic map of the base point is the null vector
ott.assert_almost_equal(sphere.logMap(east, east), [0.0, 0.0, 0.0], 1e-12, 1e-12)

# Logarithmic map at the cut locus (antipodal points) raises
with ott.assert_raises(TypeError):
    sphere.logMap(east, [-1.0, 0.0, 0.0])

# Tangent basis at east: 2 orthonormal vectors orthogonal to east
basis = sphere.getTangentBasis(east)
assert basis.getSize() == 2
assert basis.getDimension() == 3
for k in range(2):
    ott.assert_almost_equal([sum(basis[k][i] * east[i] for i in range(3))], [0.0], 1e-12, 1e-12)
    ott.assert_almost_equal([sum(basis[k][i] * basis[k][i] for i in range(3))], [1.0], 1e-12, 1e-12)
ott.assert_almost_equal([sum(basis[0][i] * basis[1][i] for i in range(3))], [0.0], 1e-12, 1e-12)

# Volume density is 1 at the origin and (sin r / r)^(n-2) elsewhere
ott.assert_almost_equal(sphere.getExpMapVolumeDensity([0.0, 0.0]), 1.0, 1e-12, 1e-12)
radius = 1.2
ott.assert_almost_equal(sphere.getExpMapVolumeDensity([radius, 0.0]), math.sin(radius) / radius, 1e-12, 1e-12)
ott.assert_almost_equal(circle.getExpMapVolumeDensity([radius]), 1.0, 1e-12, 1e-12)

# Wrong coordinates dimension raises
with ott.assert_raises(TypeError):
    sphere.getExpMapVolumeDensity([0.0])

# Projection of the null vector raises
with ott.assert_raises(TypeError):
    sphere.projectToManifold([0.0, 0.0, 0.0])

# Wrong ambient dimension raises
with ott.assert_raises(TypeError):
    sphere.expMap([1.0, 0.0], [0.0, 0.0])

# Ambient dimension 1 is rejected
with ott.assert_raises(ValueError):
    otexp.SphereManifold(1)

# SO(3) manifold of flattened row-major 3x3 rotations
rotations = otexp.SO3Manifold()
print("Manifold ", repr(rotations))
print("Manifold ", rotations)
assert rotations.getAmbientDimension() == 9
assert rotations.getIntrinsicDimension() == 3

# Total volume: 8 pi^2
ott.assert_almost_equal(rotations.getVolume(), 8.0 * math.pi * math.pi, 1e-12, 1e-12)

# Identity rotation, row-major flattened
identity = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
assert rotations.isOnManifold(identity)
assert not rotations.isOnManifold([2.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0])

# Rotation of angle theta around the z axis
theta = 0.7
cosT = math.cos(theta)
sinT = math.sin(theta)
rotZ = [cosT, -sinT, 0.0, sinT, cosT, 0.0, 0.0, 0.0, 1.0]
assert rotations.isOnManifold(rotZ)

# Geodesic distance from identity is the rotation angle
ott.assert_almost_equal(rotations.getGeodesicDistance(identity, rotZ), theta, 1e-12, 1e-12)
ott.assert_almost_equal(rotations.getGeodesicDistance(identity, identity), 0.0, 1e-12, 1e-12)

# Exponential map of a null increment is the base point
ott.assert_almost_equal(rotations.expMap(identity, [0.0] * 9), identity, 1e-12, 1e-12)

# Logarithmic map inverts the exponential map, tangent vector is skew * base
logZ = rotations.logMap(identity, rotZ)
print("logZ=", logZ)
expected = [0.0, -theta, 0.0, theta, 0.0, 0.0, 0.0, 0.0, 0.0]
ott.assert_almost_equal(logZ, expected, 1e-12, 1e-12)
ott.assert_almost_equal(rotations.expMap(identity, logZ), rotZ, 1e-12, 1e-12)

# Round trip at a non-trivial base point: exp then log
logAtZ = rotations.logMap(rotZ, identity)
ott.assert_almost_equal(rotations.expMap(rotZ, logAtZ), identity, 1e-10, 1e-10)
ott.assert_almost_equal(rotations.getGeodesicDistance(rotZ, identity), theta, 1e-12, 1e-12)

# Polar projection of a scaled rotation recovers the rotation
ott.assert_almost_equal(rotations.projectToManifold([2.0 * x for x in rotZ]), rotZ, 1e-12, 1e-12)

# Tangent projection at identity keeps the skew part only
skewPlusSym = [1.0, 1.0, 0.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0]
projectedTangent = rotations.projectToTangentSpace(identity, skewPlusSym)
print("projectedTangent=", projectedTangent)
ott.assert_almost_equal(projectedTangent, [0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0], 1e-12, 1e-12)

# Logarithmic map at the cut locus (half-turn) raises
halfTurn = [-1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0]
assert rotations.isOnManifold(halfTurn)
ott.assert_almost_equal(rotations.getGeodesicDistance(identity, halfTurn), math.pi, 1e-12, 1e-12)
with ott.assert_raises(TypeError):
    rotations.logMap(identity, halfTurn)

# Wrong ambient dimension raises
with ott.assert_raises(TypeError):
    rotations.expMap([1.0, 0.0, 0.0], [0.0] * 9)

# Tangent basis at identity: 3 Frobenius-orthonormal skew-symmetric matrices
rotationBasis = rotations.getTangentBasis(identity)
assert rotationBasis.getSize() == 3
assert rotationBasis.getDimension() == 9
for k in range(3):
    norm2 = sum(rotationBasis[k][i] * rotationBasis[k][i] for i in range(9))
    ott.assert_almost_equal([norm2], [1.0], 1e-12, 1e-12)
    for j in range(k):
        cross = sum(rotationBasis[k][i] * rotationBasis[j][i] for i in range(9))
        ott.assert_almost_equal([cross], [0.0], 1e-12, 1e-12)

# Volume density is 1 at the origin and (sin(theta/2)/(theta/2))^2 elsewhere
ott.assert_almost_equal(rotations.getExpMapVolumeDensity([0.0, 0.0, 0.0]), 1.0, 1e-12, 1e-12)
angle = 1.1
ratio = math.sin(0.5 * angle) / (0.5 * angle)
ott.assert_almost_equal(rotations.getExpMapVolumeDensity([angle, angle, 0.0]), ratio * ratio, 1e-12, 1e-12)

# Interface object wraps an implementation and forwards all operations
wrapped = otexp.Manifold(otexp.SphereManifold(3))
assert wrapped.getAmbientDimension() == 3
assert wrapped.getIntrinsicDimension() == 2
ott.assert_almost_equal(wrapped.getVolume(), 4.0 * math.pi, 1e-12, 1e-12)
ott.assert_almost_equal(wrapped.expMap(east, [0.0, 0.5 * math.pi, 0.0]), north2, 1e-12, 1e-12)
assert wrapped.isOnManifold(east)
wrapped.setEpsilon(1e-8)
ott.assert_almost_equal(wrapped.getEpsilon(), 1e-8, 1e-15, 0.0)

# Persistence through the Study mechanism
if ot.PlatformInfo.HasFeature("libxml2"):
    file = "study_manifold.xml"
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(file))
    study.add("sphere", sphere)
    study.add("rotations", rotations)
    study.save()

    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(file))
    study.load()
    loadedSphere = otexp.SphereManifold()
    loadedRotations = otexp.SO3Manifold()
    study.fillObject("sphere", loadedSphere)
    study.fillObject("rotations", loadedRotations)
    assert loadedSphere == sphere
    assert loadedRotations == rotations
    os.remove(file)
