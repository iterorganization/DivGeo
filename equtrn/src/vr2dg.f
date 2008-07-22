!> Translation of TdeV equilibrium data into dg-compatible format
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
c*** Translation of TdeV equilibrium data into dg-compatible format
c***
c=====================================================
      parameter (ngpr=1025, ngpz=1025)
      real*8 pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real*8 rcntc,psilim,btorc
c=====================================================
c
      call open_files('')

      call rdeqvr(1,ngpr,ngpz,iret,nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== vr2dg: error in rdeqvr. iret =',iret
          stop
      end if
      write(*,*) 'rdeqvr ok'
c
      psilim=0.
      print *,'psilim = ',psilim
      call wreqdg(2,ngpr,ngpz,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== vr2dg: error in wreqdg. iret = ',iret
      end if
      write(*,*) 'wreqdg ok'
c
      end
