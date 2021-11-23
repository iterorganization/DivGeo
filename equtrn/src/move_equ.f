!> Shift of the equilibrium up-down or left-right by a specified increment
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> shift is read in from standard input
!>
!> \version  18.09.15 13:13	Stefano Carli

      program move_equ
c
c  version : 30.01.96 21:58
c
c=====================================================
c*** Shift of the magnetic flux by a specified increment
c=====================================================
      use eqdim
      implicit none
      integer iret,nr,nz
      real(kind=R8) :: pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real(kind=R8) :: rcntc,psilim,btorc,shiftr,shiftz
c=====================================================
c
      call open_files('the increment is taken from the standard input')

      call rdeqdg(1,iret,nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== move_equ: error in rdeqdg. iret =',iret
          stop
      end if
c
      print *,'desired shift along r in metres ='
      read *,shiftr
      print *,'desired shift along z in metres ='
      read *,shiftz
      rgr(1:nr)=rgr(1:nr)+shiftr
      zgr(1:nz)=zgr(1:nz)+shiftz
      psilim=0.
      call wreqdg(2,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== move_equ: error in wreqdg. iret = ',iret
      end if
c
      end
