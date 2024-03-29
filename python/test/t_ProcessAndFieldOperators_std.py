import openturns as ot

ot.TESTPREAMBLE()

mesh = ot.RegularGrid(0, 1, 4)
vals1 = [
    ot.Sample([[1, 2, 3], [2, 3, 4], [3, 4, 5], [4, 5, 6]]),
    ot.Sample([[5, 6, 7], [6, 7, 8], [7, 8, 9], [8, 9, 10]]),
]
vals2 = [
    ot.Sample([[0, 1, 2], [1, 2, 3], [2, 3, 4], [3, 4, 5]]),
    ot.Sample([[4, 5, 6], [5, 6, 7], [6, 7, 8], [7, 8, 9]]),
]

ps1 = ot.ProcessSample(mesh, vals1)
ps2 = ot.ProcessSample(mesh, vals2)

f1 = ot.Field(mesh, vals1[0])
f2 = ot.Field(mesh, vals2[0])

s = vals1[0]
p = ot.Point((1, 2, 3))

# ProcessSample and ProcessSample operators
sumVals = [
    ot.Sample([[2, 4, 6], [4, 6, 8], [6, 8, 10], [8, 10, 12]]),
    ot.Sample([[10, 12, 14], [12, 14, 16], [14, 16, 18], [16, 18, 20]]),
]
psSum = ot.ProcessSample(mesh, sumVals)
psDiff = ot.ProcessSample(mesh, 2, 3)

print("Sum between ProcessSamples", ps1 + ps2)
print("Difference between ProcessSamples", ps1 - ps2)
ps1 += ps2
print(ps1)
ps1 -= ps2
print(ps1)

print("Sum between ProcessSample and Field", ps1 + f1)
print("Difference between ProcessSample and Field", ps1 - f1)

ps1 += f1
print(ps1)
ps1 -= f1
print(ps1)

print("Sum between ProcessSample and Sample", ps1 + s)
print("Difference between ProcessSample and Sample", ps1 - s)

ps1 += s
print(ps1)
ps1 -= s
print(ps1)

print("Sum between ProcessSample and Point", ps1 + p)
print("Difference between ProcessSample and Point", ps1 - p)

ps1 += p
print(ps1)
ps1 -= p
print(ps1)


print("Sum between Fields", f1 + f2)
print("Difference between Fields", f1 - f2)

f1 += f2
print(f1)
f1 -= f2
print(f1)

print("Sum between Field and Sample", f1 + s)
print("Difference between Field and Sample", f1 - s)

f1 += s
print(f1)
f1 -= s
print(f1)

print("Sum between Field and Point", f1 + p)
print("Difference between Field and Point", f1 - p)

f1 += p
print(f1)
f1 -= p
print(f1)
