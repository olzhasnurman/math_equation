import os

VERILATE_COMMAND_TRACE = "verilator --assert -I./rtl/ --Wall --trace --cc ./rtl/math_equation.sv --exe test/tb_math_equation.cpp"
MAKE_COMMAND = "make -C obj_dir -f Vmath_equation.mk"
SAVE_COMMAND = "./obj_dir/Vmath_equation"
CLEAN_COMMAND = "rm -r ./obj_dir"

def main():
    os.system(VERILATE_COMMAND_TRACE)
    os.system(MAKE_COMMAND)
    os.system(SAVE_COMMAND)
    os.system(CLEAN_COMMAND)



main()
