!> Translation of IDS equilibrium and wall data into dg compatible format
!>
!!
!!      In terminal, run the following command:
!!
!!      @verbatim
!!          $ids2dg --shot <shot> --run <run> --wall <wall> --wall_run <wall_run>
!!          --step <step> --username <username> --device <device> --version <version>
!!          <DG output filename stem>
!!      @endverbatim
!!
!!      The command can be shortened as:
!!
!!      @verbatim
!!          $ids2dg -s <shot> -r <run> -w <wall> -R <wall_run>
!!          -S <step> -u <username> -d <device> -v <version> <DG output filename stem>
!!      @endverbatim
!!
!!      The arguments marked with < ... > are the parameters of the IDS database
!!      where the data is to be stored:
!!          - \b shot:     The shot number of the equilibrium IDS being read
!!                         (if the number is negative, no equilibrium will be translated)
!!          - \b run:      The run number of the equilibrium IDS being read
!!          - \b wall:     The shot number of the wall IDS being read (default: same as <shot>)
!!                         (if the number is negative, no wall description will be translated)
!!          - \b wall_run: The run number of the wall IDS being read (default: same as <run>)
!!          - \b step:     The time slice index for the equilibrium IDS (default: 1)
!!          - \b username: Creator/owner of the IMAS IDS database (default: $USER)
!!          - \b device:   Device name of the IMAS IDS database
!!                         (i. e. solps-iter, iter, aug) (default: $DEVICE)
!!          - \b version:  Major version of the IMAS IDS database (default: 3)
!!      Example of the command:
!!      @verbatim
!!          $ids2dg
!!          --shot 1512 --run 6 --username penkod --device solps-iter --version 3 DG_case_file
!!      @endverbatim
!!
!!-----------------------------------------------------------------------------

!>
!> \version  03.02.20 13:42

      program ids2dg
c
c  version : 03.02.20 13:42
c
c=====================================================
c*** Translation of IDS equilibrium and wall data into dg compatible format
c=====================================================
      implicit none
#include "eqdim.inc"
      integer iret, nr, nz, ipestg
      real(kind=R8) :: fg(ngpr)
      real(kind=R8) :: pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real(kind=R8) :: rcntc,psimin,psilim,btorc
      real(kind=R8), parameter :: twopi =
     *     2.0_R8*3.141592653589793238462643383280_R8
      real(kind=R8) :: rwall(ngpr), zwall(ngpr)
      integer :: nunits, npts(ngpr)

    !! Local variables
      character(len=24) :: treename   !< The name of the IMAS IDS database
      character(len=24) :: username   !< Creator/owner of the IMAS IDS database
      character(len=24) :: device     !< Device name of the IMAS IDS database
        !< (i. e. solps-iter, iter, aug)
      character(len=24) :: version    !< Major version of the IMAS IDS database
      integer :: shot      !< The shot number of the IDS equilibrium being read
                           !< If negative, no equilibrium IDS is translated
      integer :: wall      !< The shot number of the IDS wall being read
                           !< If negative, no wall description IDS is translated
      integer :: step      !< The time slice index for the chosen IDS equilibrium
      integer :: run       !< The run number of the IDS equilibrium being read
      integer :: wall_run  !< The run number of the IDS wall being read
#ifndef NO_GETENV
      character(len=24) :: device_env
#endif
    !! Dummy variables
      character(len=24) :: shot_string
      character(len=24) :: wall_string
      character(len=24) :: step_string
      character(len=24) :: run_string
      character(len=24) :: wall_run_string
      character(len=24) :: argName
      character(len=24) :: dg_file
#ifdef USE_PXFGETENV
      integer lenval, ierror
#else
#ifdef NAGFOR
      integer lenval, ierror
#endif
#endif
      integer narg, cptArg
      logical do_equilibrium, do_wall
      character*24 usrnam
      logical streql
      external usrnam, streql

c=====================================================
c
    !! Set default value for number of steps
      iret = 0
      treename = 'ids'
      version = "3"
      username = usrnam()
      device = 'solps-iter'
      run_string = ' '
      shot_string = ' '
      wall_string = ' '
      wall_run_string = ' '
      step = 1
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
      do_equilibrium = .false.
      do_wall = .false.

      !! Loop across arguments and allocate proper values to variables
      do cptArg = 1, narg
        call get_command_argument( cptArg, argName )
        if ( cptArg.eq.narg ) dg_file = argName
        select case( adjustl( argName ) )
          case("--shot","-s")
            call get_command_argument( cptArg + 1, shot_string )
            !! Transform dummy string variable to integer
            read( shot_string, *) shot
            do_equilibrium = shot.ge.0
          case("--step","-S")
            call get_command_argument( cptArg + 1, step_string )
            !! Transform dummy string variable to integer
            read( step_string, *) step
          case("--wall","-w")
            call get_command_argument( cptArg + 1, wall_string )
            !! Transform dummy string variable to integer
            read( wall_string, *) wall
            do_wall = wall.ge.0
          case("--run","-r")
            call get_command_argument( cptArg + 1, run_string )
            !! Transform dummy string variable to integer
            read( run_string, *) run
          case("--wall_run","-R")
            call get_command_argument( cptArg + 1, wall_run_string )
            !! Transform dummy string variable to integer
            read( wall_run_string, *) wall_run
          case("--username","-u")
            call get_command_argument( cptArg + 1, username )
          case("--device","-d")
            call get_command_argument( cptArg + 1, device )
          case("--version","-v")
            call get_command_argument( cptArg + 1, version )
        end select
      end do
      if (streql(wall_string, " ") .and. do_equilibrium) then
        wall = shot
        do_wall = .true.
      end if
      if (streql(wall_run_string, " ") .and. do_equilibrium .and.
     &    .not.streql(run_string, " ")) then
        wall_run = run
        write(wall_run_string,'(i6)') wall_run
      end if
    !! If not at least shot, run, username and device were defined,
    !! display the error message and and a full command example
      if( narg.lt.5 .or. mod(narg,2).eq.0 .or.
     &  (do_equilibrium .and. streql(run_string," ")) .or.
     &  (do_wall .and. streql(wall_run_string," ")) .or.
     &  (.not.do_equilibrium .and. .not.do_wall) ) then
        write(0,*) "ERROR! In order to run ids2dg, ",
     &   "at least the shot and run variables ",
     &   "(for the equilibrium and/or the wall IDS) and ",
     &   "the DG output file must be defined. ",
     &   "Example (terminal): "
        write(0,*) "ids2dg --shot 1 --run 1 <DG_case_file>"
        write(0,*) "Other options are :"
        write(0,*) " --wall, --step, --username, --device, --version"
        call exit(0)
      end if
      if (do_equilibrium.and.shot.gt.214748) then
        write(0,*) 'Invalid shot number for equilibrium IDS'
        call exit(0)
      end if
      if (do_wall.and.wall.gt.214748) then
        write(0,*) 'Invalid shot number for wall IDS'
        call exit(0)
      end if
      if (do_equilibrium.and..not.(0.le.run.and.run.le.99999)) then
        write(0,*) 'Invalid equilibrium IDS run number'
        call exit(0)
      end if
      if (do_wall.and..not.(0.le.wall_run.and.wall_run.le.99999)) then
        write(0,*) 'Invalid wall IDS run number'
        call exit(0)
      end if
      if (do_equilibrium) open(2,file=trim(dg_file)//'.equ')
      if (do_wall) open(3,file=trim(dg_file)//'.ogr')

      write(*,*) 'Requesting IDS files: '
      if (do_equilibrium) write(*,'(2(a,i8),2(a,a24),(a,i8))')
     . ' Shot: ', shot, ' Run: ', run,
     . ' User: ', trim(username), ' Device: ', trim(device),
     . ' Step: ', step
      if (do_wall .and. wall.ne.shot)
     > write(*,'(2(a,i8),2(a,a24))')
     . ' Wall: ', wall, ' Run: ', wall_run,
     . ' User: ', trim(username), ' Device: ', trim(device)

      call rdids(treename,shot,wall,run,wall_run,step,
     ,           username,device,version,
     ,           do_equilibrium,do_wall,
     ,           iret,ipestg,nr,nz,
     ,           rcntc,psimin,psilim,
     ,           btorc,fg,pfm,rgr,zgr,
     ,           nunits,npts,rwall,zwall)
      if(iret.eq.5) do_wall = .false.
      if(iret.ne.0) then
        print *,'==== ids2dg: error in rdids. iret =',iret
      end if
c
      if (do_equilibrium) then
        psimin = psimin / twopi
        psilim = psilim / twopi
        pfm = pfm / twopi
        print *,'psilim = ',psilim
        iret = 0
        call wreqdg(2,iret,nr,nz,psilim,btorc,rcntc,rgr,zgr,pfm)
        if(iret.ne.0) then
          print *,'==== ids2dg: error in wreqdg. iret = ',iret
          goto 999
        else
          write(*,*) 'Wrote DG equilibrium file ',trim(dg_file)//'.equ'
        end if
      end if
      if (do_wall) then
        rwall = rwall * 1000.0_R8
        zwall = zwall * 1000.0_R8
        iret = 0
        call wrtrdg(3,iret,nunits,npts,rwall,zwall)
        if(iret.ne.0) then
          print *,'==== ids2dg: error in wrtrdg. iret = ',iret
          goto 999
        else
          write(*,*) 'Wrote DG template file ',trim(dg_file)//'.trg'
        end if
      end if
c
  999 continue
      if (do_equilibrium .or.
     .   (iret.eq.1 .or. iret.eq.3 .or. iret.eq.4)) close(2)
      if (do_wall .or. (iret.eq.2 .or. iret.eq.5)) close(3)
      stop
      end
