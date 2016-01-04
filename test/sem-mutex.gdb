# -*- mode: gdb-script -*-

set verbose off
set confirm off

break kmain-sem-mutex.c:11
commands
  print $pc
  continue
end
break kmain-sem-mutex.c:12
commands
  print $pc
  continue
end
break kmain-sem-mutex.c:13
commands
  print $pc
  continue
end
break kmain-sem-mutex.c:14
commands
  print $pc
  continue
end

break kmain-sem-mutex.c:22
commands
  print $pc
  continue
end
break kmain-sem-mutex.c:24
commands
  print $pc
  continue
end
break kmain-sem-mutex.c:25
commands
  print $pc
  continue
end

break kmain-sem-mutex.c:32
commands
  print $pc
  continue
end
break kmain-sem-mutex.c:34
commands
  print $pc
  continue
end

break kmain-sem-mutex.c:36
commands
  assert_results
end


define assert_results
  # integer used as boolean
  set $ok = 1 
  
  # test results
  set $ok *= ($1 == process1+4)
  set $ok *= ($2 == process1+12)
  set $ok *= ($3 == process2+4)
  set $ok *= ($4 == process3+4)
  set $ok *= ($5 == process1+16)
  set $ok *= ($6 == process1+20)
  set $ok *= ($7 == process2+12)
  set $ok *= ($8 == process2+16)
  set $ok *= ($9 == process3+12)
  
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
