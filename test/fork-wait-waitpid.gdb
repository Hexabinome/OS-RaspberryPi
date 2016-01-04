# -*- mode: gdb-script -*-

set verbose off
set confirm off

break process
commands
  p $pc
  continue
end

break kmain-fork-wait-waitpid.c:25
commands
  p $pc
  continue
end
break kmain-fork-wait-waitpid.c:32
commands
  p status
  continue
end

break kmain-fork-wait-waitpid.c:38
commands
  p $pc
  continue
end
break kmain-fork-wait-waitpid.c:45
commands
  p status
  continue
end

break kmain-fork-wait-waitpid.c:51
commands
  p $pc
  continue
end
break kmain-fork-wait-waitpid.c:60
commands
  p $pc
  continue
end
break kmain-fork-wait-waitpid.c:66
commands
  p status
  continue
end
break kmain-fork-wait-waitpid.c:67
commands
  p status
  assert_results
end

define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == process+8)
  
  set $ok *= ($2 == process+28)
  set $ok *= ($3 == 3)
  
  set $ok *= ($4 == process+84)
  set $ok *= ($5 == 13)
  
  set $ok *= ($6 == process+136 || $6 == process+172)
  set $ok *= ($7 == process+172 || $7 == process+136)
  set $ok *= ($6 != $7)
  set $ok *= ($8 == 20 || $8 == 21)
  set $ok *= ($9 == 21 || $9 == 20)
  set $ok *= ($8 != $9)
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
