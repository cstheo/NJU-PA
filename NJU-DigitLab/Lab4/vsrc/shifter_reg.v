module shifter_reg(sel, q, clk, cin, set);
  input cin;
  input clk;
  input [2:0] sel;
  input [7:0] set;
  output reg [7:0] q;
  
  reg [31:0] count;
  
  initial begin
    count = 0;
    q = 0;
  end
  
  always @(posedge clk) begin
    count <= count + 1;
    if (count >= 5000000) begin
      count <= 0;
      case (sel)
        3'b000: q <= 0;
        3'b001: q <= set;
        3'b010: q <= {1'b0, q[7:1]};
        3'b011: q <= {q[6:0], 1'b0};
        3'b100: q <= {q[7], q[7:1]};
        3'b101: q <= {cin, q[7:1]};
        3'b110: q <= {q[0], q[7:1]};
        3'b111: q <= {q[6:0], q[7]};
      endcase
    end
  end
endmodule
