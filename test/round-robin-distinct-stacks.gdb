# -*- mode: gdb-script -*-

set verbose off
set confirm off

set $started=0

break user_process
commands
  set $started++
  print &v
  continue 
end

break sys_yield
commands
  if $started==5
      assess_execution
  end
  continue 
end

define assess_execution
  # integer used as boolean, multiplication used as logical AND
  set $ok = 1 
  # check that no two stacks share the same location
  set $ok *= ($1!=$2 && $1!=$3 && $1!=$4 && $1!=$5 ) 
  set $ok *= ($2!=$1 && $2!=$3 && $2!=$4 && $2!=$5 ) 
  set $ok *= ($3!=$1 && $3!=$2 && $3!=$4 && $3!=$5 ) 
  set $ok *= ($4!=$1 && $4!=$2 && $4!=$3 && $4!=$5 ) 
  set $ok *= ($5!=$1 && $5!=$2 && $5!=$3 && $5!=$4 ) 
  
  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
