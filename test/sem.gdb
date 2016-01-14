# -*- mode: gdb-script -*-

set verbose off
set confirm off

break sem_down
commands
  p $pc
  continue
end
break sem_up
commands
  p $pc
  continue
end

break kmain-sem.c:process1
commands
  p $pc
  continue
end
break kmain-sem.c:11
commands
  p $pc
  continue
end

break kmain-sem.c:process2
commands
  p $pc
  continue
end
break kmain-sem.c:19
commands
  p $pc
  continue
end

break kmain-sem.c:process3
commands
  p $pc
  continue
end
break kmain-sem.c:27
commands
  p $pc
  continue
end

break kmain-sem.c:process4
commands
  p $pc
  continue
end
break kmain-sem.c:34
commands
  p $pc
  continue
end

break kmain-sem.c:54
commands
  p $pc
  assert_results
end

define assert_results
  # integer used as boolean
  set $ok = 1 
  
  # test results
  set $ok *= ($1 == process1+4)
  set $ok *= ($2 == sem_down+12)
  set $ok *= ($3 == process2+4)
  set $ok *= ($4 == sem_down+12)
  set $ok *= ($5 == process3+4)
  set $ok *= ($6 == sem_down+12)
  set $ok *= ($7 == process4+4)
  
  set $ok *= ($8 == process1+32)
  set $ok *= ($9 == sem_up+8)
  set $ok *= ($10 == process3+32)
  set $ok *= ($11 == sem_up+8)
  set $ok *= ($12 == process2+32)
  set $ok *= ($13 == sem_up+8)
  set $ok *= ($14 == process4+24)
  set $ok *= ($15 == sem_up+8)
  
  set $ok *= ($16 == kmain+72)
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
