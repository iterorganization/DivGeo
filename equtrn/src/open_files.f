!> utility routine for opening files
!>
!> argument 1 to the program (via getarg) is attached to file unit 1
!> argument 2 to the program (via getarg) is attached to file unit 2
      subroutine open_files(outstring)

      character*256 filename
      character*(*) outstring
      logical ex
#ifndef NAGFOR
      integer iargc
#else
      integer lenval, ierror
#endif

#ifndef NAGFOR
      if(iargc().lt.2) then
#else
      if(command_argument_count().lt.2) then
#endif
        write(*,*) '1st arg == input equilibrium file'
        write(*,*) '2nd arg == output equilibrium file'
        write(*,*) '3rd arg == btor data [optional]'
        if(trim(outstring).ne.trim(' '))
     &	 write(*,*) trim(outstring)
        stop
      endif

#ifndef NAGFOR
      call getarg(1,filename)
      open(1,file=filename)
      call getarg(2,filename)
      open(2,file=filename)
      if(iargc().gt.2) then
         call getarg(3,filename)
         if(filename.ne.'') then
            inquire(file=filename, exist=ex)
            if (ex) open(3,file=filename)
         endif
      endif
#else
      call get_command_argument(1,filename,lenval,ierror)
      open(1,file=filename)
      call get_command_argument(2,filename,lenval,ierror)
      open(2,file=filename)
      if(command_argument_count().gt.2) then
         call get_command_argument(3,filename,lenval,ierror)
         if(filename.ne.'') then
            inquire(file=filename, exist=ex)
            if (ex) open(3,file=filename)
         endif
      endif
#endif
      return
      end
