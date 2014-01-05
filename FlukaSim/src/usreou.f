*$ CREATE USREOU.FOR
*COPY USREOU
*
*=== Usreou ===========================================================*
*
      SUBROUTINE USREOU

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
*     USeR Event OUtput: this routine is called at the end of each     *
*     event                                                            *
*                                                                      *
*     Created on 01 january 1991   by    Alfredo Ferrari & Paola Sala  *
*                                                   Infn - Milan       *
*                                                                      *
*     Last change on 09-apr-99     by    Alfredo Ferrari               *
*                                                                      *
*                                                                      *
*----------------------------------------------------------------------*
*
!       WRITE(8,*) '----------END event-----------'
      RETURN
*=== End of subroutine Usreou =========================================*
      END

