type expr = 
  Float of float
  | Symbol of string
  | Operation of string * expr array
