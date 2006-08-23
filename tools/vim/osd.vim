" Vim syntax file
" Language:	Orocos State Diagram
" Maintainer:	Wim Meeussen
" Last Change:	2006 Aug 22

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif



" osd keywords
syn keyword osdStatement	new delete this do set select
syn keyword osdConditional	if then else switch
syn keyword osdType		bool double
syn keyword osdExceptions	throw try catch
syn keyword osdStorageClass	var const
syn keyword osdStructure        StateMachine state entry handle exit transitions preconditions
syn keyword osdNumber		NPOS
syn keyword osdBoolean		true false

syn region osdString		start=+L\="+ skip=+\\\\\|\\"+ end=+"+
syn region osdCommentL	        start="//" skip="\\$" end="$" keepend 


" Default highlighting
if version >= 508 || !exists("did_osd_syntax_inits")
  if version < 508
    let did_osd_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif
  HiLink osdExceptions		Exception
  HiLink osdConditional		Conditional
  HiLink osdStatement		Statement
  HiLink osdType		Type
  HiLink osdStorageClass	StorageClass
  HiLink osdStructure		Structure
  HiLink osdNumber		Number
  HiLink osdBoolean		Boolean
  HiLink osdString		String
  HiLink osdCommentL		Comment
  delcommand HiLink
endif

let b:current_syntax = "osd"

" vim: ts=8
