library(shiny)
library(DT)
library(htmltools)

alltimes <- read.csv("alltimes.csv")

shinyServer(function(input, output) {
  output$t1 <- renderDataTable({ 
    alltimes
  },
  extensions = 'Buttons',
  options = list(
    "dom" = 'T<"clear">lBfrtip',
    buttons = list('copy', 'csv'),
    lengthMenu = c(10, 25, 50, 100, 500, 1000, 3000, 65000)
  ),
  filter = 'top',
  rownames = FALSE)
})