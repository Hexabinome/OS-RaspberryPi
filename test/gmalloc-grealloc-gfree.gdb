# -*- mode: gdb-script -*-

set verbose off
set confirm off

break kmain-gmalloc-grealloc-gfree.c:19
commands
  p momo
  continue
end

break kmain-gmalloc-grealloc-gfree.c:24
commands
  p momo
  p *momo
  p *(momo+4)
  continue
end

break kmain-gmalloc-grealloc-gfree.c:31
commands
  p jojo
  continue
end

break gfree
commands
  p $pc
  continue
end

break kmain-gmalloc-grealloc-gfree.c:33
commands
  p jojo2
  p *(jojo2+4)
  assert_results
end

define assert_results
  set $ok = 1
  set $ok *= ($1 == $2)
  set $ok *= ($3 == 45)
  set $ok *= ($4 == 33)
  
  set $ok *= ($5 == gfree+8)
  set $ok *= ($1 != $6)
  
  set $ok *= ($6 != $7)
  set $ok *= ($8 == 123)

 if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
