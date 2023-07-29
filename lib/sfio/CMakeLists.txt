add_library(sfio STATIC
  # Header files
  sfhdr.h
  sfio.h
  sfio_t.h

  # Source files
  sfclose.c
  sfcvt.c
  sfdisc.c
  sfexcept.c
  sfextern.c
  sffilbuf.c
  sfflsbuf.c
  sfmode.c
  sfnew.c
  sfnputc.c
  sfopen.c
  sfpkrd.c
  sfraise.c
  sfrd.c
  sfread.c
  sfscanf.c
  sfsetbuf.c
  sfsk.c
  sfstack.c
  sfswap.c
  sfsync.c
  sftable.c
  sfungetc.c
  sfprint.c
  sfvscanf.c
  sfwr.c
  sfwrite.c
  Sfio_f/_sfgetc.c
  Sfio_f/_sfslen.c
)

target_include_directories(sfio PRIVATE
  ..
)

if(WIN32 AND NOT MINGW)
  target_include_directories(sfio PRIVATE
    ../../windows/include/unistd
  )
endif()
