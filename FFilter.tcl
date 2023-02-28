set MAC_unroll_factor [lindex $argv 0]
set Z_dim2_cyclic_enable [lindex $argv 1]

set parts {xcu280-fsvh2892-2L-e}

set period 10
set top_name "Top"
set sol_name "solution-${MAC_unroll_factor}_${Z_dim2_cyclic_enable}"

# Create a project
open_project "proj_$top_name"

# Add design files
set cflags "-I src"
set cflags "$cflags -D MAC_UNROLL_FACTOR=$MAC_unroll_factor"
set cflags "$cflags -D Z_DIM2_CYCLIC_ENABLE=$Z_dim2_cyclic_enable"
puts "CFLAGS $cflags"
add_files -cflags $cflags src/BVector_Filter.cpp

# Add test bench & files
set cflags "-I src"
puts "CFLAGS $cflags"
add_files -tb -cflags $cflags src/BVector_Filter_Test.cpp

set cflags "-I data"
puts "CFLAGS $cflags"
add_files -tb -cflags $cflags data/input_imag.bin
add_files -tb -cflags $cflags data/input_real.bin
add_files -tb -cflags $cflags data/output_imag.bin
add_files -tb -cflags $cflags data/output_real.bin

# Set the top-level function
set_top "${top_name}"

# ########################################################
# Create a solution
open_solution -flow_target vitis $sol_name
# Define technology and clock rate
set_part $parts
create_clock -period $period
config_compile -unsafe_math_optimizations

csim_design
csynth_design
exit
