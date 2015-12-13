# -*- mode: gdb-script -*-

set verbose off
set confirm off

break vmem_init
commands
  print $pc
  continue 
end

break kmain-pagination.c:12
commands
  p/x res
  continue
end

break kmain-pagination.c:13
commands
  p/x res
  continue
end
break kmain-pagination.c:14
commands
  p/x res
  continue
end
break kmain-pagination.c:15
commands
  p/x res
  continue
end

break kmain-pagination.c:17
commands
  p/x res
  continue
end

break kmain-pagination.c:18
commands
  p/x res
  continue
end
break kmain-pagination.c:19
commands
  p/x res
  continue
end
break kmain-pagination.c:20
commands
  p/x res
  continue
end


break kmain-pagination.c:22
commands
	p/x res
	assert_results
end

define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == vmem_init+4)
  set $ok *= ($2 == 0x0)
  set $ok *= ($3 == 0x1)
  set $ok *= ($4 == 0x20000000)
  set $ok *= ($5 == 0x20123456)
  set $ok *= ($6 == 0x20FFFFFF)
  set $ok *= ($7 == 0xfffffff1)
  set $ok *= ($8 == 0xfffffff1)
  set $ok *= ($9 == 0xfffffff1)
  set $ok *= ($10 == 0xfffffff1)

  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
