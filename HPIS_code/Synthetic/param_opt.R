trainFunc <- function(xVec) {
	#print(str(xVec))
	x <- xVec[3:length(xVec)]
	y <- xVec[2]
	wx <- sum(get("w", inherits = TRUE) * x)
	sigma <- 1 / (1 + exp(wx * y))
	#print(paste("Sigma:", sigma))
	assign("w", w + alpha * y * sigma * x, inherits = TRUE)
	#print(paste("w:", get("w", inherits = TRUE)))
}


epochFunc <- function(x, data, numPartition) {
	partitionSize <- nrow(data) %/% numPartition
	shuffleIdx <- sapply(1:numPartition, FUN = function(x, partitionSize, dataSize) {
		partitionSize * (x - 1) + sample.int(min(partitionSize, dataSize - partitionSize * (x - 1)))
	}, partitionSize = partitionSize, dataSize = nrow(data)) 
	if (nrow(data) > partitionSize * numPartition) {
		shuffleIdx <- append(shuffleIdx, partitionSize * numPartition + sample.int(nrow(data) - partitionSize * numPartition))
	}
	#temporarily disable shuffling for testing
	shuffledData <- data[shuffleIdx,]
	#shuffledData <- data
	if (nrow(shuffledData) != nrow(data)) {
		print(nrow(shuffledData))
		print(nrow(data))
		print("Error!")
	}
	#print(shuffledData[1,])
	apply(shuffledData, MARGIN = 1, FUN = trainFunc)
	assign("alpha", alpha * decay, inherits = TRUE)
	#loss <- sum(apply(shuffledData, MARGIN = 1, FUN = lossFunc))
	#print(paste("Loss at epoch ", x, ":", loss))
	#print(paste("w",w))
}

lossFunc <- function(xVec) {
	x <- xVec[3:length(xVec)]
	y <- xVec[2]
	loss <- log(1 + exp(-1 * sum(get("w", inherits = TRUE) * x) * y))
	#print(paste("Loss: ", loss))
	#print(sum(get("w", inherits = TRUE) * x) * y)
	#q()
}

optLossFunc <- function(param, data, numPartition) {
	alpha <<- param[1]
	decay <<- param[2]
	w <<- rep(0, ncol(data) - 2)
	apply(data.frame(c(1:40)), MARGIN = 1, FUN = epochFunc, data = data, numPartition = numPartition)
	loss <- sum(apply(data, MARGIN = 1, FUN = lossFunc))
	#print(paste("Loss:", loss))
	#print(paste("w", w))
}
#alpha <- 0.3958048
#decay <- 0.6930205
args = commandArgs(trailingOnly=TRUE)
if (length(args) != 2) {
	print("Error args: <table name> <num partition> !")
	q()
}
table <- read.table(args[1], sep = " ")
data <- matrix(unlist(table), nrow = nrow(table))
w <- rep(0, ncol(data) - 2)
#optLossFunc(c(alpha, decay), data, 736);
res <- optim(c(0.01, 0.98), fn = optLossFunc, method = "SANN", data = data, control = list(maxit = 10, reltol = 0.001), numPartition = min(strtoi(args[2]), nrow(table)))
print(res$par)
print(optLossFunc(res$par, data, min(strtoi(args[2]), nrow(table))))
#trainFunc(data[1,])
#print(sum(apply(data, MARGIN = 1, FUN = lossFunc)))
#print(w)
