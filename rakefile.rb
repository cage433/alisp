require 'rake'
task :default => [:run_tests]

rule(/\.o/ => [proc{|task_name| "src/" + task_name.sub(/\.o/, '.cpp')}] + FileList['include/*']) do |t| 
  sh "g++ -ggdb -I include/ -c #{t.source}"
end

file "tests" => ["test/tests.cpp", "token.o", "lexer.o", "expression.o", "parser.o"] do
  sh "g++ -ggdb -I include/ -lcpptest token.o test/tests.cpp lexer.o parser.o -o tests"
end
    
task :run_tests => ["tests"] do
  system('./tests')
end
