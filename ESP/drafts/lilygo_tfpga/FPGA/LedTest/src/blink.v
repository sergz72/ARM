module blink
(
	input wire clk,
	output wire led
);
	reg [26:0] counter = 0;
	
	assign led = counter[24];
	
	always @(posedge clk) begin
		counter <= counter + 1;
	end
	
endmodule
