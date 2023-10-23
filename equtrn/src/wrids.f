      subroutine wrids(iret,nunits,npts,rwall,zwall,ref_temp,dg_file,
     ,           treename,shot,run,username,database,version)
      use ids_schemas  ! IGNORE
     , , only : ids_wall
#if ( IMAS_MINOR_VERSION > 8 || IMAS_MAJOR_VERSION > 3 )
      use ids_schemas  ! IGNORE
     , , only : ids_real
#endif
      use ids_routines ! IGNORE
     , , only : imas_create_env, ids_deallocate, ids_put, imas_close
      use eqdim
      implicit none
#include "git_version_DG.h"
      type (ids_wall) :: vessel   !< IDS designed to store wall data
      character(len=256), intent(in) :: dg_file   !< DG template input file name
      character(len=24), intent(in) :: treename   !< The name of the IMAS IDS database
      integer, intent(in) :: shot      !< The shot number of the wall IDS being written
      integer, intent(in) :: run       !< The run number of the wall IDS being written
      character(len=24), intent(in) :: username   !< Creator/owner of the IMAS IDS database
      character(len=24), intent(in) :: database   !< IMAS IDS database name
            !< (i. e. solps-iter, ITER, aug)
      character(len=24), intent(in) :: version    !< Major version of the IMAS IDS database
      integer, intent(out) :: iret
      real(kind=R8), intent(in) :: rwall(ngpr), zwall(ngpr), ref_temp
      integer, intent(in) :: nunits, npts(ngpr)
      character*8 imas_version, al_version, ggd_version
      character*8 date
      character*10 ctime
      character*5 zone
      integer tvalues(8)
      integer :: idx, i, j, icnt, jcnt, status
#if ( IMAS_MINOR_VERSION < 9 && IMAS_MAJOR_VERSION < 4 )
      integer, parameter :: IDS_REAL = R8
#endif
#ifndef NO_GETENV
      integer lenval, ierror
#ifndef USE_PXFGETENV
      intrinsic get_environment_variable
#endif
#endif
c=====================================================
c
      iret=0

      !! Collect metadata
      call date_and_time (date, ctime, zone, tvalues)
#ifdef NO_GETENV
      write(imas_version,'(i1,a1,i2,a1,i1)') IMAS_MAJOR_VERSION,'.',
     &                                       IMAS_MINOR_VERSION,'.',
     &                                       IMAS_MICRO_VERSION
      write(al_version,'(i1,a1,i2,a1,i1)') AL_MAJOR_VERSION,'.',
     &                                     AL_MINOR_VERSION,'.',
     &                                     AL_MICRO_VERSION
      write(ggd_version,'(i1,a1,i2,a1,i1)') GGD_MAJOR_VERSION,'.',
     &                                      GGD_MINOR_VERSION,'.',
     &                                      GGD_MICRO_VERSION
#elif defined(USE_PXFGETENV)
      CALL PXFGETENV ('IMAS_VERSION', 0, imas_version, lenval, ierror)
#if AL_MAJOR_VERSION > 4
      CALL PXFGETENV ('AL_VERSION', 0, al_version, lenval, ierror)
#else
      CALL PXFGETENV ('UAL_VERSION', 0, al_version, lenval, ierror)
#endif
      CALL PXFGETENV ('GGD_VERSION', 0, ggd_version, lenval, ierror)
#else
      call get_environment_variable
     . ('IMAS_VERSION', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('IMAS_VERSION', value=imas_version)
#if AL_MAJOR_VERSION > 4
      call get_environment_variable
     . ('AL_VERSION', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('AL_VERSION', value=al_version)
#else
      call get_environment_variable
     . ('UAL_VERSION', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('UAL_VERSION', value=al_version)
#endif
      call get_environment_variable
     . ('GGD_VERSION', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('GGD_VERSION', value=ggd_version)
#endif

      vessel%ids_properties%homogeneous_time = 1
      allocate( vessel%ids_properties%comment(1) )
      vessel%ids_properties%comment = "DivGeo template"
#if ( IMAS_MINOR_VERSION > 14 || IMAS_MAJOR_VERSION > 3 )
#if ( IMAS_MINOR_VERSION > 33 || IMAS_MAJOR_VERSION > 3 )
      allocate( vessel%ids_properties%provenance%node(1) )
      allocate( vessel%ids_properties%provenance%node(1)%sources(1) )
      vessel%ids_properties%provenance%node(1)%sources(1) = dg_file
#else
      allocate( vessel%ids_properties%source(1) )
      vessel%ids_properties%source = dg_file
#endif
      allocate( vessel%ids_properties%provider(1) )
      vessel%ids_properties%provider = username
      allocate( vessel%ids_properties%creation_date(1) )
      vessel%ids_properties%creation_date =
     & date//' '//ctime//' '//' '//zone
#if ( IMAS_MINOR_VERSION > 21 || IMAS_MAJOR_VERSION > 3 )
      allocate( vessel%ids_properties%version_put%data_dictionary(1) )
      vessel%ids_properties%version_put%data_dictionary = imas_version
      allocate( vessel%ids_properties%version_put%access_layer(1) )
      vessel%ids_properties%version_put%access_layer = al_version
      allocate(
     & vessel%ids_properties%version_put%access_layer_language(1) )
      vessel%ids_properties%version_put%access_layer_language =
     & 'FORTRAN'
#endif
#endif
      allocate( vessel%time(1) )
      vessel%time(1) = 0.0_IDS_real
#if ( IMAS_MINOR_VERSION > 32 || IMAS_MAJOR_VERSION > 3 )
      if (ref_temp.gt.0.0_R8) then
        vessel%temperature_reference%data = ref_temp
        allocate( vessel%temperature_reference%description(1) )
        vessel%temperature_reference%description =
     &   'DG2IDS input temperature'
      end if
#endif
      allocate( vessel%code%name(1) )
      vessel%code%name = "DivGeo"
      allocate( vessel%code%version(1) )
      vessel%code%version = "2.11"
      allocate( vessel%code%commit(1) )
      vessel%code%commit = GIT_VERSION_DG
      allocate( vessel%code%repository(1) )
      vessel%code%repository = "ssh://git.iter.org/bnd/divgeo.git"
      allocate( vessel%code%output_flag(1) )
      vessel%code%output_flag(1) = 0
#if ( IMAS_MINOR_VERSION > 29 || IMAS_MAJOR_VERSION > 3 )
      allocate( vessel%code%library(1) )
      allocate( vessel%code%library(1)%name(1) )
      vessel%code%library(1)%name = 'GGD'
      allocate( vessel%code%library(1)%version(1) )
      vessel%code%library(1)%version = ggd_version
      allocate( vessel%code%library(1)%repository(1) )
      vessel%code%library(1)%repository =
     . "ssh://git.iter.org/imex/ggd.git"
#endif

      allocate( vessel%description_2d(1) )
      allocate( vessel%description_2d(1)%type%name(1) )
      vessel%description_2d(1)%type%name = "DG template"
      vessel%description_2d(1)%type%index = 1
      allocate( vessel%description_2d(1)%type%description(1) )
      vessel%description_2d(1)%type%description =
     . "DivGeo template file "//trim(dg_file)
      vessel%description_2d(1)%limiter%type%index = 1

      icnt = 0
      jcnt = 0
      allocate( vessel%description_2d(1)%limiter%unit(nunits) )
      do i = 1, nunits
        jcnt = icnt + npts(i)
        if (rwall(icnt+1).eq.rwall(jcnt) .and.
     &      zwall(icnt+1).eq.zwall(jcnt)) then
          vessel%description_2d(1)%limiter%unit(i)%closed = 1
          allocate(vessel%
     .     description_2d(1)%limiter%unit(i)%outline%r(npts(i)-1) )
          allocate(vessel%
     .     description_2d(1)%limiter%unit(i)%outline%z(npts(i)-1) )
          do j = icnt+1, jcnt-1
            vessel%description_2d(1)%limiter%unit(i)%outline%r(j-icnt)
     .       = rwall(j)
            vessel%description_2d(1)%limiter%unit(i)%outline%z(j-icnt)
     .       = zwall(j)
          end do
        else
          vessel%description_2d(1)%limiter%unit(i)%closed = 0
          allocate(
     .     vessel%description_2d(1)%limiter%unit(i)%outline%r(npts(i)) )
          allocate(
     .     vessel%description_2d(1)%limiter%unit(i)%outline%z(npts(i)) )
          do j = icnt+1, jcnt
            vessel%description_2d(1)%limiter%unit(i)%outline%r(j-icnt)
     .       = rwall(j)
            vessel%description_2d(1)%limiter%unit(i)%outline%z(j-icnt)
     .       = zwall(j)
          end do
        end if
        icnt = jcnt
      end do

      !! Create and modify new wall IDS
      call imas_create_env( treename, shot, run,
     & 0, 0, idx, username, database, version, status )
      if (status.ne.0) stop 'Error opening IMAS database !'

#if AL_MAJOR_VERSION > 3
      call ids_put( idx, "wall", vessel, status )
      if (status.ne.0) stop 'Error putting wall description IDS !'
#else
      call ids_put( idx, "wall", vessel )
#endif
      call ids_deallocate( vessel )
      call imas_close( idx, status )
      if (status.ne.0) stop 'Error closing IMAS database !'

      write(0,*) "Wall IDS write finished"
      
      return
      end
