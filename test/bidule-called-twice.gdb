# -*- mode: gdb-script -*-

set verbose off
set confirm off

break *kmain
commands
  print $pc
  continue
end

break *bidule
commands
  print $pc
  continue
end

break kmain-bidule.c:13
commands
  if $1 == &kmain && $2 == &bidule && $3 == &bidule
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
