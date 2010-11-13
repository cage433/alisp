module Token = 
  struct
    type token = LeftParen | RightParen | Int of int | Float of float | Symbol of string
  end;;
