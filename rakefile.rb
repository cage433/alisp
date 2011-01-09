file "token.o" => ["src/token.c", "include/token.h"] do
  sh "gcc -ggdb -I include/ -c src/token.c"
end

file "lexer.o" => ["src/lexer.c", "include/lexer.h", "include/token.h"] do
  sh "gcc -ggdb -I include/ -c src/lexer.c"
end

file "tests.o" => ["test/tests.c", "include/lexer.h", "include/token.h"] do
  sh "gcc -ggdb -I include/ -c test/tests.c"
end

file "tests" => ["tests.o", "lexer.o", "token.o"] do
  sh "gcc -ggdb -I include/ -lcheck token.o lexer.o tests.o -o tests"
end

task :run_tests => ["tests"] do
  system('./tests')
end
