module Ast =
  struct
    type expr = 
      Float of float
      | Int of int
      | Symbol of string
      | Operation of string * expr array
  end;;
