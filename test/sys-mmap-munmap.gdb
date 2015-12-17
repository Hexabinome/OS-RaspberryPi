# -*- mode: gdb-script -*-

set verbose off
set confirm off

break vmem_init
commands
  print $pc
  continue 
end

break kmain-mmap-munmap.c:12
commands
  p/x tab
  continue
end
break kmain-mmap-munmap.c:14
commands
  p/x phy_addr
  continue
end
break kmain-mmap-munmap.c:18
commands
  p tab[0]
  p tab[1]
  p tab[4]
  p tab[9]
  continue
end
break kmain-mmap-munmap.c:22
commands
  p/x phy_addr
  continue
end


break kmain-mmap-munmap.c:26
commands
  p/x tab
  continue
end
break kmain-mmap-munmap.c:30
commands
  p tab[0]
  p tab[1]
  p tab[4]
  p tab[9]
  continue
end



break kmain-mmap-munmap.c:32
commands
  assert_results
end

define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == vmem_init+4)
  set $ok *= ($2 == 0x1000000)
  set $ok *= ($3 == 0x1000000)
  set $ok *= ($4 == 1)
  set $ok *= ($5 == 0)
  set $ok *= ($6 == 4)
  set $ok *= ($7 == 9)
  set $ok *= ($8 == 0xfffffff1)
  
  set $ok *= ($9 == 0x1000000)
  set $ok *= ($10 == 1)
  set $ok *= ($11 == 0)
  set $ok *= ($12 == 4)
  set $ok *= ($13 == 9)

  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
