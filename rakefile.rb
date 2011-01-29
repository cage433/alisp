task :default => [:run_tests]

include_files = FileList['include/*.h']
src_files = FileList['src/*.c']
obj_files = src_files.collect{|f| File.basename(f).sub(/\.c$/, ".o")}

rule(/\.o/ => [proc{|task_name| "src/" + task_name.sub(/\.o/, '.c')}] + include_files) do |t| 
  sh "gcc -ggdb -I include/ -c #{t.source}"
end

file "tests.o" => ["test/tests.c"] + include_files do
  sh "gcc -ggdb -I include/ -c test/tests.c"
end

file "tests" => ["tests.o"] + obj_files do
  sh "gcc -ggdb -I include/ -lcheck token.o lexer.o expression.o tests.o -o tests"
end

task :run_tests => ["tests"] do
  system('./tests')
end
