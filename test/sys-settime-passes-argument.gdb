# -*- mode: gdb-script -*-

set verbose off
set confirm off

break sys_settime
commands
  print/x date_ms
  continue 
end

break set_date_ms
commands
  print/x date_ms
  continue
end

break kmain-settime.c:10
commands
  if ($1 == 0x12345678CACACACA && $1 == $2)
    printf "test OK\n"
    quit
  else
    printf "test ERROR\n"
    quit
  end
end

target remote:1234
continue
