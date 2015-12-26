# -*- mode: gdb-script -*-

set verbose off
set confirm off

break process
commands
  p $pc
  n
  p momo
  continue
end

break kmain-fork.c:24
commands
  p momo[0]
  p momo[1]
  assert_results
end

break kmain-fork.c:29
commands
  p momo[0]
  p momo[1]
  p momo
  n
  p momo[1]
  continue
end


define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == process+8)
  set $ok *= ($2 != 0)
  
  set $ok *= ($3 == 15)
  set $ok *= ($4 == 40)
  set $ok *= ($5 == $2)
  set $ok *= ($6 == 2)
  
  set $ok *= ($7 == 15)
  set $ok *= ($8 == 40)
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
