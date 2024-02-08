# sudo apt install -y r-cran-rcpp r-cran-gsl ...
# R -e 'install.packages(c("extRemes", "evmix"))'
# Rscript validation/src/ValidGeneralizedParetoFactory.R

data <- as.matrix(read.csv("python/src/usecases/rain.csv"))
# print(data)

# mean residual life
library(extRemes)
png("mlr.png")
mrlplot(data)

library(eva)
png("mlexi.png")
# mle
cat("\n--- MLE ---\n")
fit <- gpdFit(data, threshold=30)
print(fit)

cat("\n--- covariates ---\n")
n <- length(data)
covs <- as.data.frame(seq(1, n, 1))
covs$const <- 1
names(covs) <- c("Trend1")
fit_cov <- gpdFit(data, threshold = 30, scalevars = covs, scaleform = ~ Trend1, scalelink = exp, method= "mps")
print(fit_cov)

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
tshapeplot(data, tlim = c(0, 50))
png("scale.png")
tscaleplot(data, tlim = c(0, 50)) # , try.thresh = c(8, 10, 12)
tscaleplot(data, tlim = c(10, 40))
