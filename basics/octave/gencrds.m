clear;

anchors=[
	10,10,
	-10,10,
	-10,-10,
	10,-10];
scatter(anchors(:,1),anchors(:,2));

crds=[];
for N=1:30
	[x,y,btns]=ginput(1);

	crds=[crds; x y];
	plot(crds(:,1),crds(:,2));
	axis([-10 10 -10 10]);	
end

crds