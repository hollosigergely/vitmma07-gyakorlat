%%%%%%%%%%%%%%
% Kétdimenziós helymeghatározási algoritmus
% 
% Az algoritmus meghatározza egy eszköz helyzetét olyan módon, hogy K darab horgonyponttól (anchor)
% megméri a távolságát. A távolságok természetesen zajokat és hibákat is tartalmaznak, így a megoldás nem
% egyértelmű.
%%%%%%%%%%%%%%

clear all;
close all;

% Mérések betöltése
%  d(i,j) - i-dik időpontban az eszköz távolsága a j-dik anchortól
%  dvar(i,j) - az i-dik időpontban a távolságmérés hibájának szórása
load measurements;

% A mérőeszközök helyzetének betöltése
% anchors(k,1) - az k-dik anchor X koordinátája
% anchors(k,2) - az k-dik anchor Y koordinátája
load anchors;

% Az eszköz valós helyzete a hiba vizsgálatához
% real_loc(i,1) - az i-dik időpontban az eszköz X koordinátája
% real_loc(i,2) - az i-dik időpontban az eszköz Y koordinátája
load real_loc;

% Hiba eltárolására használt mátrix
err_hist=[];

% Newton-Gauss állapotvektor
nghist=[];

% Kalman filter állapotvektor
kfhist=[];
m=[];
P=eye(4);

sd=10;
dt=0.1;
A=[1 0 dt 0;
    0 1 0 dt;
    0 0 1 0;
    0 0 0 1;];
G=[.5*dt*dt 0;
    0 .5*dt*dt;
    dt 0
    0 dt];
Q=G*G'*sd*sd;


for step=1:size(d,1)	% Iteráció az időpillanatokra
  clf;
  
  % Horgonypontok kirajzolása
  main_plot=scatter(anchors(:,1),anchors(:,2),'r');
  axis([-12 22 -12 12]);
  axis equal;
  hold on;
  
  % Valós helyzet ábrázolása
  plot(real_loc(1:step,1),real_loc(1:step,2),'rx-','LineWidth',2);

  % Távolságok kirajzolása körökként
  for i=1:size(d,2)
    circle(anchors(i,1),anchors(i,2),d(step,i),'b');
  end;
  
  % Newton-Gauss algoritmus, 10 iterációval
  ngloc=[0;0];
  ngloc_list=[];
  for iter=1:10
     J=[];	% Jacobi mártix
     for ai=1:size(anchors,1)
	J(ai,1)=(ngloc(1)-anchors(ai,1))/sqrt((ngloc(1)-anchors(ai,1))^2+(ngloc(2)-anchors(ai,2))^2);
	J(ai,2)=(ngloc(2)-anchors(ai,2))/sqrt((ngloc(1)-anchors(ai,1))^2+(ngloc(2)-anchors(ai,2))^2);
	
	eps(ai,1)=sqrt((ngloc(1)-anchors(ai,1))^2+(ngloc(2)-anchors(ai,2))^2)-d(step,ai);
     end
     
     ngloc=ngloc-inv(J'*J)*J'*eps;
     ngloc_list=[ngloc_list; ngloc'];
  end
  plot(ngloc_list(:,1),ngloc_list(:,2),'go-');
  
  % Historikus útvonal gyűjtése és kirajzolása
  nghist=[nghist; ngloc'];
  plot(nghist(:,1),nghist(:,2),'gx-','LineWidth',2);

  
    % Kalman-szűrő  
  if isempty(m)
    m=[0;0;0;0];	% A kezdeti hely a Newton-Gauss iteráció alapján
  end;
  
  % Predikciós lépés
  m=A*m;
  P=A*P*A'+Q;
  
  H=[];
  for ai=1:size(anchors,1)
    H(ai,1)=(m(1)-anchors(ai,1))/sqrt((m(1)-anchors(ai,1))^2+(m(2)-anchors(ai,2))^2);
    H(ai,2)=(m(2)-anchors(ai,2))/sqrt((m(1)-anchors(ai,1))^2+(m(2)-anchors(ai,2))^2);
    H(ai,3)=0;
    H(ai,4)=0;
    
    h(ai,1)=sqrt((m(1)-anchors(ai,1))^2+(m(2)-anchors(ai,2))^2);
  end
  R=diag(dvar(step,:));
  
  % Frissítési lépés
  v=d(step,:)'-h;
  S=H*P*H'+R;
  K=P*H'*inv(S);
  m=m+K*v;
  P=P-K*S*K';
  
  % Historikus útvonal gyűjtése és kirajzolása
  kfhist=[kfhist; m'];
  plot(kfhist(:,1),kfhist(:,2),'bx-','LineWidth',2);
  circle(kfhist(end,1),kfhist(end,2),P(1,1),'b');	% A hiba mértékének jelzése
  
  
  % RANSAC
  error_threshold=2;	% Az inlier mérések maximális hibája, méterben
  best_inlier_idx=[];	% Az iterációk során eddig megtalált legnagyobb inlier halmaz
  for iter=1:30
    % Minimális mintahalmaz kiválasztása (2 pont a körök metszéséhez)
    anchor_idx1=randi(size(anchors,1));
    anchor_idx2=randi(size(anchors,1));
    
    % Figyeljünk arra, hogy ne legyen a két index azonos
    while anchor_idx1 == anchor_idx2
        anchor_idx2=randi(size(anchors,1));
    end
    
    % A két kiválasztott horgonyponthoz tartozó távolsági görbék metszete (2 db, ebből egy kiválasztása véletlenszerűen)
    test_point=circle_intersect(anchors(anchor_idx1,1),anchors(anchor_idx1,2),d(step,anchor_idx1),anchors(anchor_idx2,1),anchors(anchor_idx2,2),d(step,anchor_idx2));
    test_point=test_point(randi(2),:);
    
    % A próbamegoldáshoz tartozó inlierek összegyűjtése
    inlier_idx=[];
    for ai=1:size(anchors,1)
      distance=sqrt((anchors(ai,1)-test_point(1))^2+(anchors(ai,2)-test_point(2))^2);
      if abs(distance-d(step,ai)) < error_threshold
	inlier_idx=[inlier_idx ai];
      end
    end
    
    % Ha a talált megoldás jobb, akkor mentsük le
    if length(inlier_idx) > length(best_inlier_idx)
      best_inlier_idx = inlier_idx;
    end
  end
  
  % Az inlier távolságok ábrázolása vastagabb körként
  for i=best_inlier_idx
    circle(anchors(i,1),anchors(i,2),d(step,i),'LineWidth',2);
  end
  
  
  % Hiba kiszámítása az egyes módszerekre
  % err_hist=[err_hist; norm(real_loc(step,:)'-ngloc) norm(real_loc(step,:)'-m)];
  
  input('Hit key');
end;

figure;
plot(err_hist);
