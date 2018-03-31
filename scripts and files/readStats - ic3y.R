library(rvest)
library(psych)
library(RCurl)

# stripping html tags
stripHtml <- function(htmlString) {
  return(gsub("<.*?>", "", htmlString))
}

# strip spaces and line breaks
stripOther <- function(htmlString) {
  htmlString <- gsub("\n", "", htmlString)
  htmlString <- gsub(" ", "", htmlString)
  return(htmlString)
}

# maplist
html <- read_html("http://race.q2jump.de/maps.html")
maplist <- html_nodes(html, "div")
maplist <- stripHtml(maplist) 
maplist <- stripOther(maplist)
maplist <- data.frame(matrix(maplist, ncol = 5, byrow = TRUE))
maplist <- maplist[-1,]
maplist <- data.frame(maplist$X2)

# map loop
length <- length(maplist[[1]])
prefix <- "http://race.q2jump.de/"
suffix <- ".html"
usermaps <- data.frame(NULL)
i <- 1
while (i < length + 1) {
  if (url.exists(paste(prefix, maplist[i,], suffix, sep = "")) == FALSE) {}
  else {
    html <- read_html(paste(prefix, maplist[i,], suffix, sep = ""))
    mapfile <- html_nodes(html, "div")
    mapfile <- stripHtml(mapfile)
    mapfile <- stripOther(mapfile)
    mapfile <- data.frame(matrix(mapfile, ncol = 5, byrow = TRUE))
    mapfile <- mapfile[-1, -c(3:4)]
    mapfile <- cbind(mapfile, maplist[i,])
    usermaps <- rbind(usermaps, mapfile)
  }
  print(paste(i, "/", length, sep = ""))
  i <- i + 1
}


colnames(usermaps) <- c("pos", "name", "time", "map")
write.csv(usermaps, file = "maptimes-ic3y.csv")