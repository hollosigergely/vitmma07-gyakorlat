clear;

load real_loc;
load anchors;

d=[];
dvar=[];
for N=1:size(real_loc,1)
  for I=1:size(anchors,1)
      d(N,I)=sqrt((anchors(I,1)-real_loc(N,1))^2+(anchors(I,2)-real_loc(N,2))^2);    
      dvar(N,I)=(rand)*1.5;
      
      d(N,I)=d(N,I)+randn*dvar(N,I);
      
      % Outlier
      if rand > .9 && N>20
	d(N,I) = d(N,I)+10*rand;
      end
  end
end

save -mat-binary measurements.mat d dvar;