# -*- mode: gdb-script -*-

set verbose off
set confirm off

break process
commands
  p $pc
  continue
end

break kmain-fork.c:17
commands
  p cpt
  p pid
  assert_results
end

break kmain-fork.c:23
commands
  p cpt
  p pid
  continue
end


define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == process+8)
  
  set $ok *= ($2 == 11)
  set $ok *= ($3 == 2)
  
  set $ok *= ($4 == 9)
  set $ok *= ($5 == 0)
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
