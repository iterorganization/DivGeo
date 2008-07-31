!> Refine the equilibrium a factor of 2 in both directions using splines
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> \version  16.01.2001 16:55

      program dg2dg
c
c  version : 16.01.2001 16:55
c
      implicit none
      integer ngpr,ngpz
      parameter (ngpr=1025, ngpz=1025)
      real*8 pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real*8 rcntc,psilim,btorc
      integer nr,nz,iret
c      character tab
c----------------------------------------------------------------------

c      tab=char(9)

      call open_files(' ')

      call rdeqdg(1,ngpr,ngpz,iret, nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
	  print *,'==== dg2dg: error in rdeqdg. iret =',iret
	  stop
      end if

      call double(pfm,ngpr,ngpz,rgr,nr,zgr,nz)

      psilim=0

      call wreqdg(2,ngpr,ngpz,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
	  print *,'==== dg2dg: error in wreqdg. iret = ',iret
      end if

      end
