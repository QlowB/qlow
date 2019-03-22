" Vim syntax file
" Language: Qlow Programming Language
" Maintainer: Nicolas Winkler
" Latest Revision: 07 March 2018


if exists("b:current_syntax")
  finish
endif



syn keyword keywordy class struct do end if while return extern as new
syn keyword typey Integer Boolean
syn keyword typey String Char 
syn keyword typey Float32 Float64


syntax match identifiery "[a-zA-Z][a-zA-Z0-9]*"
syntax match numbery "\d\+"
syntax match stringy "\".\+\""
syntax match commenty "//.*"
syntax region multicommenty start="/\*"  end="\*/" contains=multicommenty

syntax keyword operatory not or and xor
syntax match operatory "\v\:\="


hi def link identifiery None
hi def link numbery Number
hi def link stringy String
hi def link keywordy Keyword
hi def link operatory Operator 
hi def link typey Type
hi def link commenty Comment
hi def link multicommenty Comment
hi Operator guifg=#FF0000 guibg=NONE gui=none

let b:current_syntax = "qlw"


