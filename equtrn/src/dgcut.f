!> chop rows and columns from a dg equilibrium
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> This reads a requested rmin, rmax and zmin, zmax and removes points
!> outside this range
!>
!> \version  16.11.95 20:04

      program dgcut
c
c  version : 16.11.95 20:04
c
c=====================================================
c*** Translation of efit equilibrium data into dg compatible format
c=====================================================
      use eqdim
      implicit none
      real(kind=R8) :: pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real(kind=R8) :: rcntc,psilim,btorc,rmin,rmax,zmin,zmax
      integer nr,nz,ix1,ix2,iy1,iy2,iret,ir,iz
c=====================================================
c
c      character tab
c----------------------------------------------------------------------

c      tab=char(9)

      call open_files(' ')
 
      call rdeqdg(1,iret,nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
        print *,'==== dgcut: error in rdeqdg. iret =',iret
        stop
      end if

      rmin=rgr(1)
      rmax=rgr(nr)
      zmin=zgr(1)
      zmax=zgr(nz)
      write (*,*) 
      write (*,*) 'Source grid:'
      write (*,*) '   rmin,rmax=', rgr(1),rgr(nr)
      write (*,*) '   zmin,zmax=', zgr(1),zgr(nz)
      write (*,*) 'Input new values (list-directed format) =>'
      read (*,*,end=20) rmin,rmax,zmin,zmax

      if(rmin.lt.rgr(1) .or. rmin.gt.rgr(nr))
     *     stop 'requested rmin not between actual rmin and rmax'
      if(rmax.lt.rgr(1) .or. rmax.gt.rgr(nr))
     *     stop 'requested rmax not between actual rmin and rmax'
      if(zmin.lt.zgr(1) .or. zmin.gt.zgr(nz))
     *     stop 'requested zmin not between actual zmin and zmax'
      if(zmax.lt.zgr(1) .or. zmax.gt.zgr(nz))
     *     stop 'requested zmax not between actual zmin and zmax'

      do ir = 1, nr
         if(rmin.le.rgr(ir)) then
            ix1=ir
            exit
         endif
      enddo
      do ir = nr, 1, -1
         if(rmax.ge.rgr(ir)) then
            ix2=ir
            exit
         endif
      enddo   
      do iz = 1, nz
         if(zmin.le.zgr(iz)) then
            iy1=iz
            exit
         endif
      enddo
      do iz = nz, 1, -1
         if(zmax.ge.zgr(iz)) then
            iy2=iz
            exit
         endif
      enddo   
      write(*,*) rgr(1), rgr(ix1-1), rmin, rgr(ix1), 
     *     rgr(ix2), rmax, rgr(ix2+1), rgr(nr)
      write(*,*) zgr(1), zgr(iy1-1), zmin, zgr(iy1), 
     *     zgr(iy2), zmax, zgr(iy2+1), zgr(nz)

      write(*,*) 1, ix1, ix2, nr
      write(*,*) 1, iy1, iy2, nz

      if(ix1.ge.ix2) stop 'not enough radial points'
      if(iy1.ge.iy2) stop 'not enough vertical points'

c need to add something here to remove the first few columns
      nr=ix2-ix1+1
      nz=iy2-iy1+1
      rgr(1:nr)=rgr(ix1:ix2)
      zgr(1:nz)=zgr(iy1:iy2)
      pfm(1:nr,1:nz)=pfm(ix1:ix2,iy1:iy2)

      psilim=0

      call wreqdg(2,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
        print *,'==== dgcut: error in wreqdg. iret = ',iret
      end if

 20   continue

      end
