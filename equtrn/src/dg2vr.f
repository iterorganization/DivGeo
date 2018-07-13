!> Translation of TdeV equilibrium data into dg compatible format
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> \version  30.01.96 21:59

      program dg2vr
c
c  version : 30.01.96 21:59
c
c=====================================================
c*** Translation of TdeV equilibrium data into dg compatible format
c=====================================================
#include "eqdim.inc"
      real(kind=R8) :: pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real(kind=R8) :: rcntc,psilim,btorc
c=====================================================
c
      call open_files(' ')

      call rdeqdg(1,iret,nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== dg2vr: error in rdeqdg. iret =',iret
          stop
      end if
c
      psilim=0.
      print *,'psilim = ',psilim
      call wreqvr(2,ngpr,iret,nr,nz,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== dg2vr: error in wreqvr. iret = ',iret
      end if
c
      end
