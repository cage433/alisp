task :default => ["token.o"]

file "token.o" => ["cpp_src/token.cpp", "cpp_include/token.h"] do
  sh "g++ -ggdb -I cpp_include/ -c cpp_src/token.cpp"
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
#task :run_cpp_tests => ["cpp_tests"] do
#  system('./cpp_tests')
#end
