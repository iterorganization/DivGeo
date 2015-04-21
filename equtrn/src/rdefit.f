      subroutine rdefit(lun,ngpr,ngpz,iret, title,date,ipestg,nr,nz,
     ,           rdim,zdim,zmsmid,rcntc,redge,rma,zma,psimin,psilim,
     ,           btorc,fg,pg,ffg,ppg,pfm,rgr,zgr)
c=====================================================
c*** where:
c***
c*** i)  nr, nz, rdim, redge, and zdim define the rectangular mesh used
c***     to store the psi values via the functions rr & zz defined at
c***     the top of these code,
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
      real*8 fg(*),pg(*),ffg(*),ppg(*),pfm(ngpr,*),rgr(*),zgr(*)
      real*8 rdim,zdim,rcntc,redge,zmsmid,rma,zma,psimin,psilim,btorc
      integer l
      character title*40, date*8, cvect(80)*1
c=====================================================
      rr(r)=r/float(nr-1)*rdim+redge
      zz(z)=(z-float(nz+1)/float(2))/float(nz-1)*zdim+zmsmid
c=====================================================
c
      iret=0
      rewind lun
      read(lun,'(a40,a8,3i4)') title,date,ipestg,nr,nz
      write(*,*) ipestg,nr,nz
      if(nr.gt.ngpr) then
          write (6,'(a,i4,a,i4,a)') 
     .     '=== rdefit: nr (',nr,') > ngpr (',ngpr,')'
          iret=2
      end if
      if(nz.gt.ngpz) then
          write (6,'(a,i4,a,i4,a)') 
     .     '=== rdefit: nz (',nz,') > ngpz (',ngpz,')'
          iret=2
      end if
      if(nr.le.0) then
          write (6,'(a,i4,a)') '=== rdefit: nr (',nr,') < 1'
          iret=4
      end if
      if(nz.le.0) then
          write (6,'(a,i4,a)') '=== rdefit: nz (',nz,') < 1'
          iret=4
      end if
      if(iret.ne.0) return
c
      read(lun,'(80a1)',advance='no',size=l) cvect
      backspace(lun)
      if (l.eq.80) then
        read(lun,'(5e16.9)') rdim,zdim,rcntc,redge,zmsmid
      else
        read(lun,*) rdim,zdim,rcntc,redge,zmsmid
      endif
      read(lun,'(80a1)',advance='no',size=l) cvect
      backspace(lun)
      if (l.eq.80) then
        read(lun,'(5e16.9)') rma,zma,psimin,psilim,btorc
      else
        read(lun,*) rma,zma,psimin,psilim,btorc
      endif
      read(lun,'()')
      read(lun,'()')
      read(lun,'(5e16.9)') (fg(i),i=1,nr)
      read(lun,'(5e16.9)') (pg(i),i=1,nr)
      read(lun,'(5e16.9)') (ffg(i),i=1,nr)
      read(lun,'(5e16.9)') (ppg(i),i=1,nr)
      read(lun,'(5e16.9)') ((pfm(i,j),i=1,nr),j=1,nz)
      do i=1,nr
        rgr(i)=rr(float(i-1))
      enddo
      do i=1,nz
        zgr(i)=zz(float(i))
      enddo
c
      end
