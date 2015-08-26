      program cnveir
c
c  version : 14.07.98 16:57
c
c======================================================================
c*** This routine converts the wall geometry data from an eirene input
c*** file into the DG template format.
c*** The program acts like a filter: it reads the standard input and
c*** writes the data into the standard output, using stderr for the
c*** error messages.
c======================================================================
c*** nxelm:   the maximum number of segments
c*** eps:     tolerance in the alignment of the segment ends
      parameter (nxelm=500, eps=1.e-2)
c
c*** x1, y1, x2, y2 contain the coordinates of the edge points
      real x1(nxelm),x2(nxelm),y1(nxelm),y2(nxelm)
c
c*** inext and iprvs are the references to the next and previous segment
      integer inext(nxelm),iprvs(nxelm)
c
c*** done:  the segment is not yet written to the output
c*** next:    the next segment not found yet
c*** prvs:    the previous segment not found yet
c*** free:    one of the ends of the segment is not yet linked
      logical done(nxelm),next(nxelm),prvs(nxelm),free(nxelm),b,open
      logical bbb
c
      character ss*72,s1,s3*3
      equivalence (ss,s1,s3)
c======================================================================
c*** Read the data from the input file
c*** First, find the beginning of the relevant data
c
 10   read(*,'(a72)',end=990) ss
      call locase(ss,ss,72)
      if(index(ss,'*** 3b. ').ne.1) go to 10
c----------------------------------------------------------------------
c*** Found. Let's read the data
c
      read(*,'(i6)',err=980) ns
      write(0,*) '== ns = ',ns
      k=1
      do i=1,ns
 20     read(*,'(a72)',err=970) ss
        if(s1.ne.'*') go to 20
        read (*,*,err=970) u
        if(u.ge.2.) then
          read (*,*)
          read (*,'(5e12.0)',err=970) x1(k),y1(k),u,x2(k),y2(k)
          k=k+1
        end if
      end do
      ns=k-1
      write(0,*) '== data is read. ',ns,'  plain surfaces'
c      do i=1,ns
c        write(0,*) x1(i),y1(i),x2(i),y2(i)
c      end do
c
c*** Now arrange the structures
c
      do i=1,ns
        free(i)=.true.
        next(i)=.true.
        prvs(i)=.true.
        done(i)=.false.
        inext(i)=0
        iprvs(i)=0
      end do
      do 100 i=1,ns-1
c        write(0,*) '=== i = ',i
c        bbb=i.eq.135
c        write(0,*) '  free next prvs inext iprvs'
c        write(0,'(3l5,2i6)') (free(l),next(l),prvs(l),inext(l),iprvs(l),
c     ,                                                          l=1,ns)
        if(free(i)) then
          do 110 j=i+1,ns
c            if(bbb) write(0,*) '=== j = ',j,free(j),next(j),
c     ,                                        prvs(j),inext(j),iprvs(j)
            if(.not. free(j)) go to 110
            if(next(j)) then
c                write(0,*) '== next(j)'
              if(prvs(i)) then
c
c*** begin i vs end j
c
c                write(0,*) '== prvs(i)',x1(i),x2(j),y1(i),y2(j)
                if(abs(x1(i)-x2(j)).le.eps .and.
     .                                    abs(y1(i)-y2(j)).le.eps) then
c                  bbb=j.eq.136 .or.bbb
c                  if(bbb) write(0,*) '=== i,j ',i,j,
c     ,                                iprvs(j),inext(j),prvs(j),next(j)
                  inext(j)=i
                  iprvs(i)=j
                  prvs(i)=.false.
                  next(j)=.false.
                  free(j)=next(j).or.prvs(j)
                  free(i)=next(i).or.prvs(i)
                  if(.not.free(i)) go to 100
                  go to 110
                end if
              end if
              if(next(i)) then
c
c*** end i vs end j
c
c                if(bbb) write(0,*) '== next(i)',x2(i),x2(j),y2(i),y2(j)
                if(abs(x2(i)-x2(j)).le.eps .and.
     .                          abs(y2(i)-y2(j)).le.eps) then
c                  bbb=j.eq.136 .or.bbb
c                  if(bbb) write(0,*) '=== i,j ',i,j,
c     ,                                iprvs(j),inext(j),prvs(j),next(j)
c                  if(bbb) write(0,*) i,
c     ,                                iprvs(i),prvs(i),inext(i),next(i)
c                  if(bbb) write(0,*) j,
c     ,                                iprvs(j),prvs(j),inext(j),next(j)
                  inext(j)=i
                  next(j)=.false.
                  free(j)=next(j).or.prvs(j)
                  inext(i)=j
                  next(i)=.false.
                  free(i)=next(i).or.prvs(i)
c
c*** change the orientation of the chain descending from j
c
                  k=j
c                  if(bbb) write(0,*) '*40: start. k =',k
                  do ii=1,ns
c                    if(bbb) write(0,*) ii,k,
c     ,                                iprvs(k),prvs(k),inext(k),next(k)
                    w=x1(k)
                    x1(k)=x2(k)
                    x2(k)=w
                    w=y1(k)
                    y1(k)=y2(k)
                    y2(k)=w
                    b=prvs(k)
                    prvs(k)=next(k)
                    next(k)=b
                    l=iprvs(k)
                    iprvs(k)=inext(k)
                    inext(k)=l
c                    if(bbb) write(0,*) ii,k,inext(k),next(k)
                    if(next(k)) go to 40
                    k=inext(k)
                  end do
                  write(0,*) 'Something wrong with re-ordering'
                  stop
 40               if(.not.free(i)) go to 100
                  go to 110
                end if
              end if
            end if
            if(prvs(j)) then
c
c*** end i vs begin j
c
c              write(0,*) '== prvs(j)'
              if(next(i)) then
c              write(0,*) '== next(i)',x2(i),x1(j),y2(i),y1(j)
                if(abs(x2(i)-x1(j)).le.eps .and.
     .                          abs(y2(i)-y1(j)).le.eps) then
c                  bbb=j.eq.136 .or.bbb
c                  if(bbb) write(0,*) '=== i,j ',i,j,
c     ,                                iprvs(j),inext(j),prvs(j),next(j)
                  iprvs(j)=i
                  inext(i)=j
                  next(i)=.false.
                  prvs(j)=.false.
                  free(j)=prvs(j).or.next(j)
                  free(i)=prvs(i).or.next(i)
                  if(.not.free(i)) go to 100
                  go to 110
                end if
              end if
              if(prvs(i)) then
c
c*** begin i vs begin j
c
c                write(0,*) '== prvs(i)',x1(i),x1(j),y1(i),y1(j)
                if(abs(x1(i)-x1(j)).le.eps .and.
     .                          abs(y1(i)-y1(j)).le.eps) then
c                  bbb=j.eq.136 .or.bbb
c                  if(bbb) write(0,*) '=== i,j ',i,j,
c     ,                                iprvs(j),inext(j),prvs(j),next(j)
                  iprvs(j)=i
                  prvs(j)=.false.
                  free(j)=prvs(j).or.next(j)
                  iprvs(i)=j
                  prvs(i)=.false.
                  free(i)=prvs(i).or.next(i)
c
c*** change the orientation of the chain accending from j
c
                  k=j
c                  if(bbb) write(0,*) '*50: start. k ='
 50               w=x2(k)
                  x2(k)=x1(k)
                  x1(k)=w
                  w=y2(k)
                  y2(k)=y1(k)
                  y1(k)=w
                  b=next(k)
                  next(k)=prvs(k)
                  prvs(k)=b
                  l=inext(k)
                  inext(k)=iprvs(k)
                  iprvs(k)=l
                  if(.not.prvs(k)) then
                    k=iprvs(k)
                    go to 50
                  end if
                end if
                if(.not.free(i)) go to 100
                go to 110
              end if
            end if
 110      continue
        end if
 100  continue
c      write(0,*) '=== Rearranged.'
c      write(0,'(3l5,2i6)') (free(i),next(i),
c     ,                                prvs(i),inext(i),iprvs(i),i=1,ns)
c
c*** We have got the lists now.
c*** Open the output file
c
c      open(1,file='convert.tpl')
c
c*** Start output of the chains separated by blank lines,
c*** converting cms to mms
c
c      write(0,*) '=== Starting output  '
      open=.true.
      k=0
      do iii=1,2
c
c*** First, output the open structures - they must start from an end,
c*** and then, at the second iteration, add the closed structures
c
c          write(0,*)
c          write(0,'(3l5,2i6)') (free(i),next(i),
c     ,                                prvs(i),inext(i),iprvs(i),i=1,ns)
c          write(0,'(1p,4e12.3)') (x1(i),x2(i),y1(i),y2(i),i=1,ns)
        do 120 i=1,ns
c         write(0,*) '== i = ',i,done(i),free(i),prvs(i),iprvs(i)
          if(done(i) .or. (.not.free(i).and.open)) go to 120
c
c*** This is an end of a chain
c
          if(k.gt.0) write (*,*)
          k=k+1
c
c*** Output the chain
c
c          write(0,*) '   i  free next prvs inext iprvs'
c          write(0,'(i4,3l5,2i6)') (ii,free(ii),
c     ,                   prvs(ii),next(ii),iprvs(ii),inext(ii),ii=1,ns)
          j=i
          if(prvs(i)) then
            write(*,*) 10.*x1(i),10.*y1(i)
            do ii=1,ns
c              write(0,*) 'j = ',j,next(j),inext(j)
              write(*,*) 10.*x2(j),10.*y2(j)
              done(j)=.true.
              if(next(j)) go to 120
              j=inext(j)
            end do
          else
            write(*,*) 10.*x2(i),10.*y2(i)
            do ii=1,ns
              write(*,*) 10.*x1(j),10.*y1(j)
              done(j)=.true.
              if(prvs(j)) go to 120
              j=iprvs(j)
            end do
          end if
 120    continue
        open=.false.
      end do
      stop
c======================================================================
 970  write(0,*) '=== cnveir: format error. Surface number ',i
      stop
c======================================================================
 980  write(0,*) '=== cnveir: cannot read the number of surfaces'
      stop
c======================================================================
 990  write(0,*) '=== cnveir: the beginning of section 3B not found'
      stop
c======================================================================
      end
