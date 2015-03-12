!> Translation of equilibrium data from P. Barabasci into dg compatible format
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> \version  02.09.99 19:37

      program pb2dg
c
c  version : 02.09.99 19:37
c
c======================================================================
c*** Translation of equilibrium data from P. Barabasci
c*** into dg compatible format
c======================================================================
      parameter (ngpr=257, ngpz=513)
      real*8 pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real*8 rcntc,psilim,btorc,pi
      parameter (pi=3.1415926535898)
c======================================================================
c
      call open_files(' ')

      call rdeqpb(1,ngpr,ngpz, iret,nr,nz,btorc,psilim,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== pb2dg: error in rdeqpb. iret =',iret
          stop
      end if

c*** Re-scale the psi values for DG
c      do i=1,nr
c        do j=1,nz
c          pfm(i,j)=pfm(i,j)/(2.*pi)
c        end do
c      end do
c      psilim=psilim/(2.*pi)


      rcntc=1.
c
      print *,'psilim = ',psilim
      call wreqdg(2,ngpr,ngpz,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== pb2dg: error in wreqdg. iret = ',iret
      end if
c======================================================================
      end
