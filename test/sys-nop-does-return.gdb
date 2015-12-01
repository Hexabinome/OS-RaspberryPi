# -*- mode: gdb-script -*-

set verbose off
set confirm off

break *kmain
commands
  print $pc
  continue
end

break *sys_nop
commands
  print $pc
  continue
end

break *swi_handler
commands
  print $pc
  continue
end

break *do_sys_nop
commands
  print $pc
  continue
end

# after sys_nop returns
break kmain-nop-reboot.c:9
commands
  print 123456
  assess_execution
end

define assess_execution
  # integer used as boolean
  set $ok = 1 
  # multiplication used as logical AND
  set $ok *= ($1 == kmain)
  set $ok *= ($2 == sys_nop)
  set $ok *= ($3 == swi_handler)
  set $ok *= ($4 == do_sys_nop)
  set $ok *= ($5 == 123456)
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end


target remote:1234
continue
