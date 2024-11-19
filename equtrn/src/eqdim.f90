
  module eqdim
#ifdef CONSTANTS_PROVIDED
  use eqdim_constants
  use mathematical_constants, only: PI => M_PI    ! IGNORE
#elif ( defined(IMAS) && ( IMAS_MINOR_VERSION < 35 && IMAS_MAJOR_VERSION < 4 ) )
  use imas_constants_module, only: imas_constants ! IGNORE
#endif
  implicit none

  INTEGER, PARAMETER :: R8 = SELECTED_REAL_KIND (14)
  INTEGER, PARAMETER :: R4 = SELECTED_REAL_KIND (6)
  INTEGER, PARAMETER :: I4 = SELECTED_INT_KIND (8)

! Indicate here the maximum size of the equilibrium files to be used
! ngpr: number of points in the radial direction
! ngpz: number of points in the vertical direction
  integer ngpr, ngpz
  parameter (ngpr=1775, ngpz=1775)
#ifndef CONSTANTS_PROVIDED
  real (kind=R8) :: pi
#ifdef IMAS
#if ( IMAS_MINOR_VERSION < 35 && IMAS_MAJOR_VERSION < 4 )
  parameter (pi=imas_constants%pi)
#else
  parameter (pi=3.141592653589793_R8)
#endif
#else
  parameter (pi=3.141592653589793_R8)
#endif
#endif

  public :: int2str, real2str

  contains

  function int2str(intval) result(string)
    integer, intent(in) :: intval
    character(len=2-max(sign(1,intval),0)+max( &
         min(abs(intval)/10**1,1)*1, &
         min(abs(intval)/10**2,1)*2, &
         min(abs(intval)/10**3,1)*3, &
         min(abs(intval)/10**4,1)*4, &
         min(abs(intval)/10**5,1)*5, &
         min(abs(intval)/10**6,1)*6, &
         min(abs(intval)/10**7,1)*7, &
         min(abs(intval)/10**8,1)*8, &
         min(abs(intval)/10**9,1)*9) ) :: string
    integer :: absn,j,k,is

    absn = abs(intval)
    if ( absn == intval ) then
            is = 1
    else
            is = 2
            string(1:1) = "-"
    end if
    do j=len(string),is,-1
            k = modulo(absn,10)+1
            string(j:j) = "0123456789"(k:k)
            absn = absn / 10
    end do
    return

  end function int2str

  !> Write a real to a string with 15 digits
  !> +1.23456789012345E000
  function real2str(realval) result(string)
    character(*), parameter :: SAMPLE = '+1.23456789012345E000'
    !                                     0.10000000000000E+03
    !                                    -0.10000000000000E+03
    real(kind=R8), intent(in) :: realval
    character(len(SAMPLE)) :: string

    write( string, '(es21.14)' ) realval
    return

  end function real2str

  end module eqdim

!!!Local Variables:
!!! mode: f90
!!! End:
