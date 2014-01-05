*$ CREATE USREIN.FOR
*COPY USREIN
*
*=== Usrein ===========================================================*
*
      SUBROUTINE USREIN

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(SOUEVT)'
      INCLUDE '(EVTFLG)'
*
*----------------------------------------------------------------------*
*                                                                      *
*     Copyright (C) 1991-2005      by    Alfredo Ferrari & Paola Sala  *
*     All Rights Reserved.                                             *
*                                                                      *
*                                                                      *
*     USeR Event INitialization: this routine is called before the     *
*     showering of an event is started, but after the source particles *
*     of that event have been already loaded on the stack              *
*                                                                      *
*     Created on 01 january 1991   by    Alfredo Ferrari & Paola Sala  *
*                                                   Infn - Milan       *
*                                                                      *
*     Last change on 09-apr-99     by    Alfredo Ferrari               *
*                                                                      *
*                                                                      *
*----------------------------------------------------------------------*
*
      INCLUDE '(SOURCM)'
      INCLUDE '(FLKSTK)'
      INCLUDE '(MYSTCK)'
      INCLUDE '(MYSOURCE)'

      integer*4 today(3), now(3)
      call idate(today)   ! today(1)=day, (2)=month, (3)=year
      call itime(now)     ! now(1)=hour, (2)=minute, (3)=second
      write (28 , 1000 )today(2), today(1), today(3), now
1000  format ( 'Date ', i2.2, '/', i2.2, '/', i4.4, '; time ',
     &         i2.2, ':', i2.2, ':', i2.2 )

      NTRK_TOT=0
      WRITE (101) NEV_CURR
      IF(LUSSRC) THEN
!          WRITE(28,*)"USING SIMPLE GEN!"
         RETURN
      ENDIF
!       WRITE(28,*)"USING SOURCE FILE!"
      NIDMAX=0
      NVTS=0
      NUM_TRK=-1;
      NUM_EVT=NUM_EVT+1
      NEV_CURR=NUM_EVT
      NSTP=0
      EV_NUM=NEV_CURR
      EV_TYPE(1)=IJSOEV(1)
      EV_ETOT=PMSOEV(1)
      EV_TX=TXSOEV(1)
      EV_TY=TYSOEV(1)
      EV_X=XSOEVT(1)
      EV_Y=YSOEVT(1)
      EV_Z=ZSOEVT(1)

      CALL BIN_EVOUT
!      RETURN

      !FIXME - what is this?!
      NTRK_CURR=0
      NVTX_CURR=0

      DO 100 I = 1, NPFLKA
!          WRITE(28,*)"*prim ",I," of ",NPFLKA
         NIDMAX=NIDMAX+1
         LOUSE(I)=NIDMAX
         ISPARK (1,I)=0
         ISPARK (2,I)=ILOFLK(I)
         CALL BIN_TRKOUT(-2,I)
 100   CONTINUE
!       WRITE(28,*)"*========================="
!       WRITE(28,12),'~Evt=',NUM_EVT,IJSOEV(1),
! &                 PMSOEV(1),TXSOEV(1),TYSOEV(1)
! 12    FORMAT(A5,I5.1,I5.1,G18.8,G18.8,G18.8)
!       WRITE(18,*),'=NewEvt='
      RETURN
      
*=== End of subroutine Usrein =========================================*
      END

