# -*- mode: gdb-script -*-

set verbose off
set confirm off

break *sys_gettime
commands
  print $pc
  continue 
end

break *get_date_ms
commands
  print $pc
  continue
end

break kmain-gettime.c:12
commands
  # integer used as boolean
  set $ok = 1 
  # multiplication used as logical AND
  set $ok *= ($1 == sys_gettime)
  set $ok *= ($2 == get_date_ms)
  set $ok *= ( time == 287762808899)

  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
