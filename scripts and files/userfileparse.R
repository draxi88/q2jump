users <- read.table("users.t", fill = TRUE, col.names = paste("column", 1:10, sep = "_"))
colnames(users) <- c("id", "comps", "comp_score", "1st", "2nd", "3rd", "4th", "5th", "score", "name")
users <- users[order(users$name),]
write.csv(users, "users.csv")
