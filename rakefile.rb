task :default => [:run_tests]

file "token.o" => ["src/token.cpp", "include/token.h"] do
  sh "g++ -ggdb -I include/ -c src/token.cpp"
end

file "tests.o" => ["test/tests.cpp", "include/token.h"] do
  sh "g++ -ggdb -I include/ -c test/tests.cpp"
end

file "lexer.o" => ["src/lexer.cpp", "include/token.h", "include/lexer.h"] do 
  sh "g++ -ggdb -I include/ -c src/lexer.cpp"
end

file "tests" => ["tests.o", "token.o", "lexer.o"] do
  sh "g++ -ggdb -I include/ -lcpptest token.o tests.o lexer.o -o tests"
end
    
task :run_tests => ["tests"] do
  system('./tests')
end
