function ret = circle_intersect(x1,y1,r1,x2,y2,r2)
  R=sqrt((x1-x2)^2+(y1-y2)^2);
  
  coeff1=(r1^2-r2^2)/(2*R^2);
  coeff2=.5*sqrt(2*(r1^2+r2^2)/(R^2)-((r1^2-r2^2)^2)/(R^4)-1);
  
  ret(1,1)=.5*(x1+x2)+coeff1*(x2-x1)+coeff2*(y2-y1);
  ret(1,2)=.5*(y1+y2)+coeff1*(y2-y1)+coeff2*(x1-x2);
  
  ret(2,1)=.5*(x1+x2)+coeff1*(x2-x1)-coeff2*(y2-y1);
  ret(2,2)=.5*(y1+y2)+coeff1*(y2-y1)-coeff2*(x1-x2); 
end