      subroutine rdeqjt(lun,iret,nr,nz,psib,btf,rtf,rgr,zgr,pfm)
c=====================================================
c*** Read the equilibrium data written in the JET compatible format.
c***
c*** Input:
c***  lun     the logical unit number for the input
c***  ngpr    the maximum number of points in R direction
c***  ngpz    the maximum number of points in Z direction
c***
c*** Output:
c***  iret    return code (0 means OK)
c***  nr      the actual number of points in R direction
c***  nz      the actual number of points in Z direction
c***  psib    the psi value at the separatrix
c***  btf     the toroidal magnetic field at the R=rtf
c***  rgr     the R values for the grid points
c***  zgr     the Z values for the grid points
c***  pfm     the values of the (poloidal flux - separatrix flux)
c***
c*** If the input file contains no data on the toroidal field (old
c*** version), then the field file must be pre-connected to LUN 3
c=====================================================
c
c  version : 05.08.95 16:50
c
      use eqdim
      implicit none
      integer i, j, iret, lun, nr, nz
      real(kind=R8) rgr(*), zgr(*), pfm(ngpr, *)
c... toroidal field in tesla, radius in m
      real(kind=R8) btf, rtf, psib, upsib
      real(kind=R8) ubtf, urtf
c-----------------------------------------------------
c
c*** Read the plasma equilibrium ...
c
      iret=0
      call rdeqjh(lun,nr,nz,upsib,ubtf,urtf,iret)
      if (iret.eq.1) goto 99
      btf=ubtf
      rtf=urtf
      psib=upsib
      if(rtf.le.0.) then
c
c*** Read the toroidal field and the corresponding radius
c*** from a separate file (for compatibility with old versions)
c
          read(3,*,err=99)
          read(3,*,err=99) btf, rtf
          close(3)
      end if
      if(nr.gt.ngpr) then
          print *,'==== rdeqjt: nr > ngpr ',nr
          iret=2
      end if
      if(nz.gt.ngpz) then
          print *,'=== rdeqjt: nz > ngpz'
          iret=2
      end if
      if(nr.le.0) then
          print *,'=== rdeqjt: nr < 1'
          iret=4
      end if
      if(nz.le.0) then
          print *,'=== rdeqjt: nz < 1'
          iret=4
      end if
      if(iret.ne.0) return
c
      read(lun,8000) (rgr(i),i=1,nr)
      read(lun,*)
      read(lun,*)
      read(lun,8000) (zgr(i),i=1,nz)
      read(lun,*)
      read(lun,*)
      read(lun,8000) ((pfm(i,j),i=1,nr),j=1,nz)
 8000 format(5(3x,e14.8))
      iret=0
      return
c-----------------------------------------------------
c
 99   print *,'==== rdeqjt: error in the input files'
      iret=8
      return
c
      end
