*$ CREATE MDSTCK.FOR
*COPY MDSTCK
*
*=== Mdstck ===========================================================*
*
      SUBROUTINE MDSTCK ( IFLAG, NPSECN )

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
*
*----------------------------------------------------------------------*
*                                                                      *
*     Copyright (C) 1998-2007      by    Alfredo Ferrari & Paola Sala  *
*     All Rights Reserved.                                             *
*                                                                      *
*                                                                      *
*     Created on    19 may 1998    by    Alfredo Ferrari & Paola Sala  *
*                                                   Infn - Milan       *
*                                                                      *
*     Last change on 18-jan-07     by    Alfredo Ferrari               *
*                                                                      *
*                                                                      *
*        Iflag = 1: standard Kaskad call                               *
*              = 2: Kaskad call after elastic (but hydrogen)           *
*              = 3: Kasneu call                                        *
*              = 4: Emfsco call                                        *
*                                                                      *
*----------------------------------------------------------------------*
*
      INCLUDE '(EMFSTK)'
      INCLUDE '(FHEAVY)'
      INCLUDE '(GENSTK)'
      INCLUDE '(TRACKR)'
      INCLUDE '(FLKSTK)'
      INCLUDE '(EVTFLG)'
      INCLUDE '(PAPROP)'
      INCLUDE '(MYSTCK)'
      INCLUDE '(MYSOURCE)'

!       WRITE(28,*)"MDSTK",IFLAG
      RETURN
      IF (IFLAG .EQ. 4) RETURN
      
*
!        GOTO 222
       !remove EXTREMELY heavy particles
        DO I = 1,NPHEAV
           WRITE(28,*),"* hvy(",KHEAVY(I),") = ",ANHEAV(KHEAVY(I))
        END DO
      
!        RETURN
        !IF(.TRUE.)GOTO 111

        NVTS_MY=NP
        LVTXGUT=1
        GOTO 113
!         WRITE(28,'(A,I3,I3)'),"NP,Flag=",NPSECN,IFLAG
!        FLUSH(28)
        DO 111 I = NP,1,-1
           !remove EXTREMELY heavy particles
           !remove weak compton electrons
           !remove neutrinos!
!           WRITE(28,*),"----",I,KPART(I)
!           FLUSH(28)
           IF((KPART(I) .EQ. 5 .OR. KPART(I) .EQ. 6).OR.
     &      (KPART(I) .EQ.27 .OR. KPART(I) .EQ.28).OR.
     &      (KPART(I) .EQ.43 .OR. KPART(I) .EQ.44))THEN

            WRITE(28,*)"Nu!",KPART(I)
            LVTXGUT=1
            GOTO 102
          ENDIF

           IF(KPART(I) .LE. -10000)THEN
C          .OR.
C     &      (KPART(I) .EQ. 3 .AND. PLR(I) .LE. 0.01))THEN
            WRITE(28,*)"Kill ",KPART(I),"---",I," of ",NP
            WRITE(28,*)"Kill ",PRNAME(KPART(I))
            NP=NP-1
!             I=I-1
            NP0=NP0-1
            NPSECN=NPSECN-1
            !IF(I.GE.NP)GOTO 112
               DO 112 J=I,NP
!                    WRITE(28,*),J+1,"->",J
                   CXR(J)=CXR(J+1)
                   CYR(J)=CYR(J+1)
                   CZR(J)=CZR(J+1)
                   CXRPOL(J)=CXRPOL(J+1)
                   CYRPOL(J)=CYRPOL(J+1)
                   CZRPOL(J)=CZRPOL(J+1)
                   PLR(J)=PLR(J+1)
                   TKI(J)=TKI(J+1)
                   WEI(J)=WEI(J+1)
                   AGESEC(J)=AGESEC(J+1)
                   INFEXT(J)=INFEXT(J+1)
                   KPART(J)=KPART(J+1)
112            END DO
           ELSE
           WRITE(28,*)I,"new:",KPART(I)
            NVTS_MY=NVTS_MY+1
102       END IF
111     END DO
!         IF(NVTS_MY.NE.NP)WRITE(28,*)"Reduce:",NVTS_MY,NPSECN
! 222     NVTS_MY=NPSECN
!         WRITE(28,*),"NVTS_MY=",NVTS_MY
!         FLUSH(28)
        IF(NVTS_MY .GE. 0)LVTXGUT=1
!         IF((NVTS_MY .GE. 0).AND.(IFLAG.GE.2))THEN
!           LVTXGUT=1
          WRITE(28,*),"Interaction:",IFLAG,NVTS_MY
!         ENDIF
113     IF(LVTXGUT.EQ.1) THEN
          NTRK_VTX=NVTS_MY
          NVTX_CURR=NVTX_CURR+1
          NTRK_CURR=0
          VCURR_X=XTRACK(NSTAOL)
          VCURR_Y=YTRACK(NSTAOL)
          VCURR_Z=ZTRACK(NSTAOL)
          CALL BIN_TRKOUT(1,NSTAOL)
          NTRK_TOT=NTRK_TOT+NTRK_VTX
        END IF

! 1     FORMAT(A4,I5.1,I5.1,G18.8,G18.8,G18.8,G18.8,G18.8,G18.8,
!      &     G18.8,G18.8,G18.8)

! 2     FORMAT(A5,I5.1,I5.1,G18.8,G18.8,G18.8,G18.8,G18.8)
      RETURN
*=== End of subroutine Mdstck =========================================*
      END

