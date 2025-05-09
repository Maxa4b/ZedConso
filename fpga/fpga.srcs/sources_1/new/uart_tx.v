module uart_tx (
    input wire clk,
    input wire start,
    input wire [7:0] data,
    output wire tx,
    output reg busy = 0
);

parameter CLKS_PER_BIT = 868; 

reg [9:0] tx_shift = 10'b1111111111;
reg [15:0] clk_count = 0;
reg [3:0] bit_index = 0;
reg sending = 0;

assign tx = tx_shift[0];

always @(posedge clk) begin
    if (start && !sending) begin
        tx_shift <= {1'b1, data, 1'b0};
        busy <= 1;
        sending <= 1;
        bit_index <= 0;
        clk_count <= 0;
    end
    else if (sending) begin
        if (clk_count < CLKS_PER_BIT - 1)
            clk_count <= clk_count + 1;
        else begin
            clk_count <= 0;
            tx_shift <= {1'b1, tx_shift[9:1]}; 
            bit_index <= bit_index + 1;
            if (bit_index == 9) begin
                sending <= 0;
                busy <= 0;
            end
        end
    end
end

endmodule
