# read in a maptimes condump
filename <- 'C:/quake2/jump/condumps/scores.txt'

# read in condump
scores <- scan(filename, what = 'raw', sep = '\n')
scores2 <- c()
i <- 1
for(x in scores) {
  if(grepl('^\\d', x)) {
    scores2[i] <- x
    i <- i + 1
  }
}

# write to a new text file for cleaner reading
write.table(scores2, 'cleaned.txt', row.names = FALSE, col.names = FALSE, quote = FALSE)

# read in new text file
scores <- read.table('cleaned.txt', header = FALSE)
scores <- scores[1:2]

#assign point values
points <- c(15:1)
scores$points <- points[scores$V1]

# aggregate the points for each person
scorescount <- aggregate(scores$points, by = list(scores$V2), FUN = length)
scores <- aggregate(scores$points, by = list(scores$V2), FUN = sum)
scores <- merge(x = scores, y = scorescount, by = "Group.1")
colnames(scores) = c('name','points','maps')

# calc avg finish
scores$avg.finish <- 16 - round(scores$points / scores$maps, 1)

# order by score
scores <- scores[order(scores$points, decreasing = TRUE),]
rownames(scores) <- NULL
scores
