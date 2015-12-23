# -*- mode: gdb-script -*-

set verbose off
set confirm off

break vmem_init
commands
  print $pc
  continue
end



break kmain-gmalloc-gfree.c:13
commands
  p/x test1
  p/x test2
  continue
end
break kmain-gmalloc-gfree.c:26
commands
  p/x a
  p/x aa
  continue
end

break kmain-gmalloc-gfree.c:27
commands
	assert_results
end


define assert_results
  set $ok = 1

  set $ok *= ($1 == $2)

  set $ok *= ($3 == $4)

 if $ok
    printf "test OK\n"
  else
    printf "test ERROR\n"
  end
  quit
end

target remote:1234
continue
