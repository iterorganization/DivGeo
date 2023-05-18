
#ifdef CONSTANTS_PROVIDED
include 'mathematical_constants.f90'              ! IGNORE
#endif

  module eqdim
#ifdef CONSTANTS_PROVIDED
  use mathematical_constants, only: PI => M_PI    ! IGNORE
#elif ( defined(IMAS) && IMAS_MINOR_VERSION < 35 )
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
#if IMAS_MINOR_VERSION < 35
  parameter (pi=imas_constants%pi)
#else
  parameter (pi=3.141592653589793_R8)
#endif
#else
  parameter (pi=3.141592653589793_R8)
#endif
#endif
  end module eqdim

!!!Local Variables:
!!! mode: f90
!!! End:
