*$ CREATE STUPRE.FOR
*COPY STUPRE
*
*=== stupre ===========================================================*
*
      SUBROUTINE STUPRE

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
*     SeT User PRoperties for Emf particles:                           *
*                                                                      *
*     Created on  09 october 1997  by    Alfredo Ferrari & Paola Sala  *
*                                                   Infn - Milan       *
*                                                                      *
*     Last change on  14-aug-99    by    Alfredo Ferrari               *
*                                                                      *
*     Useful variables:                                                *
*                                                                      *
*           Npstrt = EMF stack index before the interaction (since     *
*                    the projectile disappears it is also the start-   *
*                    ing index of secondaries)                         *
*                                                                      *
*     On contrary of Stuprf, the region/position etc are               *
*     already set inside the stack arrays                              *
*                                                                      *
*                                                                      *
*----------------------------------------------------------------------*
*
      INCLUDE '(EMFSTK)'
      INCLUDE '(EVTFLG)'
      INCLUDE '(TRACKR)'
      INCLUDE '(MYSTCK)'
*
      LOGICAL LBHABH
*
      IF ( LDLTRY ) THEN
         LBHABH = ICHEMF (NPEMF) * ICHEMF (NPEMF-1) .LT. 0
      ELSE
         LBHABH = .FALSE.
      END IF
      DO 500 NPNW = NPSTRT,NPEMF
         DO 51 ISPR = 1, MKBMX1
            ESPARK (ISPR,NPNW) = SPAUSR (ISPR)
 51      CONTINUE
         DO 52 ISPR = 1, MKBMX2
            IESPAK (ISPR,NPNW) = ISPUSR (ISPR)
 52      CONTINUE
         LOUEMF (NPNW) = LLOUSE
 500  CONTINUE
*  Increment the track number and put it into the last flag:
      NEXTRK = 0
      DO 100 KP = NPSTRT, NPEMF
         NCRTRK = NTRCKS
         IF ( LPAIRP .OR. LPHOEL .OR. LANNFL .OR. LANNRS ) THEN
            IF ( NCRTRK .EQ. 2000000000 ) NCRTRK = -2000000000
            NEXTRK = NEXTRK + 1
            NCRTRK = NCRTRK + NEXTRK
            IESPAK (MKBMX2,KP) = NCRTRK
         ELSE IF ( LCMPTN ) THEN
            IF ( ICHEMF (KP) .NE. 0 ) THEN
               IF ( NCRTRK .EQ. 2000000000 ) NCRTRK = -2000000000
               NEXTRK = NEXTRK + 1
               NCRTRK = NCRTRK + NEXTRK
               IESPAK (MKBMX2,KP) = NCRTRK
            END IF
         ELSE IF ( LBRMSP ) THEN
            IF ( ICHEMF (KP) .EQ. 0 ) THEN
               IF ( NCRTRK .EQ. 2000000000 ) NCRTRK = -2000000000
               NEXTRK = NEXTRK + 1
               NCRTRK = NCRTRK + NEXTRK
               IESPAK (MKBMX2,KP) = NCRTRK
            END IF
         ELSE IF ( LDLTRY ) THEN
            IF ( LBHABH ) THEN
               IF ( ICHEMF (KP) .LT. 0 ) THEN
                  IF ( NCRTRK .EQ. 2000000000 ) NCRTRK = -2000000000
                  NEXTRK = NEXTRK + 1
                  NCRTRK = NCRTRK + NEXTRK
                  IESPAK (MKBMX2,KP) = NCRTRK
               END IF
            ELSE IF ( KP .EQ. NPEMF ) THEN
               IF ( NCRTRK .EQ. 2000000000 ) NCRTRK = -2000000000
               NEXTRK = NEXTRK + 1
               NCRTRK = NCRTRK + NEXTRK
               IESPAK (MKBMX2,KP) = NCRTRK
            END IF
         END IF
 100  CONTINUE
      IF ( NTRCKS .EQ. 2000000000 .AND. NEXTRK .GT. 0 )
     &   NTRCKS = -2000000000
      NTRCKS = NTRCKS + NEXTRK

       DO 200 I = NPSTRT, NPEMF
!          NIDMAX=NIDMAX+1
         LOUEMF(I)=LLOUSE!NIDMAX
            IESPAK(1,I)=LLOUSE
            IESPAK(2,I)=7
!             WRITE(28,2),"-outE-",LOUEMF(I),7,IESPAK(1,I),
!  &          ETEMF(I)/1000.,UEMF(I), VEMF(I), WEMF(KP)
  200  CONTINUE

! 2     FORMAT(A5,I5.1,I5.1,I5.1,G18.8,G18.8,G18.8,G18.8)
      RETURN
*=== End of subroutine Stupre =========================================*
      END

