c  version : 21.10.2000 00:19
      integer            ndisk, nread, nwrite, nwrptdg, nparm
     +,                  nodele, nnodes, nelems, npr, npz
      parameter 
     +(                  ndisk  = 4,     nread = 7,      nwrite = 6
     +,                  nwrptdg = 99,   nparm=5
     +,                  nodele = 6,     nnodes = 5000,  nelems  = 3000 
     +,                  npr    = ngpr,  npz = ngpz
     +)                    
      integer            keynod(nodele,nelems), keyreg(nelems)
     +,                  kbound(nnodes)
      integer            kerror, kel, mpr, mpz
      real(kind=R8) ::   pnodr(6), pnodz(6), xrp(npr), xzp(npz)
     +,                  psi4(npr,npz), psiele(6), psi(nnodes)
     +,                  rznods(2,nnodes), yrp, yzp, ypsi, csi, eta
     +,                  ybpolr, ybpolz, bpr(npr,npz), bpz(npr,npz)
      common  /cotran/   keynod,         kerror
     +,                  keyreg,         kbound,         kel
     +,                  mpr,            mpz
     +,                  pnodr,          pnodz,          psiele
     +,                  psi,            rznods
     +,                  xrp,            xzp,            psi4 
     +,                  yrp,            yzp,            ypsi 
     +,                  csi,            eta
     +,                  ybpolr,         ybpolz
     +,                  bpr,            bpz
      character          cmesht*80    
      common  /cochar/   cmesht                                 
c                                                                       
c-----------------------------------------------------------------------
c                                                                       
