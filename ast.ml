type expr = 
  Float of float
  | Operation of string * expr array
