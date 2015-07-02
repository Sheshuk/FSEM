*$ CREATE SOURCE.FOR
*COPY SOURCE
*
*=======================================================================
*=======================================================================
*=======================================================================

!       INCLUDE 'binout.f'

      SUBROUTINE PUSHPART(IPART,PX,PY,PZ,X,Y,Z)
      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(BEAMCM)'
      INCLUDE '(FHEAVY)'
      INCLUDE '(FLKSTK)'
      INCLUDE '(IOIOCM)'
      INCLUDE '(LTCLCM)'
      INCLUDE '(PAPROP)'
      INCLUDE '(SOURCM)'
      INCLUDE '(SUMCOU)'
      INCLUDE '(MYSOURCE)'
      DOUBLE PRECISION PX,PY,PZ,X,Y,Z
*  Push one source particle to the stack. Note that you could as well
*  push many but this way we reserve a maximum amount of space in the
*  stack for the secondaries to be generated
* Npflka is the stack counter: of course any time source is called it
* must be =0

      NPFLKA = NPFLKA + 1
* Wt is the weight of the particle
      WTFLK  (NPFLKA) = ONEONE
      WEIPRI = WEIPRI + WTFLK (NPFLKA)
* Particle type (1=proton.....). Ijbeam is the type set by the BEAM
* card
*  +-------------------------------------------------------------------*

*  |  (Radioactive) isotope:
      IF ( IPART .EQ. -2 .AND. LRDBEA ) THEN
         IARES  = IPROA
         IZRES  = IPROZ
         IISRES = IPROM
         CALL STISBM ( IARES, IZRES, IISRES )
         IJHION = IPROZ  * 1000 + IPROA
         IJHION = IJHION * 100 + KXHEAV
         IONID  = IJHION
         CALL DCDION ( IONID )
         CALL SETION ( IONID )
*  |
*  +-------------------------------------------------------------------*
*  |  Heavy ion:
      ELSE IF ( IPART .EQ. -2 ) THEN
         IJHION = IPROZ  * 1000 + IPROA
         IJHION = IJHION * 100 + KXHEAV
         IONID  = IJHION
         CALL DCDION ( IONID )
         CALL SETION ( IONID )
         ILOFLK (NPFLKA) = IJHION
*  |  Flag this is prompt radiation
         LRADDC (NPFLKA) = .FALSE.
*  |  Group number for "low" energy neutrons, set to 0 anyway
         IGROUP (NPFLKA) = 0
*  |
*  +-------------------------------------------------------------------*
*  |  Normal hadron:
      ELSE
         IONID = IPART
         ILOFLK (NPFLKA) = IPART
*  |  Flag this is prompt radiation
         LRADDC (NPFLKA) = .FALSE.
*  |  Group number for "low" energy neutrons, set to 0 anyway
         IGROUP (NPFLKA) = 0
      END IF
*  |
*  +-------------------------------------------------------------------*
* From this point .....
* Particle generation (1 for primaries)
      LOFLK  (NPFLKA) = 1
* User dependent flag:
      LOUSE  (NPFLKA) = 0
* User dependent spare variables:
      DO 100 ISPR = 1, MKBMX1
         SPAREK (ISPR,NPFLKA) = ZERZER
 100  CONTINUE
* User dependent spare flags:
      DO 200 ISPR = 1, MKBMX2
         ISPARK (ISPR,NPFLKA) = 0
 200  CONTINUE
* Save the track number of the stack particle:
      ISPARK (MKBMX2,NPFLKA) = NPFLKA
      NPARMA = NPARMA + 1
      NUMPAR (NPFLKA) = NPARMA
      NEVENT (NPFLKA) = 0
      DFNEAR (NPFLKA) = +ZERZER
* ... to this point: don't change anything
*========================================================
* here we will read events from ascii file (#33)
*========================================================
!       READ(33,*) NEV_CURR,
* Particle age (s)
      AGESTK (NPFLKA) = +ZERZER
      AKNSHR (NPFLKA) = -TWOTWO
* Kinetic energy of the particle (GeV)
!       DOUBLE PRECISION PTOT2
      PTOT2=PX*PX+PY*PY+PZ*PZ
      TKEFLK (NPFLKA) = SQRT ( PTOT2 + AM (IONID)**2 ) - AM (IONID)
* Particle momentum
!       DOUBLE PRECISION PTOT
      PTOT=SQRT(PTOT2)
      PMOFLK (NPFLKA) = PTOT
*     PMOFLK (NPFLKA) = SQRT ( TKEFLK (NPFLKA) * ( TKEFLK (NPFLKA)
*              &                       + TWOTWO * AM (IONID) ) )
* Cosines (tx,ty,tz)
      TXFLK  (NPFLKA) = PX/PTOT
      TYFLK  (NPFLKA) = PY/PTOT
      TZFLK  (NPFLKA) = PZ/PTOT
*     TZFLK  (NPFLKA) = SQRT ( ONEONE - TXFLK (NPFLKA)**2
*              &                       - TYFLK (NPFLKA)**2 )
* Polarization cosines:
      TXPOL  (NPFLKA) = -TWOTWO
      TYPOL  (NPFLKA) = +ZERZER
      TZPOL  (NPFLKA) = +ZERZER
* Particle coordinates
      XFLK   (NPFLKA) = X
      YFLK   (NPFLKA) = Y
      ZFLK   (NPFLKA) = Z
*  Calculate the total kinetic energy of the primaries: don't change
      IF ( ILOFLK (NPFLKA) .EQ. -2 .OR. ILOFLK (NPFLKA) .GT. 100000 )
     &   THEN
         TKESUM = TKESUM + TKEFLK (NPFLKA) * WTFLK (NPFLKA)
      ELSE IF ( ILOFLK (NPFLKA) .NE. 0 ) THEN
         TKESUM = TKESUM + ( TKEFLK (NPFLKA) + AMDISC (ILOFLK(NPFLKA)) )
     &          * WTFLK (NPFLKA)
      ELSE
         TKESUM = TKESUM + TKEFLK (NPFLKA) * WTFLK (NPFLKA)
      END IF
      RADDLY (NPFLKA) = ZERZER
*  Here we ask for the region number of the hitting point.
*     NREG (NPFLKA) = ...
*  The following line makes the starting region search much more
*  robust if particles are starting very close to a boundary:
      CALL GEOCRS ( TXFLK (NPFLKA), TYFLK (NPFLKA), TZFLK (NPFLKA) )
      CALL GEOREG ( XFLK  (NPFLKA), YFLK  (NPFLKA), ZFLK  (NPFLKA),
     &              NRGFLK(NPFLKA), IDISC )
*  Do not change these cards:
      CALL GEOHSM ( NHSPNT (NPFLKA), 1, -11, MLATTC )
      NLATTC (NPFLKA) = MLATTC
      CMPATH (NPFLKA) = ZERZER
      CALL SOEVSV
      RETURN
      END SUBROUTINE PUSHPART
*=======================================================================
*=======================================================================
*=======================================================================
*=== source ===========================================================*
*
      SUBROUTINE SOURCE ( NOMORE )
!       INCLUDE 'gc2fc.f'
      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
*----------------------------------------------------------------------*
*                                                                      *
*     Copyright (C) 1990-2009      by    Alfredo Ferrari & Paola Sala  *
*     All Rights Reserved.                                             *
*                                                                      *
*                                                                      *
*     New source for FLUKA9x-FLUKA20xy:                                *
*                                                                      *
*     Created on 07 january 1990   by    Alfredo Ferrari & Paola Sala  *
*                                                   Infn - Milan       *
*                                                                      *
*     Last change on 08-feb-09     by    Alfredo Ferrari               *
*                                                                      *
*  This is just an example of a possible user written source routine.  *
*  note that the beam card still has some meaning - in the scoring the *
*  maximum momentum used in deciding the binning is taken from the     *
*  beam momentum.  Other beam card parameters are obsolete.            *
*                                                                      *
*       Output variables:                                              *
*                                                                      *
*              Nomore = if > 0 the run will be terminated              *
*                                                                      *
*----------------------------------------------------------------------*
      INCLUDE '(BEAMCM)'
      INCLUDE '(FHEAVY)'
      INCLUDE '(FLKSTK)'
      INCLUDE '(IOIOCM)'
      INCLUDE '(LTCLCM)'
      INCLUDE '(PAPROP)'
      INCLUDE '(SOURCM)'
      INCLUDE '(SUMCOU)'
      INCLUDE '(GENSTK)'
      INCLUDE '(MYSTCK)'
      INCLUDE '(MYSOURCE)'
*
*
      LOGICAL LFIRST
      LOGICAL LOPND
      LOGICAL LSECND   !FLAG: do we need to read secondary vts?
      CHARACTER PTH*250
      CHARACTER pth1*250
      DOUBLE PRECISION PS0(3)   !primary interaction position - from beam!
      DOUBLE PRECISION PS(3)    !secondary interaction position
      DOUBLE PRECISION MM(3)
      DOUBLE PRECISION EMU
*
      SAVE LFIRST
      DATA LFIRST / .TRUE. /
      SAVE LSECND
      DATA LSECND / .TRUE. /
*======================================================================*
*                                                                      *
*                 OPERA VERSION - to read OpNegn beamfiles             *
*                                                                      *
*======================================================================*
      NOMORE = 0
*  +-------------------------------------------------------------------*
*  |  First call initializations:
      IF ( LFIRST ) THEN
*  |  *** The following 3 cards are mandatory ***
         TKESUM = ZERZER
         LFIRST = .FALSE.
         LUSSRC = .TRUE.
*  |  *** User initialization ***
         CALL getenv('READ_SEC',pth1)
         IF(pth1.EQ."OFF")THEN
           WRITE(28,*),"*Secondary vertices disabled!"
           WRITE(28,*),"FLUKA decay all TAU and DS"
           LSECND=.FALSE.
         ELSE
           WRITE(28,*),"*Secondary vertices enabled!"
           WRITE(28,*),"OPNEGN decay all TAU and DS"
         ENDIF
         EV_TYPE(1)=11
         EV_TYPE(2)=11
      END IF
            
      NIDMAX=0
      NVTS=0
      NUM_EVT=NUM_EVT+1
      WRITE(28,*)"*=========S=O=U=R=C=E========"
      CALL getenv('BEAMFILES',pth1)

****   POSITION GENERATION - based on input cards
      PS0(1)=XBEAM
      PS0(2)=YBEAM
      IF(WHASOU(1).EQ.1)THEN
      PS0(1)=PS0(1)+(0.5-FLRNDM())*XSPOT
      PS0(2)=PS0(2)+(0.5-FLRNDM())*YSPOT
      END IF
      IF(WHASOU(1).EQ.2)THEN
      CALL FLNRR2 (RGAUS1, RGAUS2)
      PS0(1)=PS0(1)+RGAUS1*XSPOT
      PS0(2)=PS0(2)+RGAUS2*YSPOT
      END IF
      PS0(3)=WHASOU(2)
      IF(WHASOU(2).NE.WHASOU(3))THEN
      PS0(3)=PS0(3)+FLRNDM()*(WHASOU(3)-WHASOU(2))
      END IF
      
      EV_X=PS0(1);
      EV_Y=PS0(2);
      EV_Z=PS0(3);

      
****  determine the type of source file provided:
*      WRITE(28,*)"WHASOU(6)=",WHASOU(6)
      IF(WHASOU(6).EQ.1) GOTO 601
      IF(WHASOU(6).EQ.2) GOTO 602
****************************************************************
601   NTRXTOT=0 !OpNegn beamfile
***************************************************************
      NSTP=0
      IGCODE=0
      IDUNNO=0
      IPARNT=0
      IDVTX=0
      IERR=0

      PTH=
     &      pth1(1:LNBLNK(pth1))//
     &      SDUSOU(1:LNBLNK(SDUSOU))//'.beamfile'
      INQUIRE(UNIT=45,OPENED= LOPND)
      IF(LOPND.EQV..FALSE.) THEN
        WRITE(28,*)"* OPEN FILE",PTH
        OPEN(45,FILE=PTH,ACCESS='SEQUENTIAL')
      END IF
********* READ EVENT *************************      
      READ(45,91,END=100),NEV_CURR,EV_TYPE(1),EV_TYPE(2),IDUNNO,
     &      NVTX_TOT,NTRK_TOT,EV_ETOT
      EV_TYPE(1)=GC2FC(EV_TYPE(1))
      EV_TYPE(2)=GC2FC(EV_TYPE(2))
      EV_NUM=NEV_CURR
      NTRK_CURR=0
      !WRITE BINARY OUTPUT
      call BIN_EVOUT
      NTRK_TOT=0
!       WRITE(28,*)"*",NEV_CURR," E=",EV_ETOT," tot=",NVTX_TOT
      LVTXGUT=1
      DO 10 I = 1, NVTX_TOT
        READ(45,92,END=110),NVTX_CURR,NTRK_VTX,IPARNT,IGCODE,
     &           PS(1),PS(2),PS(3)
!         WRITE(28,*) "vertex #[",NVTX_CURR,"/",NVTX_TOT,"]= (",
!      &  NTRXTOT,PS(1),PS(2),PS(3),")"
        aaa=0.1
        VCURR_X=EV_X+aaa*PS(1)
        VCURR_Y=EV_Y+aaa*PS(2)
        VCURR_Z=EV_Z+aaa*PS(3)
        INCOD=GC2FC(IGCODE)
        NSTAOL=-1
        
! 	call BIN_TRKOUT(1,NSTAOL)
        IF ((IPARNT.NE.0).AND.(LSECND)) THEN
!         this is secondary vtx!
!         do we need it? depends on LSECND
          DO 30 I2 = 1, NPFLKA
!           cycle to find the parent track
            IF(LOUSE(I2) .EQ. IPARNT) THEN
              NSTAOL=I2
            END IF
30        CONTINUE
!         Add one stopping point for this particle
          NSTP=NSTP +1
          ISTP(NSTP)=NSTAOL
          XSTP(NSTP)=VCURR_X
          YSTP(NSTP)=VCURR_Y
          ZSTP(NSTP)=VCURR_Z
          DIST(NSTP)=100.
!           WRITE(28,*)"* stop #",NSTP,": id=",ISTP(NSTP),
!      &       "at [",XSTP(NSTP),YSTP(NSTP),ZSTP(NSTP),"]"
        !Set this particle to be a RAY - forbid all interactions to her,
        !so it will not decay until we need it
          ILOFLK(NSTAOL)=0
        END IF
        
        NTRK_CURR=0
        call BIN_TRK_IN(1)
!         WRITE(28,*)"*",NVTX_CURR,"IN=",IDUNNO
        DO 20 I1 = 1, NTRK_VTX
            READ(45,93,END=110),NTRK_TOT,NTRK_CURR,IDVTX,IGCODE,
     &           MM(1),MM(2),MM(3)
            IF((IPARNT.EQ.0).OR.LSECND) THEN
             NTRK_CURR=NTRK_CURR-1
             call PUSHPART(GC2FC(IGCODE),
     &              MM(1),MM(2),MM(3),VCURR_X,VCURR_Y,VCURR_Z)
             call STUPRF(INCOD,-1,VCURR_X,VCURR_Y,VCURR_Z,NPFLKA,0)
             call BIN_TRKOUT(2,NPFLKA)
            ENDIF
20      CONTINUE
      
10    NTRK_TOT=NTRK_TOT+NTRK_VTX
      CONTINUE
      WRITE(28,*)"*===END SOURCE==="
      RETURN
***********************************************************************
602   NTRXTOT=0    !MuRay beamfile
***********************************************************************
      
      PTH=
     &      pth1(1:LNBLNK(pth1))//'/'//
     &      SDUSOU(1:LNBLNK(SDUSOU))//'.muflux'
        WRITE(28,*)"* OPEN FILE",PTH
        INQUIRE(UNIT=45,OPENED= LOPND)
      IF(LOPND.EQV..FALSE.) THEN
        OPEN(45,FILE=PTH,ACCESS='SEQUENTIAL',ERR=111)
        INQUIRE(UNIT=45,OPENED= LOPND)
        NEV_CURR=0
        EV_NUM=NEV_CURR
      END IF
******************** Read muon line ***********************************
      MM(1)=0; MM(2)=0; MM(3)=0;
*      READ(45,*,END=110),LINE 
*      WRITE(28,'(A)')LINE
*      FLUSH(28)
*      return
*      IF(LINE(1:1).EQ.'#') THEN
        !COMMENT - write it and go back
*        WRITE(28,'(A,I5,A)')"Read line #",NEV_CURR,LINE
*        GOTO 6020
*      END IF
      
      ! READ(45,*,END=100)EMU,MM(1),MM(2),MM(3)
      READ(45,*,END=100)TX,TY,EMU
      MM(3)=EMU/SQRT(TX*TX+TY*TY+1)
      MM(1)=TX*MM(3);
      MM(2)=TY*MM(3);

***   randomly chose direction and side
      WRITE(28,*)MM(1),MM(2),MM(3);      
      IF(TY.GT.0)THEN
        !WRITE(28,*)"|<---- direction",WHASOU(3);
        MM(3)=-MM(3);
        MM(2)=-MM(2);
        MM(1)=-MM(1);
        PS0(3)=WHASOU(3);
      ELSE 
        !WRITE(28,*)"---->| direction",WHASOU(2);
        !MM(3)=-MM(3);
        PS0(3)=WHASOU(2);
      ENDIF
      EV_Z=PS0(3);
      
      NEV_CURR=NEV_CURR+1;
      EV_NUM=NEV_CURR
      NTRK_CURR=0
      !WRITE BINARY OUTPUT
      call BIN_EVOUT
      call PUSHPART(11,
     &              MM(1),MM(2),MM(3),EV_X,EV_Y,EV_Z)
      call STUPRF(INCOD,-1,EV_X,EV_Y,EV_Z,NPFLKA,0)
      call BIN_TRKOUT(2,NPFLKA)
      return
*  +-------------------------------------------------------------------*

******************** EXIT ROUTINES **************
111   WRITE(28,*)"*COULD NOT OPEN FILE"
110   WRITE(28,*)"*ERROR"
100   WRITE(28,*)"*QUIT"
      FLUSH(28)
      CLOSE(45)
      NOMORE=1
      RETURN

   
! 12    FORMAT(A5,I5.1,I5.1,G18.8,G18.8,G18.8,G18.8,G18.8,G18.8)
91    FORMAT(I8.1,I8.1,I8.1,I8.1,I8.1,I8.1,G10.6)
92    FORMAT(I5.1,I5.1,I5.1,I5.1,G10.5,G10.5,G10.5)
93    FORMAT(I5.1,I5.1,I5.1,I5.1,G10.5,G10.5,G10.5)
*=== End of subroutine Source =========================================*
      END

