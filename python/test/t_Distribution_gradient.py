#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

h = 1e-4

for factory in ot.DistributionFactory.GetContinuousUniVariateFactories():
    dist = factory.build()
    if dist.getName() == "MeixnerDistribution":
        # unstable
        continue
    p = dist.getParameter()
    n = len(p)
    x = dist.computeQuantile(0.51)
    pdfgrad = dist.computePDFGradient(x)
    cdfgrad = dist.computeCDFGradient(x)
    pdf_ref = dist.computePDF(x)
    cdf_ref = dist.computeCDF(x)

    pdfgrad_fd = ot.Point(n)
    for j in range(n):
        p_tmp = ot.Point(p)
        p_tmp[j] = p_tmp[j] + h
        dist.setParameter(p_tmp)
        pdfgrad_fd[j] = (dist.computePDF(x) - pdf_ref) / h
    print(dist.getName(), "PDF", pdfgrad, pdfgrad_fd)
    ott.assert_almost_equal(pdfgrad_fd, pdfgrad, 1e-3, 1e-3, dist.getName() + ".pdf")

    cdfgrad_fd = ot.Point(n)
    for j in range(n):
        p_tmp = ot.Point(p)
        p_tmp[j] = p_tmp[j] + h
        dist.setParameter(p_tmp)
        cdfgrad_fd[j] = (dist.computeCDF(x) - cdf_ref) / h
    print(dist.getName(), "CDF", cdfgrad, cdfgrad_fd)
    ott.assert_almost_equal(cdfgrad_fd, cdfgrad, 1e-3, 1e-3, dist.getName() + ".cdf")
