# -*- mode: gdb-script -*-

set verbose off
set confirm off

break vmem_init
commands
  print $pc
  continue 
end

break process1
commands
  print $pc
  continue
end
break kmain-isolation.c:13
commands
  p/x momo1
  p/x a1
  continue
end

break process2
commands
  p $pc
  continue
end
break kmain-isolation.c:29
commands
  p/x momo2
  p/x a2
  continue
end

break kmain-isolation.c:18
commands
  p/x a1
  break data_handler
  commands
    p $pc
    continue
  end
  continue
end

break kmain-isolation.c:34
commands
  p/x a2
  assert_results
end


define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == vmem_init+4)
  set $ok *= ($2 == process1+8)
  # $3 = momo1 and $4 = a1
  set $ok *= ($5 == process2+8)
  # $6 = momo2 and $7 = a2
  set $ok *= ($3 == $6)
  set $ok *= ($4 != $7)
  
  set $ok *= ($8 == 0xfffffff0)
  set $ok *= ($9 == data_handler+8)
  set $ok *= ($10 == 0xfffffff0)
  
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
