module Parser = 
  struct

    let rec parse_expr = 
      let rec parse_args accumulator = parser
        [< expr = parse_expr; args = parse_args (expr::accumulator) >] -> List.rev args
        | [<>] -> accumulator
      in parser
        [< 'Token.LeftParen; 'Token.Symbol sym; args = parse_args []; 'Token.RightParen >] -> Ast.Operation(sym, Array.of_list args)
        | [< 'Token.Float x >] -> Ast.Float x
    and parse = parser
        [< expr=parse_expr; stream = parse >] -> expr :: stream
      | [<>] -> []
  end;;
