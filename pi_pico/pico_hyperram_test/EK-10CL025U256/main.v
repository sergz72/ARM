module main
(
  input wire ck,
  input wire nreset,
  input wire ncs,
  inout wire rwds,
  inout wire [7:0] dq,
  input wire io_direction,
  input wire rwds_direction,
  output wire hyperram_ck,
  output wire hyperram_ckn,
  output wire hyperram_nreset,
  output wire hyperram_ncs,
  inout wire hyperram_rwds,
  inout wire [7:0] hyperram_dq,
  output wire [3:0] leds
);
	reg [1:0] counter = 0;
	
	assign hyperram_ck = ck;
	assign hyperram_ckn = !ck;
	assign hyperram_ncs = ncs;
	assign hyperram_nreset = nreset;
	
	assign hyperram_dq = io_direction ? 8'hz : dq;
	assign dq = io_direction ? hyperram_dq : 8'hz;

	assign hyperram_rwds = rwds_direction ? 1'bz : rwds;
	assign rwds = rwds_direction ? hyperram_rwds : 1'bz;
	
	assign leds[0] = counter != 0;
	assign leds[1] = counter != 1;
	assign leds[2] = counter != 2;
	assign leds[3] = counter != 3;
	
	always @(posedge ck) begin
		counter <= counter + 1;
	end

endmodule
