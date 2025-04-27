module barrel_shifter (
    input [7:0] din,
    input [2:0] shamt,
    input L_R,
    input A_L,
    output reg [7:0] dout
);

always @(*) begin
    if (L_R) begin
        if (A_L) begin
            dout = din << shamt;
        end else begin
            dout = din << shamt;
        end
    end else begin
        if (A_L) begin
            dout = ($signed(din)) >>> shamt;
        end else begin
            dout = din >> shamt;
        end
    end
end

endmodule
