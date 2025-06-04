!> Translation of IDS equilibrium and wall data into dg compatible format
!>
!!
!!      In terminal, run the following command:
!!
!!      \verbatim
!!          $ids2dg --pulse <pulse> --run <run> --wall <wall> --wall_run <wall_run>
!!          --step <step> --occurrence <occurrence>
!!          --username <username> --database <database> --version <version>
!!          <DG output filename stem>
!!      \endverbatim
!!
!!      The command can be shortened as:
!!
!!      \verbatim
!!          $ids2dg -s <pulse> -r <run> -w <wall> -R <wall_run>
!!          -S <step> -o <occurrence>
!!          -u <username> -d <database> -v <version> <DG output filename stem>
!!      \endverbatim
!!
!!      The arguments marked with < ... > are the parameters of the IDS database
!!      where the data is to be stored:
!!          - \b path:       The path of the equilibrium IDS being read
!!                           (if missing and shot not defined, no equilibrium will be read)
!!          - \b wall_path:  The path of the wall IDS being read (default: same as <path>)
!!          - \b pulse:      The pulse (previously shot) number of the equilibrium IDS being read
!!                           (if the number is negative, no equilibrium will be translated)
!!          - \b run:        The run number of the equilibrium IDS being read
!!          - \b wall:       The pulse number of the wall IDS being read (default: same as <pulse>)
!!                           (if the number is negative, no wall description will be translated)
!!          - \b wall_run:   The run number of the wall IDS being read (default: same as <run>)
!!          - \b occurrence: The occurrence index of the equilibrium IDS to be read (default: 1)
!!                           (Recall occurrence indices start at zero !)
!!          - \b step:       The time slice index for the equilibrium IDS (default: 1)
!!          - \b username:   Creator/owner of the IMAS IDS database (default: $USER)
!!          - \b database:   IMAS IDS database name
!!                           (i. e. solps-iter, ITER, aug) (default: $DEVICE)
!!          - \b version:    Major version of the IMAS IDS database (default: 3)
!!      Example of the command:
!!      \verbatim
!!          $ids2dg
!!          --pulse 1512 --run 6 --username penkod --database solps-iter --version 3 DG_case_file
!!      \endverbatim
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
      use eqdim
      implicit none
      integer iret, nr, nz, ipestg
      real(kind=R8) :: fg(ngpr)
      real(kind=R8) :: pfm(ngpr,ngpz),rgr(ngpr),zgr(ngpz)
      real(kind=R8) :: rcntc,psimin,psilim,btorc
      real(kind=R8) :: twopi
      real(kind=R8) :: rwall(ngpr), zwall(ngpr)
      integer :: nunits, npts(ngpr)

      !! Local variables
      character(len=24) :: treename   !< The name of the IMAS IDS database
      character(len=24) :: username   !< Creator/owner of the IMAS IDS database
      character(len=24) :: database   !< IMAS IDS database name
                                      !< (i. e. solps-iter, ITER, aug)
      character(len=24) :: version    !< Major version of the IMAS IDS database
      integer :: shot      !< The pulse (previously shot) number of the IDS equilibrium being read
                           !< If negative, no equilibrium IDS is translated
      integer :: wall      !< The pulse number of the IDS wall being read
                           !< If negative, no wall description IDS is translated
      integer :: occ       !< The occurrence index for the chosen IDS equilibrium
      integer :: step      !< The time slice index for the chosen IDS equilibrium
      integer :: run       !< The run number of the IDS equilibrium being read
      integer :: wall_run  !< The run number of the IDS wall being read

      !! Dummy variables
      character(len=24) :: shot_string
      character(len=24) :: wall_string
      character(len=24) :: step_string
      character(len=24) :: user_string
      character(len=24) :: occ_string
      character(len=24) :: run_string
      character(len=24) :: wall_run_string
      character(len=24) :: argName
      character(len=24) :: dg_file
#ifndef NO_GETENV
      character(len=24) :: device_env
      integer lenval, ierror
#ifndef USE_PXFGETENV
      intrinsic get_environment_variable
#endif
#endif
      integer l, m
      character*256 wall_path, eq_path, path
      character*256 home_dir, imasdir, imas_home, solpstop
      logical wall_absolute, eq_absolute
      integer narg, cptArg
      logical do_equilibrium, do_wall
      character*24 usrnam
      logical streql
      external usrnam, streql
#if AL_MAJOR_VERSION > 4
      intrinsic index
#endif

c=====================================================
c
      !! Set default value for number of steps
      iret = 0
#if AL_MAJOR_VERSION > 4
      path = ' '
      eq_path = ' '
      wall_path = ' '
#endif
      treename = 'ids'
      write(version,'(i1)') IMAS_MAJOR_VERSION
      username = usrnam()
      database = 'solps-iter'
      home_dir = '/home/'//trim(username)
      solpstop = ' '
      run_string = ' '
      shot_string = ' '
      wall_string = ' '
      wall_run_string = ' '
      shot = 0
      wall = 0
      step = 1
      occ = 1
      imas_home = ' '
      eq_absolute = .false.
      wall_absolute = .false.
#ifndef NO_GETENV
      device_env = ' '
#ifdef USE_PXFGETENV
      CALL PXFGETENV ('HOME', 0, home_dir, lenval, ierror)
      CALL PXFGETENV ('DEVICE', 0, device_env, lenval, ierror)
      CALL PXFGETENV ('IMASDIR', 0, imasdir, lenval, ierror)
      CALL PXFGETENV ('IMAS_HOME', 0, imas_home, lenval, ierror)
      CALL PXFGETENV ('SOLPSTOP', 0, solpstop lenval, ierror)
#else
      call get_environment_variable
     . ('HOME', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('HOME', value=home_dir)
      call get_environment_variable
     . ('DEVICE', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('DEVICE', value=device_env)
      call get_environment_variable
     . ('IMASDIR', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('IMASDIR', value=imasdir)
      call get_environment_variable
     . ('IMAS_HOME', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('IMAS_HOME', value=imas_home)
      call get_environment_variable
     . ('SOLPSTOP', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('SOLPSTOP', value=solpstop)
#endif
      if (.not.streql(device_env,' ')) database = device_env
      if (streql(database,'iter')) database = 'ITER'
#endif
      imasdir = trim(home_dir)//'/public/imasdb/'
     &        //trim(database)//'/'//trim(version)
#if AL_MAJOR_VERSION < 5
     &        //'/0'
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
#if AL_MAJOR_VERSION > 4
          case("--path","-p")
            call get_command_argument( cptArg + 1, path )
            do_equilibrium = .true.
            !! Parse SOLPSTOP and HOME and remove IMASDIR prefix if present
            l=index(path,'$HOME')
            if (l.gt.0) then
              eq_path = trim(home_dir)//trim(path(l+5:256))
              path = eq_path
            end if
            l=index(path,'$SOLPSTOP')
            if (l.gt.0) then
              eq_path = trim(solpstop)//trim(path(l+9:256))
              path = eq_path
            end if
            l=index(path,'$IMASDIR')
            if (l.gt.0) then
              eq_path = trim(imasdir)//trim(path(l+8:256))
              path = eq_path
            end if
            l=index(path,trim(imasdir))
            if (l.eq.0) then
              m=index(path,'/')
            else
              m=index(path(l+len_trim(imasdir):256),'/')
            end if
            eq_absolute = l.eq.1.or.(l.eq.0.and.m.eq.1)
            if (.not.eq_absolute) then
              eq_path = trim(path(m+l+len_trim(imasdir):256))
            else
              eq_path = path
            end if
          case("--wall_path","-P")
            call get_command_argument( cptArg + 1, wall_path )
            do_wall = .true.
            !! Parse HOME and SOLPSTOP and remove IMASDIR prefix if present
            l=index(wall_path,'$HOME')
            if (l.gt.0) then
              wall_path = trim(home_dir)//trim(wall_path(l+5:256))
            end if
            l=index(wall_path,'$SOLPSTOP')
            if (l.gt.0) then
              wall_path = trim(solpstop)//trim(wall_path(l+9:256))
            end if
            l=index(wall_path,'$IMASDIR')
            if (l.gt.0) then
              wall_path = trim(imasdir)//trim(wall_path(l+8:256))
            end if
            l=index(wall_path,trim(imasdir))
            if (l.eq.0) then
              m=index(path,'/')
            else
              m=index(wall_path(l+len_trim(imasdir):256),'/')
            end if
            wall_absolute = l.eq.1.or.(l.eq.0.and.m.eq.1)
            if (.not.wall_absolute)
     >       wall_path = trim(wall_path(m+l+len_trim(imasdir):256))
#endif
          case("--pulse","--shot","-s")
            call get_command_argument( cptArg + 1, shot_string )
            !! Transform dummy string variable to integer
            read( shot_string, *) shot
            do_equilibrium = shot.ge.0
          case("--step","-S")
            call get_command_argument( cptArg + 1, step_string )
            !! Transform dummy string variable to integer
            read( step_string, *) step
          case("--occurrence","-o")
            call get_command_argument( cptArg + 1, occ_string )
            !! Transform dummy string variable to integer
            read( occ_string, *) occ
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
          case("--username","--user","-u")
            call get_command_argument( cptArg + 1, user_string )
          case("--database","--device","-d")
            call get_command_argument( cptArg + 1, database )
          case("--version","-v")
            call get_command_argument( cptArg + 1, version )
        end select
      end do
      if (streql(wall_path, " ") .and. do_equilibrium .and. wall.ge.0)
     > then
        wall_absolute = eq_absolute
        wall_path = eq_path
        do_wall = .true.
      end if
      if (streql(wall_string, " ") .and. do_equilibrium .and. wall.eq.0)
     > then
        wall = shot
        do_wall = .true.
      end if
      if (streql(wall_run_string, " ") .and. streql(wall_path, " ")
     &    .and. do_equilibrium .and. .not.streql(run_string, " ")) then
        wall_run = run
        write(wall_run_string,'(i6)') wall_run
      end if
#if AL_MAJOR_VERSION > 4
    !! If not at least pulse and run or path, username and database were defined,
    !! display the error message and a full command example
      if( narg.lt.3 .or. mod(narg,2).eq.0 .or.
     &  (do_equilibrium .and.
     &   streql(path," ") .and. streql(run_string," ")) .or.
     &  (do_wall .and.
     &   streql(wall_path," ") .and. streql(wall_run_string," ")) .or.
     &  (.not.do_equilibrium .and. .not.do_wall) ) then
        write(0,'(a)') 'Standard ids2dg usage:'
        write(0,'(a)')
     &   'ids2dg -s <pulse> -r <run> -w <wall> -R <wall_run> '//
     &   '<DG_file_stem>'
#else
    !! If not at least shot and run or path, username and database were defined,
    !! display the error message and a full command example
      if( narg.lt.5 .or. mod(narg,2).eq.0 .or.
     &  (do_equilibrium .and. streql(run_string," ")) .or.
     &  (do_wall .and. streql(wall_run_string," ")) .or.
     &  (.not.do_equilibrium .and. .not.do_wall) ) then
        write(0,'(a)') 'Standard ids2dg usage:'
        write(0,'(a)')
     &   'ids2dg -s <shot> -r <run> -w <wall> -R <wall_run> '//
     &   '<DG_file_stem>'
#endif
        write(0,'(a)') ' '
        write(0,'(a)') 'Available options are:'
#if AL_MAJOR_VERSION > 4
        write(0,'(a)') '--path, -p:               '//
     &   'Directory path to the equilibrium IDS to import '//
     &   '(if this and "pulse" missing, '//
     &   'do not import an equilibrium IDS)'
        write(0,'(a)') '--wall_path, -P:          '//
     &   'Directory path to the wall IDS to import '//
     &   '(if missing, same as for the equilibrium IDS)'
        write(0,'(a)') '--pulse, --shot, -s:      '//
     &   'Pulse number of the equilibrium IDS to import '//
     &   '(if negative, do not import an equilibrium IDS)'
#else
        write(0,'(a)') '--shot, -s:               '//
     &   'Shot number of the equilibrium IDS to import '//
     &   '(if negative, do not import an equilibrium IDS)'
#endif
        write(0,'(a)') '--run,  -r:               '//
     &   'Run number of the equilibrium IDS to import'
        write(0,'(a)') '--occurrence, -o:         '//
     &   'Occurrence index of the equilibrium IDS to import '//
     &   '(default: 1)'
        write(0,'(a)') '--step, -S:               '//
     &   'Time step of the equilibrium IDS to import (default: 1)'
#if AL_MAJOR_VERSION > 4
        write(0,'(a)') '--wall, -w:               '//
     &   'Pulse number of the wall description IDS to import '//
     &   '(if negative, do not import a wall description IDS) '//
     &   '(default: same as pulse value)'
#else
        write(0,'(a)') '--wall, -w:               '//
     &   'Shot number of the wall description IDS to import '//
     &   '(if negative, do not import a wall description IDS) '//
     &   '(default: same as shot value)'
#endif
        write(0,'(a)') '--wall_run, -R:           '//
     &   'Run number of the wall description IDS to import '//
     &   '(default: same as run value)'
        write(0,'(a)') '--username, --user, -u:   '//
     &   'User name for the database to be read '//
     &   '(default is $USER)'
        write(0,'(a)') '--database, --device, -d: '//
     &   'Device database name to be read '//
     &   '(default is $DEVICE if defined, "solps-iter" otherwise)'
        write(0,'(a)') '--version, -v:            '//
     &   'IMAS version being used '//
     &   '(only supported and default value: 3)'
        call exit(0)
      end if
#if AL_MAJOR_VERSION < 5
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
#else
      if (do_equilibrium.and..not.streql(run_string," ")) then
        if (run.lt.0) then
          write(0,*) 'Invalid equilibrium IDS run number'
          call exit(0)
        end if
      end if
#endif
      if (do_equilibrium.and..not.(0.le.occ)) then
        write(0,*) 'Invalid equilibrium occurrence index'
        call exit(0)
      end if
#if AL_MAJOR_VERSION < 5
      if (do_wall.and..not.(0.le.wall_run.and.wall_run.le.99999)) then
        write(0,*) 'Invalid wall IDS run number'
        call exit(0)
      end if
#else
      if (do_wall.and..not.streql(wall_run_string," ")) then
        if (wall_run.lt.0) then
          write(0,*) 'Invalid wall IDS run number'
          call exit(0)
        end if
      end if
#endif
      if (streql(user_string,'public')) then
        write(imasdir,'(a)')
     .   trim(imas_home)//'/shared/imasdb/'//
     .   trim(database)//'/'//trim(version)
#if AL_MAJOR_VERSION < 4
     .                 //'/'//int2str(run/10000)
#endif
      else if (.not.streql(user_string,username)) then
        l=index(imasdir,username)
        m=index(imasdir(l+len_trim(username):256),'/')
        write(imasdir,'(a)')
     .   imasdir(1:l-1)//trim(user_string)//
     .   trim(imasdir(m+l+len_trim(username)-1:256))
        username = user_string
      end if
      if (index(imasdir,'imasdb/'//trim(database)).eq.0) then
        l=index(imasdir,'imasdb/')
        m=index(imasdir(l+7:256),'/')
        write(imasdir,'(a)')
     .   imasdir(1:l+6)//trim(database)//trim(imasdir(m+l+6:256))
      end if
      if (.not.streql(version,int2str(IMAS_MAJOR_VERSION))) then
        l=len_trim(version)
        m=len_trim(imasdir)
#if AL_MAJOR_VERSION > 4
        if(.not.streql(imasdir(m:m),version))
     >   write(imasdir,'(a)') imasdir(1:m-1)//trim(version)
#else
        if(.not.streql(imasdir(m-2:m-2),version))
     >   write(imasdir,'(a)') imasdir(1:m-3)//trim(version)//'/'
     &                                      //int2str(run/10000)
      else if (run.ge.10000) then
        m=len_trim(imasdir)
        write(imasdir,'(a)') imasdir(1:m-1)//int2str(run/10000)
#endif
      end if
      if (do_equilibrium) then
        if (streql(path,' ')) then
          eq_path =
     .     trim(imasdir)//'/'//int2str(shot)//'/'//int2str(run)
        else if (.not.eq_absolute) then
          eq_path = trim(imasdir)//'/'//trim(path)
        end if
      end if
      if (do_wall) then
        if (streql(wall_path,' ')) then
          wall_path =
     .     trim(imasdir)//'/'//int2str(wall)//'/'//int2str(wall_run)
        else if (.not.wall_absolute) then
          wall_path = trim(imasdir)//'/'//trim(wall_path)
        end if
      end if

      if (do_equilibrium .and. do_wall) then
        write(*,*) 'Requesting IDS files: '
      else if (do_equilibrium .or. do_wall) then
        write(*,*) 'Requesting IDS file: '
      end if
      if (do_equilibrium)
#if AL_MAJOR_VERSION > 4
     > write(*,'(2a,2(a,i8))') ' Equilibrium IDS path: ',trim(eq_path),
#else
     > write(*,'(2(a,i8),2(a,a24),2(a,i8))')
     . ' Shot: ', shot, ' Run: ', run,
     . ' User: ', trim(username), ' Database: ', trim(database),
#endif
     . ' Occurrence: ', occ, ' Step: ', step
#if AL_MAJOR_VERSION > 4
      if (do_wall .and. .not.streql(eq_path,wall_path))
     > write(*,'(2a)') ' Wall: ',trim(wall_path)
#else
      if (do_wall .and. wall.ne.shot)
     > write(*,'(2(a,i8),2(a,a24))')
     . ' Wall: ', wall, ' Run: ', wall_run,
     . ' User: ', trim(username), ' Database: ', trim(database)
#endif

      call rdids(do_equilibrium,do_wall,occ,step,
#if AL_MAJOR_VERSION > 4
     ,           imas_home,eq_path,wall_path,
#endif
     ,           treename,shot,wall,run,wall_run,database,version,
     ,           username,iret,ipestg,nr,nz,
     ,           rcntc,psimin,psilim,
     ,           btorc,fg,pfm,rgr,zgr,
     ,           nunits,npts,rwall,zwall)
      if(iret.eq.5) do_wall = .false.
      if(iret.ne.0) then
        print *,'==== ids2dg: error in rdids. iret =',iret
      end if
c
      if (do_equilibrium) then
        twopi = 2.0_R8 * pi
        open(2,file=trim(dg_file)//'.equ')
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
        open(3,file=trim(dg_file)//'.ogr')
        rwall = rwall * 1000.0_R8
        zwall = zwall * 1000.0_R8
        iret = 0
        call wrtrdg(3,iret,nunits,npts,rwall,zwall)
        if(iret.ne.0) then
          print *,'==== ids2dg: error in wrtrdg. iret = ',iret
          goto 999
        else
          write(*,*) 'Wrote DG template file ',trim(dg_file)//'.ogr'
        end if
      end if
c
  999 continue
      if (do_equilibrium .or.
     .   (iret.eq.1 .or. iret.eq.3 .or. iret.eq.4)) close(2)
      if (do_wall .or. (iret.eq.2 .or. iret.eq.5)) close(3)
      stop
      end program ids2dg
