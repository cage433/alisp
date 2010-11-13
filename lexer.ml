module Lexer = 
  struct
    let rec lex = parser
        
      (* Skip any whitespace. *)
      | [< ' (' ' | '\n' | '\r' | '\t'); stream >] -> lex stream

      (* identifier: [a-zA-Z][a-zA-Z0-9] *)
      | [< ' ('A' .. 'Z' | 'a' .. 'z' as c); stream >] ->
          let buffer = Buffer.create 1 in
          Buffer.add_char buffer c;
          lex_ident buffer stream

      (* number - float or int *)
      | [< ' ('0' .. '9' as c); stream >] ->
          let buffer = Buffer.create 1 in
          Buffer.add_char buffer c;
          lex_number buffer stream

      | [< ''('; stream=lex >] -> [< 'Token.LeftParen ; stream >]
      | [< '')'; stream=lex >] -> [< 'Token.RightParen ; stream >]
        
      | [<>] -> [<>]

    and lex_ident buffer = parser
      | [< ' ('A' .. 'Z' | 'a' .. 'z' | '0' .. '9' as c); stream >] ->
          Buffer.add_char buffer c;
          lex_ident buffer stream
      | [<stream=lex>] -> [< 'Token.Symbol (Buffer.contents buffer); stream>]

    and lex_number buffer = parser
      | [< ' ('0' .. '9' as c); stream >] ->
          Buffer.add_char buffer c;
          lex_number buffer stream
      | [< ''.' ; stream>] ->
          Buffer.add_char buffer '.';
          lex_float buffer stream
      | [<stream=lex>] -> [< 'Token.Int (int_of_string (Buffer.contents buffer)); stream >]

    and lex_float buffer = parser
      | [< ' ('0' .. '9' as c); stream >] ->
          Buffer.add_char buffer c;
          lex_float buffer stream
      | [<stream=lex>] -> [< 'Token.Float (float_of_string (Buffer.contents buffer)); stream >]
  end;;



