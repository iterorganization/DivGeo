      subroutine wrids(iret,nunits,npts,rwall,zwall,ref_temp,dg_file,
#if AL_MAJOR_VERSION > 4
     ,           username,ids_path)
#else
     ,           treename,shot,run,username,database,version)
#endif
      use ids_schemas  ! IGNORE
     , , only : ids_wall
#if ( IMAS_MINOR_VERSION > 8 || IMAS_MAJOR_VERSION > 3 )
      use ids_schemas  ! IGNORE
     , , only : ids_real
#endif
      use ids_routines ! IGNORE
     , , only : ids_deallocate, ids_put, imas_close
#if ( IMAS_MAJOR_VERSION > 4 || ( IMAS_MAJOR_VERSION == 4 && IMAS_MINOR_VERSION > 0 ) )
      use al_wall_component_identifier              ! IGNORE
     , , only : set_identifier => set_wall_component_identifier
      use al_wall_description_2d_type_identifier    ! IGNORE
     , , only : set_wall_description_identifier
#endif
#if ( IMAS_MINOR_VERSION > 39 || IMAS_MAJOR_VERSION > 3 )
#if AL_MAJOR_VERSION > 4
      use al_wall_component_identifier              ! IGNORE
     , , only : wall_component_identifier
      use al_wall_description_2d_type_identifier    ! IGNORE
     , , only : wall_description_2d_type_identifier
#else
      use imas_wall_component_identifier            ! IGNORE
     , , only : wall_component_identifier
      use imas_wall_description_2d_type_identifier  ! IGNORE
     , , only : wall_description_2d_type_identifier
#endif
#endif
#if AL_MAJOR_VERSION > 4
      use ids_routines ! IGNORE
     & , only : imas_open, FORCE_CREATE_PULSE, STRMAXLEN
#else
      use ids_routines ! IGNORE
     & , only : imas_create_env
#endif
      use eqdim
      implicit none
#include "git_version_DG.h"
      type (ids_wall) :: vessel   !< IDS designed to store wall data
      character(len=256), intent(in) :: dg_file   !< DG template input file name
      character(len=24), intent(in) :: username   !< Creator/owner of the IMAS IDS database
#if AL_MAJOR_VERSION > 4
      character(len=256) :: ids_path   !< The path of the directory where the IDS is written
#else
      character(len=24), intent(in) :: treename   !< The name of the IMAS IDS database
      integer, intent(in) :: shot      !< The pulse (previously shot) number of the wall IDS being written
      integer, intent(in) :: run       !< The run number of the wall IDS being written
      character(len=24), intent(in) :: database   !< IMAS IDS database name
            !< (i. e. solps-iter, ITER, aug)
      character(len=24), intent(in) :: version    !< Major version of the IMAS IDS database
#endif
      integer, intent(out) :: iret
      real(kind=R8), intent(in) :: rwall(ngpr), zwall(ngpr), ref_temp
      integer, intent(in) :: nunits, npts(ngpr)
      character*8 ggd_version
      character*8 date
      character*10 ctime
      character*5 zone
      integer tvalues(8)
      integer :: idx, i, j, icnt, jcnt, status
#if ( IMAS_MINOR_VERSION < 9 && IMAS_MAJOR_VERSION < 4 )
      integer, parameter :: IDS_REAL = R8
#endif
#if AL_MAJOR_VERSION > 4
      character(len=:), allocatable :: message
      character(len=STRMAXLEN) :: uri
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
      write(ggd_version,'(i1,a1,i2,a1,i1)') GGD_MAJOR_VERSION,'.',
     &                                      GGD_MINOR_VERSION,'.',
     &                                      GGD_MICRO_VERSION
#elif defined(USE_PXFGETENV)
      CALL PXFGETENV ('GGD_VERSION', 0, ggd_version, lenval, ierror)
#else
      call get_environment_variable
     . ('GGD_VERSION', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('GGD_VERSION', value=ggd_version)
#endif

      vessel%ids_properties%homogeneous_time = 2
      allocate( vessel%ids_properties%comment(1) )
      vessel%ids_properties%comment = "DivGeo template"
#if ( IMAS_MINOR_VERSION > 14 || IMAS_MAJOR_VERSION > 3 )
#if ( IMAS_MINOR_VERSION > 33 || IMAS_MAJOR_VERSION > 3 )
      allocate( vessel%ids_properties%provenance%node(1) )
#if ( IMAS_MINOR_VERSION > 41 || IMAS_MAJOR_VERSION > 3 )
      allocate( vessel%ids_properties%provenance%node(1)%reference(1) )
      allocate(
     . vessel%ids_properties%provenance%node(1)%reference(1)%name(1) )
      vessel%ids_properties%provenance%node(1)%reference(1)%name(1) =
     . dg_file
#else
      allocate( vessel%ids_properties%provenance%node(1)%sources(1) )
      vessel%ids_properties%provenance%node(1)%sources(1) = dg_file
#endif
#else
      allocate( vessel%ids_properties%source(1) )
      vessel%ids_properties%source = dg_file
#endif
      allocate( vessel%ids_properties%provider(1) )
      vessel%ids_properties%provider = username
      allocate( vessel%ids_properties%creation_date(1) )
      vessel%ids_properties%creation_date =
     & date//' '//ctime//' '//' '//zone
#endif
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
#if ( IMAS_MAJOR_VERSION > 4 || ( IMAS_MAJOR_VERSION == 4 && IMAS_MINOR_VERSION > 0 ) )
      call set_wall_description_identifier(
     . vessel%description_2d(1)%type, "multiple_units_no_vessel" )
#elif ( IMAS_MINOR_VERSION > 39 || IMAS_MAJOR_VERSION > 3 )
      allocate( vessel%description_2d(1)%type%name(1) )
      allocate( vessel%description_2d(1)%type%description(1) )
      vessel%description_2d(1)%type%index =
     . wall_description_2d_type_identifier%multiple_units_no_vessel
      vessel%description_2d(1)%type%name =
     . wall_description_2d_type_identifier%name(
     . wall_description_2d_type_identifier%multiple_units_no_vessel )
      vessel%description_2d(1)%type%description =
     . wall_description_2d_type_identifier%description(
     . wall_description_2d_type_identifier%multiple_units_no_vessel )
#else
      allocate( vessel%description_2d(1)%type%name(1) )
      allocate( vessel%description_2d(1)%type%description(1) )
      vessel%description_2d(1)%type%index = 2
      vessel%description_2d(1)%type%name = "DG template"
      vessel%description_2d(1)%type%description =
     . "DivGeo template file "//trim(dg_file)
#endif
      allocate( vessel%description_2d(1)%limiter%type%name(1) )
      allocate( vessel%description_2d(1)%limiter%type%description(1) )
      vessel%description_2d(1)%limiter%type%index = 1
      vessel%description_2d(1)%limiter%type%name = "DG structure"
      vessel%description_2d(1)%limiter%type%description =
     . "DivGeo structure from template file "//trim(dg_file)

      icnt = 0
      jcnt = 0
      allocate( vessel%description_2d(1)%limiter%unit(nunits) )
      do i = 1, nunits
        allocate( vessel%description_2d(1)%limiter%unit(i)%name(1) )
        vessel%description_2d(1)%limiter%unit(i)%name =
     .   "DivGeo structure number "//int2str(i)
#if ( IMAS_MAJOR_VERSION > 4 || ( IMAS_MAJOR_VERSION == 4 && IMAS_MINOR_VERSION > 0 ) )
        call set_wall_component_identifier(
     &   vessel%description_2d(1)%limiter%unit(i)%component_type,
     &   "other" )
#elif ( IMAS_MINOR_VERSION > 39 || IMAS_MAJOR_VERSION > 3 )
        vessel%description_2d(1)%limiter%unit(i)%component_type%
     &   index = wall_component_identifier%other
        allocate( vessel%description_2d(1)%limiter%unit(i)%
     &   component_type%name(1) )
        vessel%description_2d(1)%limiter%unit(i)%component_type%
     &   name = wall_component_identifier%name(
     &   wall_component_identifier%other )
        allocate( vessel%description_2d(1)%limiter%unit(i)%
     &   component_type%description(1) )
        vessel%description_2d(1)%limiter%unit(i)%component_type%
     &   description = wall_component_identifier%description(
     &   wall_component_identifier%other )
#endif
        jcnt = icnt + npts(i)
#if IMAS_MAJOR_VERSION < 4
        if (rwall(icnt+1).eq.rwall(jcnt) .and.
     &      zwall(icnt+1).eq.zwall(jcnt)) then
#if ( IMAS_MAJOR_VERSION != 3 || IMAS_MINOR_VERSION != 40 || IMAS_MICRO_VERSION != 0 )
          vessel%description_2d(1)%limiter%unit(i)%closed = 1
#else
          stop "This structure cannot be described with DD/3.40.0!"
#endif
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
#if ( IMAS_MAJOR_VERSION != 3 || IMAS_MINOR_VERSION != 40 || IMAS_MICRO_VERSION != 0 )
          vessel%description_2d(1)%limiter%unit(i)%closed = 0
#endif
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
#else
        allocate(
     .   vessel%description_2d(1)%limiter%unit(i)%outline%r(npts(i)) )
        allocate(
     .   vessel%description_2d(1)%limiter%unit(i)%outline%z(npts(i)) )
        do j = icnt+1, jcnt
          vessel%description_2d(1)%limiter%unit(i)%outline%r(j-icnt)
     .     = rwall(j)
          vessel%description_2d(1)%limiter%unit(i)%outline%z(j-icnt)
     .     = zwall(j)
        end do
#endif
        icnt = jcnt
      end do

      !! Create and modify new wall IDS
#if AL_MAJOR_VERSION > 4
      uri = 'imas:mdsplus?path='//trim(ids_path)
      call imas_open( uri, FORCE_CREATE_PULSE, idx, status, message )
      if (status.ne.0) then
        write(0,*) trim(message)
        stop
      end if
#else
      call imas_create_env( treename, shot, run,
     & 0, 0, idx, username, database, version, status )
      if (status.ne.0) stop 'Error opening IMAS database !'
#endif

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
      end subroutine wrids
