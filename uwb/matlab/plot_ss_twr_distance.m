function dists = plot_ss_twr_distance(data)
   CNT_TO_SEC=1/(499.2e6*128);
   SOL=3e8;
   
   Tr = cdiff(data(:,4),data(:,1));
   Td = cdiff(data(:,3),data(:,2));
   
   TOF = (Tr-Td)/2*CNT_TO_SEC;
   plot(TOF*SOL);
   ylabel('distance (m)');
   grid;
end