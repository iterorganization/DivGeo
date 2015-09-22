!> Change the sign of the flux
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> \version  30.01.96 21:58

      program dg_minuspsi_dg
c
c  version : 30.01.96 21:58
c
c=====================================================
c*** Change the sign of the flux
c=====================================================
#include "eqdim.inc"
      real*8 pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real*8 rcntc,psilim,btorc,shift
      character*256 filename
c=====================================================
c
      call open_files(' ')

      call rdeqdg(1,ngpr,ngpz,iret,nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== dg_minuspsi_dg: error in rdeqdg. iret =',iret
          stop
      end if
c
      do j=1,nz
          do i=1,nr
              pfm(i,j)=-pfm(i,j)
          end do
      end do
      psilim=0.

      call wreqdg(2,ngpr,ngpz,iret,nr,nz,-psilim,btorc,rcntc,
     .            rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== dg_minuspsi_dg: error in wreqdg. iret = ',iret
      end if
c
      end
