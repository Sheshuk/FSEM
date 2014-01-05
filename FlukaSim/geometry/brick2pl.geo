0    0                                                             OPERA brick
* BODIES DESCRIPTION
*...+....1....+....2....+....3....+....4....+....5....+....6....+....7...
RPP  World      -50.000000  50.000000 -50.000000  50.000000 -20.000000  50.000000
RPP  Vac        -20.000000  20.000000 -20.000000  20.000000 -10.000000  30.000000
RPP  emul_00     -6.250000   6.250000  -4.967500   4.967500  -0.029300   0.000000
RPP  base_00     -6.250000   6.250000  -4.967500   4.967500  -0.024900  -0.004400
RPP  emul_01     -6.250000   6.250000  -4.967500   4.967500   0.098000   0.127300
RPP  base_01     -6.250000   6.250000  -4.967500   4.967500   0.102400   0.122900
RPP  emul_02     -6.250000   6.250000  -4.967500   4.967500   0.225300   0.254600
RPP  base_02     -6.250000   6.250000  -4.967500   4.967500   0.229700   0.250200
RPP  Brick       -6.250000   6.250000  -4.967500   4.967500  -0.029300   0.254600
END
* REGIONS DESCRIPTION
*...+....1....+....2....+....3....+....4....+....5....+....6....+....7...
Emul00   5 +emul_00 -base_00
Emul01   5 +emul_01 -base_01
Emul02   5 +emul_02 -base_02
Lead     5 +Brick-(emul_00 |emul_01 |emul_02)
Base     5 base_00 |base_01 |base_02
BkHole   5 +World -Vac
Vacuum   5 +Vac -Brick
END