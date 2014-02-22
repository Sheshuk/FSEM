*$ CREATE MGDRAW.FOR
*COPY MGDRAW
*                                                                      *
*=== mgdraw ===========================================================*
*                                                                      *
!      INCLUDE 'binout.f'
      SUBROUTINE MGDRAW ( ICODE, MREG )

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
*
*----------------------------------------------------------------------*
*                                                                      *
*     Copyright (C) 1990-2006      by        Alfredo Ferrari           *
*     All Rights Reserved.                                             *
*                                                                      *
*                                                                      *
*     MaGnetic field trajectory DRAWing: actually this entry manages   *
*                                        all trajectory dumping for    *
*                                        drawing                       *
*                                                                      *
*     Created on   01 march 1990   by        Alfredo Ferrari           *
*                                              INFN - Milan            *
*     Last change  05-may-06       by        Alfredo Ferrari           *
*                                              INFN - Milan            *
*                                                                      *
*----------------------------------------------------------------------*
*
      INCLUDE '(CASLIM)'
      INCLUDE '(COMPUT)'
      INCLUDE '(SOURCM)'
      INCLUDE '(FHEAVY)'
      INCLUDE '(FLKSTK)'
      INCLUDE '(FLKMAT)'
      INCLUDE '(GENSTK)'
      INCLUDE '(MGDDCM)'
      INCLUDE '(PAPROP)'
      INCLUDE '(QUEMGD)'
      INCLUDE '(SUMCOU)'
      INCLUDE '(TRACKR)'
      INCLUDE '(MYSTCK)'
      INCLUDE '(MYSOURCE)'
      INCLUDE '(MYOUT)'
      INCLUDE '(MYGLOB)'
*
      DIMENSION DTQUEN ( MXTRCK, MAXQMG )
*
      CHARACTER*20 FILNAM
      LOGICAL LFCOPE
      SAVE LFCOPE
      DATA LFCOPE / .FALSE. /
      
      REAL*8 DR(3)
      REAL*8 DP(3)
      REAL*8 DRDP,DST
      REAL*8 DP2,DR2
      REAL*8 COS_TH
      LOGICAL LNEW
        
*
*----------------------------------------------------------------------*
*                                                                      *
*     Icode = 1: call from Kaskad                                      *
*     Icode = 2: call from Emfsco                                      *
*     Icode = 3: call from Kasneu                                      *
*     Icode = 4: call from Kashea                                      *
*     Icode = 5: call from Kasoph                                      *
*                                                                      *
*----------------------------------------------------------------------*
!       return
! WORKAROUND to kill decaying particle
!       WRITE(28,*)"part ",LLOUSE,ICODE
      IF(NSTP.EQ.0)GOTO 11
      DO 10 I=1,NSTP
      IF(LLOUSE .NE. ISTP(I))GOTO 10
      DR(1)=XSTP(I)-XTRACK(0)
      DR(2)=YSTP(I)-YTRACK(0)
      DR(3)=ZSTP(I)-ZTRACK(0)
      DP(1)=XTRACK(NTRACK)-XTRACK(0)
      DP(2)=YTRACK(NTRACK)-YTRACK(0)
      DP(3)=ZTRACK(NTRACK)-ZTRACK(0)
      DR2=DR(1)*DR(1)+DR(2)*DR(2)+DR(3)*DR(3)
      DP2=DP(1)*DP(1)+DP(2)*DP(2)+DP(3)*DP(3)
      DRDP=DR(1)*DP(1)+DR(2)*DP(2)+DR(3)*DP(3)
      IF(DRDP<0)GOTO 11
      DRDP=DRDP*DRDP/DP2
      IF(DRDP>DP2)GOTO 11
      DST=DR2-DRDP
      IF(DST .LE. 0.00025) THEN
            LPKILL=.TRUE.
            XTRACK(NTRACK)=XSTP(I)
            YTRACK(NTRACK)=YSTP(I)
            ZTRACK(NTRACK)=ZSTP(I)
!             WRITE(28,*)"* KILL #",LLOUSE," in ",MATNAM(MEDFLK(MREG,1)),
!      &        " (",MREG,")  [",XSTP(I)*1e4,YSTP(I)*1e4,ZSTP(I)*1e4,"]"
        ENDIF
      
      DIST(I)=SNGL(DST)
10    CONTINUE

!  STORE track segment information:
        IF(MATNAM(MEDFLK(MREG,1)).NE."EMULSION") RETURN
        IF(OUT_TXT(4))WRITE(28,*)LLOUSE,ISPUSR(2),"ZFRTTK=",ZFRTTK
11      IF(ZFRTTK.EQ.0)RETURN
      
!         NTRACK=NTRACK+1
!       ELSE IF(NTRACK.GE.1) THEN
       
       IF(OUT_TXT(5))THEN 
          WRITE(28,*) "TRAJECTORY FOR TRK#",LLOUSE," (",ISPUSR(2)
     #,") in reg=",MREG," np=",NTRACK
          DO 999 I=0,NTRACK,1
999       WRITE(28,1111)I,SNGL 
     #    (XTRACK(I)*1E4),YTRACK(I)*1E4,ZTRACK(I)*1E4
       ENDIF
       IF(OUT_BIN(5))THEN
        WRITE (101) INT2(MREG), INT(LLOUSE), INT(ISPUSR(2)),
     #              SNGL (ZFRTTK),
     #              SNGL (ETRACK*1e3),
     #              SNGL (PTRACK*1e3),
     #              SNGL (XTRACK (0)*1E4),
     #              SNGL (YTRACK (0)*1E4),
     #              SNGL (ZTRACK (0)*1E4),
     #              SNGL (XTRACK (NTRACK)*1E4),
     #              SNGL (YTRACK (NTRACK)*1E4),
     #              SNGL (ZTRACK (NTRACK)*1E4)
      ENDIF
      RETURN
1111    FORMAT(I3.1,G18.8,G18.8,G18.8)
*
*======================================================================*
*                                                                      *
*     Boundary-(X)crossing DRAWing:                                    *
*                                                                      *
*     Icode = 1x: call from Kaskad                                     *
*             19: boundary crossing                                    *
*     Icode = 2x: call from Emfsco                                     *
*             29: boundary crossing                                    *
*     Icode = 3x: call from Kasneu                                     *
*             39: boundary crossing                                    *
*     Icode = 4x: call from Kashea                                     *
*             49: boundary crossing                                    *
*     Icode = 5x: call from Kasoph                                     *
*             59: boundary crossing                                    *
*                                                                      *
*======================================================================*
*                                                                      *
      ENTRY BXDRAW ( ICODE, MREG, NEWREG, XSCO, YSCO, ZSCO )
         IF(.NOT.(OUT_TXT(4).OR.OUT_BIN(4)))return

*       WRITE(28,*),"ICODE=",ICODE,MREG,NEWREG,JTRACK,LLOUSE,ZFRTTK
       FLUSH(28)
         IF(JTRACK.LE.-10000) THEN
            WRITE(28,*),"*BXDRAW skips part #",LLOUSE
            RETURN
         END IF
      
      IF(ZFRTTK.EQ.0)RETURN
      

         IF(MATNAM(MEDFLK(MREG,1)).EQ."EMULSION")
     &         call BIN_EMOUT(4,MREG)
         IF(MATNAM(MEDFLK(NEWREG,1)).EQ."EMULSION")
     &         call BIN_EMOUT(3,NEWREG)
      RETURN
*
*======================================================================*
*                                                                      *
*     Event End DRAWing:                                               *
*                                                                      *
*======================================================================*
*                                                                      *
      ENTRY EEDRAW ( ICODE )
      RETURN
*
*======================================================================*
*                                                                      *
*     ENergy deposition DRAWing:                                       *
*                                                                      *
*     Icode = 1x: call from Kaskad                                     *
*             10: elastic interaction recoil                           *
*             11: inelastic interaction recoil                         *
*             12: stopping particle                                    *
*             13: pseudo-neutron deposition                            *
*             14: escape                                               *
*             15: time kill                                            *
*     Icode = 2x: call from Emfsco                                     *
*             20: local energy deposition (i.e. photoelectric)         *
*             21: below threshold, iarg=1                              *
*             22: below threshold, iarg=2                              *
*             23: escape                                               *
*             24: time kill                                            *
*     Icode = 3x: call from Kasneu                                     *
*             30: target recoil                                        *
*             31: below threshold                                      *
*             32: escape                                               *
*             33: time kill                                            *
*     Icode = 4x: call from Kashea                                     *
*             40: escape                                               *
*             41: time kill                                            *
*             42: delta ray stack overflow                             *
*     Icode = 5x: call from Kasoph                                     *
*             50: optical photon absorption                            *
*             51: escape                                               *
*             52: time kill                                            *
*                                                                      *
*======================================================================*
*                                                                      *
      ENTRY ENDRAW ( ICODE, MREG, RULL, XSCO, YSCO, ZSCO )
        
      RETURN
*
*======================================================================*
*                                                                      *
*     SOurce particle DRAWing:                                         *
*                                                                      *
*======================================================================*
*
      ENTRY SODRAW

      RETURN
*
*======================================================================*
*                                                                      *
*     USer dependent DRAWing:                                          *
*                                                                      *
*     Icode = 10x: call from Kaskad                                    *
*             100: elastic   interaction secondaries                   *
*             101: inelastic interaction secondaries                   *
*             102: particle decay  secondaries                         *
*             103: delta ray  generation secondaries                   *
*             104: pair production secondaries                         *
*             105: bremsstrahlung  secondaries                         *
*             110: decay products                                      *
*     Icode = 20x: call from Emfsco                                    *
*             208: bremsstrahlung secondaries                          *
*             210: Moller secondaries                                  *
*             212: Bhabha secondaries                                  *
*             214: in-flight annihilation secondaries                  *
*             215: annihilation at rest   secondaries                  *
*             217: pair production        secondaries                  *
*             219: Compton scattering     secondaries                  *
*             221: photoelectric          secondaries                  *
*             225: Rayleigh scattering    secondaries                  *
*     Icode = 30x: call from Kasneu                                    *
*             300: interaction secondaries                             *
*     Icode = 40x: call from Kashea                                    *
*             400: delta ray  generation secondaries                   *
*  For all interactions secondaries are put on GENSTK common (kp=1,np) *
*  but for KASHEA delta ray generation where only the secondary elec-  *
*  tron is present and stacked on FLKSTK common for kp=npflka          *
*                                                                      *
*======================================================================*
*

      ENTRY USDRAW ( ICODE, MREG, XSCO, YSCO, ZSCO )

        
       IF(OUT_TXT(2))
     &   WRITE(28,*)"UsrDrw",ICODE,"NP=",NP,NP0,NPHEAV,
     &   "TRK=",JTRACK,ISPUSR(2)
        FLUSH(28)
        IF(ICODE/100.EQ.2)RETURN
        NTRK_VTX=NP
        NVTX_CURR=NVTX_CURR+1
        VCURR_X=XSCO
        VCURR_Y=YSCO
        VCURR_Z=ZSCO
        CALL BIN_TRK_IN(0,ICODE)
        NTRK_CURR=0
        DO 901 I = 1,NP
!          WRITE(28,*)I,"part",KPART(I)
!          FLUSH(28)
          LNEW=.TRUE.
          IF(I.EQ.NP.AND.ICODE.EQ.300)THEN
            LNEW=.FALSE.
          ELSE 
            IF(KPART(I) .EQ. JTRACK) THEN
            COS_TH=(CXR(I)*CXTRCK+
     &              CYR(I)*CYTRCK+
     &              CZR(I)*CZTRCK)
             IF(COS_TH.GE.MIN_COS)LNEW=.FALSE.  
           END IF
          END IF
C          IF(((KPART(I).EQ.3).OR.(KPART(I).EQ.4.))
C     &   .AND.PLR(I).LT.0.03)THEN
C            WRITE(28,*) "skip e/p"
C            INFEXT(I)=-1
C            GOTO 901
C          END IF
         
          IF(LNEW)THEN
            NIDMAX=NIDMAX+1
            INFEXT(I)=NIDMAX
          ELSE
            INFEXT(I)=LLOUSE
          ENDIF
          NTRK_CURR=NTRK_CURR+1
          call BIN_TRK_OUT(I)
901     END DO
!       WRITE(28,*)"Done",NP
        DO I = 1,NPHEAV
          NIDMAX=NIDMAX+1
          INFHEA(I)=NIDMAX
          call BIN_HVY_OUT(I)
        END DO
!      WRITE(28,*)"Done hvy",NPHEAV
      RETURN
! 1     FORMAT(A4,I5.1,I5.1,G18.8,G18.8,G18.8,G18.8,G18.8,G18.8,
!      &     G18.8,G18.8,G18.8)
*=== End of subrutine Mgdraw ==========================================*
      END

