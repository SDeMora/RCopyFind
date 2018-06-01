cleanup<-function(script_file){
  file_name <- paste(script_file,".txt", sep="")
  if (file.exists(file_name)) file.remove(file_name)
}