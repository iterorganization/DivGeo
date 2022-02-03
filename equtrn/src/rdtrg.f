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
c  version : 03.02.20 13:42
c
      use eqdim
      implicit none
      integer, intent(in) :: lun
      integer, intent(inout) :: iret
      integer, intent(out) :: nunits
      integer, intent(out) :: npts(ngpr)
      real(kind=R8), intent(out) :: rwall(ngpr), zwall(ngpr)
      real(kind=R8) :: pp(2,ngpr)
      real(kind=R8) :: pp1(2,ngpr), pp2(2,ngpr)
      real(kind=R8) :: r, z, help(2,2)
      integer i, j, k, icnt, jcnt, kcnt, nseg, new_nunits
      integer, allocatable :: j_block(:,:,:), k_block(:,:,:)
      integer ibeg(ngpr), iend(ngpr)
      character*80 zeile, altzeile
      logical blank_line_format, j_merged
      logical, allocatable :: closed(:)
      logical streql
      external streql
c
c The DivGeo template files can exist in two formats:
c 1. Each wall segment is listed by having both end vertices written
c    over two lines, with no line breaks between segment.
c 2. Each limiter unit is listed one vertex at a time, with a blank line
c    between units. If the unit is a closed contour, the first and last
c    vertices will be the same.
c
      iret=0
      icnt=0
      jcnt=-1 ! here, the point count at first occurrence of a blank line
              ! after some data has been read
      kcnt=0  ! here, the point count at the last occurrence of a blank line
      npts = 0
      nunits = 0
      altzeile = 'undefined'
      blank_line_format = .false.
      do
   89   continue
        read(lun,'(A80)',end=90) zeile
        if (streql(zeile,' ')) then
          blank_line_format = .true.
          if (jcnt.eq.-1.and.icnt.gt.0) jcnt = icnt
          kcnt = max(kcnt,icnt)
          if (.not.streql(altzeile,' ')) nunits = nunits + 1
        else
          read(zeile,*,err=89) r, z
          icnt = icnt + 1
          if (icnt.gt.ngpr) goto 98
          pp(1,icnt) = r
          pp(2,icnt) = z
          npts(nunits+1) = npts(nunits+1) + 1
        end if
        altzeile = trim(zeile)
      end do
   90 continue
      if (.not.streql(altzeile,' ')) nunits = nunits + 1

c We check that all the blank lines were not at the end of the file
      if (jcnt.eq.kcnt) blank_line_format = .false.
c If there is only one unit, we differentiate by checking if
c the second and third point of the list are different
      if (nunits.eq.1 .and.
     &   (pp(1,2).ne.pp(1,3) .or. pp(2,2).ne.pp(2,3)))
     > blank_line_format = .true.

      if (blank_line_format) then
        allocate(closed(nunits))
c We fill in the pp1 and pp2 arrays
        k = 0
        jcnt = 1 ! index of the first vertex of that unit
        do i = 1, nunits
          ibeg(i) = jcnt
          do icnt = 1, npts(i)-1
            k = k + 1
            kcnt = jcnt + icnt - 1
            pp1(1,kcnt) = pp(1,k)
            pp1(2,kcnt) = pp(2,k)
            pp2(1,kcnt) = pp(1,k+1)
            pp2(2,kcnt) = pp(2,k+1)
          end do
          if (pp2(1,kcnt).ne.pp1(1,ibeg(i)).or.
     &        pp2(2,kcnt).ne.pp1(2,ibeg(i))) then
            closed(i) = .false.
          else
            closed(i) = .true.
          end if
          k = k + 1
          npts(i) = npts(i) - 1
          jcnt = jcnt + npts(i)
          iend(i) = jcnt - 1
        end do
c We now check if some open units can be merged
        new_nunits = nunits
        do i = 1, nunits-1
          if (closed(i)) cycle ! unit already closed: no merging possible
          do j = i+1, nunits
            if (closed(j)) cycle ! unit already closed: no merging possible
            if (closed(i)) cycle ! unit now closed: skip
            j_merged = .false.
            if (pp2(1,iend(i)).eq.pp1(1,ibeg(j)).and.
     &          pp2(2,iend(i)).eq.pp2(2,ibeg(j))) then
c First case: unit i ends where unit j begins
c Merge the two units and and add points from unit j at end of unit i
c Also re-order the points if necessary
c j_block contains the points from unit j
c k_block contains the points from all units between i and j (exclusively)
              if (ibeg(j).ne.iend(i)+1) then
                allocate(j_block(2,2,npts(j)))
                allocate(k_block(2,2,iend(j-1)-iend(i)))
                do icnt = ibeg(i+1), iend(j-1)
                  kcnt = icnt - iend(i)
                  k_block(1,1,kcnt) = pp1(1,icnt)
                  k_block(1,2,kcnt) = pp1(2,icnt)
                  k_block(2,1,kcnt) = pp2(1,icnt)
                  k_block(2,2,kcnt) = pp2(2,icnt)
                end do
                do icnt = ibeg(j), iend(j)
                  jcnt = icnt - ibeg(j) + 1
                  j_block(1,1,jcnt) = pp1(1,icnt)
                  j_block(1,2,jcnt) = pp1(2,icnt)
                  j_block(2,1,jcnt) = pp2(1,icnt)
                  j_block(2,2,jcnt) = pp2(2,icnt)
                end do
                do jcnt = 1, npts(j)
                  icnt = iend(i) + jcnt
                  pp1(1,icnt) = j_block(1,1,jcnt)
                  pp1(2,icnt) = j_block(1,2,jcnt)
                  pp2(1,icnt) = j_block(2,1,jcnt)
                  pp2(2,icnt) = j_block(2,2,jcnt)
                end do
                do icnt = ibeg(i+1), iend(j-1)
                  kcnt = icnt - ibeg(i+1) + 1
                  jcnt = icnt + npts(j)
                  pp1(1,jcnt) = k_block(1,1,kcnt)
                  pp1(2,jcnt) = k_block(1,2,kcnt)
                  pp2(1,jcnt) = k_block(2,1,kcnt)
                  pp2(2,jcnt) = k_block(2,2,kcnt)
                end do
                deallocate(j_block)
                deallocate(k_block)
              end if
              j_merged = .true.
            else if (pp1(1,ibeg(i)).eq.pp2(1,iend(j)).and.
     &               pp1(2,ibeg(i)).eq.pp2(2,iend(j))) then
c Second case: unit i begins where unit j ends
c Merge the two units and add points from unit j at beginning of unit i
c Also re-order the points
c j_block contains the points from unit j
c k_block contains the points from unit i up to unit j
              allocate(j_block(2,2,npts(j)))
              allocate(k_block(2,2,iend(j-1)-ibeg(i)+1))
              do icnt = ibeg(j), iend(j)
                jcnt = icnt - ibeg(j) + 1
                j_block(1,1,jcnt) = pp1(1,icnt)
                j_block(1,2,jcnt) = pp1(2,icnt)
                j_block(2,1,jcnt) = pp2(1,icnt)
                j_block(2,2,jcnt) = pp2(2,icnt)
              end do
              do icnt = ibeg(i), iend(j-1)
                kcnt = icnt - ibeg(i) + 1
                k_block(1,1,kcnt) = pp1(1,icnt)
                k_block(1,2,kcnt) = pp1(2,icnt)
                k_block(2,1,kcnt) = pp2(1,icnt)
                k_block(2,2,kcnt) = pp2(2,icnt)
              end do
              do jcnt = 1, npts(j)
                icnt = ibeg(i) + jcnt - 1
                pp1(1,icnt) = j_block(1,1,jcnt)
                pp1(2,icnt) = j_block(1,2,jcnt)
                pp2(1,icnt) = j_block(2,1,jcnt)
                pp2(2,icnt) = j_block(2,2,jcnt)
              end do
              do icnt = ibeg(i), iend(j-1)
                kcnt = icnt - ibeg(i) + 1
                jcnt = icnt + npts(j)
                pp1(1,jcnt) = k_block(1,1,kcnt)
                pp1(2,jcnt) = k_block(1,2,kcnt)
                pp2(1,jcnt) = k_block(2,1,kcnt)
                pp2(2,jcnt) = k_block(2,2,kcnt)
              end do
              deallocate(j_block)
              deallocate(k_block)
              j_merged = .true.
            else if (pp1(1,ibeg(i)).eq.pp1(1,ibeg(j)).and.
     &               pp1(2,ibeg(i)).eq.pp1(2,ibeg(j))) then
c Third case: unit i and unit j begin from the same place
c but are ordered in opposite directions
c Merge the two units and add points from unit j at beginning of unit i
c but reverting the order of points from unit j
c Also re-order the points
c j_block contains the points from unit j, in reverse order
c k_block contains the points from unit i up to unit j
              allocate(j_block(2,2,npts(j)))
              allocate(k_block(2,2,iend(j-1)-ibeg(i)+1))
              do icnt = ibeg(j), iend(j)
                jcnt = iend(j) - icnt + 1
                j_block(1,1,jcnt) = pp2(1,icnt)
                j_block(1,2,jcnt) = pp2(2,icnt)
                j_block(2,1,jcnt) = pp1(1,icnt)
                j_block(2,2,jcnt) = pp1(2,icnt)
              end do
              do icnt = ibeg(i), iend(j-1)
                kcnt = icnt - ibeg(i) + 1
                k_block(1,1,kcnt) = pp1(1,icnt)
                k_block(1,2,kcnt) = pp1(2,icnt)
                k_block(2,1,kcnt) = pp2(1,icnt)
                k_block(2,2,kcnt) = pp2(2,icnt)
              end do
              do jcnt = 1, npts(j)
                icnt = ibeg(i) + jcnt - 1
                pp1(1,icnt) = j_block(1,1,jcnt)
                pp1(2,icnt) = j_block(1,2,jcnt)
                pp2(1,icnt) = j_block(2,1,jcnt)
                pp2(2,icnt) = j_block(2,2,jcnt)
              end do
              do icnt = ibeg(i), iend(j-1)
                kcnt = icnt - ibeg(i) + 1
                jcnt = icnt + npts(j)
                pp1(1,jcnt) = k_block(1,1,kcnt)
                pp1(2,jcnt) = k_block(1,2,kcnt)
                pp2(1,jcnt) = k_block(2,1,kcnt)
                pp2(2,jcnt) = k_block(2,2,kcnt)
              end do
              deallocate(j_block)
              deallocate(k_block)
              j_merged = .true.
            else if (pp1(1,iend(i)).eq.pp1(1,iend(j)).and.
     &               pp1(2,iend(i)).eq.pp1(2,iend(j))) then
c Fourth case: unit i and unit j end at the same place
c but are ordered in opposite directions
c Merge the two units and add points from unit j after end of unit i
c but reverting the order of points from unit j
c Also re-order the points
c j_block contains the points from unit j, in reverse order
c k_block contains the points from unit i to unit j (exclusively) (if needed)
              allocate(j_block(2,2,npts(j)))
              do icnt = ibeg(j), iend(j)
                jcnt = icnt - ibeg(j) + 1
                j_block(1,1,jcnt) = pp2(1,icnt)
                j_block(1,2,jcnt) = pp2(2,icnt)
                j_block(2,1,jcnt) = pp1(1,icnt)
                j_block(2,2,jcnt) = pp1(2,icnt)
              end do
              if (ibeg(j).ne.iend(i)+1) then
                allocate(k_block(2,2,iend(j-1)-iend(i)))
                do icnt = ibeg(i+1), iend(j-1)
                  kcnt = icnt - iend(i)
                  k_block(1,1,kcnt) = pp1(1,icnt)
                  k_block(1,2,kcnt) = pp1(2,icnt)
                  k_block(2,1,kcnt) = pp2(1,icnt)
                  k_block(2,2,kcnt) = pp2(2,icnt)
                end do
                do icnt = ibeg(i+1), iend(j-1)
                  kcnt = icnt - iend(i)
                  jcnt = icnt + npts(j)
                  pp1(1,jcnt) = k_block(1,1,kcnt)
                  pp1(2,jcnt) = k_block(1,2,kcnt)
                  pp2(1,jcnt) = k_block(2,1,kcnt)
                  pp2(2,jcnt) = k_block(2,2,kcnt)
                end do
                deallocate(k_block)
              end if
              do jcnt = 1, npts(j)
                icnt = iend(i) + jcnt
                pp1(1,icnt) = j_block(1,1,jcnt)
                pp1(2,icnt) = j_block(1,2,jcnt)
                pp2(1,icnt) = j_block(2,1,jcnt)
                pp2(2,icnt) = j_block(2,2,jcnt)
              end do
              deallocate(j_block)
              j_merged = .true.
            end if
            if (j_merged) then
              npts(i) = npts(i) + npts(j)
              iend(i) = iend(i) + npts(j)
              do k = i+1, j-1
                ibeg(k) = ibeg(k) + npts(j)
                iend(k) = iend(k) + npts(j)
              end do
              ibeg(j) = 0
              iend(j) = 0
              npts(j) = 0
              new_nunits = new_nunits - 1
              closed(i) = pp1(1,ibeg(i)).eq.pp2(1,iend(i)).and.
     &                    pp1(2,ibeg(i)).eq.pp2(2,iend(i))
              closed(j) = closed(i)
            end if
          end do
        end do
        nunits = new_nunits
        deallocate(closed)
      else
c Both end vertices of each segment were read (blank_line_format=F)
        kcnt = 0
        do jcnt = 1, icnt-1, 2
          kcnt = kcnt + 1
          pp1(1,kcnt) = pp(1,jcnt)
          pp1(2,kcnt) = pp(2,jcnt)
          pp2(1,kcnt) = pp(1,jcnt+1)
          pp2(2,kcnt) = pp(2,jcnt+1)
        end do
        nseg = kcnt
c We need to re-order the elements to build up sequences
        jcnt = 1
   10   continue
        i = jcnt + 1
   20   continue
        if (pp2(1,jcnt).eq.pp1(1,i) .and.
     &      pp2(2,jcnt).eq.pp1(2,i)) then
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
     &           pp2(2,jcnt).eq.pp2(2,i)) then
          help(1,1) = pp1(1,jcnt+1)
          help(1,2) = pp1(2,jcnt+1)
          help(2,1) = pp2(1,jcnt+1)
          help(2,2) = pp2(2,jcnt+1)
          if (i.ne.jcnt+1) then
            pp1(1,jcnt+1) = pp2(1,i)
            pp1(2,jcnt+1) = pp2(2,i)
            pp2(1,jcnt+1) = pp1(1,i)
            pp2(2,jcnt+1) = pp1(2,i)
            pp1(1,i) = help(1,1)
            pp1(2,i) = help(1,2)
            pp2(1,i) = help(2,1)
            pp2(2,i) = help(2,2)
          else
            pp1(1,jcnt+1) = help(2,1)
            pp1(2,jcnt+1) = help(2,2)
            pp2(1,jcnt+1) = help(1,1)
            pp2(2,jcnt+1) = help(1,2)
          end if
          jcnt = jcnt + 1
          i = jcnt + 1
        else
          i = i + 1
        end if
        if (i.le.nseg) goto 20
        if (jcnt.lt.nseg) then
          jcnt = jcnt + 1
          goto 10
        end if
      end if

c Initializing the output
      rwall = 0.0_R8
      zwall = 0.0_R8

c Filling in the wall units
      if (blank_line_format) then
        jcnt = 0
        do i = 1, nunits
          do icnt = 1, npts(i)
            jcnt = jcnt + 1
            kcnt = icnt + ibeg(i) - 1
            rwall(jcnt) = pp1(1,kcnt)
            zwall(jcnt) = pp1(2,kcnt)
          end do
          jcnt = jcnt + 1
          rwall(jcnt) = pp2(1,iend(i))
          zwall(jcnt) = pp2(2,iend(i))
          npts(i) = npts(i) + 1
        end do
      else
        nunits = 1
        rwall(1) = pp1(1,1)
        zwall(1) = pp1(2,1)
        rwall(2) = pp2(1,1)
        zwall(2) = pp2(2,1)
        npts(1) = 2
        kcnt = 2
        i = 1
   30   continue
        i = i + 1
        kcnt = kcnt + 1
        if (pp1(1,i).eq.pp2(1,i-1).and.pp1(2,i).eq.pp2(2,i-1)) then
          rwall(kcnt) = pp2(1,i)
          zwall(kcnt) = pp2(2,i)
          npts(nunits) = npts(nunits) + 1
        else if (i.le.nseg) then
          nunits = nunits + 1
          rwall(kcnt) = pp1(1,i)
          zwall(kcnt) = pp1(2,i)
          rwall(kcnt+1) = pp2(1,i)
          zwall(kcnt+1) = pp2(2,i)
          npts(nunits) = 2
          kcnt = kcnt + 1
        end if
        if (i.lt.nseg) goto 30
      end if

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
