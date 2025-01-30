!> Translation of dg compatible equilibrium data into efit format
!>
!> arg1: input equilibrium file name
!>
!> arg2: output equilibrium file name
!>
!> \version  02.09.99 14:19

      program dg2ef
c
c  version : 02.09.99 14:19
c
c=====================================================
c*** Translation of dg compatible equilibrium data into efit format
c=====================================================
      use eqdim
      implicit none
      integer i, j, ia, ja, idum(1)
      integer iret, nr, nz, ipestg
      real(kind=R8) :: fg(ngpr),pg(ngpr),ffg(ngpr),ppg(ngpr)
      real(kind=R8) :: pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real(kind=R8) :: rdim,zdim,rcntc,redge,zmsmid,rma,zma,
     ,  psimin,psilim,btorc
      real(kind=R8) :: u
      character title*40, date*9
      data title/'Conversion from the dg format'/
c=====================================================
c
c      call date2(date)
      call date_and_time(date)

      call open_files(' ')

      call rdeqdg(1,iret,nr,nz,btorc,rcntc,rgr,zgr,pfm)
      if(iret.ne.0) then
          print *,'==== dg2ef: error in rdeqdg. iret =',iret
          stop
      end if
c
      do i=1,nr
          fg(i)=btorc*rcntc
          pg(i)=1.
          ffg(i)=0.
          ppg(i)=0.
      end do
      ipestg=3
      psilim=0.
      psimin=1.e30
      do 100 j=nz,1,-1
          u=minval(pfm(1:nr,j))
          if(u.lt.psimin) then
              psimin=u
              ja=j
              idum=minloc(pfm(1:nr,j))
              ia=idum(1)
          end if
 100  continue
      redge=rgr(1)
      rdim=rgr(nr)-rgr(1)
      zdim=abs(zgr(nz)-zgr(1))
      zmsmid=0.5*(zgr(nz)+zgr(1))
      rma=float(ia)/(nr-1)*rdim+redge
      zma=(float(ja)-(nz+1)/2)/(nz-1)*zdim
c
      call wrefit(2,iret,title,date,ipestg,nr,nz,
     ,           rdim,zdim,zmsmid,rcntc,redge,rma,zma,psimin,psilim,
     ,           btorc,fg,pg,ffg,ppg,pfm)
      if(iret.ne.0) then
          print *,'==== dg2ef: error in wrefit. iret = ',iret
      end if
c
      end
