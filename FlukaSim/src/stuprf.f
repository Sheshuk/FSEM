*$ CREATE STUPRF.FOR
*COPY STUPRF
*
*=== stuprf ===========================================================*
*
!       INCLUDE 'binout.f'
      
      SUBROUTINE STUPRF ( IJ, MREG, XX, YY, ZZ, NPSECN, NPPRMR )

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
*
*----------------------------------------------------------------------*
*                                                                      *
*     Copyright (C) 1997-2005      by    Alfredo Ferrari & Paola Sala  *
*     All Rights Reserved.                                             *
*                                                                      *
*                                                                      *
*     SeT User PRoperties for Fluka particles:                         *
*                                                                      *
*     Created on  09 october 1997  by    Alfredo Ferrari & Paola Sala  *
*                                                   Infn - Milan       *
*                                                                      *
*     Last change on  14-jul-05    by    Alfredo Ferrari               *
*                                                                      *
*                                                                      *
*----------------------------------------------------------------------*
*
      INCLUDE '(EVTFLG)'
      INCLUDE '(FLKSTK)'
      INCLUDE '(FHEAVY)'
      INCLUDE '(GENSTK)'
      INCLUDE '(TRACKR)'
      INCLUDE '(MYSTCK)'
      INCLUDE '(MYSOURCE)'
      
      IF(NP.EQ.0) THEN
*     treat source particles      
         NIDMAX=NIDMAX+1
         LOUSE(NPFLKA)=NIDMAX
         ISPARK (1,NPFLKA)=-1
         IF(NSTAOL.GE.0)ISPARK(1,NPFLKA)=LOUSE(NSTAOL)
         ISPARK (2,NPFLKA)=ILOFLK(NPFLKA)
      ELSE
         LOUSE(NPFLKA)=INFEXT(NPSECN)
         ISPARK(1,NPFLKA)=LOUSE(NSTAOL)
         ISPARK(2,NPFLKA)=KPART(NPSECN)
      ENDIF

      
*      copy all the flags
      
      DO 100 ISPR = 1, MKBMX1
         SPAREK (ISPR,NPFLKA) = SPAUSR (ISPR)
100   END DO
      DO 200 ISPR = 3, MKBMX2
         ISPARK (ISPR,NPFLKA) = ISPUSR (ISPR)
200   END DO 
!         WRITE(28,*)"stuprf for trk",NPSECN,NP,NPFLKA,LOUSE(NPFLKA)
!     &," = ",ILOFLK(NPFLKA)
!       IF(NP.GT.0)THEN
!         CALL BIN_TRK_OUT(NPSECN)
!       ELSE 
!        IF(NP.EQ.0)CALL BIN_TRKOUT(2,NPFLKA)
!       ENDIF
*  Increment the track number and put it into the last flag:
      IF ( NPSECN .GT. NPPRMR ) THEN
         IF ( NTRCKS .EQ. 2000000000 ) NTRCKS = -2000000000
         NTRCKS = NTRCKS + 1
         ISPARK (MKBMX2,NPFLKA) = NTRCKS
      ENDIF
      RETURN
! 
!       REAL*8 COEF, COST
!       LOGICAL LNEW
! 
!       LNEW=.TRUE.
! 
!       IF(NSTAOL .EQ. -1) THEN
!         ISPARK (1,NPFLKA)=-1
!         GOTO 111
!       ENDIF
!       WRITE(28,*)"new part:",NPSECN,KPART(NPSECN),PLR(NPSECN),
!      &   NPFLKA,"old:",NPPRMR,LOUSE(NPPRMR)
!       IF(LVTXGUT .EQ. 0)RETURN     
! 
! 
! !       IF(NPFLKA.EQ.MFSTCK) NPFLKA=NSTAOL
! *  Increment the track number and put it into the last flag:
!       IF ( NPSECN .GT. NPPRMR ) THEN
!          IF ( NTRCKS .EQ. 2000000000 ) NTRCKS = -2000000000
!          NTRCKS = NTRCKS + 1
!          ISPARK (MKBMX2,NPFLKA) = NTRCKS
!       ENDIF
!       
! 
! 
!       
! *     If this track momentum is almost eq to mother's momentum, then the ID is the same as mother's
!        IF(KPART(NP) .EQ. ILOFLK(NSTAOL)) THEN
!        COST=(CXR(NPSECN)*TXFLK(NSTAOL)+
!      &       CYR(NPSECN)*TYFLK(NSTAOL)+
!      &       CZR(NPSECN)*TZFLK(NSTAOL))
!        WRITE(28,*)"cos_kink=",COST
! !           COEF=COST*PLR(NPSECN)/PMOFLK(NSTAOL)
!        LNEW=(COST .LE. 0.998)
!        ENDIF
! 
! 
! 
!       IF(LNEW)THEN
! **** make a new track! *******
! 
!       IF(NPFLKA.EQ.MFSTCK)THEN
! *      treat strange events (?)
! !        RETURN
!        NPFLKA=NSTAOL
!        PMOFLK(NPFLKA)=PLR(NPSECN)
!        TXFLK(NPFLKA)=CXR(NPSECN)
!        TYFLK(NPFLKA)=CYR(NPSECN)
!        TZFLK(NPFLKA)=CZR(NPSECN)
!        XFLK(NPFLKA)=XX
!        YFLK(NPFLKA)=YY
!        ZFLK(NPFLKA)=ZZ
!       ENDIF
! *      set parent
!       ISPARK(1,NPFLKA)=LOUSE(NSTAOL)      
! *      copy all the flags
! 111   NIDMAX=NIDMAX+1   
!       DO 100 ISPR = 1, MKBMX1
!          SPAREK (ISPR,NPFLKA) = SPAUSR (ISPR)
! 100   CONTINUE
!       DO 200 ISPR = 2, MKBMX2
!          ISPARK (ISPR,NPFLKA) = ISPUSR (ISPR)
! 200   CONTINUE 
!        
!      
! 
!       LOUSE(NPFLKA)=NIDMAX 
!       ISPARK(2,NPFLKA)=ILOFLK(NPFLKA)
!       IF(ILOFLK(NPFLKA).LE.-10000)ISPARK(2,NPFLKA)=-50
!       ENDIF
! 
!       NTRK_CURR=NTRK_CURR+1
!       CALL BIN_TRKOUT(2,NPFLKA)
!       
!       RETURN
*=== End of subroutine Stuprf =========================================*
      END

