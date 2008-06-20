!> utility routine for opening files
!>
!> argument 1 to the program (via getarg) is attached to file unit 1
!> argument 2 to the program (via getarg) is attached to file unit 2
      subroutine open_files(outstring)

      integer iargc
      character*256 filename
      character*(*) outstring

      if(iargc().ne.2) then
        write(*,*) '1st arg == input equilibrium file'
        write(*,*) '2nd arg == output equilibrium file'
	if(trim(outstring).ne.trim(' ')) 
     &	 write(*,*) trim(outstring)
	stop
      endif

      call getarg(1,filename)
      open(1,file=filename)
      call getarg(2,filename)
      open(2,file=filename)

      return
      end
