module encode83(x, en, f, y, h);
  input [7:0] x;
  input en;
  output reg f;
  output reg [2:0]y;
  output [6:0]h;
  integer i;
  
  bcd7seg seg0(.b({1'b0, y}), .h(h));

  always @(x or en) begin
      if (en) begin
          y = 0;
          f = 0;
          for (i = 0; i <= 7; i = i + 1) begin
              if (x[i] == 1) begin
                  y = i[2:0];
                  f = 1;
              end
          end
      end
      else begin
          y = 0;
          f = 0;
      end
  end

endmodule 
