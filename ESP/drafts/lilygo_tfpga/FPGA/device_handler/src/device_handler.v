module device_handler
(
    input wire clk,
    input wire nreset,
    input wire di,
    output wire do,
    input wire sclk,
    input wire ncs,
    input wire [5:0] module_in,
    output wire [5:0] module_out,
    output wire [5:0] module_oe,
    input wire sda_in,
    output reg sda_oe = 0,
    input wire scl_in,
    output reg scl_oe = 0
);
    localparam MODE_NONE = 0;
    localparam MODE_DDS  = 1;

    reg [31:0] dds_code = 0;
    reg [5:0] dds_ram[0:255];
    wire [7:0] dds_out;
    wire [7:0] dds_ram_address;
    wire [5:0] dds_ram_data;
    reg [5:0] dds_result = 0;

    reg prev_ncs = 1;

    reg [35:0] input_reg;
    reg [31:0] output_reg;
    wire [3:0] device_id;
    reg [3:0] mode = MODE_NONE;

    function [5:0] module_oe_f(input [3:0] mode);
        case (mode)
            MODE_DDS: module_oe_f = 6'h3F;
            default: module_oe_f = 0;
        endcase
    endfunction

    function [5:0] module_out_f(input [3:0] mode);
        case (mode)
            MODE_DDS: module_out_f = dds_result;
            default: module_out_f = 0;
        endcase
    endfunction

    assign module_oe = module_oe_f(mode);
    assign module_out = module_out_f(mode);

    assign device_id = input_reg[3:0];
    assign dds_ram_address = input_reg[11:4];
    assign dds_ram_data = input_reg[17:12];
    assign do = ncs ? 1'b1 : output_reg[31];

    dds_bin #(.OUT_WIDTH(8)) dds(.clk(clk), .code(dds_code), .out(dds_out));

    always @(posedge sclk) begin
        if (!ncs) begin
            input_reg <= {input_reg[33:0], di};
            output_reg <= {output_reg[30:0], 1'b0};
        end
        else begin
            case (device_id)
                8: output_reg <= 0;
                default: begin end
            endcase
        end
    end

    always @(posedge clk) begin
        dds_result <= nreset ? dds_ram[dds_out] : 0;
        if (!nreset) begin
            mode <= MODE_NONE;
            prev_ncs <= 1;
        end
        else begin
            if (!prev_ncs && ncs) begin // posedge ncs
                case (device_id)
                    0: mode <= input_reg[7:4];
                    1: dds_ram[dds_ram_address] <= dds_ram_data;
                    2: dds_code <= input_reg[35:4];
                    default: begin end
                endcase
            end
            prev_ncs <= ncs;
        end
    end
endmodule
