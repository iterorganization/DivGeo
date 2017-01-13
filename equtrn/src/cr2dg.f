!> Translation of create equilibrium data into dg compatible format
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> \version  16.11.95 20:04

      program cr2dg
c
c  version : 16.11.95 20:04
c
c=====================================================
c*** Translation of create equilibrium data into dg compatible format
c=====================================================
#include "eqdim.inc"
      real*8 fg(ngpr),pg(ngpr),ffg(ngpr),ppg(ngpr)
      real*8 pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real*8 rdim,zdim,rcntc,redge,zmsmid,rma,zma,psimin,psilim,btorc
      real*8, parameter :: twopi = 
     *     2.0d0*3.141592653589793238462643383280d0
      character title*48
c=====================================================
c
      call open_files(' ')

      call rdcreate(1,ngpr,ngpz,iret, title,ipestg,nr,nz,
     ,           rdim,zdim,zmsmid,rcntc,redge,rma,zma,psimin,psilim,
     ,           btorc,fg,pg,ffg,ppg,pfm,rgr,zgr)
      if(iret.ne.0) then
          print *,'==== cr2dg: error in rdcreate. iret =',iret
          stop
      end if
c
      psilim = psilim / twopi
      pfm = pfm / twopi
      print *,'psilim = ',psilim
      call wreqdg(2,ngpr,ngpz,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== cr2dg: error in wreqdg. iret = ',iret
      end if
c
      end
