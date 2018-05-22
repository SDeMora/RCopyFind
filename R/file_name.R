#######################
# Function: file_name #
#######################

# dir = string/character of directory where matches.html resides
file_name <- function(dir) {
  paste0("file://", dir, "/matches.html")
}