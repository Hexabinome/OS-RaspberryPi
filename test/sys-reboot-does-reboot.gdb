# -*- mode: gdb-script -*-

set verbose off
set confirm off

break *kmain
commands
  print $pc
  continue
end

break *sys_reboot
commands
  print $pc
  continue
end

break *swi_handler
commands
  print $pc
  continue
end

break *do_sys_reboot
commands
  print $pc
  continue
end

set $boot_count=0

break *reset_asm_handler
commands
  print $pc

  set $boot_count += 1
  
  if $boot_count == 1
    # this is our first boot, we proceed to running the program
    continue
  end
  
  # this is our second boot, time to check if we did well
  assess_execution
  continue
end

define assess_execution
  # integer used as boolean
  set $ok = 1 
  # multiplication used as logical AND
  set $ok *= ($1 == reset_asm_handler)
  set $ok *= ($2 == kmain)
  set $ok *= ($3 == sys_reboot)
  set $ok *= ($4 == swi_handler)
  set $ok *= ($5 == do_sys_reboot)
  set $ok *= ($6 == reset_asm_handler)
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end


target remote:1234
continue
