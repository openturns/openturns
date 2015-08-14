dim <- 3
size <- 100
#sample <- matrix(nrow=size, ncol=dim)
#for (i in 1:size)
#{
#  x <- i - 1
#  sample[i, 1] <- x
#  sample[i, 2] <- x * x
#  sample[i, 3] <- size - 1 - sqrt(x)
#}
sample <- matrix(nrow=4, ncol=3)
sample[1, 1] <- 1
sample[1, 2] <- 0
sample[1, 3] <- 9

sample[2, 1] <- 2
sample[2, 2] <- 3
sample[2, 3] <- 5

sample[3, 1] <- 5
sample[3, 2] <- 1
sample[3, 3] <- 8

sample[4, 1] <- 6
sample[4, 2] <- 7
sample[4, 3] <- 2
print("min")
print(apply(sample, 2, min))
print("max")
print(apply(sample, 2, max))
print("mean")
print(apply(sample, 2, mean))
print("cov")
print(cov(sample))
print("sd")
print(chol(cov(sample)))
print("sd by component")
print(apply(sample, 2, sd))
print("cor Pearson")
print(cor(sample))
print("cor Spearman")
print(cor(sample, method="spearman"))
print("cor kendall")
print(cor(sample, method="kendall"))
print("range by component")
print(apply(sample, 2, range))
print("var by component")
print(apply(sample, 2, var))
