#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <random>
#include <verilated.h>
#include <queue>
#include <verilated_vcd_c.h>
#include "Vmath_equation.h"

#define MAX_SIM_TIME 10000
vluint64_t sim_time = 0;
vluint64_t posedge_cnt = 0;

int32_t sign_extend_20bit(uint32_t val) {
    if (val & (1 << 19)) {
        return static_cast<int32_t>(val | 0xFFF00000);
    } else {
        return static_cast<int32_t>(val & 0x000FFFFF);
    }
}


void dut_reset (Vmath_equation *dut, vluint64_t &sim_time){
    
    if( sim_time < 100 ){
        dut->rst = 1;
    }
    else {
        dut->rst = 0;
    }
}

int main(int argc, char** argv, char** env) {
    Verilated::commandArgs(argc, argv);
    Vmath_equation *dut = new Vmath_equation;
    Verilated::traceEverOn(true);
    VerilatedVcdC* sim_trace = new VerilatedVcdC;
    dut->trace(sim_trace, 10);
    sim_trace->open("./waveform.vcd");

    // Setup random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int8_t> dist(-128, 127); // for 8 bit data.
    std::uniform_int_distribution<int> valid_dist(0, 99);

    int8_t data_a = static_cast<int8_t>(dist(gen));
    int8_t data_b = static_cast<int8_t>(dist(gen));
    int8_t data_c = static_cast<int8_t>(dist(gen));
    int8_t data_d = static_cast<int8_t>(dist(gen));

    std::queue<int32_t> expected_results;

    bool pass = true;

    while (sim_time < MAX_SIM_TIME) {
        dut_reset(dut, sim_time);

        // Clk.
        dut->clk ^= 1;

        // Valid assignment.
        if ( (sim_time >= 120) ) {
            dut->valid_i = (valid_dist(gen) < 70) ? 1 : 0; // 70% of time valid inserted.
        }
        else {
            dut->valid_i = 0;
        }

        // Input signals.
        dut->a = data_a;
        dut->b = data_b;
        dut->c = data_c;
        dut->d = data_d;
        if ( (dut->clk == 0)) {
            data_a = static_cast<int8_t>(dist(gen));
            data_b = static_cast<int8_t>(dist(gen));
            data_c = static_cast<int8_t>(dist(gen));
            data_d = static_cast<int8_t>(dist(gen));
        }

        // Evaluate.
        dut->eval();

        // Calculating expected output.
        int32_t res = 0;
        int32_t a = static_cast<int32_t>(data_a);
        int32_t b = static_cast<int32_t>(data_b);
        int32_t c = static_cast<int32_t>(data_c);
        int32_t d = static_cast<int32_t>(data_d);

        int32_t diff = a - b;
        int32_t mul_term = 1 + (3 * c);
        int32_t product = diff * mul_term;
        int32_t sub_term = 4 * d;    
        res = (product - sub_term) / 2;

        if ((dut->clk == 1) & (dut->valid_i == 1)) {
            expected_results.push(res);
        }
        

        // Check.
        if ((dut->valid_o == 1) && (dut->clk == 0)) {
            int32_t delayed_res = expected_results.front();
            expected_results.pop();

            int32_t dut_result = sign_extend_20bit(dut->q);

            if (delayed_res != dut_result) {
                pass = false;
                std::cout << "Fail. Expected: " << delayed_res 
                          << " Actual: " << dut_result << "\n";
            }
            else {
                // std::cout << "Pass\n";
            }
        }
        

        if (dut->clk == 1){
            posedge_cnt++;
        }

        sim_trace->dump(sim_time);
        sim_time++;
    }
    if (pass) std::cout << "PASS\n";
    else      std::cout << "FAIL\n";

    sim_trace->close();
    delete sim_trace;
    delete dut;
    exit(EXIT_SUCCESS);
}
