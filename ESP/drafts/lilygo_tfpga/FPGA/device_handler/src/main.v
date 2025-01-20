module main
(
    input wire clk,
    input wire nreset,
    input wire di,
    output wire do,
    input wire sclk,
    input wire ncs1,
    input wire ncs2,
    inout wire [5:0] module1_io,
    inout wire sda1,
    inout wire scl1,
    inout wire [5:0] module2_io,
    inout wire sda2,
    inout wire scl2,
    inout wire [1:0] module3_io,
    inout wire sda3,
    inout wire scl3
);
    wire do1, do2, do3;
    wire sda1_oe, sda2_oe, sda3_oe, scl1_oe, scl2_oe, scl3_oe;
    wire [5:0] module1_oe, module2_oe, module3_oe;
    wire [5:0] module1_out, module2_out, module3_out;

    assign do = do1 & do2 & do3;
    assign sda1 = sda1_oe ? 0 : 1'bz;
    assign scl1 = scl1_oe ? 0 : 1'bz;
    assign sda2 = sda2_oe ? 0 : 1'bz;
    assign scl2 = scl2_oe ? 0 : 1'bz;
    assign sda3 = sda3_oe ? 0 : 1'bz;
    assign scl3 = scl3_oe ? 0 : 1'bz;

    assign module1_io[0] = module1_oe[0] ? module1_out[0] : 1'bz;
    assign module1_io[1] = module1_oe[1] ? module1_out[1] : 1'bz;
    assign module1_io[2] = module1_oe[2] ? module1_out[2] : 1'bz;
    assign module1_io[3] = module1_oe[3] ? module1_out[3] : 1'bz;
    assign module1_io[4] = module1_oe[4] ? module1_out[4] : 1'bz;
    assign module1_io[5] = module1_oe[5] ? module1_out[5] : 1'bz;

    assign module2_io[0] = module2_oe[0] ? module2_out[0] : 1'bz;
    assign module2_io[1] = module2_oe[1] ? module2_out[1] : 1'bz;
    assign module2_io[2] = module2_oe[2] ? module2_out[2] : 1'bz;
    assign module2_io[3] = module2_oe[3] ? module2_out[3] : 1'bz;
    assign module2_io[4] = module2_oe[4] ? module2_out[4] : 1'bz;
    assign module2_io[5] = module2_oe[5] ? module2_out[5] : 1'bz;

    assign module3_io[0] = module3_oe[0] ? module3_out[0] : 1'bz;
    assign module3_io[1] = module3_oe[1] ? module3_out[1] : 1'bz;

    device_handler device1(.clk(clk), .nreset(nreset), .di(di), .do(do1), .sclk(sclk), .ncs(ncs1 | ncs2), .module_in(module1_io),
                            .module_out(module1_out), .module_oe(module1_oe), .sda_in(sda1), .scl_in(scl1), .sda_oe(sda1_oe), .scl_oe(scl1_oe));
    device_handler device2(.clk(clk), .nreset(nreset), .di(di), .do(do2), .sclk(sclk), .ncs(!ncs1 | ncs2), .module_in(module2_io),
                            .module_out(module2_out), .module_oe(module2_oe), .sda_in(sda2), .scl_in(scl2), .sda_oe(sda2_oe), .scl_oe(scl2_oe));
    device_handler device3(.clk(clk), .nreset(nreset), .di(di), .do(do3), .sclk(sclk), .ncs(ncs1 | !ncs2), .module_in({module3_io, 4'b0}),
                            .module_out(module3_out), .module_oe(module3_oe), .sda_in(sda3), .scl_in(scl3), .sda_oe(sda3_oe), .scl_oe(scl3_oe));

endmodule
