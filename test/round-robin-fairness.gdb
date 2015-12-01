# -*- mode: gdb-script -*-

set verbose off
set confirm off


# to avoid the "History has not yet reached $1" error
set $started=0

break user_process
commands
  set $started=1
  print &v
  continue 
end

break kmain-yield.c:12
commands
  # this is just cosmetic
  printf "v=%d\n", v
  continue 
end

break sys_yield
commands
  if $started
    if *$1 >= 10
      assess_execution
    end
  end
  continue 
end

define assess_execution
  
  printf "process 1: v=%d\n", *$1
  printf "process 2: v=%d\n", *$2
  printf "process 3: v=%d\n", *$3
  printf "process 4: v=%d\n", *$4
  printf "process 5: v=%d\n", *$5

  # integer used as boolean
  set $ok = 1 
  # multiplication used as logical AND
  set $ok *= (*$1 >=9 && *$1 <=11 ) 
  set $ok *= (*$2 >=9 && *$2 <=11 ) 
  set $ok *= (*$3 >=9 && *$3 <=11 ) 
  set $ok *= (*$4 >=9 && *$4 <=11 ) 
  set $ok *= (*$5 >=9 && *$5 <=11 ) 
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
