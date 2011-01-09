file "token.o" => ["src/token.cpp", "include/token.h"] do
  sh "g++ -ggdb -I include/ -c src/token.cpp"
end

file "lexer.o" => ["src/lexer.cpp", "include/lexer.h", "include/token.h"] do
  sh "g++ -ggdb -I include/ -c src/lexer.cpp"
end

file "parser.o" => ["src/parser.cpp", "include/parser.h", "include/token.h"] do
  sh "g++ -ggdb -I include/ -c src/parser.cpp"
end

file "tests.o" => ["test/tests.cpp", "include/lexer.h", "include/token.h"] do
  sh "g++ -ggdb -I include/ -c test/tests.cpp"
end

file "tests" => ["tests.o", "lexer.o", "token.o"] do
  sh "g++ -ggdb -I include/ -lcheck token.o lexer.o tests.o -o tests"
end

task :run_tests => ["tests"] do
  system('./tests')
end
