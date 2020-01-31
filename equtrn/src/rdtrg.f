      subroutine rdtrg(lun,iret,nunits,npts,rwall,zwall)
c=====================================================
c*** Read the template data in the dg compatible format.
c***
c*** Input:
c***  lun     the logical unit number for the output
c***
c*** Output:
c***  iret    return code (0 means OK)
c***  nunits  the number of structure units
c***  npts    the number of points in each structure
c***  rwall   the R coordinates of the structure points
c***  zwall   the Z coordinates of the structure points
c=====================================================
c
c  version : 23.06.97 17:23
c
      implicit none
#include "eqdim.inc"
      integer, intent(in) :: lun
      integer, intent(inout) :: iret
      integer, intent(out) :: nunits
      integer, intent(out) :: npts(ngpr)
      real(kind=R8), intent(out) :: rwall(ngpr), zwall(ngpr)
      real(kind=R8) :: pp1(2,ngpr), pp2(2,ngpr)
      real(kind=R8) :: r1, z1, r2, z2, help(2,2)
      integer i, j, icnt, jcnt, kcnt
      character*80 zeile
      logical streql
      external streql
c
      iret=0
      icnt=0
      do
   89   continue
        read(lun,'(A80)',end=90) zeile
        if (streql(zeile,' ')) cycle
        read(zeile,*,err=89) r1, z1
        icnt = icnt + 1
        if (icnt.gt.ngpr) goto 98
        pp1(1,icnt) = r1
        pp1(2,icnt) = z1 
        read(lun,'(A80)',end=90) zeile
        read(zeile,*) r2, z2
        pp2(1,icnt) = r2
        pp2(2,icnt) = z2
      end do
   90 continue

c Now re-order the elements to make sequences
      jcnt = 1
   10 continue
      i = jcnt + 1
   20 continue
      if (pp2(1,jcnt).eq.pp1(1,i) .and.
     &    pp2(2,jcnt).eq.pp1(2,i)) then
        if (i.gt.jcnt+1) then
          help(1,1) = pp1(1,jcnt+1)
          help(1,2) = pp1(2,jcnt+1)
          help(2,1) = pp2(1,jcnt+1)
          help(2,2) = pp2(2,jcnt+1)
          pp1(1,jcnt+1) = pp1(1,i)
          pp1(2,jcnt+1) = pp1(2,i)
          pp2(1,jcnt+1) = pp2(1,i)
          pp2(2,jcnt+1) = pp2(2,i)
          pp1(1,i) = help(1,1)
          pp1(2,i) = help(1,2)
          pp2(1,i) = help(2,1)
          pp2(2,i) = help(2,2)
        end if
        jcnt = jcnt + 1
        i = jcnt + 1
      else if (pp2(1,jcnt).eq.pp2(1,i) .and.
     &         pp2(2,jcnt).eq.pp2(2,i)) then
        help(1,1) = pp1(1,jcnt+1)
        help(1,2) = pp1(2,jcnt+1)
        help(2,1) = pp2(1,jcnt+1)
        help(2,2) = pp2(2,jcnt+1)
        pp1(1,jcnt+1) = pp2(1,i)
        pp1(2,jcnt+1) = pp2(2,i)
        pp2(1,jcnt+1) = pp1(1,i)
        pp2(1,jcnt+1) = pp1(2,i)
        pp1(1,i) = help(1,1)
        pp1(2,i) = help(1,2)
        pp2(1,i) = help(2,1)
        pp2(2,i) = help(2,2)
        jcnt = jcnt + 1
        i = jcnt + 1
      else
        i = i + 1
      end if
      if (i.le.icnt) goto 20
      if (jcnt.lt.icnt) then
        jcnt = jcnt + 1
        goto 10
      end if

c Initializing the output
      rwall = 0.0_R8
      zwall = 0.0_R8
      nunits = 0
      npts = 0

c Filling in the wall units
      nunits = 1
      rwall(1) = pp1(1,1)
      zwall(1) = pp1(2,1)
      rwall(2) = pp2(1,1)
      zwall(2) = pp2(2,1)
      npts(1) = 2
      kcnt = 2
      i = 1
   30 continue
      i = i + 1
      kcnt = kcnt + 1
      if (pp1(1,i).eq.pp2(1,i-1).and.pp1(2,i).eq.pp2(2,i-1)) then
        rwall(kcnt) = pp2(1,i)
        zwall(kcnt) = pp2(2,i)
        npts(nunits) = npts(nunits) + 1
      else
        nunits = nunits + 1
        rwall(kcnt) = pp1(1,i)
        zwall(kcnt) = pp1(2,i)
        rwall(kcnt+1) = pp2(1,i)
        zwall(kcnt+1) = pp2(2,i)
        npts(nunits) = 2
        kcnt = kcnt + 1
      end if
      if (i.lt.icnt) goto 30
      return
c-----------------------------------------------------
c
   98 print *,'==== rdtrg: too many elements in template file'
      print *,'==== increase value of ngpr. Currently, ngpr = ',ngpr
      iret=4
      return
   99 print *,'==== rdtrg: error reading the template file'
      iret=5
      return
c
      end
