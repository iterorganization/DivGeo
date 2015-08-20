!> up-down symmetrize psi
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> \version  30.01.96 21:58

      program dg_symm_dg
c
c  version : 30.01.96 21:58
c
c=====================================================
c*** do arbitrary re-processing of the equilibrium
c*** in this case symmetrize the equilibrium up-down
c=====================================================
      parameter (ngpr=1025, ngpz=1025)
      real*8 pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real*8 rcntc,psilim,btorc,psisymm
      character*256 filename
      integer chop
c=====================================================
c
      call open_files(' ')

      call rdeqdg(1,ngpr,ngpz,iret,nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== dg_proc_dg: error in rdeqdg. iret =',iret
          stop
      end if
c
      do iz=1,nz/2
        do ir=1,nr
          psisymm = (pfm(ir,iz)+pfm(ir,nz-iz+1))/2.0
          pfm(ir,iz) = psisymm
          pfm(ir,nz-iz+1) = psisymm
        enddo
      enddo
      psilim=0.

      call wreqdg(2,ngpr,ngpz,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== dg_symm_dg: error in wreqdg. iret = ',iret
      end if
c
      end
