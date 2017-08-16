      program cnvtria
c
c======================================================================
c*** This routine converts the TRIA output (data for triangular mesh)
c*** into the DG template format.
c*** The program acts like a filter: it reads the standard input and
c*** writes the data into the standard output, using stderr for the
c*** error messages.
c*** The input file should consist from TRIA *.elemente file (triangles contains)  
c*** and *.npco_char file (elements coordinates) connected with 'cat' command 
c*** (use script cnvtria from /src/bin/common/)
c======================================================================
c***  npoint is the actual number of points
c***  ntria ... of triangles
c***  npair ... of pairs (matched nodes)
c***  px(:),py(:) are the x and y coordintes of nodes
c***  tri(3,:) is the triangle constants (nodes indices for each triangle)
c***  pairs(2,:) is the list of present pairs (matched nodes)

      integer npoint,ntria, npair
      real,allocatable :: px(:),py(:)
      integer,allocatable :: tri(:,:), pairs(:,:)

c======================================================================
c*** Read the data from the input file
c
c*** read *.npco_char
      read(*,*) npoint
      write(0,'(a,t15,i6)') 'npoint', npoint
      allocate(px(npoint),py(npoint))
      read(*,*) px
      read(*,*) py
      write(0,*) '    reading *.npco_char is finished'
c*** read *.elemente 
      read(*,*) ntria
      write(0,'(a,t15,i6)') 'ntria', ntria
      allocate(tri(3,3*ntria),pairs(2,3*ntria))
      do i=1,ntria
       read(*,*) j,(tri(l,i),l=1,3)
      end do
      write(0,*) '    reading *.element is finished'
c----------------------------------------------------------------------
c*** find pairs (matched nodes)  
      k=0
      do i=1,ntria
c*** examin each combination of nodes in each triangle
       do i1=1,3
        i2=mod(i1,3)+1
        j1=min(tri(i1,i),tri(i2,i))
        j2=max(tri(i1,i),tri(i2,i))
        do j=1,k
c*** chech if this pair is already present
         if(pairs(1,j).EQ.j1.AND.pairs(2,j).EQ.j2) goto 11
        end do
c*** add pair
        k=k+1
        pairs(1,k)=j1
        pairs(2,k)=j2
   11   continue
       end do
      end do
      npair=k
c*** output template (coordinate of triangle edges in mm)              
      do i=1,npair
        write(*,*) 10.*px(pairs(1,i)),10.*py(pairs(1,i))
        write(*,*) 10.*px(pairs(2,i)),10.*py(pairs(2,i))
        write(*,*)
      end do
      write(0,*) '    writing template is finished'
      deallocate(px,py,tri,pairs)
      end
