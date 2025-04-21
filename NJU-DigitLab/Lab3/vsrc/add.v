module add(a, b, r, cf);
  input [3:0] a, b;
  output [3:0] r;
  output cf;

  assign {cf, r} = a + b;
endmodule
