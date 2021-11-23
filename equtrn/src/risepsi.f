!> Shift of the magnetic flux by a specified increment
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> shift is read in from standard input
!>
!> \version  30.01.96 21:58

      program risepsi
c
c  version : 30.01.96 21:58
c
c=====================================================
c*** Shift of the magnetic flux by a specified increment
c=====================================================
      use eqdim
      implicit none
      integer i,j
      integer iret,nr,nz
      real(kind=R8) :: pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real(kind=R8) :: rcntc,psilim,btorc,shift
c=====================================================
c
      call open_files('the increment is taken from the standard input')

      call rdeqdg(1,iret,nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== risepsi: error in rdeqdg. iret =',iret
          stop
      end if
c
      read(*,*,err=99) shift 
      do j=1,nz
          do i=1,nr
              pfm(i,j)=pfm(i,j)+shift
          end do
      end do
      psilim=0.
      call wreqdg(2,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== risepsi: error in wreqdg. iret = ',iret
      end if
      goto 100
c
 99   write(*,*) 'Unrecognized shift value!'
 100  continue
      end
