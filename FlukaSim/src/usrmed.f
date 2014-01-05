*$ CREATE USRMED.FOR
*COPY USRMED
*                                                                      *
*=== usrmed ===========================================================*
*                                                                      *
      SUBROUTINE USRMED ( IJ, EKSCO, PLA, WEE, MREG, NEWREG, XX, YY, ZZ,
     &                    TXX, TYY, TZZ )

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
      INCLUDE '(MYSTCK)'
      INCLUDE '(TRACKR)'
      INCLUDE '(FLKMAT)'
*
*----------------------------------------------------------------------*
*                                                                      *
*     Copyright (C) 1991-2005      by    Alfredo Ferrari & Paola Sala  *
*     All Rights Reserved.                                             *
*                                                                      *
*                                                                      *
*     USeR MEDium dependent directives:                                *
*                                                                      *
*     Created on  10  may  1996    by    Alfredo Ferrari & Paola Sala  *
*                                                   Infn - Milan       *
*                                                                      *
*     Last change on   29-may-96   by    Alfredo Ferrari               *
*                                                                      *
*     Input variables:                                                 *
*             ij = particle id                                         *
*          Eksco = particle kinetic energy (GeV)                       *
*            Pla = particle momentum (GeV/c)                           *
*            Wee = particle weight                                     *
*           Mreg = (original) region number                            *
*         Newreg = (final)    region number                            *
*       Xx,Yy,Zz = particle position                                   *
*    Txx,Tyy,Tzz = particle direction                                  *
*                                                                      *
*     The user is supposed to change only WEE if MREG = NEWREG and     *
*     WEE, NEWREG, TXX, TYY, TZZ if MREG .NE. NEWREG                   *
*                                                                      *
*----------------------------------------------------------------------*
*! WORKAROUND to STOP tau at the decay point
      
      IF(LPKILL.EQV..TRUE.)THEN 
        LPKILL=.FALSE.
        WEE=ZERZER
      ENDIF
      ELSE WEE=1

      RETURN
*=== End of subroutine Usrmed =========================================*
      END

