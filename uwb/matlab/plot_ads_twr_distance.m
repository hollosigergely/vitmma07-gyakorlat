function dist = plot_ads_twr_distance(data)
   CNT_TO_SEC=1/(499.2e6*128);
   SOL=299702547;
    
   Tr1 = cdiff(data(:,4),data(:,1));
   Tr2 = cdiff(data(:,6),data(:,3));
   Td1 = cdiff(data(:,5),data(:,4));
   Td2 = cdiff(data(:,3),data(:,2));
   
   TOF = CNT_TO_SEC*(Tr1.*Tr2 - Td2.*Td1)./(Tr1+Tr2+Td1+Td2);
   dist = SOL * TOF;
   plot(dist);
   ylabel('distance (m)');
   grid;
end