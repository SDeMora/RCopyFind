run_copyfind <- function(cleanup=F, script_name=NULL){
  
  directory<-system.file("CopyFind", package="RCopyFind")
  
  shell (paste(directory, "Copyfind64.4.1.5.exe < script.txt", sep="/"))
  
  if(cleanup) {
    
    cleanup(script_name)
    
  }
  
}