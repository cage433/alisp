let rec lex = parser
    
  (* Skip any whitespace. *)
  | [< ' (' ' | '\n' | '\r' | '\t'); stream >] -> lex stream

  (* number - float only *)
  | [< ' ('0' .. '9' as c); stream >] ->
      let buffer = Buffer.create 1 in
      Buffer.add_char buffer c;
      lex_number buffer stream

  | [< ''('; stream=lex >] -> [< 'Token.LeftParen ; stream >]
  | [< '')'; stream=lex >] -> [< 'Token.RightParen ; stream >]
    
  (* symbol *)
  | [< ' ('a' .. 'z' | 'A' .. 'Z' as c); stream >] ->
      let buffer = Buffer.create 1 in
      Buffer.add_char buffer c;
      lex_identifier buffer stream

  | [<>] -> [<>]

and lex_number buffer = parser
  | [< ' ('0' .. '9' | '.' as c); stream >] ->
      Buffer.add_char buffer c;
      lex_number buffer stream
  | [<stream=lex>] -> [< 'Token.Float (float_of_string (Buffer.contents buffer)); stream >]

and lex_identifier buffer = parser
  | [< ' ('0' .. '9' | 'a' .. 'z' | 'A' .. 'Z' | '_' as c); stream >] ->
      Buffer.add_char buffer c;
      lex_identifier buffer stream

  | [<stream=lex>] -> [< 'Token.Symbol (Buffer.contents buffer); stream >]
