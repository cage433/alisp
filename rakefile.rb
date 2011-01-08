file "alisp.o" => ["src/alisp.c"] do
  sh "gcc -c src/alisp.c"
end

file "alisp" => ["alisp.o"] do
  sh "gcc alisp.o -o alisp"
end
