!> Translation of the JET PPF equilibrium data into dg compatible format
!>
!> 1st arg == shot number
!>
!> 2nd arg == time
!>
!> [3rd arg == DDA (EFIT)]
!>
!> [4th arg == PPFUID (JETPPF)]
!>
!> [5th arg == edition (0)]
!>
!> The output filename is derived from the shot, time, experiment, diagnostic and edition
!>
!> Specific JET library routines are used to access the data
!>
!> \version 2008-07-17 16:15
      PROGRAM JETPPF2DG
      IMPLICIT NONE
C
C+ .....................................................................
C
C  Procedure   : JETPPF2DG
C
C  Description : reads a JET equilibrium PPF and writes a dg equilinrium
C
C  Notes       :
C
C  Input       :
C
C  Output      :
C
C- .....................................................................
C
C..Locals
C
      INTEGER    LUNMSG
      PARAMETER (LUNMSG = 6)
      INTEGER    MR, MZ
      PARAMETER (MR = 1025, MZ = 2049)
      INTEGER    LFLUSH
      PARAMETER (LFLUSH = LUNMSG)
C
      INTEGER    IREAD, IWRITE, IERRA
      INTEGER    NSHOT, NSEQ, NX, NY, RATIOX, RATIOY, IER
      integer    iarg, iargc, i, j, iret
      INTEGER    IGO, IDUM, IERROR, NP, NR, NZ
C
      REAL       TSHOT,WIDTH
C
      REAL*8     CONV, RBTOR
      REAL*8     TWOPI, PSI0, PSIB, DPSI, PSI(MR,MZ)
C
      REAL*8     EPS, R1, R2, R(MR), Z1, Z2, Z(MZ)
      REAL*8     TIME, FP, BR, BZ, BT, FPT
      real*8     rcntc, btorc, psilim
C
      CHARACTER  PPFUID*6, DDA*4, MSG*132, arg*256, filename*256
C
C..Includes
      INCLUDE    'flchar'
C
      DATA IREAD/0/, IWRITE/1/
      
      dda='EFIT'
      ppfuid='JETPPF'
      nseq=0
      iarg=iargc()
      write(*,*) 'iarg',iarg
      if(iarg.ge.2) then
        call getarg(1,arg)
        read(arg,*) nshot
        call getarg(2,arg)
        read(arg,*) tshot
        if(iarg.ge.3) call getarg(3,dda)
        if(iarg.ge.4) call getarg(4,ppfuid)
        if(iarg.ge.5) then
          call getarg(5,arg)
          read(arg,*) nseq
        endif
      else
        write(*,*) '1st arg == shot number'
        write(*,*) '2nd arg == time'
        write(*,*) '[3rd arg == DDA (EFIT)]'
        write(*,*) '[4th arg == PPFUID (???)]'
        write(*,*) '[5th arg == edition (0)]'
        stop
      endif
      write(*,*) 'PPFUID=',ppfuid,' DDA=',dda,
     &     ' SHOT=',nshot,' TIME=',tshot
      write(filename,'(i5,''.'',i5,''.'',a6,''.'',a4,''.'',i2,''.eq'')')
     1 nshot,nint(tshot*1000),ppfuid,dda,nseq
      do i=1,11
        if(filename(i:i).eq.' ') filename(i:i)='0'
      enddo
      do i=25,26
        if(filename(i:i).eq.' ') filename(i:i)='0'
      enddo
      do i=12,len_trim(filename)
        if(filename(i:i).eq.' ') filename(i:i)='_'
      enddo
C
C-----------------------------------------------------------------------
C Create input files for carre related to equilibrium data
C-----------------------------------------------------------------------
C
C
C-----------------------------------------------------------------------
C Initialise flush
C-----------------------------------------------------------------------
C
      IF( DDA.EQ.'EFIT' )THEN
        IGO = 5
      ELSE
        MSG = 'Only set up for EFIT'
        CALL ERRMSS(LUNMSG,'EQUDAT',2,MSG,' ',' ')
        IER = 5
        RETURN
      ENDIF
C
      TIME = TSHOT
      CALL JETFLUSH(IGO,NSHOT,TIME,IDUM,NSEQ,LFLUSH,IERROR)
C
      IF( IERROR.GT.0 )THEN
        WRITE(MSG,'(A,I5)') 'FLUSH failed - return code = ',IERROR
        CALL ERRMSS(LUNMSG,'EQUDAT',2,MSG,' ',' ')
        IER = 10
        RETURN
      ENDIF
C
C-----------------------------------------------------------------------
C Data for BTOR file
C-----------------------------------------------------------------------
C
      NP = 1
C
      CALL FLUPN3(NP,XP0,YP0,FP,BR,BZ,BT,IERROR)
C
      IF( IERROR.GT.0 )THEN
        WRITE(MSG,'(A,I5)') 'FLUPN failed - return code = ',IERROR
        CALL ERRMSS(LUNMSG,'EQUDAT',2,MSG,' ',' ')
        IER = 40
        RETURN
      ENDIF
C
      RBTOR = XP0*BT*1.E-6
      WRITE(*,*) 'RBTOR=',RBTOR
C
C-----------------------------------------------------------------------
C Data for RZPSI file
C-----------------------------------------------------------------------
C
C.. Grid dimensions
C
      NR = MIN(257,MR)
      NZ = MIN(513,MZ)
C
C.. Construct grid for output
C
      EPS = (RW2-RW1)*1.D-6
      R1  = RW1 + EPS
      R2  = RW2 - EPS
      DO I=1,NR
        R(I) = R1 + (R2-R1)*FLOAT(I)/FLOAT(NR)
      ENDDO
C
      EPS = (ZW2-ZW1)*1.D-6
      Z1  = ZW1 + EPS
      Z2  = ZW2 - EPS
      DO J=1,NZ
        Z(J) = Z1 + (Z2-Z1)*FLOAT(J)/FLOAT(NZ)
      ENDDO
C
C.. Set up normalised psi and convert to psi in WB/RAD, zero at
C.. boundary, and negative inside
C
      TWOPI  = 8.*ATAN(1.)
      PSI0   = ABS(FCENTR/TWOPI)
      PSIB   = ABS(FBOUND/TWOPI)
      DPSI   = PSIB - PSI0
C
      DO J=1,NZ
        DO I=1,NR
          CALL FLUP(R(I),Z(J),FPT,IER)
          PSI(I,J)= DPSI*(FPT-1.)
        ENDDO
      ENDDO
C
C.. Unit conversion
C
      CONV = 1.E-2
C
      DO I=1,NR
        R(I) = R(I)*CONV
      ENDDO
      DO J=1,NZ
        Z(J) = Z(J)*CONV
      ENDDO
cdpc 2 pi conversion not necessary at JET
c     DO J=1,NZ
c       DO I=1,NR
c         PSI(I,J)=PSI(I,J)*TWOPI
c       ENDDO
c     ENDDO
C
C.. Output
C
      rcntc=xp0/1d2
      btorc=bt/1d4
      psilim=0
      print *,'psilim = ',psilim
      print *,'rcntc = ',rcntc
      print *,'btorc = ',btorc
      open(2,file=filename)
      psilim=0
      write(*,*) 'nr,nz=',nr,nz
      call wreqdg(2,iret,nr,nz,psilim,btorc,rcntc,r,z,psi)
      if(iret.ne.0) then
          print *,'==== shotfile2dg: error in wreqdg. iret = ',iret
      end if
C
C-----------------------------------------------------------------------
C Tidy up
C-----------------------------------------------------------------------
C
      CLOSE(LUNMSG)
C
C-----------------------------------------------------------------------
C
      STOP
      END PROGRAM JETPPF2DG
C
C+ ---------------------------------------------------------------------
C
C  File        : util.f
C
C  Description : General utilities
C
C  History     : 10/12/1999 --- (GC) Creation
C
C- ---------------------------------------------------------------------
C
C
C
C
C=======================================================================
      SUBROUTINE ERRMSS(LUN, MODULE, IER, MSG1, MSG2, MSG3)
      IMPLICIT NONE
C
C+ .....................................................................
C
C  Procedure   : ERRMSS
C
C  Description : General error message printing tool.
C
C  Notes       : The program prints non-blank messages MSG1 to MSG3
C                and then decides action according to IER.
C
C  Input       : LUN      = Unit number for error message
C                            (0 --> default).
C                MODULE   = Name of calling routine.
C                IER      = Error level
C                            (0-warning, 1-error+exit, 2-error only).
C                MSGn     = Error message for line n.
C
C  Output      :
C
C- .....................................................................
C
C
C..Arguments
      INTEGER       LUN, IER
      CHARACTER*(*) MODULE, MSG1, MSG2, MSG3
C
C..Functions
      INTEGER       LENSTR
C
C..Local
      INTEGER       LDEF, LOUT
      DATA          LDEF/6/
C
C
      IF (LUN.NE.0) THEN
          LOUT = LUN
      ELSE
          LOUT = LDEF
      END IF
C
      WRITE(LOUT,*)
      IF( IER.NE.0 )THEN
        WRITE(LOUT,*)
        WRITE(LOUT,'(4A)') '*** ERROR(',MODULE(1:LENSTR(MODULE)),') : '
     +                   , MSG1
      ELSE
        WRITE(LOUT,'(4A)')'*** WARNING(',MODULE(1:LENSTR(MODULE)),') : '
     +                   ,MSG1
      ENDIF
      IF (MSG2.NE.' ')
     +    WRITE(LOUT,'(2A)') '                    ',MSG2
      IF (MSG3.NE.' ')
     +    WRITE(LOUT,'(2A)') '                    ',MSG3
C
      IF( IER.EQ.1 ) STOP
C
C
      RETURN
      END SUBROUTINE ERRMSS
C
C
C=======================================================================
      INTEGER FUNCTION LENSTR(ASTR)
      IMPLICIT NONE
C
C+ .....................................................................
C
C  Procedure   : LENSTR
C
C  Description : Returns the effective length of a given string
C                 (Ignoring trailing blanks)
C
C  Notes       :
C
C  Input       : ASTR     = Character string.
C
C  Output      :
C
C  Returns     : LENSTR   = Effective length of string.
C
C- .....................................................................
C
C
C..Arguments
      CHARACTER*(*) ASTR
C
C..Local
      INTEGER       I
C
C
      DO 10 I = LEN(ASTR),1,-1
        IF (ASTR(I:I) .NE. ' ') GO TO 20
   10 CONTINUE
C
   20 CONTINUE
      LENSTR = I
C
C
      RETURN
      END FUNCTION LENSTR
