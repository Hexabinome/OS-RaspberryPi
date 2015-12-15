# -*- mode: gdb-script -*-

set verbose off
set confirm off

break vmem_init
commands
  print $pc
  continue 
end


break kmain-vmem-alloc-free.c:9
commands
  break kAlloc_aligned
  commands
	print $pc
	continue
  end
  continue
end
break kmain-vmem-alloc-free.c:12
commands
  p/x log_addr1
  continue
end
break kmain-vmem-alloc-free.c:13
commands
  p/x phy_addr
  continue
end
break kmain-vmem-alloc-free.c:14
commands
  p/x phy_addr
  continue
end
break kmain-vmem-alloc-free.c:15
commands
  p/x phy_addr
  continue
end
break kmain-vmem-alloc-free.c:16
commands
  p/x phy_addr
  continue
end


break kmain-vmem-alloc-free.c:19
commands
  p/x log_addr2
  continue
end
break kmain-vmem-alloc-free.c:20
commands
  p/x phy_addr
  continue
end
break kmain-vmem-alloc-free.c:21
commands
  p/x phy_addr
  continue
end
break kmain-vmem-alloc-free.c:22
commands
  p/x phy_addr
  continue
end
break kmain-vmem-alloc-free.c:23
commands
  p/x phy_addr
  continue
end
break kmain-vmem-alloc-free.c:24
commands
  p/x phy_addr
  continue
end


break kmain-vmem-alloc-free.c:27
commands
  p/x log_addr3
  continue
end
break kmain-vmem-alloc-free.c:28
commands
  p/x phy_addr
  continue
end
break kmain-vmem-alloc-free.c:29
commands
  p/x phy_addr
  continue
end
break kmain-vmem-alloc-free.c:33
commands
  p/x phy_addr
  continue
end


break kmain-vmem-alloc-free.c:34
commands
	assert_results
end


define assert_results
  set $ok = 1
  
  set $ok *= ($1 == vmem_init+4)
  
  set $ok *= ($2 == kAlloc_aligned + 16)
  set $ok *= ($3 == 0x1000000)
  set $ok *= ($4 == 0x1001000)
  set $ok *= ($5 == 0x1001001)
  set $ok *= ($6 == 0xfffffff0)
  set $ok *= ($7 == 0xfffffff1)
  
  set $ok *= ($8 == 0x1001000)
  set $ok *= ($9 == 0x1002000)
  set $ok *= ($10 == 0x1003000)
  set $ok *= ($11 == 0x1004000)
  set $ok *= ($12 == 0xfffffff0)
  set $ok *= ($13 == 0xfffffff1)
  
  set $ok *= ($14 == kAlloc_aligned + 16)
  set $ok *= ($15 == 0x1004000)
  set $ok *= ($16 == 0x1005000)
  set $ok *= ($17 == 0x1006000)
  set $ok *= ($18 == 0x1101000)

  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
