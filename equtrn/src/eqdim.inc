c
      INTEGER, PARAMETER :: R8 = SELECTED_REAL_KIND (14)
      INTEGER, PARAMETER :: R4 = SELECTED_REAL_KIND (6)
      INTEGER, PARAMETER :: I4 = SELECTED_INT_KIND (8)

c Indicate here the maximum size of the equilibrium files to be used
c ngpr: number of points in the radial direction
c ngpz: number of points in the vertical direction
      integer ngpr,ngpz
      parameter (ngpr=1175, ngpz=1175)
      real (kind=R8) :: pi
      parameter (pi=3.14159 26535 89793_R8)
