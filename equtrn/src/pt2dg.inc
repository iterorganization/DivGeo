c  version : 21.10.2000 00:19
      parameter 
     +(                  ndisk  = 4,     nread = 7,      nwrite = 6
     +,                  nwreqdg = 99,   nparm=5
     +,                  nodele = 6,     nnodes = 5000,  nelems  = 3000 
     +,                  npr    = 513,   npz = 513
     +)                                                                 
      common  /cotran/   keynod(nodele,nelems),          kerror
     +,                  keyreg(nelems), kbound(nnodes), kel 
     +,                  mpr,            mpz
     +,                  pnodr(6),       pnodz(6),       psiele(6)
     +,                  psi(nnodes),    rznods(2,nnodes)               
     +,                  xrp(npr),       xzp(npz),       psi4(npr,npz) 
     +,                  yrp,            yzp,            ypsi 
     +,                  csi,            eta
     +,                  ybpolr,         ybpolz
     +,                  bpr(npr,npz),   bpz(npr,npz)
      character          cmesht*80    
      common  /cochar/   cmesht                                 
c                                                                       
c-----------------------------------------------------------------------
c                                                                       
