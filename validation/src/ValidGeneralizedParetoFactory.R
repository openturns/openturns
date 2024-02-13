# sudo apt install -y r-cran-rcpp r-cran-gsl ...
# R -e 'install.packages(c("extRemes", "evmix"))'
# Rscript validation/src/ValidGeneralizedParetoFactory.R

xdat <- as.matrix(read.csv("python/src/usecases/rain.csv"))
n <- length(xdat)
# print(xdat)

# mean residual life
library(extRemes)
png("mlr.png")
mrlplot(xdat)

library(eva)
png("mlexi.png")
# mle
cat("\n--- MLE ---\n")
fit <- gpdFit(xdat, threshold=30)
print(fit)

cat("\n--- covariates ---\n")
covs <- as.data.frame(seq(1, n, 1))
covs$const <- 1
names(covs) <- c("Trend1", "const")
fit_cov <- gpdFit(xdat, threshold = 30, scalevars = covs, scaleform = ~ Trend1, scalelink = exp, method= "mps")
print(fit_cov)


library(extRemes)
data <- as.data.frame(xdat)
data$trend <- seq(1, n, 1)
names(data) <- c("x", "trend")
summary(data)
cat("\n--- extRemes ---\n")
scalef <- x ~ trend
fit_cov2 <- fevd(xdat, data = data, threshold = 30, type = "GP", scale.fun = scalef, use.phi = TRUE, method = "MLE")
print(fit_cov2)

cat("\n--- ismev ---\n")
library(ismev)
summary(xdat)
covs2 <- as.data.frame(seq(1, n, 1))
names(covs2) <- c("trend")
summary(covs2)
fit_cov3 <- gpd.fit(xdat, threshold = 30, ydat = covs2, sigl = c(1), shl = NULL, siglink = exp)

# return level
cat("\n--- return level ---\n")
rl <- gpdRl(fit, period = 100, method = "delta")
print(rl)

# profiled ll
cat("\n--- profiled LL ---\n")
fit2 <- gpdProfShape(fit)
print(fit2)

# parameter stability plots
library(evmix)
png("shape.png")
tshapeplot(xdat, tlim = c(0, 50))
png("scale.png")
tscaleplot(xdat, tlim = c(0, 50)) # , try.thresh = c(8, 10, 12)
tscaleplot(xdat, tlim = c(10, 40))
