module mux21(a,b,s,y);
  input   a,b,s;
  output  y;
  MuxKey #(2, 1, 1) i0 (y, s, {
    1'b0, a,
    1'b1, b
  });
endmodule
