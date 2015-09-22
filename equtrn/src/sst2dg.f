!> Translation of efit equilibrium data into dg compatible format
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> This is a special variant of the standard efit-to-dg converter for
!> SST where some information is hard-coded!
!>
!> \version 16.11.95 20:04

      program sst2dg
c
c  version : 16.11.95 20:04
c
c=====================================================
c*** Translation of efit equilibrium data into dg compatible format
c=====================================================
#include "eqdim.inc"
      real*8 gpr(ngpr),gpz(ngpz)
      real*8 pfm(ngpr,ngpz)
      real*8 rcntc,btorc
      real*8 Bt,derivative(6)
c=====================================================
c
      call open_files(' ')

      ngr=65
      ngz=65
      zfac=-0.005
      do i=1,ngr
	do j=1,ngz
	  read(1,*) gpr(i),gpz(j),pfm(i,j)
	  pfm(i,j)=pfm(i,j)+gpz(j)*zfac
	enddo
      enddo
      psilim=0.0634-0.355983*zfac
      pfm(1:ngr,1:ngz)=pfm(1:ngr,1:ngz)-psilim
      btorc=3
      rcntc=1.1
c
      print *,'psilim = ',psilim

      call wreqdg(2,ngpr,ngpz,iret,ngr,ngz,psilim,
     1 btorc,rcntc,gpr,gpz,pfm)
      if(iret.ne.0) then
          print *,'==== ef2dg: error in wreqdg. iret = ',iret
      end if
c
      end
