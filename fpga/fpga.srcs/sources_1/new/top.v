module top(
    input wire clk,
    input wire btn,
    output wire tx_uart
);

reg [7:0] data = 8'h41; 
reg tx_start = 0;
wire tx_busy;

uart_tx #(.CLKS_PER_BIT(868)) tx_inst(
    .clk(clk),
    .start(tx_start),
    .data(data),
    .tx(tx_uart),
    .busy(tx_busy)
);

reg btn_prev = 0;
reg [19:0] debounce_cnt = 0;
reg btn_debounced = 0;

always @(posedge clk) begin
    if (btn == btn_prev)
        debounce_cnt <= debounce_cnt + 1;
    else
        debounce_cnt <= 0;

    btn_prev <= btn;

    if (debounce_cnt == 20'd100_000)  // ~1 ms si clk à 100 MHz
        btn_debounced <= btn;
end

reg btn_debounced_prev = 0;
always @(posedge clk) begin
    btn_debounced_prev <= btn_debounced;

    if (btn_debounced && !btn_debounced_prev && !tx_busy)
        tx_start <= 1;
    else
        tx_start <= 0;
end

endmodule
