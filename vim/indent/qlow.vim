" indent/qlow.vim
if exists("b:did_indent")
   finish
endif
let b:did_indent = 1
setlocal indentexpr=QlowIndent()
setlocal nolisp
setlocal nosmartindent
setlocal nocindent
setlocal autoindent
setlocal comments=:--
setlocal indentkeys+==end,=do,=class

let b:undo_indent = "setl smartindent< indentkeys< indentexpr< autoindent< comments< "

function! QlowIndent()
  let line = getline(v:lnum)
  let previousNum = prevnonblank(v:lnum - 1)
  let previous = getline(previousNum)


  let ind = indent(previousNum)
  "if previous =~ "{" && previous !~ "}" && line !~ "}" && line !~ ":$"
  if previous =~ "class" || previous =~ "do"
      let ind = ind + &shiftwidth
  endif

  if getline(v:lnum) =~ '^\s*end'
      let ind = ind - &shiftwidth
  endif

  return ind
endfunction



