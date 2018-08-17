#libs
library(rvest)
library(psych)
library(stringi)

#strip html, spaces, line breaks
strip <- function(a) {
  a <- gsub("<.*?>", "", a)
  a <- gsub("\n", "", a)
  return(gsub(" ", "", a))
}

# maplist
maplist <- strip(html_nodes(read_html("http://195.93.242.155/~quake2/quake2/jump/_html/maps.html"), "div")) 
maplist <- data.frame(matrix(maplist, ncol = 5, byrow = TRUE))
maplist <- maplist[-1,]
maplist <- data.frame(maplist$X2)

# map loop
length <- length(maplist[[1]])
prefix <- "http://195.93.242.155/~quake2/quake2/jump/_html/"
suffix <- ".html"
usermaps <- data.frame(NULL)
i <- 1
while (i < length + 1) {
  if (grepl("#|qqqqqqqq", maplist[i,])) {} # remove names with #, leave the or to remember it
  else {
    mapfile <- strip(html_nodes(read_html(paste(prefix, maplist[i,], suffix, sep = "")), "div"))
    mapfile <- data.frame(matrix(mapfile, ncol = 5, byrow = TRUE))
    mapfile <- mapfile[-1, -c(4)]
    mapfile <- cbind(mapfile, maplist[i,])
    usermaps <- rbind(usermaps, mapfile)
  }
  print(paste(i, "/", length, sep = ""))
  i <- i + 1
}

#manually add railjump#1 and railjump#2, their url's are broke
for(i in (1:2)) {
  mapfile <- strip(html_nodes(read_html(paste(prefix, paste0("railjump%23" , i), suffix, sep = "")), "div"))
  mapfile <- data.frame(matrix(mapfile, ncol = 5, byrow = TRUE))
  mapfile <- mapfile[-1, -c(4)]
  mapfile <- cbind(mapfile, paste0("railjump#", i))
}
colnames(mapfile) <- colnames(usermaps)
usermaps <- rbind(usermaps, mapfile)


#final files
colnames(usermaps) <- c("pos", "name", "date", "time", "map")
ace <- subset(usermaps, name == "ace")
first <- subset(usermaps, pos == 1)
write.csv(usermaps, file = paste0(Sys.Date(), "-maptimes.csv"), row.names = FALSE)
write.csv(ace, file = paste0(Sys.Date(), "-ace.csv"), row.names = FALSE)
write.csv(first, file = paste0(Sys.Date(), "-first.csv"), row.names = FALSE)
