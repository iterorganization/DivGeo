!> up-down symmetrize psi
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> arg3: (optional) btor data
!>
!> arg4: (optional) mixing parameter for mixing in a fraction of the old with the symmetrized psi
!>
!> \version  30.01.96 21:58

      program dg_symm_dg
c
c  version : 30.01.96 21:58
c
c=====================================================
c*** do arbitrary re-processing of the equilibrium
c*** in this case symmetrize the equilibrium up-down
c=====================================================
      use eqdim
      implicit none
      integer iret,nr,nz,ir,iz
      real(kind=R8) :: pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real(kind=R8) :: rcntc,psilim,btorc,psisymm
      real(kind=R8) :: alpha=0.0_R8
      character*256 arg
      logical used
#ifndef NAGFOR
      integer iargc
#else
      integer lenval, ierror
#endif
c=====================================================
c
      call open_files(' ')
#ifndef NAGFOR
      if(iargc().ge.4) then
        call getarg(4, arg)
        read(arg,*) alpha
      else if(iargc().eq.3) then
        inquire(unit=3,opened=used)
        if(.not.used) then
          call getarg(3, arg)
          read(arg,*) alpha
        end if
      endif
#else
      if(command_argument_count().ge.4) then
        call get_command_argument(4, arg, lenval, ierror)
        read(arg,*) alpha
      else if(command_argument_count().eq.3) then
        inquire(unit=3,opened=used)
        if(.not.used) then
          call get_command_argument(3, arg, lenval, ierror)
          read(arg,*) alpha
        endif
      endif
#endif
      write(*,*) 'Using ', alpha, ' in the symmetrization'

      call rdeqdg(1,iret,nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== dg_proc_dg: error in rdeqdg. iret =',iret
          stop
      end if
c
      do iz=1,nz/2
        do ir=1,nr
          psisymm = (pfm(ir,iz)+pfm(ir,nz-iz+1))/2.0
          pfm(ir,iz) = (1-alpha)*psisymm + alpha*pfm(ir,iz)
          pfm(ir,nz-iz+1) = (1-alpha)*psisymm + alpha*pfm(ir,nz-iz+1)
        enddo
      enddo
      psilim=0.

      call wreqdg(2,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== dg_symm_dg: error in wreqdg. iret = ',iret
      end if
c
      end
