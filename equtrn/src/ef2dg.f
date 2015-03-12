!> Translation of efit equilibrium data into dg compatible format
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> \version  16.11.95 20:04

      program ef2dg
c
c  version : 16.11.95 20:04
c
c=====================================================
c*** Translation of efit equilibrium data into dg compatible format
c=====================================================
      parameter (ngpr=513, ngpz=513)
      real*8 fg(ngpr),pg(ngpr),ffg(ngpr),ppg(ngpr)
      real*8 pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real*8 rdim,zdim,rcntc,redge,zmsmid,rma,zma,psimin,psilim,btorc
      character title*40, date*8
c=====================================================
c
      call open_files(' ')

      call rdefit(1,ngpr,ngpz,iret, title,date,ipestg,nr,nz,
     ,           rdim,zdim,zmsmid,rcntc,redge,rma,zma,psimin,psilim,
     ,           btorc,fg,pg,ffg,ppg,pfm,rgr,zgr)
      if(iret.ne.0) then
          print *,'==== ef2dg: error in rdefit. iret =',iret
          stop
      end if
c
      print *,'psilim = ',psilim
      call wreqdg(2,ngpr,ngpz,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== ef2dg: error in wreqdg. iret = ',iret
      end if
c
      end
