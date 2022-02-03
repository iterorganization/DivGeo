      subroutine rdcreate(lun,iret,title,ipestg,nr,nz,
     ,           rdim,zdim,zmsmid,rcntc,redge,rma,zma,psimin,psilim,
     ,           btorc,fg,pg,ffg,ppg,pfm,rgr,zgr)
c=====================================================
c*** where:
c***
c*** i)  nr, nz, rdim, redge, and zdim define the rectangular mesh used
c***     to store the psi values via the functions rr & zz defined at
c***     the top of these codes,
c***
c*** ii) psimin is the flux value at the magnetic axis; psilim is the
c***     value at the separatrix,
c***
c*** iii) btorc is the toroidal magnetic field at a radius rcntc,
c***
c*** iv) the poloidal flux is pfm,
c***
c*** v) fg is the flux function R*Btor; ffg is its derivative with
c***    respect to psi,
c***
c*** vi) pg is the pressure & ppg is its derivative.
c=====================================================
c
c  version : 18.12.94 18:33
c
      use eqdim
      implicit none
      integer i, j
      integer lun,iret,nr,nz,ipestg
      real(kind=R8) :: fg(*),pg(*),ffg(*),ppg(*),pfm(ngpr,*),
     .  rgr(*),zgr(*)
      real(kind=R8) :: rdim,zdim,rcntc,redge,zmsmid,rma,zma,
     .  psimin,psilim,btorc
      character title*48
c=====================================================
c
      iret=0
      rewind lun
      read(lun,*) title,ipestg,nr,nz
      write(*,*) ipestg,nr,nz
      if(nr.gt.ngpr) then
          write (6,'(a,i4,a,i4,a)') 
     .     '=== rdcreate: nr (',nr,') > ngpr (',ngpr,')'
          iret=2
      end if
      if(nz.gt.ngpz) then
          write (6,'(a,i4,a,i4,a)') 
     .     '=== rdcreate: nz (',nz,') > ngpz (',ngpz,')'
          iret=2
      end if
      if(nr.le.0) then
          write (6,'(a,i4,a)') '=== rdcreate: nr (',nr,') < 1'
          iret=4
      end if
      if(nz.le.0) then
          write (6,'(a,i4,a)') '=== rdcreate: nz (',nz,') < 1'
          iret=4
      end if
      if(iret.ne.0) return
c
      read(lun,*) rdim,zdim,rcntc,redge,zmsmid
      read(lun,*) rma,zma,psimin,psilim,btorc
      read(lun,'()')
      read(lun,'()')
      read(lun,*) (fg(i),i=1,nr)
      read(lun,*) (pg(i),i=1,nr)
      read(lun,*) (ffg(i),i=1,nr)
      read(lun,*) (ppg(i),i=1,nr)
      read(lun,*) ((pfm(i,j),i=1,nr),j=1,nz)
      do i=1,nr
        rgr(i)=rr(real(i-1,R8))
      enddo
      do i=1,nz
        zgr(i)=zz(real(i,R8))
      enddo
      return
c
      contains

      function rr(r)
      implicit none
      real(kind=R8) :: rr
      real(kind=R8), intent(in) :: r

      rr = r/float(nr-1)*rdim+redge
      return
      end function rr

      function zz(z)
      implicit none
      real(kind=R8) :: zz
      real(kind=R8), intent(in) :: z

      zz = (z-float(nz+1)/float(2))/float(nz-1)*zdim+zmsmid
      return
      end function zz

      end subroutine rdcreate
