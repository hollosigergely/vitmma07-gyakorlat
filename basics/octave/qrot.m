u=[1,1,1];
p=[1,2,1];
a=30;
		
u=u/norm(u);

q=[cosd(a/2),u(1)*sind(a/2),u(2)*sind(a/2),u(3)*sind(a/2)];
v=[0,p(1),p(2),p(3)];

qinv=[q(1) -q(2:4)]/norm(q)^2;
res = [q(1)*v(1) - dot(q(2:4),v(2:4)), q(1)*v(2:4) + v(1)*q(2:4) + cross(q(2:4),v(2:4))];
res = [res(1)*qinv(1) - dot(res(2:4),qinv(2:4)), res(1)*qinv(2:4) + qinv(1)*res(2:4) + cross(res(2:4),qinv(2:4))];
		
K=[0 -u(3) u(2)
u(3) 0 -u(1)
-u(2) u(1) 0];
R=eye(3)+sind(30)*K+(1-cosd(30))*K*K;
pvRodr=R*p';
