      program dg_proc_dg
c
c  version : 30.01.96 21:58
c
c=====================================================
c*** do arbitrary re-processing of the equilibrium
c*** in this case job off the first few columns
c=====================================================
      parameter (ngpr=1025, ngpz=1025)
      real*8 pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real*8 rcntc,psilim,btorc,shift
      character*256 filename
      integer chop
c=====================================================
c
      if(iargc().ne.2) then
	write(*,*) '1st arg == input dg equilibrium file'
	write(*,*) '2nd arg == output dg equilibrium file'
	stop
      endif

      call getarg(1,filename)
      open(1,file=filename)
      call rdeqdg(1,ngpr,ngpz,iret,nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== dg_proc_dg: error in rdeqdg. iret =',iret
          stop
      end if
c
c need to add something here to remove the first few columns
      write(*,*) 'How many columns do you want to remove?'
      read(*,*) chop
      if(chop.lt.0.or.chop.gt.nr) then
         write(*,*) 'Valid inputs are between ',0,' and ',nr,' ; not ',chop
         stop 'Error!'
      endif
      rgr(1:nr-chop)=rgr(1+chop:nr)
      pfm(1:nr-chop,1:nz)=pfm(1+chop:nr,1:nz)
      nr=nr-chop
      psilim=0.
      call getarg(2,filename)
      open(2,file=filename)
      call wreqdg(2,ngpr,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== dg_proc_dg: error in wreqdg. iret = ',iret
      end if
c
      end
