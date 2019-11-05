function process()
    clear all;
    close all;
    
    data1 = load_meas('/home/hollosi/tmp/meas','BABA');
    data2 = load_meas('/home/hollosi/tmp/meas','C0DE');

    data1 = data1(:,2:end);
    data2 = data2(:,2:end);

    figure;
    subplot(2,1,1);
    plot_freq_error(data1);
    title('Frequecy error of BABA');
    
    subplot(2,1,2);
    plot_freq_error(data2);
    title('Frequecy error of C0DE');
    
    figure;
    subplot(2,1,1);
    plot_ss_twr_distance(data1);
    title('SS TWR distance of BABA');
    
    subplot(2,1,2);
    plot_ss_twr_distance(data2);
    title('SS TWR distance of C0DE');
    
    figure;
    subplot(2,1,1);
    plot_ads_twr_distance(data1);
    title('ADS TWR distance of BABA');
    
    subplot(2,1,2);
    plot_ads_twr_distance(data2);
    title('ADS TWR distance of C0DE');
end

function out=cdiff(in1,in2)
  d = in1 - in2;
  idx = find(d < 0);
  d(idx) = d(idx)+2^40;
  out = d;
end

function plot_freq_error(data)
    CNT_TO_SEC=1/(499.2e6*128);
    
    ddata = cdiff(data(2:end,:),data(1:end-1,:));
    ddata = ddata * CNT_TO_SEC;

    plot((ddata(:,1)-ddata(:,2))./ddata(:,1)*10^6);
    ylabel('ppm');
    grid;
end

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