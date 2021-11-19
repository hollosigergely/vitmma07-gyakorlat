function out=cdiff(in1,in2)
  d = in1 - in2;
  idx = find(d < 0);
  d(idx) = d(idx)+2^40;
  out = d;
end