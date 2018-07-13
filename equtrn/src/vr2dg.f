!> Translation of TdeV equilibrium data into dg compatible format
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> \version 16.11.95 20:04

      program vr2dg
c
c  version : 16.11.95 20:04
c
c=====================================================
c*** Translation of TdeV equilibrium data into dg compatible format
c***
c=====================================================
      implicit none
#include "eqdim.inc"
      integer iret, nr, nz
      real(kind=R8) :: pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real(kind=R8) :: rcntc,psilim,btorc
c=====================================================
c
      call open_files(' ')

      call rdeqvr(1,iret,nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== vr2dg: error in rdeqvr. iret =',iret
          stop
      end if
      write(*,*) 'rdeqvr ok'
c
      psilim=0.
      print *,'psilim = ',psilim
      call wreqdg(2,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== vr2dg: error in wreqdg. iret = ',iret
      end if
      write(*,*) 'wreqdg ok'
c
      end
