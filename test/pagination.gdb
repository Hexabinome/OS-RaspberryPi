# -*- mode: gdb-script -*-

set verbose off
set confirm off

break vmem_init
commands
  print $pc
  continue 
end

break vmem_translate
commands
  finish
end


define assert_results
  # integer used as boolean
  set $ok = 1 
  # test results
  set $ok *= ($1 == vmem_init)
  set $ok *= ($2  == 0x0)
  set $ok *= ($3  == 0x1)
  set $ok *= ($4  == 0x20000000)
  set $ok *= ($5  == 0x19999999)
  set $ok *= ($6  == 0x21000000)

  if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
