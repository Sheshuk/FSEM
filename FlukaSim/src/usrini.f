*$ CREATE USRINI.FOR
*COPY USRINI
*
*=== usrini ===========================================================*
*
!       INCLUDE 'gc2fc.f'
      
      SUBROUTINE USRINI ( WHAT, SDUM )

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
*
*----------------------------------------------------------------------*
*                                                                      *
*     Copyright (C) 1991-2005      by    Alfredo Ferrari & Paola Sala  *
*     All Rights Reserved.                                             *
*                                                                      *
*                                                                      *
*     USeR INItialization: this routine is called every time the       *
*                          USRICALL card is found in the input stream  *
*                                                                      *
*                                                                      *
*     Created on 01 january 1991   by    Alfredo Ferrari & Paola Sala  *
*                                                   Infn - Milan       *
*                                                                      *
*     Last change on 20-mar-05     by    Alfredo Ferrari               *
*                                                                      *
*                                                                      *
*----------------------------------------------------------------------*
*
      INCLUDE '(MYGLOB)'
      INCLUDE '(MYOUT)'
      DIMENSION WHAT (6)
      CHARACTER SDUM*8
      CHARACTER PTH*250
      CHARACTER MIN_ANG*8
*
*  Don't change the following line:
      LUSRIN = .TRUE.
* *** Write from here on *** *
      call G2F_INIT
      !Init output levels - from environments:
      CALL getenv('BIN_OUT',PTH)
      DO I=1,MXOUTP
       OUT_BIN(I)=(PTH(I:I).EQ."1")
      END DO
      CALL getenv('TXT_OUT',PTH)
      DO I=1,MXOUTP
       OUT_TXT(I)=(PTH(I:I).EQ."1")
      END DO
      CALL getenv('FLU_OUT',PTH)
!      CALL getenv('MIN_ANG',MIN_ANG)
            
      OPEN(28,FILE=PTH(1:LNBLNK(PTH))//'/FluSim.out',
     &              ACCESS='APPEND' )
!      WRITE(28,*),'mrad = ',MIN_ANG
!      READ (MIN_ANG, '(i10)') MRAD
!      *MIN_COS=COS(MRAD*0.001);

!      WRITE(28,'(A,F6.5)'),'Minimal kink angle=',MIN_COS
      WRITE(28,*),'*=========== STARTING NEW RUN ==========='
      OPEN(30,FILE=PTH(1:LNBLNK(PTH))//'/FluTop.out',
     &              ACCESS='APPEND' )
      WRITE(30,*),'*=========== STARTING NEW RUN ==========='


        OPEN(29,FILE=PTH(1:LNBLNK(PTH))//'/FluSim.bin',
     &       status='NEW',
     &       form='UNFORMATTED')
        OPEN (UNIT=101, FILE = PTH(1:LNBLNK(PTH))//"/FluTrx.bin", 
     &       STATUS = 'NEW', 
     &       FORM ='UNFORMATTED' )
        
!       &        ACCESS='DIRECT',RECL=56,
!note:
!binary format is the following:
! desc | #  | PID  | parent# | P |Tx |Ty | x | y | z
! 2B   | 2B | 2B   | 2B      |8B |8B |8B |8B |8B |8B = I*4+R*6=56B
!


      RETURN
*=== End of subroutine Usrini =========================================*
      END

