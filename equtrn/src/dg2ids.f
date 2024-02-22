!> Translation of DG template into a wall IDS
!>
!!
!!      In terminal, run the following command:
!!
!!      @verbatim
!!          $dg2ids --shot <shot> --run <run> --database <database> --version <version>
!!          --temp_ref <Reference wall temperature (K)> <DG input template file>
!!      @endverbatim
!!
!!      The command can be shortened as:
!!
!!      @verbatim
!!          $dg2ids -s <shot> -r <run> -d <database> -v <version> -t <wall_temperature> <DG input template file>
!!      @endverbatim
!!
!!      The arguments marked with < ... > are the parameters of the wall IDS
!!      where the data is to be stored:
!!          - \b shot:     The shot number of the wall IDS being written
!!          - \b run:      The run number of the wall IDS being written
!!          - \b database: IMAS IDS database name
!!                         (i. e. solps-iter, ITER, aug) (default: $DEVICE)
!!          - \b version:  Major version of the IMAS IDS database (default: 3)
!!          - \b temp_ref: Reference wall temperature (K) (optional)
!!      Example of the command:
!!      @verbatim
!!          $dg2ids
!!          --shot 10 --run 3 --database ITER --version 3 DG_template_file
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
      use eqdim
      implicit none
      integer iret
      real(kind=R8) :: rwall(ngpr), zwall(ngpr)
      integer :: nunits, npts(ngpr)

    !! Local variables
      character(len=256) :: path      !< The path where the IDS will be written
      character(len=24) :: treename   !< The name of the IMAS IDS database
      character(len=24) :: username   !< Creator/owner of the IMAS IDS database
      character(len=24) :: database   !< IMAS IDS database name
                                      !< (i. e. solps-iter, ITER, aug)
      character(len=24) :: version    !< Major version of the IMAS IDS database
      integer :: shot      !< The shot number of the wall IDS being written
      integer :: run       !< The run number of the wall IDS being written
      real(kind=R8)     :: ref_temp   !< Reference wall temperature (K)
    !! Dummy variables
      character(len=24) :: temp_string
      character(len=24) :: shot_string
      character(len=24) :: run_string
      character(len=256) :: argName
      character(len=256) :: dg_file
      character(len=256) :: ids_path, imasdir
#ifndef NO_GETENV
      character(len=24) :: device_env
      integer lenval, ierror
#ifndef USE_PXFGETENV
      intrinsic get_environment_variable
#endif
#endif
      integer l, m, narg, cptArg
      character*24 usrnam, run_user
      character*256 home_dir, solpstop
      logical ex, absolute_path
      logical streql
      external usrnam, streql

c=====================================================
c
    !! Set default values
      iret = 0
      run_user = usrnam()
      username = run_user
      solpstop = ' '
      database = 'solps-iter'
      home_dir = '/home/'//trim(run_user)
      write(version,'(i1)') IMAS_MAJOR_VERSION
#if AL_MAJOR_VERSION > 4
      path = ' '
#else
      treename = 'ids'
#endif
      run_string = ' '
      shot_string = ' '
      temp_string = ' '
      ref_temp = 0.0_R8
#ifndef NO_GETENV
      device_env = ' '
#ifdef USE_PXFGETENV
      CALL PXFGETENV ('HOME', 0, home_dir, lenval, ierror)
      CALL PXFGETENV ('IMASDIR', 0, imasdir, lenval, ierror)
      CALL PXFGETENV ('DEVICE', 0, device_env, lenval, ierror)
      CALL PXFGETENV ('SOLPSTOP', 0, solpstop, lenval, ierror)
#else
      call get_environment_variable
     . ('HOME', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('HOME', value=home_dir)
      call get_environment_variable
     . ('IMASDIR', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('IMASDIR', value=imasdir)
      call get_environment_variable
     . ('DEVICE', status=ierror, length=lenval)
      if (ierror.eq.0) call get_environment_variable
     . ('DEVICE', value=device_env)
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
      absolute_path = .false.

      !! Check if arguments are found
      narg = command_argument_count()

      if( narg > 0 ) then
        !! Loop across arguments and allocate proper values to variables
        do cptArg = 1, narg
          call get_command_argument( cptArg, argName )
          if ( cptArg.eq.narg ) dg_file = argName
          select case( adjustl( argName ) )
#if AL_MAJOR_VERSION > 4
            case("--path","-p")
              call get_command_argument( cptArg + 1, path )
              !! Parse SOLPSTOP and HOME and remove IMASDIR prefix if present
              l=index(path,'$HOME')
              if (l.gt.0) then
                ids_path = trim(home_dir)//trim(path(l+5:256))
                path = ids_path
              end if
              l=index(path,'$SOLPSTOP')
              if (l.gt.0) then
                ids_path = trim(solpstop)//trim(path(l+9:256))
                path = ids_path
              end if
              l=index(path,'$IMASDIR')
              if (l.gt.0) then
                ids_path = trim(imasdir)//trim(path(l+8:256))
                path = ids_path
              end if
              l=index(path,trim(imasdir))
              if (l.eq.0) then
                m=index(path,'/')
              else
                m=index(path(l+len_trim(imasdir):256),'/')
              end if
              absolute_path = l.eq.1.or.(l.eq.0.and.m.eq.1)
              if (absolute_path) then
                ids_path = path
              else if (l.eq.0) then
                ids_path = trim(imasdir)//'/'//trim(path)
              end if
#endif
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
            case("--database","--device","-d")
              call get_command_argument( cptArg + 1, database )
            case("--version","-v")
              call get_command_argument( cptArg + 1, version )
            case("--temp_ref","-t")
              call get_command_argument( cptArg + 1, temp_string )
              !! Transform dummy string variable to real
              read( temp_string, *) ref_temp
          end select
        end do
      end if
      !! If not at least input file,
      !! IMAS data entry shot, run, username, and database, were defined
      !! display the error message and a full command example
#if AL_MAJOR_VERSION > 4
      if( narg.lt.3 .or. mod(narg,2).eq.0 .or.
     &   ((streql(shot_string," ") .or. streql(run_string," ")) .and.
     &     streql(path," ")) ) then
        write(0,*) "ERROR! In order to run dg2ids, ",
     &   "at least the shot and run or path variables and ",
#else
      if( narg.lt.5 .or. mod(narg,2).eq.0 .or.
     &  streql(shot_string," ") .or. streql(run_string," ") ) then
        write(0,*) "ERROR! In order to run dg2ids, ",
     &   "at least the shot and run variables and ",
#endif
     &   "the DG input template file must be defined. ",
     &   "Example (terminal): "
        write(0,*) "dg2ids --shot 1 --run 1 <DG_template_file>"
        write(0,*) "Other options are :"
#if AL_MAJOR_VERSION > 4
        write(0,*)
     &     " --path, --username, --database, --temp_ref, --version"
#else
        write(0,*) " --username, --database, --temp_ref, --version"
#endif
        call exit(1)
      end if
#if AL_MAJOR_VERSION > 4
      if (.not.streql(shot_string,' ')) then
        if (0.gt.shot) then
          write(0,*) 'Invalid shot number for equilibrium IDS'
          call exit(1)
        end if
      end if
      if (.not.streql(run_string,' ')) then
        if (0.gt.run) then
          write(0,*) 'Invalid equilibrium IDS run number'
          call exit(1)
        end if
      end if
#else
      if (.not.(0.lt.shot.and.shot.le.214748)) then
        write(0,*) 'Invalid shot number for equilibrium IDS'
        call exit(1)
      end if
      if (.not.(0.le.run.and.run.le.99999)) then
        write(0,*) 'Invalid equilibrium IDS run number'
        call exit(1)
      end if
#endif
      if (index(imasdir,trim(username)).eq.0) then
        l=index(imasdir,trim(run_user))
        m=index(imasdir(l+len_trim(run_user):256),'/')
        write(imasdir,'(a)')
     .   imasdir(1:l)//trim(username)//trim(imasdir(m+l:256))
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
      if (streql(path,' ')) then
#if AL_MAJOR_VERSION > 4
        ids_path = trim(imasdir)//'/'//int2str(shot)//'/'//int2str(run)
#else
        ids_path = imasdir
#endif
      else if (absolute_path) then
        ids_path = path
      else
        ids_path = trim(imasdir)//'/'//trim(path)
      end if
      if (.not.streql(temp_string,' ')) then
        if (ref_temp.le.0.0_R8) then
          write(0,*) 'Invalid reference wall temperature'
          call exit(1)
        end if
      end if
      inquire(file=trim(dg_file),exist=ex)
      if (.not.ex) then
        write(*,'(3a)')
     .   '==== dg2ids: error in opening DG template file ',
     .    trim(dg_file),'. File not found!'
        goto 999
      else
        open(1,file=trim(dg_file),iostat=iret)
        if(iret.ne.0) then
          write(*,'(3a,i4)')
     .     '==== dg2ids: error in opening DG template file ',
     .      trim(dg_file),'. iret = ',iret
          goto 999
        end if
      end if

      if (database.eq.'iter') database = 'ITER'
      if (index(ids_path,'imasdb/iter').gt.0) then
        l=index(ids_path,'imasdb/iter')
        write(ids_path(l+7:l+10),'(a4)') 'ITER'
      end if
#if AL_MAJOR_VERSION > 4
      write(*,'(6a)') 'Path: ', trim(ids_path),
     & ' User: ', trim(username), ' Database: ', trim(database)
#else
      write(*,'(a,i8,a,i8,4a)') 'Shot: ', shot, ' Run: ', run,
     & ' User: ', trim(username), ' Database: ', trim(database)
#endif

      call rdtrg(1,iret,nunits,npts,rwall,zwall)
      if(iret.ne.0) then
        print *,'==== dg2ids: error in rdtrg. iret =',iret
        goto 999
      end if
c
      rwall = rwall / 1000.0_R8
      zwall = zwall / 1000.0_R8
      call wrids(iret,nunits,npts,rwall,zwall,ref_temp,dg_file,
#if AL_MAJOR_VERSION > 4
     .           username,ids_path)
#else
     .           treename,shot,run,username,database,version)
#endif
      if(iret.ne.0) then
        print *,'==== dg2ids: error in wrids. iret = ',iret
      end if
c
  999 continue
      close(1)
      stop
      end program dg2ids
