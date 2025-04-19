module mux41(a,s,y);
  input  [3:0] a;
  input  [1:0] s;
  output y;
  MuxKeyWithDefault #(4, 2, 1) i0 (y, s, 1'b0, {
    2'b00, a[0],
    2'b01, a[1],
    2'b10, a[2],
    2'b11, a[3]
  });
endmodule // mux41
