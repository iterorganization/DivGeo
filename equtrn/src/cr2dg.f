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
      implicit none
#include "eqdim.inc"
      integer iret, nr, nz, ipestg
      real(kind=R8) :: fg(ngpr),pg(ngpr),ffg(ngpr),ppg(ngpr)
      real(kind=R8) :: pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real(kind=R8) :: rdim,zdim,rcntc,redge,zmsmid,rma,zma,
     &  psimin,psilim,btorc
      real(kind=R8), parameter :: twopi =
     *     2.0_R8*3.141592653589793238462643383280_R8
      character title*48
c=====================================================
c
      call open_files(' ')

      call rdcreate(1,iret,title,ipestg,nr,nz,
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
      call wreqdg(2,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== cr2dg: error in wreqdg. iret = ',iret
      end if
c
      end
