task :default => [:run_tests]

include_files = FileList['include/*.h']
src_files = FileList['src/*.c']
test_files = FileList['test/*.c']
def obj_file(src_file)
    "obj/" + File.basename(src_file).sub(/\.c/, ".o")
end
(src_files + test_files).each do |file|
    obj = obj_file(file)
    file obj => [file] + include_files do
        sh "gcc -rdynamic -ggdb3 -I include/ -ldl -c #{file} -o #{obj}"
    end
end

obj_files = (src_files + test_files).collect{|f| obj_file(f)}


file "tests" => obj_files do
    obj = (obj_files - ['obj/main.o', 'obj/alisp.o']).join(" ")
  sh "gcc -rdynamic -ggdb3 -lcheck -ldl #{obj} -o tests"
end

file "main" => obj_files do
    obj = (obj_files - ['obj/tests.o', 'obj/alisp.o']).join(" ")
  sh "gcc -rdynamic -ggdb3 -lcheck -ldl #{obj} -o main"
end

file "alisp" => obj_files do
    obj = (obj_files - ['obj/tests.o', 'obj/main.o']).join(" ")
  sh "gcc -rdynamic -ggdb3 -lcheck -ldl #{obj} -o alisp"
end

task :run_tests => ["tests"] do
  system('./tests')
end

task :run_main => ["main"] do
  system('./main')
end

task :run_repl => ["alisp"] do
  system('./alisp -i')
end

task :clean => [] do
  system('rm obj/*.o *.o')
end
    
