library(shiny)
library(DT)
library(htmltools)
library(markdown)
library(yaml)
options(shiny.sanitize.errors = TRUE)

navbarPage(
  theme = "yeti.css",
  "Quake 2 Map Times Viewer",
  # tab 1
  tabPanel(
    "About",
    fluidRow(
      column(8, includeMarkdown("about.md"))
    )
  ),
  # tab 2
  tabPanel(
    "Map Times",
    fluidRow(
      dataTableOutput("t1")
    )
  )
)