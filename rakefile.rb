file "alisp.o" => ["src/alisp.c", "include/alisp.h"] do
  sh "gcc -ggdb -I include/ -c src/alisp.c"
end

file "alisp" => ["alisp.o"] do
  sh "gcc -ggdb alisp.o -o alisp"
end

file "tests.o" => ["test/tests.c", "include/alisp.h"] do
  sh "gcc -ggdb -I include/ -c test/tests.c"
end

file "tests" => ["tests.o", "alisp.o"] do
  sh "gcc -ggdb -lcheck alisp.o tests.o -o tests"
end

task :run_tests => ["tests"] do
  system('./tests')
end
