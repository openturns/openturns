#!/usr/bin/env R

# library(MASS)
# x <- PlantGrowth$weight
# But we removed the first value, as it generates a tie.

x = c(5.58, 5.18, 6.11, 4.50, 4.61, 5.17, 4.53, 
      5.33, 5.14, 4.81, 4.17, 4.41, 3.59, 5.87,
      3.83, 6.03, 4.89, 4.32, 4.69, 6.31, 5.12, 
      5.54, 5.50, 5.37, 5.29, 4.92, 6.15, 5.80, 5.26)
x<-x-mean(x)
txy <- ks.test(x,"pnorm")
print(txy$p.value,digits=20)
print(txy$statistic,digits=20)

