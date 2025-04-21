module bcd7seg(
  input  [3:0] b,
  output reg [6:0] h
);
  always @(*) begin
    case (b)
      4'b0000: h = 7'b1000000;
      4'b0001: h = 7'b1111001;
      4'b0010: h = 7'b0100100;
      4'b0011: h = 7'b0110000;
      4'b0100: h = 7'b0011001;
      4'b0101: h = 7'b0010010;
      4'b0110: h = 7'b0000010;
      4'b0111: h = 7'b1111000;
      4'b1000: h = 7'b0000000;
      4'b1001: h = 7'b0010000;
      default: h = 7'b1111111;
    endcase
  end
endmodule