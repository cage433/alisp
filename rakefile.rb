task :default => [:run_tests]

file "token.o" => ["cpp_src/token.cpp", "cpp_include/token.h"] do
  sh "g++ -ggdb -I cpp_include/ -c cpp_src/token.cpp"
end

file "tests.o" => ["cpp_test/tests.cpp", "cpp_include/token.h"] do
  sh "g++ -ggdb -I cpp_include/ -c cpp_test/tests.cpp"
end

file "lexer.o" => ["cpp_src/lexer.cpp", "cpp_include/token.h", "cpp_include/lexer.h"] do 
  sh "g++ -ggdb -I cpp_include/ -c cpp_src/lexer.cpp"
end

file "tests" => ["tests.o", "token.o", "lexer.o"] do
  sh "g++ -ggdb -I cpp_include/ -lcpptest token.o tests.o lexer.o -o tests"
end
    
#
#file "lexer.o" => ["cpp_src/lexer.cpp", "cpp_include/lexer.h", "cpp_include/token.h"] do
#  sh "g++ -ggdb -I cpp_include/ -c cpp_src/lexer.cpp"
#end
#
#file "parser.o" => ["cpp_src/parser.cpp", "cpp_include/parser.h", "cpp_include/token.h"] do
#  sh "g++ -ggdb -I cpp_include/ -c cpp_src/parser.cpp"
#end
#
#file "cpp_tests.o" => ["cpp_test/cpp_tests.cpp", "cpp_include/lexer.h", "cpp_include/token.h"] do
#  sh "g++ -ggdb -I cpp_include/ -c cpp_test/cpp_tests.cpp"
#end
#
#file "cpp_tests" => ["parser.o", "cpp_tests.o", "lexer.o", "token.o"] do
#  sh "g++ -ggdb -I cpp_include/ -lcheck parser.o token.o lexer.o cpp_tests.o -o cpp_tests"
#end
#
task :run_tests => ["tests"] do
  system('./tests')
end
