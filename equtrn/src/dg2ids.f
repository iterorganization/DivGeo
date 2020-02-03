!> Translation of DG template into a wall IDS
!>
!!
!!      In terminal, run the following command:
!!
!!      @verbatim
!!          $dg2ids --shot <shot> --run <run> --device <device> --version <version>
!!          <DG input template file>
!!      @endverbatim
!!
!!      The command can be shortened as:
!!
!!      @verbatim
!!          $dg2ids -s <shot> -r <run> -d <device> -v <version> <DG input template file>
!!      @endverbatim
!!
!!      The arguments marked with < ... > are the parameters of the wall IDS
!!      where the data is to be stored:
!!          - \b shot:     The shot number of the wall IDS being written
!!          - \b run:      The run number of the wall IDS being written
!!          - \b device:   Device name of the IMAS IDS database
!!                         (i. e. solps-iter, iter, aug) (default: $DEVICE)
!!          - \b version:  Major version of the IMAS IDS database (default: 3)
!!      Example of the command:
!!      @verbatim
!!          $dg2ids
!!          --shot 10 --run 3 --device iter --version 3 DG_template_file
!!      @endverbatim
!!
!!-----------------------------------------------------------------------------

!>
!> \version  03.02.20 13:42

      program dg2ids
c
c  version : 03.02.20 13:42
c
c=====================================================
c*** Translation of DG template file into wall IDS
c=====================================================
      implicit none
#include "eqdim.inc"
      integer iret
      real(kind=R8) :: rwall(ngpr), zwall(ngpr)
      integer :: nunits, npts(ngpr)

    !! Local variables
      character(len=24) :: treename   !< The name of the IMAS IDS database
      character(len=24) :: username   !< Creator/owner of the IMAS IDS database
      character(len=24) :: device     !< Device name of the IMAS IDS database
        !< (i. e. solps-iter, iter, aug)
      character(len=24) :: version    !< Major version of the IMAS IDS database
      integer :: shot      !< The shot number of the wall IDS being written
      integer :: run       !< The run number of the wall IDS being written
#ifndef NO_GETENV
      character(len=24) :: device_env
#endif
    !! Dummy variables
      character(len=24) :: shot_string
      character(len=24) :: run_string
      character(len=24) :: argName
      character(len=256) :: dg_file
#ifdef USE_PXFGETENV
      integer lenval, ierror
#else
#ifdef NAGFOR
      integer lenval, ierror
#endif
#endif
      integer narg, cptArg
      character*24 usrnam
      logical streql
      external usrnam, streql

c=====================================================
c
    !! Set default values
      iret = 0
      treename = 'ids'
      version = "3"
      username = usrnam()
      device = 'solps-iter'
      run_string = ' '
      shot_string = ' '
#ifndef NO_GETENV
      device_env = ' '
#ifdef NAGFOR
      call get_environment_variable
     . ('DEVICE', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('DEVICE',value=device_env)
#else
#ifdef USE_PXFGETENV
      CALL PXFGETENV ('DEVICE', 0, device_env, lenval, ierror)
#else
      call getenv ('DEVICE', device_env)
#endif
#endif
      if (.not.streql(device_env,' ')) device = device_env
#endif
      
    !! Check if arguments are found
      narg = command_argument_count()

      if( narg > 0 ) then
        !! Loop across arguments and allocate proper values to variables
        do cptArg = 1, narg
          call get_command_argument( cptArg, argName )
          if ( cptArg.eq.narg ) dg_file = argName
          select case( adjustl( argName ) )
            case("--shot","-s")
              call get_command_argument( cptArg + 1, shot_string )
              !! Transform dummy string variable to integer
              read( shot_string, *) shot
            case("--run","-r")
              call get_command_argument( cptArg + 1, run_string )
              !! Transform dummy string variable to integer
              read( run_string, *) run
            case("--username","-u")
              call get_command_argument( cptArg + 1, username )
            case("--device","-d")
              call get_command_argument( cptArg + 1, device )
            case("--version","-v")
              call get_command_argument( cptArg + 1, version )
          end select
        end do
    !! If not at least shot, run, username and device were defined display
    !! the error message and and a full command example
      else if( narg.lt.5 .or. mod(narg,2).eq.0 .or. 
     &  streql(shot_string," ") .or. streql(run_string," ") ) then
        write(0,*) "ERROR! In order to run dg2ids input IDS, ",
     &   "at least the shot and run variables and ",
     &   "the DG input template file must be defined. ",
     &   "Example (terminal): "
        write(0,*) "dg2ids --shot 1 --run 1 <DG_template_file>"
        write(0,*) "Other options are :"
        write(0,*) " --username, --device, --version"
        call exit(0)
      end if
      if (.not.(0.lt.shot.and.shot.le.214748)) then
        write(0,*) 'Invalid shot number for equilibrium IDS'
        call exit(0)
      end if
      if (.not.(0.le.run.and.run.le.9999)) then
        write(0,*) 'Invalid equilibrium IDS run number'
        call exit(0)
      end if
      open(1,file=trim(dg_file),iostat=iret)
      if(iret.ne.0) then
        print *,'==== dg2ids: error in opening ',trim(dg_file),
     .          '. iret = ',iret
        goto 999
      end if

      write(*,'(a,i8,a,i8,4a)') 'Shot: ', shot, ' Run: ', run,
     & ' User: ', trim(username), ' Device: ', trim(device)

      call rdtrg(1,iret,nunits,npts,rwall,zwall)
      if(iret.ne.0) then
        print *,'==== dg2ids: error in rdtrg. iret =',iret
        goto 999
      end if
c
      rwall = rwall / 1000.0_R8
      zwall = zwall / 1000.0_R8
      call wrids(iret,nunits,npts,rwall,zwall,dg_file,
     .           treename,shot,run,username,device,version)
      if(iret.ne.0) then
        print *,'==== dg2ids: error in wrids. iret = ',iret
      end if
c
  999 continue
      close(1)
      stop
      end
