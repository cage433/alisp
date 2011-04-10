function! CFileBaseName()
  return substitute(expand("%:t:r"), "_tests", "", "")
endfunction

function! CSourceFile()
  return "src/".CFileBaseName().".c"
endfunction

function! CIncludeFile()
  return "include/".CFileBaseName().".h"
endfunction

function! CTestFile()
  return "test/".CFileBaseName()."_tests.c"
endfunction

function! NextCFile()
  let possibles = []
  let current_file = expand("%")
  if current_file == CTestFile()
    let possibles = [CSourceFile(), CIncludeFile()]
  elseif current_file == CSourceFile()
    let possibles = [CIncludeFile(), CTestFile()]
  elseif current_file == CIncludeFile()
    let possibles = [CTestFile(), CSourceFile()]
  end

  if filereadable(get(possibles, 0))
    exec ":vi ".get(possibles, 0)
  elseif filereadable(get(possibles, 1))
    exec ":vi ".get(possibles, 1)
  end
endfunction

command! NextCFile :call NextCFile()

map <F4> :NextCFile<CR>
