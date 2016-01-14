# -*- mode: gdb-script -*-

set verbose off
set confirm off

break process
commands
  p $pc
  continue
end
break kmain-fork-gmalloc.c:14
commands
  p momo
  continue
end

break kmain-fork-gmalloc.c:25
commands
  p momo[0]
  p momo[1]
  continue
end
break kmain-fork-gmalloc.c:26
commands
  p momo[1]
  continue
end


break kmain-fork-gmalloc.c:31
commands
  p momo[0]
  p momo[1]
  p momo
  continue
end
break kmain-fork-gmalloc.c:32
commands
  p momo[1]
  continue
end

b do_sys_exit
commands
  b sys_exit
  commands
    assert_results
  end
  continue
end


define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == process+8)
  set $ok *= ($2 != 0)
  
  # Child process prints can be 3 to 5 or 7 to 9
  set $ok *= ($3 == 15 || $7 == 15)
  set $ok *= ($4 == 40 || $8 == 40)
  set $ok *= ($5 == 5 || $9 == 5)
  
  # Parent process prints can be 3 to 6 or 6 to 9
  set $ok *= ($3 == 15 || $6 == 15)
  set $ok *= ($4 == 40 || $7 == 40)
  set $ok *= ($5 == $2 || $8 == $2)
  set $ok *= ($6 == 2 || $9 == 2)

  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
