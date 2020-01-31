      subroutine wrtrdg(lun,iret,nunits,npts,rwall,zwall)
c=====================================================
c*** Write the template data in the dg compatible format.
c***
c*** Input:
c***  lun     the logical unit number for the output
c***  nunits  the number of structure units
c***  npts    the number of points in each structure
c***  rwall   the R coordinates of the structure points
c***  zwall   the Z coordinates of the structure points
c***
c*** Output:
c***  iret    return code (0 means OK)
c=====================================================
c
c  version : 23.06.97 17:23
c
      implicit none
#include "eqdim.inc"
      integer, intent(in) :: lun, nunits
      integer, intent(inout) :: iret
      integer, intent(in) :: npts(ngpr)
      real(kind=R8), intent(in) :: rwall(ngpr), zwall(ngpr)
      integer i, j, icnt
c
      iret=0
      icnt=0
      do i = 1, nunits
        do j = 1, npts(i)
          write(lun,8000) rwall(icnt+j), zwall(icnt+j)
        end do
        write(lun,1028)
        icnt = icnt + npts(i)
      end do
 8000 format(5(3x,f11.4))
 1028 format()
      return
c-----------------------------------------------------
c
   99 print *,'==== wrtrdg: error writing the template file'
      iret=8
      return
c
      end
