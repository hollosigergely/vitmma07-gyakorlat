clear all;

u=[1,1,1];
p=[1,2,1];
a=30;
		
u=u/norm(u);
q=[cosd(a/2),u(1)*sind(a/2),u(2)*sind(a/2),u(3)*sind(a/2)];
v=[0,p(1),p(2),p(3)];
pvQ=quatmultiply(quatmultiply(q,v),quatinv(q));

K=[0 -u(3) u(2)
u(3) 0 -u(1)
-u(2) u(1) 0];
R=eye(3)+sind(30)*K+(1-cosd(30))*K*K;
pvRodr=R*p';