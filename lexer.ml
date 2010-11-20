let rec lex = parser
    
  (* Skip any whitespace. *)
  | [< ' (' ' | '\n' | '\r' | '\t'); stream >] -> lex stream

  (* number - float only *)
  | [< ' ('0' .. '9' as c); stream >] ->
      let buffer = Buffer.create 1 in
      Buffer.add_char buffer c;
      lex_number buffer stream

  | [< '')'; stream >] -> [< 'Token.RightParen ; lex stream >]
  | [< ''('; stream >] -> [< 'Token.LeftParen ; lex stream >]
  | [< '';'; stream >] -> [< 'Token.SemiColon ; lex stream >]
    
  (* symbol *)
  | [< ' ('*' | '+' | '-' | '/' as c) ; stream >] -> [< 'Token.Symbol (Char.escaped c); lex stream >] 
  | [<>] -> [<>]

and lex_number buffer = parser
  | [< ' ('0' .. '9' | '.' as c); stream >] ->
      Buffer.add_char buffer c;
      lex_number buffer stream
  | [<stream>] -> [< 'Token.Float (float_of_string (Buffer.contents buffer)); lex stream >]

