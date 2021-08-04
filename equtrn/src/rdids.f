      subroutine rdids(treename,shot,wall,run,wall_run,step,
     ,           username,database,version,
     ,           do_equilibrium,do_wall,
     ,           iret,ipestg,nr,nz,
     ,           rcntc,psimin,psilim,
     ,           btorc,fg,pfm,rgr,zgr,
     ,           nunits,npts,rwall,zwall)
c=====================================================
c*** where:
c***
c*** i)  nr, nz, and redge define the rectangular mesh used
c***     to store the psi values,
c***
c*** ii) psimin is the flux value at the magnetic axis; psilim is the
c***     value at the separatrix,
c***
c*** iii) btorc is the toroidal magnetic field at a radius rcntc,
c***
c*** iv) the poloidal flux is pfm,
c***
c*** v) fg is the flux function R*Btor.
c=====================================================
c
c  version : 03.02.20 13:42
c
      use ids_schemas  ! IGNORE
     , , only : ids_equilibrium, ids_wall
#if IMAS_MINOR_VERSION > 8
      use ids_schemas  ! IGNORE
     , , only : IDS_REAL_INVALID
#endif
      use ids_routines ! IGNORE
     , , only : imas_open_env, imas_close, ids_get, ids_deallocate
      implicit none
#include "eqdim.inc"
      type(ids_equilibrium) :: eq !< IDS designed to store equilibrium data
      type(ids_wall) :: vessel    !< IDS designed to store wall data
      character(len=24), intent(in) :: treename   !< The name of the IMAS IDS database
      integer, intent(in) :: shot      !< The shot number of the IDS equilibrium being read
                                       !< If negative, do not read any equilibrium
      integer, intent(in) :: wall      !< The shot number of the IDS wall being read
                                       !< If negative, do not read wall description
      integer, intent(in) :: step      !< The time slice index of the IDS equilibrium being read
      integer, intent(in) :: run       !< The run number of the IDS equilibrium being read
      integer, intent(in) :: wall_run  !< The run number of the wall IDS being read
      character(len=24), intent(in) :: username   !< Creator/owner of the IMAS IDS database
      character(len=24), intent(in) :: database   !< IMAS IDS database name
            !< (i. e. solps-iter, iter, aug)
      character(len=24), intent(in) :: version    !< Major version of the IMAS IDS database
      logical, intent(inout) :: do_equilibrium, do_wall
      integer, intent(out) :: iret,ipestg,nr,nz
      real(kind=R8), intent(out) :: fg(ngpr),rgr(ngpr),zgr(ngpz)
      real(kind=R8), intent(out) :: pfm(ngpr,ngpz)
      real(kind=R8), intent(out) :: rcntc,psimin,psilim,btorc
      real(kind=R8), intent(out) :: rwall(ngpr), zwall(ngpr)
      integer, intent(out) :: nunits, npts(ngpr)
      integer :: nlimunits, nmobunits, nvslunits, nelem
      integer i,j,k,icnt,jcnt,idx,status
#if IMAS_MINOR_VERSION > 27
      integer iside, kk, ks, kkk, kks
      real(kind=R8) :: t1, t2, ra, rb, za, zb, rc, zc, rd, zd,
     .                 r1, r2, z1, z2, r3, z3, x4, z4, det, ! avoid clash with R4 type
     .                 rab_normal, rcd_normal, zab_normal, zcd_normal
#endif
#if IMAS_MINOR_VERSION < 9
      real(kind=R8), parameter :: IDS_REAL_INVALID = -9.0E40_R8
#endif

c=====================================================
c
      iret = 0
      !! Create and modify new shot/run
      if (do_equilibrium) then
        call imas_open_env( treename, shot, run,
     &   idx, username, database, version, status )

#if UAL_MAJOR_VERSION > 3
        if (status.eq.0)
     &   call ids_get( idx, "equilibrium/1", eq, status )
#else
        call ids_get( idx, "equilibrium/1", eq )
#endif
        if (status.ne.0 .or.
     &      eq%ids_properties%homogeneous_time < 0) then ! second attempt
          status = 0
          call imas_close( idx, status )
          if (status.ne.0) stop 'Error closing IMAS database !'
          call imas_open_env( treename, shot, run,
     &     idx, "public", database, version, status )
#if UAL_MAJOR_VERSION > 3
          if (status.eq.0)
     &     call ids_get( idx, "equilibrium/1", eq, status )
#else
          call ids_get( idx, "equilibrium/1", eq )
#endif
          if (status.eq.0 .and.
     &        eq%ids_properties%homogeneous_time .ge. 0) then
            write(*,*) 'Got IDS equilibrium from: '
            write(*,'(2(a,i8),2(a,a24))')
     .       ' Shot: ', shot, ' Run: ', run,
     .       ' User: ', 'public', ' Database: ', trim(database)
          else
            do_equilibrium = .false.
            write(*,*) 'Error reading equilibrium IDS !'
          end if
        else
          write(*,*) 'Got IDS equilibrium from: '
          write(*,'(2(a,i8),2(a,a24))')
     .     ' Shot: ', shot, ' Run: ', run,
     .     ' User: ', trim(username), ' Database: ', trim(database)
        end if
      end if

      status = 0
      if (do_wall.and.wall.ne.shot) then
        if (do_equilibrium) then
          call imas_close ( idx, status )
          if (status.ne.0) stop 'Error closing IMAS database !'
        end if
        call imas_open_env( treename, wall, wall_run,
     &     idx, username, database, version, status )
#if UAL_MAJOR_VERSION > 3
        if (status.eq.0) call ids_get( idx, "wall", vessel, status )
#else
        if (status.eq.0) call ids_get( idx, "wall", vessel )
#endif
        if (status.ne.0 .or.
     &      vessel%ids_properties%homogeneous_time < 0) then
          status = 0
          call imas_close( idx, status )
          if (status.ne.0) stop 'Error closing IMAS database !'
          call imas_open_env( treename, wall, wall_run,
     &     idx, username, trim(database)//'_MD', version, status )
#if UAL_MAJOR_VERSION > 3
          if (status.eq.0) call ids_get( idx, "wall", vessel, status )
#else
          if (status.eq.0) call ids_get( idx, "wall", vessel )
#endif
          if (status.eq.0 .and.
     &        vessel%ids_properties%homogeneous_time .ge. 0) then
            write(*,*) 'Got IDS wall data from: '
            write(*,'(2(a,i8),2(a,a24))')
     .       ' Shot: ', wall, ' Run: ', wall_run,
     .       ' User: ', trim(username),
     .       ' Database: ', trim(database)//'_MD'
          else
            status = 0
            call imas_close( idx, status )
            if (status.ne.0) stop 'Error closing IMAS database !'
            call imas_open_env( treename, wall, wall_run,
     &       idx, "public", database, version, status )
#if UAL_MAJOR_VERSION > 3
            if (status.eq.0) call ids_get( idx, "wall", vessel, status )
#else
            if (status.eq.0) call ids_get( idx, "wall", vessel )
#endif
            if (status.eq.0 .and.
     &          vessel%ids_properties%homogeneous_time .ge. 0) then
              write(*,*) 'Got IDS wall data from: '
              write(*,'(2(a,i8),2(a,a24))')
     .         ' Shot: ', wall, ' Run: ', wall_run,
     .         ' User: ', 'public', ' Database: ', trim(database)
            else
              status = 0
              call imas_close( idx, status )
              if (status.ne.0) stop 'Error closing IMAS database !'
              call imas_open_env( treename, wall, wall_run,
     &         idx, "public", trim(database)//'_MD', version, status )
#if UAL_MAJOR_VERSION > 3
              if (status.eq.0)
     &         call ids_get( idx, "wall", vessel, status )
#else
              if (status.eq.0) call ids_get( idx, "wall", vessel )
#endif
              if (status.eq.0 .and.
     &            vessel%ids_properties%homogeneous_time .ge. 0) then
                write(*,*) 'Got IDS wall data from: '
                write(*,'(2(a,i8),2(a,a24))')
     .           ' Shot: ', wall, ' Run: ', wall_run,
     .           ' User: ', 'public',
     .           ' Database: ', trim(database)//'_MD'
              else
                do_wall = .false.
                write(*,*) 'Error reading wall description IDS !'
              end if
            end if
          end if
        else
          write(*,*) 'Got IDS wall data from: '
          write(*,'(2(a,i8),2(a,a24))')
     .     ' Shot: ', wall, ' Run: ', wall_run,
     .     ' User: ', trim(username), ' Database: ', trim(database)
        end if
      else if (do_wall) then
#if UAL_MAJOR_VERSION > 3
        call ids_get( idx, "wall", vessel, status )
#else
        call ids_get( idx, "wall", vessel )
#endif
        if (status.eq.0 .and.
     &      vessel%ids_properties%homogeneous_time .ge. 0) then
          write(*,*) 'Got IDS wall data from: '
          write(*,'(2(a,i8),2(a,a24))')
     .     ' Shot: ', shot, ' Run: ', run,
     .     ' User: ', trim(username), ' Database: ', trim(database)
        else
          do_wall = .false.
          write(*,*) 'Error reading wall description IDS !'
        end if
      end if

      status = 0
      call imas_close( idx, status )
      if (status.ne.0) stop 'Error closing IMAS database !'

      if (do_equilibrium .and.
     &    eq%ids_properties%homogeneous_time < 0) then
        write(0,*) 'Equilibrium IDS not present !'
        iret = 1
        do_equilibrium = .false.
      end if
      if (do_wall .and.
     &    vessel%ids_properties%homogeneous_time < 0) then
        write(0,*) 'Wall IDS not present !'
        do_wall = .false.
        if (do_equilibrium .and. iret.ne.1) then
          write(0,*) 'Writing of template DG file skipped !'
        else
          iret = 2
          return
        end if
        iret = 2
      end if
      if (do_wall .or. do_equilibrium) write(0,*) "IDS read finished"

      nr = 0
      nz = 0
      ipestg = 0
      rcntc = 0.0_R8
      btorc = 0.0_R8
      psimin = 0.0_R8
      psilim = 0.0_R8
      rgr = 0.0_R8
      zgr = 0.0_R8
      pfm = 0.0_R8
      fg = 0.0_R8

      if (do_equilibrium) then
        if (step.le.0 .or. step.gt.size(eq%time_slice)) then
          write(0,*) 'Invalid time slice index !'
          write(0,*) 'Current equilibrium IDS contains ',
     .     size(eq%time_slice),' time slices.'
          iret = 1
          do_equilibrium = .false.
        end if

!! We take the 2nd occurence of the equilibrium, i.e. the SPIDER equilibrium, not CHEASE
        if (do_equilibrium) then
          if (eq%time_slice(step)%profiles_2d(1)%grid_type%index.ne.1)
     >     then
            write(0,*) 'IDS equilibrium grid is not rectangular !'
            write(0,*) 'Grid type : ',
     .       eq%time_slice(step)%profiles_2d(2)%grid_type%index
            iret = 1
            do_equilibrium = .false.
          end if
        end if

        if (.not.do_equilibrium) then
          if (do_wall) then
            write(0,*) 'Writing of equilibrium DG file skipped !'
          else
            return
          end if
        end if

        if (do_equilibrium) then
          nr = size(eq%time_slice(step)%profiles_2d(1)%grid%dim1)
          nz = size(eq%time_slice(step)%profiles_2d(1)%grid%dim2)
          ipestg = 3
          write(*,*) ' nr, nz : ', nr, nz
          if(nr.gt.ngpr) then
            write (6,'(a,i4,a,i4,a)')
     .       '=== rdefit: nr (',nr,') > ngpr (',ngpr,')'
            iret = 3
          end if
          if(nz.gt.ngpz) then
            write (6,'(a,i4,a,i4,a)')
     .       '=== rdefit: nz (',nz,') > ngpz (',ngpz,')'
            iret = 3
          end if
          if(nr.le.0) then
            write (6,'(a,i4,a)') '=== rdefit: nr (',nr,') < 1'
            iret = 4
          end if
          if(nz.le.0) then
            write (6,'(a,i4,a)') '=== rdefit: nz (',nz,') < 1'
            iret = 4
          end if
        end if
        if (iret.eq.3 .or. iret.eq.4) then
          do_equilibrium = .false.
          if (do_wall) then
            write(0,*) 'Writing of equilibrium DG file skipped !'
          else
            return
          end if
        end if
c
        if (do_equilibrium) then
          rcntc = eq%vacuum_toroidal_field%r0
          btorc = eq%vacuum_toroidal_field%b0(step)
          if (eq%time_slice(step)%global_quantities%psi_axis.ne.
     .     IDS_REAL_INVALID) then
            psimin = eq%time_slice(step)%global_quantities%psi_axis
          else
            psimin = 0.0_R8
          end if
          if (eq%time_slice(step)%global_quantities%psi_boundary.ne.
     .     IDS_REAL_INVALID) then
            psilim = eq%time_slice(step)%global_quantities%psi_boundary
          else
            psilim = 0.0_R8
          end if
          do i=1,nr
            rgr(i) = eq%time_slice(step)%profiles_2d(1)%grid%dim1(i)
            if (associated(eq%time_slice(step)%profiles_2d(1)%phi)) then
              fg(i) = eq%time_slice(step)%profiles_2d(1)%phi(i,1)
            else
              fg(i) = IDS_REAL_INVALID
            endif
          enddo
          do j=1,nz
            zgr(j) = eq%time_slice(step)%profiles_2d(1)%grid%dim2(j)
          enddo
          do i=1,nr
            do j=1,nz
              pfm(i,j) = eq%time_slice(step)%profiles_2d(1)%psi(i,j)
            enddo
          enddo
        end if
      end if
c
      npts = 0
      icnt = 0
      nunits = 0
      nlimunits = 0
      nmobunits = 0
      nvslunits = 0
      rwall = 0.0_R8
      zwall = 0.0_R8
c
      if (do_wall) then
#if IMAS_MINOR_VERSION > 32
        if (vessel%temperature_reference%data .ne. IDS_REAL_INVALID)
     .    write(0,*) 'Wall IDS has reference temperature ',
     .                vessel%temperature_reference%data, ' K'
#endif
        if (associated(vessel%description_2d(1)%limiter%unit)) then
          nlimunits = size(vessel%description_2d(1)%limiter%unit)
          nunits = nunits + nlimunits
          if (nlimunits.lt.0) then
            write(0,*)
     .       'Invalid number of limiter units !, nlimunits = ',
     .                                           nlimunits
            iret=5
            return
          end if
          if (nunits.gt.ngpr) then
            write(0,*)
     .       'Number of wall units too large !, nunits = ',
     .                                          nunits
            iret=5
            return
          end if
          do i=1,nlimunits
            npts(i)=
     .       size(vessel%description_2d(1)%limiter%unit(i)%outline%r)
            if (vessel%description_2d(1)%limiter%unit(i)%closed.eq.1)
     .       npts(i)=npts(i)+1
          end do
          do i=1,nlimunits
            if (icnt+npts(i).gt.ngpr) then
              write(0,*)
     .         'Number of wall points too large !, npts = ',
     .                                        icnt+npts(i)
              iret=5
              return
            end if
            do j=1,
     .         size(vessel%description_2d(1)%limiter%unit(i)%outline%r)
              rwall(icnt+j)=
     .         vessel%description_2d(1)%limiter%unit(i)%outline%r(j)
              zwall(icnt+j)=
     .         vessel%description_2d(1)%limiter%unit(i)%outline%z(j)
            end do
            if (vessel%description_2d(1)%limiter%unit(i)%closed.eq.1)
     >       then
              rwall(icnt+npts(i))=rwall(icnt+1)
              zwall(icnt+npts(i))=zwall(icnt+1)
            end if
            icnt = icnt + npts(i)
          end do
        end if
c
#if IMAS_MINOR_VERSION > 10
        if (associated(vessel%description_2d(1)%mobile%unit)) then
          nmobunits = size(vessel%description_2d(1)%mobile%unit)
          nunits = nunits + nmobunits
          if (nmobunits.lt.0) then
            write(0,*)
     .       'Invalid number of mobile units !, nmobunits = ',
     .                                          nmobunits
            iret=5
            return
          end if
          if (nunits.gt.ngpr) then
            write(0,*)
     .       'Number of wall units too large !, nunits = ',
     .                                          nunits
            iret=5
            return
          end if
          do i=nlimunits+1,nlimunits+nmobunits
            npts(i)=
     .     size(vessel%description_2d(1)%mobile%unit(i)%outline(step)%r)
            if (vessel%description_2d(1)%mobile%unit(i)%closed.eq.1)
     .       npts(i)=npts(i)+1
            if (icnt+npts(i).gt.ngpr) then
              write(0,*)
     .         'Number of wall points too large !, npts = ',
     .                                        icnt+npts(i)
              iret=5
              return
            end if
            do j=1,
     .     size(vessel%description_2d(1)%mobile%unit(i)%outline(step)%r)
              rwall(icnt+j)=
     .       vessel%description_2d(1)%mobile%unit(i)%outline(step)%r(j)
              zwall(icnt+j)=
     .       vessel%description_2d(1)%mobile%unit(i)%outline(step)%z(j)
            end do
            if (vessel%description_2d(1)%mobile%unit(i)%closed.eq.1)
     .       then
              rwall(icnt+npts(i))=rwall(icnt+1)
              zwall(icnt+npts(i))=zwall(icnt+1)
            end if
            icnt = icnt + npts(i)
          end do
        end if
#endif
c
        if (associated(vessel%description_2d(1)%vessel%unit)) then
          jcnt = nlimunits+nmobunits
          nvslunits = size(vessel%description_2d(1)%vessel%unit)
          if (nvslunits.lt.0) then
            write(0,*)
     .       'Invalid number of vessel elements !, nvslunits = ',
     .                                             nvslunits
            iret=5
            return
          end if
          do i=1,size(vessel%description_2d(1)%vessel%unit)
           if (associated(
     .          vessel%description_2d(1)%vessel%unit(i)%element)) then
            nunits = nunits +
     .       size(vessel%description_2d(1)%vessel%unit(i)%element)
            if (nunits.gt.ngpr) then
              write(0,*)
     .         'Number of wall units too large !, nunits = ',
     .                                            nunits
              iret=5
              return
            end if
            do j=1,size(vessel%description_2d(1)%vessel%unit(i)%element)
              jcnt = jcnt + 1
              npts(jcnt)=size(vessel%description_2d(1)%
     .                        vessel%unit(i)%element(j)%outline%r)
#if IMAS_MINOR_VERSION > 27
              if (vessel%description_2d(1)%
     .            vessel%unit(i)%element(j)%outline%closed.eq.1)
     .         npts(jcnt)=npts(jcnt)+1
#endif
              if (icnt+npts(jcnt).gt.ngpr) then
                write(0,*)
     .           'Number of wall points too large !, npts = ',
     .                                          icnt+npts(jcnt)
                iret=5
                return
              end if
              do k=1,size(vessel%description_2d(1)%
     .                    vessel%unit(i)%element(j)%outline%r)
                rwall(icnt+k)=vessel%description_2d(1)%
     .                        vessel%unit(i)%element(j)%outline%r(k)
                zwall(icnt+k)=vessel%description_2d(1)%
     .                        vessel%unit(i)%element(j)%outline%z(k)
              end do
#if IMAS_MINOR_VERSION > 27
              if (vessel%description_2d(1)%
     .            vessel%unit(i)%element(j)%outline%closed.eq.1) then
                rwall(icnt+npts(jcnt))=rwall(icnt+1)
                zwall(icnt+npts(jcnt))=zwall(icnt+1)
              end if
#endif
              icnt = icnt + npts(jcnt)
            end do
           end if
           if (associated(vessel%description_2d(1)%
     .               vessel%unit(i)%annular%outline_inner%r)) then
            if (size(vessel%description_2d(1)%
     .               vessel%unit(i)%annular%outline_inner%r).gt.0) then
              nunits = nunits + 1
              if (nunits.gt.ngpr) then
                write(0,*)
     .           'Number of wall units too large !, nunits = ',
     .                                              nunits
                iret=5
                return
              end if
              jcnt = jcnt + 1
              npts(jcnt)=size(vessel%description_2d(1)%
     .                        vessel%unit(i)%annular%outline_inner%r)
#if IMAS_MINOR_VERSION > 27
              if (vessel%description_2d(1)%
     .            vessel%unit(i)%annular%outline_inner%closed.eq.1)
     .         npts(jcnt)=npts(jcnt)+1
#else
              npts(jcnt)=npts(jcnt)+1 ! Outline is assumed closed in older DD versions
#endif
              if (icnt+npts(jcnt).gt.ngpr) then
                write(0,*)
     .           'Number of wall points too large !, npts = ',
     .                                          icnt+npts(jcnt)
                iret=5
                return
              end if
              do k=1,size(vessel%description_2d(1)%
     .                    vessel%unit(i)%annular%outline_inner%r)
                rwall(icnt+k)=vessel%description_2d(1)%
     .                        vessel%unit(i)%annular%outline_inner%r(k)
                zwall(icnt+k)=vessel%description_2d(1)%
     .                        vessel%unit(i)%annular%outline_inner%z(k)
              end do
#if IMAS_MINOR_VERSION > 27
              if (vessel%description_2d(1)%
     .            vessel%unit(i)%annular%outline_inner%closed.eq.1) then
                rwall(icnt+npts(jcnt))=rwall(icnt+1)
                zwall(icnt+npts(jcnt))=zwall(icnt+1)
              end if
#else
              rwall(icnt+npts(jcnt))=rwall(icnt+1)
              zwall(icnt+npts(jcnt))=zwall(icnt+1)
#endif
              icnt = icnt + npts(jcnt)
            end if
           end if
           if (associated(vessel%description_2d(1)%
     .               vessel%unit(i)%annular%outline_outer%r)) then
            if (size(vessel%description_2d(1)%
     .               vessel%unit(i)%annular%outline_outer%r).gt.0) then
              nunits = nunits + 1
              if (nunits.gt.ngpr) then
                write(0,*)
     .           'Number of wall units too large !, nunits = ',
     .                                              nunits
                iret=5
                return
              end if
              jcnt = jcnt + 1
              npts(jcnt)=size(vessel%description_2d(1)%
     .                        vessel%unit(i)%annular%outline_outer%r)
#if IMAS_MINOR_VERSION > 27
              if (vessel%description_2d(1)%
     .            vessel%unit(i)%annular%outline_outer%closed.eq.1)
     .         npts(jcnt)=npts(jcnt)+1
#else
              npts(jcnt)=npts(jcnt)+1 ! Outline is assumed closed in older DD versions
#endif
              if (icnt+npts(jcnt).gt.ngpr) then
                write(0,*)
     .           'Number of wall points too large !, npts = ',
     .                                          icnt+npts(jcnt)
                iret=5
                return
              end if
              do k=1,size(vessel%description_2d(1)%
     .                    vessel%unit(i)%annular%outline_outer%r)
                rwall(icnt+k)=vessel%description_2d(1)%
     .                        vessel%unit(i)%annular%outline_outer%r(k)
                zwall(icnt+k)=vessel%description_2d(1)%
     .                        vessel%unit(i)%annular%outline_outer%z(k)
              end do
#if IMAS_MINOR_VERSION > 27
              if (vessel%description_2d(1)%
     .            vessel%unit(i)%annular%outline_outer%closed.eq.1) then
                rwall(icnt+npts(jcnt))=rwall(icnt+1)
                zwall(icnt+npts(jcnt))=zwall(icnt+1)
              end if
#else
              rwall(icnt+npts(jcnt))=rwall(icnt+1)
              zwall(icnt+npts(jcnt))=zwall(icnt+1)
#endif
              icnt = icnt + npts(jcnt)
            end if
           end if
#if IMAS_MINOR_VERSION > 27
           if (associated(vessel%description_2d(1)%
     .               vessel%unit(i)%annular%centreline%r)) then
            if (size(vessel%description_2d(1)%
     .               vessel%unit(i)%annular%centreline%r).gt.0) then
              if (maxval(vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%thickness(:)).gt.0.0_R8
     .             .and. vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%centreline%closed.eq.1)
     .         then
                nvslunits = nvslunits + 1 ! We count the inner and outer contours
                                          ! for closed structures as two separate units
                nunits = nunits + 2
              else
                nunits = nunits + 1
              end if
              if (nunits.gt.ngpr) then
                write(0,*)
     .           'Number of wall units too large !, nunits = ',
     .                                              nunits
                iret=5
                return
              end if
              do iside = -1, 1, 2
                if (iside.eq.1 .and.
     .           maxval(vessel%description_2d(1)%
     .                  vessel%unit(i)%annular%thickness(:)).le.0.0_R8)
     .           cycle ! No thickness, so only one contour
                if (iside.eq.-1.or.vessel%description_2d(1)%vessel%
     .              unit(i)%annular%centreline%closed.eq.1)
     >           jcnt = jcnt + 1
                if (vessel%description_2d(1)%
     .              vessel%unit(i)%annular%centreline%closed.eq.1) then
                  npts(jcnt)=size(vessel%description_2d(1)%vessel%
     .                            unit(i)%annular%centreline%r)+1
                else
                  npts(jcnt)=2*size(vessel%description_2d(1)%vessel%
     .                              unit(i)%annular%centreline%r)+1
                end if
                if (icnt+npts(jcnt).gt.ngpr) then
                  write(0,*)
     .             'Number of wall points too large !, npts = ',
     .                                          icnt+npts(jcnt)
                  iret=5
                  return
                end if
cxpb We consider the k-th segment and the next segment, kk.
cxpb The k-th segment is [A,B] and the kk-th segment is [C,D]
cxpb A is point k, B/C is point kk, D is point kkk
cxpb When treating an open-ended structure, on the second pass,
cxpb points kk maps to point ks
cxpb After thickness mapping: [A,B] --> [1,2] and [C,D] --> [3,4]
cxpb We then correct point B/C mapping to point X
cxpb If [A,B] || [C,D], then
cxpb If thickness stayed constant, points 2 and 3 are the same and so is X
cxpb If thickness changed, X is mid-point of [2,3]
cxpb Otherwise, X is the intersection of (12) and (34)
                do k=1,size(vessel%description_2d(1)%
     .                      vessel%unit(i)%annular%centreline%r)-1
                  if (k.eq.size(vessel%description_2d(1)%
     .                      vessel%unit(i)%annular%centreline%r)-1) then
                    if (vessel%description_2d(1)%
     .                  vessel%unit(i)%annular%centreline%closed.eq.1)
     .               then
                      kk = 1
                      kkk = 2
                    else
                      kk = k+1
                      kkk = kk
                    end if
                  else
                    kk = k+1
                    kkk = kk+1
                  endif
                  if (iside.eq.1 .and. vessel%description_2d(1)%
     .             vessel%unit(i)%annular%centreline%closed.eq.0) then
                    ks = 2*size(vessel%description_2d(1)%vessel%
     .                          unit(i)%annular%centreline%r)+1-kk
                  else if (kk.eq.1) then
                    ks = size(vessel%description_2d(1)%vessel%
     .                        unit(i)%annular%centreline%r)
                  else
                    ks = kk
                  end if
                  ra = vessel%description_2d(1)%
     .                 vessel%unit(i)%annular%centreline%r(k)
                  za = vessel%description_2d(1)%
     .                 vessel%unit(i)%annular%centreline%z(k)
                  rb = vessel%description_2d(1)%
     .                 vessel%unit(i)%annular%centreline%r(kk)
                  zb = vessel%description_2d(1)%
     .                 vessel%unit(i)%annular%centreline%z(kk)
                  t1 = vessel%description_2d(1)%
     .                 vessel%unit(i)%annular%thickness(k)
                  rab_normal = (za-zb)/sqrt((za-zb)**2+(rb-ra)**2)
                  zab_normal = (rb-ra)/sqrt((za-zb)**2+(rb-ra)**2)
                  if (t1.eq.0.0_R8 .or. t1.eq.IDS_REAL_INVALID) then
                    r1 = ra
                    z1 = za
                    r2 = rb
                    z2 = zb
                  else
                    r1 = ra+rab_normal*(t1/2.)*iside
                    z1 = za+zab_normal*(t1/2.)*iside
                    r2 = rb+rab_normal*(t1/2.)*iside
                    z2 = zb+zab_normal*(t1/2.)*iside
                  end if
                  if (kk.ne.size(vessel%description_2d(1)%vessel%
     .                           unit(i)%annular%centreline%r).or.
     .                vessel%description_2d(1)%vessel%
     .                unit(i)%annular%centreline%closed.eq.1) then ! we are not at the open end
                    rc = vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%centreline%r(kk)
                    zc = vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%centreline%z(kk)
                    rd = vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%centreline%r(kkk)
                    zd = vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%centreline%z(kkk)
                    t2 = vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%thickness(kk)
                    rcd_normal = (zc-zd)/sqrt((zc-zd)**2+(rd-rc)**2)
                    zcd_normal = (rd-rc)/sqrt((zc-zd)**2+(rd-rc)**2)
                    if (t2.eq.0.0_R8 .or. t2.eq.IDS_REAL_INVALID) then
                      r3 = rc
                      z3 = zc
                      x4 = rd
                      z4 = zd
                    else
                      r3 = rc+rcd_normal*(t2/2.)*iside
                      z3 = zc+zcd_normal*(t2/2.)*iside
                      x4 = rd+rcd_normal*(t2/2.)*iside
                      z4 = zd+zcd_normal*(t2/2.)*iside
                    end if
                    det = (r3-x4)*(z2-z1) - (r2-r1)*(z3-z4)
                    if (r2.eq.r3 .and. z2.eq.z3) then
                      rwall(icnt+ks)=r2
                      zwall(icnt+ks)=z2
                    else if (abs(det).le.1.0e-6) then ! [AB] || [BC] case
                      rwall(icnt+ks)=(r2+r3)/2.0_R8
                      zwall(icnt+ks)=(z2+z3)/2.0_R8
                    else ! Intersection coordinates
                      rwall(icnt+ks)=((r3-x4)*(r1*(z2-z1)-z1*(r2-r1))
     .                               -(r2-r1)*(x4*(z3-z4)-z4*(r3-x4)))
     .                               /det
                      zwall(icnt+ks)=((z3-z4)*(r1*(z2-z1)-z1*(r2-r1))
     .                               -(z2-z1)*(x4*(z3-z4)-z4*(r3-x4)))
     .                               /det
                    end if
                    if (kk.eq.1) then ! The last and first points coincide
                      kks=size(vessel%description_2d(1)%
     .                         vessel%unit(i)%annular%centreline%r)+1
                      rwall(icnt+kk)=rwall(icnt+ks)
                      zwall(icnt+kk)=zwall(icnt+ks)
                      rwall(icnt+kks)=rwall(icnt+ks)
                      zwall(icnt+kks)=zwall(icnt+ks)
                    end if
                  else
                    rwall(icnt+ks)=r2
                    zwall(icnt+ks)=z2
! Going back to the first point since this is an open-ended segment
                    ra = vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%centreline%r(1)
                    za = vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%centreline%z(1)
                    rb = vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%centreline%r(2)
                    zb = vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%centreline%z(2)
                    t1 = vessel%description_2d(1)%
     .                   vessel%unit(i)%annular%thickness(1)
                    rab_normal = (za-zb)/sqrt((za-zb)**2+(rb-ra)**2)
                    zab_normal = (rb-ra)/sqrt((za-zb)**2+(rb-ra)**2)
                    r1 = ra+rab_normal*(t1/2.)*iside
                    z1 = za+zab_normal*(t1/2.)*iside
                    if (iside.eq.-1) then
                      rwall(icnt+1)=r1
                      zwall(icnt+1)=z1
                      rwall(icnt+npts(jcnt))=r1
                      zwall(icnt+npts(jcnt))=z1
                    else if (iside.eq.1) then
                      rwall(icnt+npts(jcnt)-1)=r1
                      zwall(icnt+npts(jcnt)-1)=z1
                    endif
                  endif
                end do
                if (iside.eq.1 .or. vessel%description_2d(1)%vessel%
     .                          unit(i)%annular%centreline%closed.eq.1)
     .           icnt = icnt + npts(jcnt)
              end do
            end if
           end if
#endif
          end do
        end if
      end if
c
      if (do_equilibrium) call ids_deallocate( eq )
      if (do_wall) call ids_deallocate( vessel )
c
      end
