!=================================================
      SUBROUTINE BIN_EVOUT
      INCLUDE '(DBLPRC)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(PAPROP)'
      INCLUDE '(MYOUT)'
      INCLUDE '(MYSOURCE)'
!       NREC_CURR=NREC_CURR+1
      INTEGER*2 b_desc,b_pid,b_par
      INTEGER b_num
      REAL*8 bR(6)
      b_desc=0
      b_num=INT(NEV_CURR)
      b_pid=INT2(EV_TYPE(1))
      b_par=0
      bR(1)=SNGL(EV_ETOT)
      bR(2)=SNGL(EV_TX)
      bR(3)=SNGL(EV_TY)
      bR(4)=SNGL(EV_X)
      bR(5)=SNGL(EV_Y)
      bR(6)=SNGL(EV_Z)
      IF(OUT_TXT(1))THEN
      WRITE(28,*)"*========================="
      WRITE(28,12),'~Evt=',NEV_CURR,EV_TYPE(1),
     &      EV_ETOT,EV_TX,EV_TY,EV_X,EV_Y,EV_Z
!      FLUSH(28)

      WRITE(30,114)NEV_CURR,EV_TYPE(1),EV_TYPE(2),0,
     &      NVTX_TOT,NTRK_TOT,EV_ETOT

      ENDIF
12    FORMAT(A5,I8.1,I5.1,6f10.5)
114    FORMAT(6I8,f10.5)
      IF(OUT_BIN(1))THEN
      WRITE(29)b_desc,b_num,b_pid,b_par,bR
      ENDIF
      END SUBROUTINE BIN_EVOUT
!=================================================
      SUBROUTINE BIN_VTXOUT
      INCLUDE '(DBLPRC)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(PAPROP)'
      INCLUDE '(TRACKR)'
      INCLUDE '(MYOUT)'
      INTEGER*2 b_desc,b_pid,b_par
      INTEGER  b_num
      REAL*8 b_P,b_Tx,b_Ty,b_X,b_Y,b_Z
      b_desc=1
      b_num=INT(LLOUSE)
      b_pid=INT2(JTRACK)
      b_par=INT2(ISPUSR(1))
      b_P=SNGL(PTRACK)
      b_TX=SNGL(CXTRCK)
      b_TY=SNGL(CYTRCK)
      b_TZ=SNGL(CZTRCK)
      IF(b_TZ.EQ.0) b_TZ=0.0000001
      b_X=SNGL(XTRACK(NTRACK))
      b_Y=SNGL(YTRACK(NTRACK))
      b_Z=SNGL(ZTRACK(NTRACK))
      IF(OUT_TXT(2))THEN
      WRITE(28,119),
     &      "-iN=",b_num,PRNAME(b_pid),"(",b_pid,")",b_par,
     &      b_P,b_TX,b_TY,b_X,b_Y,b_Z
119    FORMAT(A5,I5.1,A10,A1,I5.1,A1,I5.1,G18.8,G18.8,G18.8,G18.8,
     &      G18.8,G18.8)
      ENDIF
      IF(OUT_BIN(2))THEN
      WRITE(29) b_desc,b_num,b_pid,b_par,
     &      b_P,b_TX,b_TY,b_X,b_Y,b_Z
      ENDIF
      END SUBROUTINE BIN_VTXOUT
!=================================================
      SUBROUTINE BIN_TRKOUT(IDSC,NP_TRK)
      INCLUDE '(DBLPRC)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(PAPROP)'
      INCLUDE '(FLKSTK)'
      INCLUDE '(TRACKR)'
      INCLUDE '(MYOUT)'
      INCLUDE '(MYSOURCE)'
      INTEGER*2 b_desc,b_pid,b_par
      INTEGER   b_num
      REAL*8 b_P,b_Tx,b_Ty,b_Tz,b_X,b_Y,b_Z
      b_desc=INT2(IDSC)
      IF(IDSC.LE.0)b_desc=-b_desc
      IF(NP_TRK.GE.0)THEN
        b_num=INT(LOUSE(NP_TRK))
        b_pid=INT2(ISPARK(2,NP_TRK))
        b_par=INT2(ISPARK(1,NP_TRK))
      ELSE
        b_num=0
        b_pid=INT2(EV_TYPE(1))
        b_par=0
      ENDIF

      IF(IDSC.EQ.1)THEN
        b_P=SNGL(PTRACK)
        IF(CZTRCK.LE.0)b_P=-b_P
        b_TX=SNGL(CXTRCK)
        b_TY=SNGL(CYTRCK)
        b_TZ=SNGL(CZTRCK)
        IF(b_TZ.EQ.0) b_TZ=0.0000001
        b_X=SNGL(VCURR_X)
        b_Y=SNGL(VCURR_Y)
        b_Z=SNGL(VCURR_Z)
      ELSE
        b_P=SNGL(PMOFLK(NP_TRK))
        b_TX=SNGL(TXFLK(NP_TRK))
        b_TY=SNGL(TYFLK(NP_TRK))
        b_TZ=SNGL(TZFLK(NP_TRK))
        IF(b_TZ.LE.0) b_P=-b_P
        IF(b_TZ.EQ.0) b_TZ=0.0000001
        b_X=SNGL(XFLK(NP_TRK))
        b_Y=SNGL(YFLK(NP_TRK))
        b_Z=SNGL(ZFLK(NP_TRK))
      ENDIF
      IF(OUT_TXT(1+b_desc))THEN
         IF(b_desc.EQ.1) THEN
         WRITE(28,111),
     &      "-IN=",b_num,PRNAME(b_pid),"(",b_pid,")",b_par,
     &      b_P,b_TX,b_TY,b_X,b_Y,b_Z
          WRITE(30,112),
     &      NVTX_CURR,NTRK_VTX,b_num,b_pid,
     &      b_X*10,b_Y*10,b_Z*10
         ELSEIF(b_desc.EQ.2) THEN
!         WRITE(28,111),
!     &      "+out=",b_num,PRNAME(b_pid),"(",b_pid,")",b_par,
!     &      b_P,b_TX,b_TY,b_X,b_Y,b_Z
         WRITE(28,111),
     &      "+out=",b_num,PRNAME(b_pid),"(",b_pid,")",b_par,
     &      b_P,", dir=[",b_TX,b_TY,b_TZ,"]"
         WRITE(30,113),
     &      b_num,NTRK_CURR,NVTX_CURR,b_pid,
     &      b_P*b_TX,b_P*b_TY,b_P*b_TZ,
     &      0.,0.,0.,0.,0.,0.,0.
      ENDIF
!      FLUSH(28)
      ENDIF

      IF(OUT_BIN(1+b_desc))THEN
        WRITE(29) b_desc,b_num,b_pid,b_par,
     &      b_P,b_TX/b_TZ,b_TY/b_TZ,b_X,b_Y,b_Z
      ENDIF
      
111     FORMAT(A5,I5.1,A10,A1,I5.1,A1,I5.1,6G12.4)
112     FORMAT(4i5.1,3f10.5)
113     FORMAT(4i5.1,10f10.5)
      END SUBROUTINE BIN_TRKOUT
!=================================================
      SUBROUTINE BIN_TRK_IN(LPRIMARY,IFLAGG)
      INCLUDE '(DBLPRC)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(PAPROP)'
      INCLUDE '(FLKSTK)'
      INCLUDE '(TRACKR)'
      INCLUDE '(MYOUT)'
      INCLUDE '(MYSOURCE)'
      INTEGER*2 b_desc,b_pid,b_par
      INTEGER   b_num
      REAL*8 b_P,b_Tx,b_Ty,b_Tz,b_X,b_Y,b_Z
      b_desc=1
      IF(LPRIMARY.EQ.1)THEN
        IF(NSTAOL.NE.-1)THEN
        b_num=INT(LOUSE(NSTAOL))
        b_pid=INT2(ISPARK(2,NSTAOL))
        b_par=INT2(ISPARK(1,NSTAOL))
        ELSE
        b_num=0
        b_pid=INT2(EV_TYPE(1))
        b_par=0
        END IF
      ELSE
        b_num=LLOUSE
        b_pid=INT2(ISPUSR(2))
        b_par=INT2(IFLAGG)
      ENDIF
        b_P=SNGL(PTRACK)
        
        
        b_TX=SNGL(CXTRCK)
        b_TY=SNGL(CYTRCK)
        b_TZ=SNGL(CZTRCK)
        IF(b_TZ.EQ.0) b_TZ=0.0000001
        b_X=SNGL(VCURR_X)
        b_Y=SNGL(VCURR_Y)
        b_Z=SNGL(VCURR_Z)
      
     
        
      IF(OUT_TXT(1+b_desc))THEN
          WRITE(28,111),
     &      "--IN=",b_num,PRNAME(b_pid),"(",b_pid,")",b_par,
     &      b_P,", dir=[",b_TX,b_TY,b_TZ,"], pos=",b_X,b_Y,b_Z
!          FLUSH(28)
          WRITE(30,112),
     &      NVTX_CURR,NTRK_VTX,b_num,b_pid,
     &      b_X*10,b_Y*10,b_Z*10
      ENDIF
      IF(OUT_BIN(1+b_desc))THEN
        IF(b_TZ.LE.0)b_P=-b_P
        WRITE(29) b_desc,b_num,b_pid,b_par,
     &      b_P,b_TX/b_TZ,b_TY/b_TZ,b_X,b_Y,b_Z
      ENDIF
      
111     FORMAT(A5,I5.1,A10,A1,I5.1,A1,I5.1,G12.4,
     &  A7,3G12.4,A7,3G12.4)
112     FORMAT(4i5.1,3f10.5)
      END SUBROUTINE BIN_TRK_IN
!=================================================
      SUBROUTINE BIN_TRK_OUT(NP_TRK)
      INCLUDE '(DBLPRC)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(PAPROP)'
      INCLUDE '(GENSTK)'
      INCLUDE '(TRACKR)'
      INCLUDE '(MYOUT)'
      INCLUDE '(MYSOURCE)'
      INTEGER*2 b_desc,b_pid,b_par
      INTEGER   b_num
      REAL*8 b_P,b_Tx,b_Ty,b_Tz,b_X,b_Y,b_Z
      b_desc=2
      b_num=INFEXT(NP_TRK)
      IF(KPART(NP_TRK).GE.-6)THEN
        b_pid=INT2(KPART(NP_TRK))
      ELSE 
        b_pid=-2
      END IF
      b_par=INT2(LLOUSE)
      b_P=SNGL(PLR(NP_TRK))
      
      b_TX=SNGL(CXR(NP_TRK))
      b_TY=SNGL(CYR(NP_TRK))
      b_TZ=SNGL(CZR(NP_TRK))
      IF(b_TZ.EQ.0) b_TZ=0.0000001
      b_X=SNGL(VCURR_X)
      b_Y=SNGL(VCURR_Y)
      b_Z=SNGL(VCURR_Z)
      IF(OUT_TXT(1+b_desc))THEN
      IF(b_pid.EQ.-7)THEN
      WRITE(28,111),
     &      "+ouH=",b_num,"HEAVY","(",b_pid,")",b_par,
     &      b_P,", dir=[",b_TX,b_TY,b_TZ,"]"
      ELSE
      WRITE(28,111),
     &      "+out=",b_num,PRNAME(b_pid),"(",b_pid,")",b_par,
     &      b_P,", dir=[",b_TX,b_TY,b_TZ,"]"
      END IF
      WRITE(30,113),
     &      b_num,NTRK_CURR,NVTX_CURR,b_pid,
     &      b_P*b_TX,b_P*b_TY,b_P*b_TZ,
     &      0.,0.,0.,0.,0.,0.,0.
!      FLUSH(28)
      ENDIF

      IF(OUT_BIN(1+b_desc))THEN
        IF(b_TZ.LE.0) b_P=-b_P
        WRITE(29) b_desc,b_num,b_pid,b_par,
     &      b_P,b_TX/b_TZ,b_TY/b_TZ,b_X,b_Y,b_Z
      ENDIF
      
111     FORMAT(A5,I5.1,A10,A1,I5.1,A1,I5.1,G12.4,
     &  A7,3G12.4,A1)
113     FORMAT(4i5.1,10f10.5)
      END SUBROUTINE BIN_TRK_OUT
!=================================================
      SUBROUTINE BIN_HVY_OUT(NP_TRK)
      INCLUDE '(DBLPRC)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(PAPROP)'
      INCLUDE '(FHEAVY)'
      INCLUDE '(TRACKR)'
      INCLUDE '(MYOUT)'
      INCLUDE '(MYSOURCE)'
      INTEGER*2 b_desc,b_pid,b_par
      INTEGER   b_num
      REAL*8 b_P,b_Tx,b_Ty,b_Tz,b_X,b_Y,b_Z
      b_desc=2
      b_num=INFHEA(NP_TRK)
      b_pid=-INT2(KHEAVY(NP_TRK))
      b_par=INT2(LLOUSE)
      b_P=SNGL(PHEAVY(NP_TRK))
      
      b_TX=SNGL(CXHEAV(NP_TRK))
      b_TY=SNGL(CYHEAV(NP_TRK))
      b_TZ=SNGL(CZHEAV(NP_TRK))
      IF(b_TZ.EQ.0) b_TZ=0.0000001
      b_X=SNGL(VCURR_X)
      b_Y=SNGL(VCURR_Y)
      b_Z=SNGL(VCURR_Z)
      IF(OUT_TXT(1+b_desc))THEN
      WRITE(28,111),
     &      "+HVY=",b_num,ANHEAV(-b_pid),"(",b_pid,")",
     &      ICHEAV(-b_pid),"/",IBHEAV(-b_pid),
     &      b_P,", dir=[",b_TX,b_TY,b_TZ,"]"
      WRITE(30,113),
     &      b_num,NTRK_CURR,NVTX_CURR,b_pid,
     &      b_P*b_TX,b_P*b_TY,b_P*b_TZ,
     &      0.,0.,0.,0.,0.,0.,0.
!      FLUSH(28)
      ENDIF

      IF(OUT_BIN(1+b_desc))THEN
        IF(b_TZ.LE.0) b_P=-b_P
        WRITE(29) b_desc,b_num,b_pid,b_par,
     &      b_P,b_TX/b_TZ,b_TY/b_TZ,b_X,b_Y,b_Z
      ENDIF
      
111     FORMAT(A5,I5.1,A10,A1,I5.1,A1,I3.1,A1,I4.1,G12.4,
     &  A7,3G12.4,A1)
113     FORMAT(4i5.1,10f10.5)
      END SUBROUTINE BIN_HVY_OUT
!=================================================
      SUBROUTINE BIN_EMOUT(IDESC,MREG)
      INCLUDE '(DBLPRC)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(TRACKR)'
      INCLUDE '(FLKSTK)'
      INCLUDE '(MYOUT)'
      INTEGER*2 b_desc,b_pid,b_par
      INTEGER b_num
      REAL*8 b_P,b_Tx,b_Ty,b_X,b_Y,b_Z
      b_desc=INT2(IDESC)
      b_num=INT(LLOUSE)
      b_pid=INT2(JTRACK)
      b_par=INT2(MREG)
      b_P=SNGL(PTRACK)
      !IF(CZTRCK.EQ.0)CZTRCK=0.0000001
      b_TX=SNGL(CXTRCK)
      b_TY=SNGL(CYTRCK)
      b_TZ=SNGL(CZTRCK)
      b_X=SNGL(XTRACK(NTRACK))
      b_Y=SNGL(YTRACK(NTRACK))
      b_Z=SNGL(ZTRACK(NTRACK))
      IF(OUT_TXT(4)) THEN
      IF(IDESC .eq. 3)THEN
      WRITE(28,10),"<EM_i=",LLOUSE,JTRACK,ISPUSR(1),MREG,PTRACK,
     &     XTRACK(NTRACK),YTRACK(NTRACK),ZTRACK(NTRACK),
     &     CXTRCK,CYTRCK,CZTRCK;
      ELSE
      WRITE(28,10),">EM_o=",LLOUSE,JTRACK,ISPUSR(1),MREG,PTRACK,
     &     XTRACK(NTRACK),YTRACK(NTRACK),ZTRACK(NTRACK),
     &     CXTRCK,CYTRCK,CZTRCK;
      ENDIF
!      WRITE(28,*),"emul: P=[",CXTRCK*PTRACK,CYTRCK*PTRACK,CZTRCK*PTRACK,"]"
!      WRITE(28,*),"vs  : P=[",PMOFLK(LLOUSE)*TXFLK(LLOUSE),
!     &     PMOFLK(LLOUSE)*TYFLK(LLOUSE),
!     &     PMOFLK(LLOUSE)*TZFLK(LLOUSE),"]"
!      FLUSH(28)
      ENDIF
      IF(OUT_BIN(4)) THEN
      WRITE(29) b_desc,b_num,b_pid,b_par,
     &      b_P,b_TX/b_TZ,b_TY/b_TZ,b_X,b_Y,b_Z
      ENDIF
10       FORMAT(A6,4I5.1,7G15.5)
      END SUBROUTINE BIN_EMOUT
!========================================================
