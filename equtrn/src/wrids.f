      subroutine wrids(iret,nunits,npts,rwall,zwall,dg_file,
     ,           treename,shot,run,username,device,version)
      use ids_schemas  ! IGNORE
      use ids_routines ! IGNORE
      implicit none
#include "git_version_DG.h"
#include "eqdim.inc"
      type (ids_wall) :: vessel   !< IDS designed to store wall data
      character(len=256), intent(in) :: dg_file   !< DG template input file name
      character(len=24), intent(in) :: treename   !< The name of the IMAS IDS database
      integer, intent(in) :: shot      !< The shot number of the wall IDS being written
      integer, intent(in) :: run       !< The run number of the wall IDS being written
      character(len=24), intent(in) :: username   !< Creator/owner of the IMAS IDS database
      character(len=24), intent(in) :: device     !< Device name of the IMAS IDS database
            !< (i. e. solps-iter, iter, aug)
      character(len=24), intent(in) :: version    !< Major version of the IMAS IDS database
      integer, intent(out) :: iret
      real(kind=R8), intent(in) :: rwall(ngpr), zwall(ngpr)
      integer, intent(in) :: nunits, npts(ngpr)
      character*8 imas_version, ual_version
      character*24 dg_version
      character*8 date
      character*10 ctime
      character*5 zone
      integer tvalues(8)
      integer :: idx, i, j, icnt, jcnt
#ifdef USE_PXFGETENV
      integer lenval, ierror
#else
#ifdef NAGFOR
      integer lenval, ierror
#endif
#endif
c=====================================================
c
      iret=0

      !! Collect metadata
      call date_and_time (date, ctime, zone, tvalues)
#ifdef NAGFOR
      call get_environment_variable
     . ('IMAS_VERSION',status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('IMAS_VERSION',value=imas_version)
      call get_environment_variable
     . ('UAL_VERSION',status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('UAL_VERSION',value=ual_version)
      call get_environment_variable
     . ('GIT_VERSION_DG',status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('GIT_VERSION_DG',value=dg_version)
#else
#ifdef USE_PXFGETENV
      CALL PXFGETENV ('IMAS_VERSION', 0, imas_version, lenval, ierror)
      CALL PXFGETENV ('UAL_VERSION', 0, ual_version, lenval, ierror)
      CALL PXFGETENV ('GIT_VERSION_DG', 0, dg_version, lenval, ierror)
#else
      call getenv ('IMAS_VERSION', imas_version)
      call getenv ('UAL_VERSION', ual_version)
      call getenv ('GIT_VERSION_DG', dg_version)
#endif
#endif

      vessel%ids_properties%homogeneous_time = 1
      allocate( vessel%ids_properties%comment(1) )
      vessel%ids_properties%comment = "DivGeo template"
      allocate( vessel%ids_properties%source(1) )
      vessel%ids_properties%source = dg_file
      allocate( vessel%ids_properties%provider(1) )
      vessel%ids_properties%provider = username
      allocate( vessel%ids_properties%creation_date(1) )
      vessel%ids_properties%creation_date =
     & date//' '//ctime//' '//' '//zone
      allocate( vessel%ids_properties%version_put%data_dictionary(1) )
      vessel%ids_properties%version_put%data_dictionary = imas_version
      allocate( vessel%ids_properties%version_put%access_layer(1) )
      vessel%ids_properties%version_put%access_layer = ual_version
      allocate(
     & vessel%ids_properties%version_put%access_layer_language(1) )
      vessel%ids_properties%version_put%access_layer_language =
     & 'FORTRAN'
      allocate( vessel%time(1) )
      vessel%time(1) = 0.0_IDS_real
      allocate( vessel%code%name(1) )
      vessel%code%name = "DivGeo"
      allocate( vessel%code%version(1) )
      vessel%code%version = "2.11"
      allocate( vessel%code%commit(1) )
      vessel%code%commit = dg_version
      allocate( vessel%code%repository(1) )
      vessel%code%repository = "git.iter.org"
      allocate( vessel%code%output_flag(1) )
      vessel%code%output_flag(1) = 0

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
     &      zwall(icnt+1).eq.rwall(jcnt)) then
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
     & 0, 0, idx, username, device, version )

      call ids_put( idx, "wall", vessel )
      call ids_deallocate( vessel )
      call imas_close( idx )

      write(0,*) "Wall IDS write finished"
      
      end
