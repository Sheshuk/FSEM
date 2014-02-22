function fullpath -d "Get absolute path" 
	echo (readlink -f (dirname $argv[1]))
end
  
set -x FSEM (fullpath $argv[1])

set -x PATH $PATH $FSEM/app
#setup FLUKA variables:

set -x FLUPRO /data/sw/fluka
set -x FLUFOR gfortran
set -x GFORFLU gfortran-4.7
#set -x BRICKFLU_DIR $PWD/

# set path to beamfiles from OpNegn:
set -x BEAMFILES /data/opera/beamfile/
set -x FLU_OUT /data/opera/FluSim/muon/

# next two variables define output level of FLUKA simulation.
# BIN_OUT - for binary output (to file FlukaSim.bin)
# TXT_OUT - for ascii output  (to file FlukaSim.out)
# bits are the following:
#   1st position: output event header
#   2nd position: output track interaction (IN tracks)
#   3rd position: output daughter tracks of interaction (out tracks)
#   4th position: output charged tracks going IN and OUT of emulsion volumes
# for example 11111 - full output, 11001 - output event header and interacting tracks only, 0001 - output only enter/exit points from emulsion
set -x BIN_OUT 11110
set -x TXT_OUT 11111

#uncomment following line to forbid secondary decays from OpNegn, so FLUKA will transport and decay all primary particles, including TAU and CHARM
#unsetenv READ_SEC
#setenv READ_SEC OFF
