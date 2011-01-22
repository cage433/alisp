require 'rake'
task :default => [:run_tests]
llvm_include = `llvm-config-2.8 --cppflags`.strip
rule(/\.o/ => [proc{|task_name| "src/" + task_name.sub(/\.o/, '.cpp')}] + FileList['include/*']) do |t| 
  sh "g++ -ggdb -I include/ #{llvm_include} -c #{t.source}"
end


llvm_gcc = `llvm-config-2.8 --cppflags --ldflags --libs core jit native`.gsub("\n", " ")

file "tests" => ["test/tests.cpp", "token.o", "lexer.o", "expression.o", "parser.o"] do
  sh "g++ -ggdb -I include/  -lcpptest token.o test/tests.cpp lexer.o parser.o  expression.o #{llvm_gcc} -o tests"
end
    
task :run_tests => ["tests"] do
  system('./tests')
end
