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
load measurements;

% A mérőeszközök helyzetének betöltése
% anchors(k,1) - az k-dik anchor X koordinátája
% anchors(k,2) - az k-dik anchor Y koordinátája
load anchors;

% Az eszköz valós helyzete a hiba vizsgálatához
% real_loc(i,1) - az i-dik időpontban az eszköz X koordinátája
% real_loc(i,2) - az i-dik időpontban az eszköz Y koordinátája
load real_loc;


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
  
  pause;
end;
