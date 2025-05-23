
module math_equation 
#(
    parameter WIDTH = 8
)
(
    // Common clock & reset.
    input  logic                        clk,
    input  logic                        rst,

    // Inputs.
    input  logic                        valid_i, 
    input  logic signed [WIDTH   - 1:0] a,
    input  logic signed [WIDTH   - 1:0] b,
    input  logic signed [WIDTH   - 1:0] c,
    input  logic signed [WIDTH   - 1:0] d,

    // Output.
    output logic                        valid_o,
    output logic signed [2*WIDTH + 3:0] q
);
    // Internal nets.
    
    // Valids.
    logic valid_0;
    logic valid_1;

    // Stage 0.
    logic signed [WIDTH + 2:0] res_0; // because unsigned 3 needs to be represented as 2-bit binary 11 and adding 1 requires another bit to prevent overflow. So width is WIDTH + 3.
    logic signed [WIDTH    :0] sub_0; // Width increases by one.
    logic signed [WIDTH - 1:0] d_delayed;

    // Stage 1.
    logic signed [2*WIDTH + 3:0] res_1_0; // Requires bit-width of  2*WIDTH + 4.
    logic signed [  WIDTH + 1:0] res_1_1; // WIDTH + 2.

    
    // Valids.
    always_ff @(posedge clk, posedge rst) begin
        if (rst) begin 
            valid_0 <= '0;
            valid_1 <= '0;
            valid_o <= '0;
        end
        else begin
            valid_0 <= valid_i;
            valid_1 <= valid_0;
            valid_o <= valid_1;
        end
    end


    // Stage 0.
    always_ff @(posedge clk) begin
        if (valid_i) begin
            res_0     <= 1 + ($signed(3'd3) * $signed(c));
            sub_0     <= $signed(a) - $signed(b);
            d_delayed <= d;
        end
    end

    // Stage 1.
    always_ff @(posedge clk) begin
        if (valid_0) begin
            res_1_0 <= $signed(res_0) * $signed(sub_0);
            res_1_1 <= $signed(d_delayed) * $signed(4'd4);
        end
    end

    // Results. Stage 2.
    always_ff @(posedge clk) begin
        if (valid_1) begin
            q <= (($signed(res_1_0) - $signed(res_1_1)) / 2);
        end
    end

endmodule