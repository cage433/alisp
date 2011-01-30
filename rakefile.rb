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
        sh "gcc -ggdb3 -I include/ -c #{file} -o #{obj}"
    end
end

obj_files = (src_files + test_files).collect{|f| obj_file(f)}


file "tests" => obj_files do
    obj = obj_files.join(" ")
  sh "gcc -ggdb3 -I include/ -lcheck #{obj} -o tests"
end

task :run_tests => ["tests"] do
  system('./tests')
end
