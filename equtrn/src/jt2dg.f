!> Translation of efit equilibrium data into dg compatible format
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> \version  09.07.97 17:14

      program jt2dg
c
c  version : 09.07.97 17:14
c
c=====================================================
c*** Translation of efit equilibrium data into dg compatible format
c=====================================================
      use eqdim
      implicit none
      integer i,j
      integer iret,nr,nz
      real(kind=R8) :: pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real(kind=R8) :: rcntc,psilim,btorc
c=====================================================
c
      call open_files(' ')

      call rdeqjt(1,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
        print *,'==== jt2dg: error in rdeqjt. iret =',iret
        stop
      end if
c
      do i=1,nr
        do j=1,nz
          pfm(i,j)=pfm(i,j)-psilim
        end do
      end do
      print *,'psilim = ',psilim
      call wreqdg(2,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
        print *,'==== jt2dg: error in wreqdg. iret = ',iret
      end if
c
      end
