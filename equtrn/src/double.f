!> use splines to refine the equilibrium
      subroutine double(pfm,ngpr,ngpz,rgr,nr,zgr,nz)

      implicit none
      integer ir,ifail,nn,nz2,iz,nr2,nr,ngpz,ngpr,j,i,nz
      real*8 pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real   pfms(nr,nz),rgrs(ngpr),zgrs(ngpz)
      real   pfmss(ngpr,ngpz),rgrss(ngpr),zgrss(ngpz)
      real   cx(nr,3,nz),cy(nz,3,nr),cc(nz,3,nr,3),wrkz(nz),wrkr(nr,4)

c----------------------------------------------------------------------
c      character tab
c
c      tab=char(9)
c----------------------------------------------------------------------

      pfms(1:nr,1:nz)=pfm(1:nr,1:nz)
      rgrs(1:nr)=rgr(1:nr)
      zgrs(1:nz)=zgr(1:nz)

      if(ngpr.lt.2*nr-1) stop 'increase ngpr'
      if(ngpz.lt.2*nz-1) stop 'increase ngpz'

      nn=nr*nz
      call splbcb(rgrs, nr, zgrs, nz, pfms, nr, cx, nn*3, cy, nn*3,
     ,					      cc, nn*9, wrkz, nz, ifail)
      if(ifail.ne.0) then
	write(*,*) 'splbcb: ifail ',ifail
	stop
      endif

      do ir=1,nr
	rgr(2*ir-1)=rgrs(ir)
      enddo
      nr2=2*nr-1
      do ir=2,nr2,2
	rgr(ir)=0.5*(rgr(ir-1)+rgr(ir+1))
      enddo
      do iz=1,nz
	zgr(2*iz-1)=zgrs(iz)
      enddo
      nz2=2*nz-1
      do iz=2,nz2,2
	zgr(iz)=0.5*(zgr(iz-1)+zgr(iz+1))
      enddo

      rgrss(1:nr2)=rgr(1:nr2)
      zgrss(1:nz2)=zgr(1:nz2)

      call evlbcs(rgrss, nr2, zgrss, nz2, pfmss, ngpr,
     ,            rgrs, nr, zgrs, nz, pfms, nr, cx, nn*3, cy, nn*3,
     ,                            cc, nn*9, wrkr, 4*nr, wrkz, nz, ifail)
      if(ifail.ne.0) then
	write(*,*) 'evlbcs: ifail ',ifail
	if(ifail.lt.0) stop
      end if

      nr=nr2
      nz=nz2
      do i=1,nr !{
        do j=1,nz !{
          pfm(i,j)=pfmss(i,j)
        end do !}
      end do !}

      end
