set MAC_UNROLL_FACTOR [lindex $argv 0]
set Z_DIM2_CYCLIC_ENABLE [lindex $argv 1]

set parts {xcu280-fsvh2892-2L-e}

set period 10
set top_name "Top"
#set design_files {"src/BVector_Filter.cpp"}
#set tb_files {"src/BVector_Filter_Test.cpp"}

set inc_dir "src"
set sol_name "solution-${MAC_UNROLL_FACTOR}_${Z_DIM2_CYCLIC_ENABLE}"

set cflags "-I $inc_dir"
set cflags "$cflags -D MAC_UNROLL_FACTOR=$MAC_UNROLL_FACTOR"
set cflags "$cflags -D Z_DIM2_CYCLIC_ENABLE=$Z_DIM2_CYCLIC_ENABLE"
puts "CFLAGS $cflags"

# Create a project
open_project "proj_$top_name"

# Add design files
add_files -cflags $cflags src/BVector_Filter.cpp

# Add test bench & files
add_files -tb src/BVector_Filter_Test.cpp
add_files -tb data/input_imag.bin
add_files -tb data/input_real.bin
add_files -tb data/output_imag.bin
add_files -tb data/output_real.bin

# Set the top-level function
set_top "${top_name}"

# ########################################################
# Create a solution
open_solution -flow_target vitis $sol_name
# Define technology and clock rate
set_part $parts
create_clock -period $period
config_compile -unsafe_math_optimizations

#csim_design
csynth_design
exit
