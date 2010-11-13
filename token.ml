(* # load "dynlink.cma" *)
# load "camlp4o.cma"
type token = LeftParen | RightParen | Int of int | Float of float | Symbol of string

let rec lex = parser
    
  (* Skip any whitespace. *)
  | [< ' (' ' | '\n' | '\r' | '\t'); stream >] -> lex stream

    (* identifier: [a-zA-Z][a-zA-Z0-9] *)
  | [< ' ('A' .. 'Z' | 'a' .. 'z' as c); stream >] ->
      let buffer = Buffer.create 1 in
      Buffer.add_char buffer c;
      lex_ident buffer stream
  | [<>] -> [<>]

and lex_ident buffer = parser
  | [< ' ('A' .. 'Z' | 'a' .. 'z' | '0' .. '9' as c); stream >] ->
      Buffer.add_char buffer c;
      lex_ident buffer stream
  | [<stream=lex>] -> [< 'Symbol (Buffer.contents buffer); stream>]



