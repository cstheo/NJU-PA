module alu(a, b, sel, r, zf, of, cf);
  input [3:0] a, b;
  input [2:0] sel;
  output reg [3:0] r;
  output reg zf, of, cf;
  
  wire [3:0] add_result;
  wire add_cf;
  wire [3:0] b_in;
  
  assign b_in = (sel == 3'b001) ? ~b + 4'b0001 : b;
  
  add adder(
    .a(a),
    .b(b_in),
    .r(add_result),
    .cf(add_cf)
  );

  always @(a or b or sel or add_result or add_cf or b_in) begin
    zf = 0;
    of = 0;
    cf = 0;

    case (sel)
      3'b000: begin
        r = add_result;
        cf = add_cf;
        of = (a[3] == b[3]) & (a[3] != r[3]);
      end
      3'b001: begin
        r = add_result;
        cf = add_cf;
        of = (a[3] != b[3]) & (a[3] != r[3]);
      end
      3'b010: r = a ^ 1;
      3'b011: r = a & b;
      3'b100: r = a | b;
      3'b101: r = a ^ b;
      3'b110: r = a < b ? 4'b0001 : 4'b0000;
      3'b111: r = a == b ? 4'b0001 : 4'b0000;
    endcase

    zf = ~|r;
  end
endmodule
