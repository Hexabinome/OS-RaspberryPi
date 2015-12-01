# -*- mode: gdb-script -*-

set verbose off
set confirm off

break *swi_handler
commands
  print /x $sp
  continue 
end

break do_sys_reboot
commands
  if ($1 == $2)
    printf "test OK\n"
    quit
  else
    printf "test ERROR\n"
    quit
  end
end

target remote:1234
continue
