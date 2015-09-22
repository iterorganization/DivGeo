!> Translation of efit equilibrium data into dg compatible format
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> This is a special variant of the standard efit-to-dg converter for
!> Culham.  A number of columns of the flux matrix are removed, and
!> the resolution is quadrupled in each direction using splines.
!>
!> \version  16.11.95 20:04

      program culham
c
c  version : 16.11.95 20:04
c
c=====================================================
c*** Translation of efit equilibrium data into dg compatible format
c=====================================================
#include "eqdim.inc"
      real*8 fg(ngpr),pg(ngpr),ffg(ngpr),ppg(ngpr)
      real*8 pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real*8 rdim,zdim,rcntc,redge,zmsmid,rma,zma,psimin,psilim,btorc
      character title*40, date*8
      integer chop
c=====================================================
c
c      character tab
c----------------------------------------------------------------------

c      tab=char(9)

      call open_files(' ')
 
      call rdefit(1,ngpr,ngpz,iret, title,date,ipestg,nr,nz,
     ,           rdim,zdim,zmsmid,rcntc,redge,rma,zma,psimin,psilim,
     ,           btorc,fg,pg,ffg,ppg,pfm,rgr,zgr)
      if(iret.ne.0) then
          print *,'==== ef2dg: error in rdefit. iret =',iret
          stop
      end if

c need to add something here to remove the first few columns
      chop=2
      rgr(1:nr-chop)=rgr(1+chop:nr)
      pfm(1:nr-chop,1:nz)=pfm(1+chop:nr,1:nz)
      nr=nr-chop
c of positive flux

      call double(pfm,ngpr,ngpz,rgr,nr,zgr,nz)
      call double(pfm,ngpr,ngpz,rgr,nr,zgr,nz)

c      psilim=0

      call wreqdg(2,ngpr,ngpz,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
	  print *,'==== dg2dg: error in wreqdg. iret = ',iret
      end if

      end
