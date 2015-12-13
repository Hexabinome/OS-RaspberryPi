# -*- mode: gdb-script -*-

set verbose off
set confirm off

break vmem_init
commands
  print $pc
  continue 
end

break kmain-vmem-alloc.c:13
commands
  p/x phy_addr
  p/x *phy_addr
  continue
end

# End condition
break kmain-vmem-alloc.c:15
commands
	p/x phy_addr
	p/x *phy_addr
	assert_results
end

define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == vmem_init+4)
  

  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
