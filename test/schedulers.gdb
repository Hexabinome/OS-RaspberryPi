# -*- mode: gdb-script -*-

set verbose off
set confirm off

break process1
commands
  p $pc
  continue
end
break process2
commands
  p $pc
  continue
end
break process3
commands
  p $pc
  continue
end

break kmain-schedulers.c:29
commands
  p ret
  continue
end
break kmain-schedulers.c:40
commands
  p ret
  continue
end
break kmain-schedulers.c:51
commands
  p ret
  continue
end
break kmain-schedulers.c:62
commands
  p ret
  continue
end
break kmain-schedulers.c:72
commands
  p ret
  assert_results
end

define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == 0)
  set $ok *= ($2 == process1)
  set $ok *= ($3 == process2)
  set $ok *= ($4 == process3)
  
  set $ok *= ($5 == 0)
  set $ok *= ($6 == process1 || $6 == process2 || $6 == process3)
  set $ok *= ($7 == process1 || $7 == process2 || $7 == process3)
  set $ok *= ($8 == process1 || $8 == process2 || $8 == process3)
  set $ok *= ($6 != $7 && $7 != $8 && $8 != $6)
  
  set $ok *= ($9 == 0)
  set $ok *= ($10 == process1)
  set $ok *= ($11 == process3)
  set $ok *= ($12 == process2)
  
  set $ok *= ($13 == 0)
  set $ok *= ($14 == process1 || $14 == process2 || $14 == process3)
  set $ok *= ($15 == process1 || $15 == process2 || $15 == process3)
  set $ok *= ($16 == process1 || $16 == process2 || $16 == process3)
  set $ok *= ($14 != $15 && $15 != $16 && $16 != $14)
  
  set $ok *= ($17 == -1)

  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
