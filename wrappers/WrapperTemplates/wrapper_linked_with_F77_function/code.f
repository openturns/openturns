      SUBROUTINE CALCUL(X,N,Y,P,RC)
      IMPLICIT NONE

      INTEGER*4 N,P,RC
      REAL*8    X(*),Y(*)

      INTEGER*4 I,J
      REAL*8    S

      DO J = 1, P
         S = J-1
         DO I = 1, N
            S = S + X(I)
         ENDDO
         Y( J ) = S
      ENDDO

      RC = 0

      END
