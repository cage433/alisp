module Parser = 
  struct

    let rec parse_expr = 
      let rec parse_args accumulator = parser
        [< expr = parse_expr; args = parse_args (expr::accumulator) >] -> List.rev args
        | [<>] -> accumulator
      in parser
        [< 'Token.Float x >] -> Ast.Float x
        | [< 'Token.Int n >] -> Ast.Int n
        | [< 'Token.Symbol s >] -> Ast.Symbol s
        | [< 'Token.LeftParen; 'Token.Symbol sym; args = parse_args []; 'Token.RightParen >] -> Ast.Operation(sym, Array.of_list args)
  end;;
