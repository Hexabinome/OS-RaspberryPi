# -*- mode: gdb-script -*-

set verbose off
set confirm off

break kmain-command-parser.c:16
commands
  p argc
  p *(res)
  p *(res+1)
  p *(res+2)
  p *(res+3)
  p *(res+4)
  assert_results
end

define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == 5)
  
  set $ok *= ($2 == "hello")
  set $ok *= ($3 == "my")
  set $ok *= ($4 == "name")
  set $ok *= ($5 == "is")
  set $ok *= ($6 == "david")
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
