module main
#(parameter
RESET_BIT = 28
)
(
    input wire clk,
    input wire nreset,
    inout wire led
);
    wire [14:0] unused_gpio;

	Gowin_EMPU_Top arm_cpu(
		.sys_clk(clk), //input sys_clk
		.gpio({unused_gpio, led}), //inout [15:0] gpio
		.reset_n(nreset) //input reset_n
	);

endmodule
