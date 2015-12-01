# -*- mode: gdb-script -*-

set verbose off
set confirm off

source utils.gdb

break kmain-yieldto.c:16
commands
  print /x$cpsr
  print_sr
  continue 
end

# "negative" is bit 31 of the status register
set $nflag=(1<<31)

break kmain-yieldto.c:23
commands
  # artificially set the 'N' flag in process 2
  set $cpsr=$cpsr | $nflag
  continue 
end


set $iterations=0

break kmain-yieldto.c:26
commands
  print /x$cpsr
  print_sr
  
  set $iterations++
  if $iterations==5
    assess_execution
  end
  continue 
end

define assess_execution
  # integer used as boolean
  set $ok = 1 
  # multiplication used as logical AND
  set $ok *= (($1 & $nflag) == 0) 
  set $ok *= (($2 & $nflag) == $nflag) 
  set $ok *= (($3 & $nflag) == 0)      
  set $ok *= (($4 & $nflag) == $nflag) 
  set $ok *= (($5 & $nflag) == 0)      
  set $ok *= (($6 & $nflag) == $nflag) 
  set $ok *= (($7 & $nflag) == 0)      
  set $ok *= (($8 & $nflag) == $nflag) 
  set $ok *= (($9 & $nflag) == 0)      

  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
